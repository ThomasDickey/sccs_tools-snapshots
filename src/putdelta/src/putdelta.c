#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/putdelta.c,v 3.10 1991/06/25 15:42:17 dickey Exp $";
#endif

/*
 * Title:	putdelta.c (create new or initial sccs delta)
 * Author:	T.E.Dickey
 * Created:	25 Apr 1986
 * Modified:
 *		25 Jun 1991, revised 'usage()'. Added "-n" option.  Use
 *			     'sccs2name()' and 'name2sccs()'. Made this work in
 *			     set-uid mode.
 *		20 Jun 1991, use 'shoarg()'
 *		13 Sep 1988, use 'catchall()'
 *		06 Sep 1988, 'admin' doesn't recognize "-s" switch.
 *		28 Jul 1988, renamed from 'sccsbase', rewrote to be a complete
 *			     package for admin/delta.
 *		10 Jun 1988, recoded to use 'newzone()'.
 *		19 May 1988, ported to Apollo bsd4.2; do putenv here, not in
 *			     shell.
 *		01 May 1986, added '-s' silent option.  Ignore directories and
 *			     devices.
 *
 * Function:	This is a package around the SCCS utilities "admin" and "delta"
 *		which greatly simplies their use:
 *
 *		(*) if the sccs-directory does not exist, it is created.
 *
 *		(*) files need not be locked to apply a delta.
 *
 *		(*) files are never removed after making a delta.
 *
 *		(*) after checkin, "putdelta" modifies the delta-date of
 *		    the corresponding SCCS-file to be the same as the
 *		    modification date of the file (provided that the
 *		    delta-date is newer than the file).
 *
 *		(*) also after checkin, the mode of the s-file is set to 0444 or
 *		    0555, depending on whether the g-file was executable.  This
 *		    lets "getdelta" use the mode-information a la RCS's ci/co
 *		    to store the executable-status of shell scripts.  (The
 *		    SCCS utilities do not care about this).
 *
 *		The SCCS-files are assumed to be in the standard location:
 *
 *			name => $SCCS_DIR/s.name
 *
 * Options:	(see usage)
 */

#define	ACC_PTYPES
#define	SIG_PTYPES
#define	STR_PTYPES
#include	"ptypes.h"
#include	"sccsdefs.h"

#include	<ctype.h>
#include	<errno.h>
#include	<time.h>
#include	<signal.h>
extern	struct	tm *localtime();
extern	FILE	*tmpfile();
extern	long	packdate();
extern	char	*getuser();
extern	char	*pathcat();
extern	int	localzone;

extern	char	*optarg;
extern	int	optind;
extern	int	errno;
extern	char	*sys_errlist[];

/* local declarations: */
#define	NAMELEN		80	/* length of tokens in sccs-header */
#define	TIMEZONE	5	/* time-zone we use to store dates */

#define	SHOW	if (ShowIt(TRUE))  PRINTF
#define	VERBOSE	   (ShowIt(FALSE))
#define	TELL	if (ShowIt(FALSE)) PRINTF

static	FILE	*fpT;
static	int	silent	= FALSE,
		no_op	= FALSE,
		ShowedIt;
static	char	username[NAMELEN],
		admin_opts[BUFSIZ],
		delta_opts[BUFSIZ];

#define	HDR_DELTA	"\001d D "
#define	LEN_DELTA	5	/* strlen(HDR_DELTA) */

#ifdef	__STDCPP__
#define	FOR_USER(F,U)	if (for_user2(F, U, d_group) < 0) failed(#F)
#else
#define	FOR_USER(F,U)	if (for_user2(F, U, d_group) < 0) failed("F")
#endif	/* __STDCPP__ */

static	char	fmt_lock[]  = "%s %s %s %s %s\n";
static	char	fmt_delta[] = "%s %s %s %s %d %d\n";
static	char	fmt_date[]  = "%02d/%02d/%02d";
static	char	fmt_time[]  = "%02d:%02d:%02d";

				/* names of the current file */
static	char	g_file[BUFSIZ],
		s_file[BUFSIZ],
		p_file[BUFSIZ];
static	int	s_mode;
				/* data set by TestDelta */
static	char	rev_code[BUFSIZ],
		rev_pgmr[BUFSIZ],
		rev_date[BUFSIZ],
		rev_time[BUFSIZ];
static	int	rev_this,
		rev_last;

				/* data used by EditFile */
static	unsigned short chksum;
				/* data used in 'actual_mkdir' */
static	char	d_path[BUFSIZ];
static	int	d_mode,
		d_user,
		d_group;
				/* data used in 'actual_checkin' */
static	char	*put_verb,
		put_opts[BUFSIZ];
				/* data used in 'force_lock' */
static	char	lock_buffer[BUFSIZ];

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static
usage ()
{
	static	char	*msg[] = {
 "Usage: putdelta [options] files"
,""
,"Options"
,"  -n       (no-op) shows actions, but does not perform them"
,"  -s       (silent) suppress all but essential messages reporting updates"
,"           which were made."
,"  -y TEXT  describes the delta (otherwise you will be prompted)"
	};
	register int	j;
	for (j = 0; j < sizeof(msg)/sizeof(msg[0]); j++)
		FPRINTF(stderr, "%s\n", msg[j]);
	(void)exit(FAIL);
}

/*
 * Show the current filename, once before the first message applying to it.
 * If the silent-option is inactive, each filename is shown even if no messages
 * apply to it.
 */
static
ShowIt (doit)
{
	if (!ShowedIt && (doit || !silent)) {
		PRINTF ("File \"%s\"\n", g_file);
		ShowedIt++;
	}
	return (doit || !silent);
}

/*
 * Verify that we got a directory in the 'stat()' call. We use this for
 * ownership information.
 */
static
NeedDirectory(path, sb)
char	*path;
struct	stat	*sb;
{
	if (stat(path, sb) < 0)
		failed(path);
	if ((sb->st_mode & S_IFMT) != S_IFDIR) {
		errno = ENOTDIR;
		failed(path);
	}
	if (access(path, X_OK | R_OK) < 0)
		failed(path);
	d_user = sb->st_uid;
	d_group = sb->st_gid;
}

/*
 * See if the specified file exists.  If so, verify that it is indeed a file.
 */
static
isFILE(name, mode_)
char	*name;
int	*mode_;
{
	struct	stat	sb;

	if (stat(name, &sb) >= 0) {
		if ((sb.st_mode & S_IFMT) == S_IFREG) {
			if (mode_)	/* save executable-status */
				*mode_ = sb.st_mode & 0555;
			return (sb.st_mtime);
		} else {
			TELL ("?? \"%s\" is not a file\n", name);
			(void)exit(FAIL);
			/*NOTREACHED*/
		}
	}
	return (0);
}

/*
 * Test a string to see if it looks like a revision-code.
 */
static
isCODE(string)
char	*string;
{
	while (*string)
		if (isdigit(*string) || *string == '.')
			string++;
		else
			return (FALSE);
	return (TRUE);
}

/*
 * Given a revision-code, compute the next one
 */
static
char *
NextDelta(this)
char	*this;
{
	int	next;
	char	*s;
	static	char	bfr[80];

	if (s = strrchr(this, '.')) {
		if (sscanf(++s, "%d", &next) != 1)
			next = 1;
		FORMAT(bfr, "%.*s%d", s-this, this, next+1);
	} else
		(void)strcat(strcpy(bfr, this), ".1");
	return (bfr);
}

/*
 * Test the buffer to see if it contains a delta-control line
 */
static
TestDelta(bfr)
char	*bfr;
{
	if (!strncmp(bfr, HDR_DELTA, LEN_DELTA)) {
		bfr += LEN_DELTA;
		if (sscanf(bfr, fmt_delta,
		    rev_code, rev_date, rev_time, rev_pgmr,
		    &rev_this, &rev_last) == 6) {
			return (TRUE);
		}
	}
	return (FALSE);
}

/*
 * Writes the p-file forcing a lock
 */
static
force_lock()
{
	FILE	*fp;
	if (fp = fopen(p_file, "a+")) {
		(void)fputs(lock_buffer, fp);
		(void)fclose(fp);
		return;
	}
	failed(p_file);
	/*NOTREACHED*/
}

/*
 * Inspect the p-file to see if we have a lock on a version.
 * If there are no locks, fake one so that "delta" will see it.
 *
 * patch: This restricts us to making locks only if none exist.
 */
static
TestLock()
{
	FILE	*fp;
	int	year, mon, mday, hour, min, sec;
	int	new_lock = FALSE;
	char	bfr[BUFSIZ];
	char	old_rev[NAMELEN];
	char	new_rev[NAMELEN];
	char	who_rev[NAMELEN];

	if (fp = fopen(p_file, "r")) {
		while (fgets(bfr, sizeof(bfr), fp)) {
			if ((sscanf(bfr, fmt_lock,
				old_rev, new_rev, who_rev,
				rev_date, rev_time) == 5)
			&&  (sscanf(rev_date, fmt_date,
				&year, &mon, &mday) == 3)
			&&  (sscanf(rev_time, fmt_time,
				&hour, &sec, &min) == 3)
			&& isCODE(old_rev)
			&& isCODE(new_rev) ) {
				if (!strcmp(who_rev, username) ) {
					return (TRUE);
				} else {
					TELL("?? lock on %s by %s\n",
						old_rev, who_rev);
					return (FALSE);
				}
			}
		}
		(void)fclose(fp);
	}

	/* try to determine the delta to which we should fake a lock */
	if (fp = fopen(s_file, "r")) {
		while (fgets(bfr, sizeof(bfr), fp) && *bfr == '\001') {
			if (TestDelta(bfr)) {
				FORMAT(lock_buffer, fmt_lock,
					rev_code, NextDelta(rev_code),
					username, rev_date, rev_time);
				new_lock = TRUE;
				break;
			}
		}
		(void)fclose(fp);
	}

	/* if we found a correctly-formatted delta in the s-file, lock it */
	if (new_lock != FALSE) {
		if (VERBOSE) shoarg(stdout, "lock", g_file);
		FOR_USER(force_lock,getuid());
		return (TRUE);
	}
	TELL("?? cannot lock %s\n", g_file);
	return (FALSE);
}

/*
 * Modify the given delta's time to correspond with the file modification time.
 */
static
EditDelta(bfr, modtime, t)
char	*bfr;
time_t	modtime;
struct	tm	*t;
{
	time_t	delta;
	int	year, mon, mday, hour, min, sec;

	if ((sscanf(rev_date, fmt_date, &year, &mon, &mday) == 3)
	&&  (sscanf(rev_time, fmt_time, &hour, &min, &sec)  == 3)) {

		newzone(TIMEZONE,0,FALSE);/* interpret in EST/EDT */
		delta = packdate (1900+year, mon, mday, hour, min, sec);
		oldzone();

		TELL("** old: %s", ctime(&delta));
		TELL("** new: %s", ctime(&modtime));

		FORMAT(rev_date, fmt_date, t->tm_year, t->tm_mon+1, t->tm_mday);
		FORMAT(rev_time, fmt_time, t->tm_hour, t->tm_min, t->tm_sec);
		FORMAT(&bfr[LEN_DELTA], fmt_delta,
			rev_code, rev_date, rev_time, rev_pgmr,
			rev_this, rev_last);
		if (delta <= modtime) {
			if (delta < modtime)
				SHOW ("** file is newer than delta\n");
			return (FALSE);
		}
		return (TRUE);
	}
	TELL ("** delta formatting error, line:\n%s\n", bfr);
	return (FALSE);
}

/*
 * If we wrote an altered delta-date to the temp-file, recopy it back over the
 * original SCCS-file:
 */
static
EditFile(lines)
long	lines;
{
	FILE	*fpS;
	register int j;
	char	bfr[BUFSIZ];

	if (chmod(s_file, 0644)) {
		   TELL ("** cannot write-enable \"%s\"\n", s_file);
		   return;
	}
	catchall(SIG_IGN);
	fpS = fopen (s_file, "w");
	FPRINTF (fpS, "\001h%05d\n", chksum);
	(void) rewind (fpT);
	for (j = 1; j < lines; j++) {
		(void) fgets (bfr, sizeof(bfr), fpT);
		(void) fputs (bfr, fpS);
	}
	(void) fclose (fpS);
	(void) chmod (s_file, s_mode);
	catchall(SIG_DFL);
	SHOW ("** %d lines processed\n", lines);
	(void) fflush (stdout);
}

/*
 * Modify the check-in date in a single SCCS s-file:
 */
static
ProcessFile(modtime)
time_t	modtime;
{
	FILE		*fpS;
	struct	tm	tfix;
	time_t		fixtime;
	long		lines	= 0;
	int		j, len,
			changed = FALSE;
	char		bfr[BUFSIZ];

	chksum	= 0;
	oldzone();		/* initialize 'localzone' */
	fixtime = modtime - (((TIMEZONE*60) - localzone) * 60);
	tfix = *localtime(&fixtime);

	(void) rewind (fpT);
	if (fpS = fopen (s_file, "r")) {
		while (fgets (bfr, sizeof(bfr), fpS)) {
			lines++;
			len = strlen(bfr);
			if (!changed && TestDelta(bfr)) {
				if (! EditDelta(bfr, modtime, &tfix))
					break;
				changed++;
			}
			if (strncmp(bfr, "\001h", 2)) {
				for (j = 0; j < len; j++)
					chksum += (bfr[j] & 0xff);
				(void) fputs (bfr, fpT);
			}
		}
		(void) fclose (fpS);

		if (changed)
			EditFile(lines);
	} else {
		TELL ("** could not open \"%s\"\n", s_file);
	}
}

/*
 * Performs the 'mkdir()' using the proper ownership/mode.
 */
static
actual_mkdir()
{
	int	old_mask = umask(0);
	if (mkdir(d_path, d_mode) < 0)
		failed(d_path);
	(void)umask(old_mask);
}

/*
 * Performs the actual checkin
 */
static
actual_checkin()
{
	if (execute(put_verb, put_opts) < 0) {
		perror(put_verb);
		return;
	}
}

/*
 * Process a single file.  If we are given the name of a non-sccs file, compute
 * the name of the corresponding sccs file.  Otherwise, compute the name of the
 * file to be checked-out from the sccs file name.
 */
static
DoFile(name)
char	*name;
{
	time_t	put_time;
	register char	*s;
	struct	stat	sb;
	auto	char	temp[BUFSIZ];

	ShowedIt = FALSE;
	(void)strcpy(s_file, name2sccs(name, FALSE));
	(void)strcpy(g_file, sccs2name(name, FALSE));

	/* The file must exist; otherwise we give up! */
	if ((put_time = isFILE(g_file, &s_mode)) == 0) {
		perror(g_file);
		return;
	}

	/* Construct the name of the corresponding p-file */
	if (s = strrchr(strcpy(p_file, s_file), '/'))
		*(++s) = 'p';
	else
		*p_file = 'p';

	/* If the sccs-directory does not exist, make it */
	if (s = strrchr(s_file, '/')) {
		size_t	len	= s - s_file;
		(strncpy(d_path, s_file, len))[len] = EOS;

		if (stat(d_path, &sb) >= 0) {
			NeedDirectory(d_path, &sb);
		} else {
			/* mode and ownership should propagate from the
			 * directory in which we create the sccs-directory.
			 */
			abspath(pathcat(temp, d_path, ".."));
			NeedDirectory(temp, &sb);

			if (VERBOSE) shoarg(stdout, "mkdir", d_path);
			if (!no_op) {
				d_mode = sb.st_mode & 0777;
				FOR_USER(actual_mkdir,d_user);
			}
		}
	} else		/* ...else... user is putting files in "." */
		NeedDirectory(".", &sb);

	if (getuid() == d_user) {
		revert("set-uid mode redundant");
		d_group = getegid();
	}

	/* If the s-file exists, we make a delta; otherwise initial insertion */
	if (isFILE(s_file, (int *)0)) {
		if (!TestLock())
			return;
		put_verb = "delta";
		catarg(strcpy(put_opts, delta_opts), s_file);
	} else {
		put_verb = "admin";
		FORMAT(temp, "-i%s", g_file);
		catarg(strcpy(put_opts, admin_opts), temp);
		catarg(put_opts, s_file);
	}

	if (VERBOSE) shoarg(stdout, put_verb, put_opts);
	if (!no_op) {
		FOR_USER(actual_checkin,getuid());
		ProcessFile(put_time);
	}
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/

main (argc, argv)
char	*argv[];
{
	register int	j;
	char	tmp[BUFSIZ];

	(void)strcpy(username, getuser());

	catarg(delta_opts, "-n");
	fpT = tmpfile();
	while ((j = getopt(argc, argv, "nsy:")) != EOF)
		switch (j) {
		case 'n':
			no_op	= TRUE;
			break;
		case 's':
			silent	= TRUE;
			catarg(delta_opts, "-s");
			break;
		case 'y':
			FORMAT(tmp, "-y%s", optarg);
			catarg(admin_opts, tmp);
			catarg(delta_opts, tmp);
			break;
		default:
			usage ();
		}

	for (j = optind; j < argc; j++)
		DoFile (argv[j]);
	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
