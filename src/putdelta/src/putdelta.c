#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/putdelta.c,v 3.22 1991/07/01 09:35:50 dickey Exp $";
#endif

/*
 * Title:	putdelta.c (create new or initial sccs delta)
 * Author:	T.E.Dickey
 * Created:	25 Apr 1986
 * Modified:
 *		26 Jun 1991, added 'reftime' hack to account for filesystem
 *			     times ahead of system-clock.  Added code to make
 *			     z-file around critical zones.  Use x-file for
 *			     intermediate-form when copying back to s-file.
 *		25 Jun 1991, revised 'usage()'. Added "-n" option.  Use
 *			     'sccs2name()' and 'name2sccs()'. Made this work in
 *			     set-uid mode.
 *		20 Jun 1991, use 'shoarg()'
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
 *
 * patch:	assumes no line is longer than BUFSIZ
 */

#define	ACC_PTYPES
#define	STR_PTYPES
#include	"ptypes.h"
#include	"sccsdefs.h"

#include	<ctype.h>
#include	<errno.h>
#include	<time.h>
extern	struct	tm *localtime();
extern	FILE	*tmpfile();
extern	long	packdate();
extern	char	*getuser();
extern	char	*pathcat();
extern	time_t	time();
extern	int	localzone;

extern	char	*optarg;
extern	int	optind;
extern	int	errno;
extern	char	*sys_errlist[];

/************************************************************************
 *	local definitions						*
 ************************************************************************/
#define	COPY(dst,src)	strncpy(dst, src, sizeof(dst))[sizeof(dst)-1] = EOS
#define	PATHLEN		256	/* length of pathname */
#define	NAMELEN		80	/* length of tokens in sccs-header */
#define	TIMEZONE	5	/* time-zone we use to store dates */

#define	SHOW	if (ShowIt(TRUE))  PRINTF
#define	VERBOSE	   (ShowIt(FALSE))
#define	TELL	if (ShowIt(FALSE)) PRINTF

#define	HDR_DELTA	"\001d D "
#define	LEN_DELTA	5	/* strlen(HDR_DELTA) */

#ifndef	ADMIN_TOOL
#define	ADMIN_TOOL	"admin"
#endif

#ifndef	DELTA_TOOL
#define	DELTA_TOOL	"delta"
#endif

/************************************************************************
 *	local data							*
 ************************************************************************/
static	FILE	*fpT;
static	int	silent	= FALSE,
		no_op	= FALSE,
		ShowedIt;
static	char	username[NAMELEN],
		admin_opts[BUFSIZ],
		delta_opts[BUFSIZ];

static	char	fmt_lock[]  = "%s %s %s %8s %8s\n";
static	char	fmt_delta[] = "%s %8s %8s %s %d %d\n";
static	char	fmt_date[]  = "%02d/%02d/%02d";
static	char	fmt_time[]  = "%02d:%02d:%02d";

				/* names of the current file */
static	char	g_file[PATHLEN],
		s_file[PATHLEN];
static	int	s_mode;
				/* data set by TestDelta */
static	char	rev_code[NAMELEN],
		rev_pgmr[NAMELEN],
		rev_date[9],
		rev_time[9];
static	int	rev_this,
		rev_last;

				/* data used by EditFile */
static	unsigned short chksum;
				/* data used in MkDir */
static	char	d_path[PATHLEN];
static	int	d_mode,
		d_user,
		d_group;

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
 * Construct the specified sccs filename from the s-file
 */
static
MakeName(dst, code)
char	*dst, code;
{
	register char	*s;
	if (s = strrchr(strcpy(dst, s_file), '/'))
		*(++s) = code;
	else
		*dst = code;
}

/*
 * Begin/end critical zone (i.e., prevent other sccs applications from running)
 */
static
Critical(begin)
int	begin;
{
	auto	char	z_file[PATHLEN];
	static	int	began;
	static	char	*msg	= "cannot create lock file (cm4)";

	if (no_op)
		return;

	MakeName(z_file, 'z');
	if (begin == TRUE) {
		short	id = getpid();
		int	fd = open(z_file, O_EXCL | O_CREAT | O_WRONLY, 0444);
		began	= TRUE;
		if (fd < 0
		 || write(fd, (char *)&id, sizeof(id)) < 0
		 || close(fd) < 0) {
			perror(msg);
			exit(FAIL);
		}
	} else if (began) {
		int	save	= errno;
		if ((unlink(z_file) < 0) && (begin == FALSE)) {
			if (errno != EPERM
			 && errno != ENOENT)
				perror(msg);
		}
		errno	= save;
		began	= FALSE;
	}
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
	if (access(path, R_OK | W_OK | X_OK) < 0)
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
	auto	char	tmp_code[BUFSIZ],
			tmp_pgmr[BUFSIZ];
	if (!strncmp(bfr, HDR_DELTA, LEN_DELTA)) {
		bfr += LEN_DELTA;
		if (sscanf(bfr, fmt_delta,
		    tmp_code, rev_date, rev_time, tmp_pgmr,
		    &rev_this, &rev_last) == 6) {
			COPY(rev_code, tmp_code);
			COPY(rev_pgmr, tmp_pgmr);
			return (TRUE);
		}
	}
	return (FALSE);
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
	char	bfr[BUFSIZ],
		old_rev[BUFSIZ],
		new_rev[BUFSIZ],
		who_rev[BUFSIZ],
		p_file[PATHLEN];

	MakeName(p_file, 'p');
	Critical(TRUE);
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
				FCLOSE(fp);
				Critical(FALSE);
				if (!strcmp(who_rev, username) ) {
					return (TRUE);
				} else {
					TELL("?? lock on %s by %s\n",
						old_rev, who_rev);
					return (FALSE);
				}
			}
		}
		FCLOSE(fp);
	}

	/* try to determine the delta to which we should fake a lock */
	if (fp = fopen(s_file, "r")) {
		while (fgets(bfr, sizeof(bfr), fp) && *bfr == '\001') {
			if (TestDelta(bfr)) {
				FORMAT(bfr, fmt_lock,
					rev_code, NextDelta(rev_code),
					username, rev_date, rev_time);
				new_lock = TRUE;
				break;
			}
		}
		FCLOSE(fp);
	}

	/* if we found a correctly-formatted delta in the s-file, lock it */
	if (new_lock != FALSE) {
		if (VERBOSE) shoarg(stdout, "lock", g_file);
		if (no_op) {
			Critical(FALSE);
			return (TRUE);
		} else if (fp = fopen(p_file, "a+")) {
			(void)fputs(bfr, fp);
			FCLOSE(fp);
			Critical(FALSE);
			return (TRUE);
		}
	}
	TELL("?? cannot lock %s\n", g_file);
	Critical(FALSE);
	return (FALSE);
}

/*
 * Modify the given delta's time to correspond with the file modification time.
 */
static
EditDelta(bfr, modtime, reftime, t)
char	*bfr;
time_t	modtime, reftime;
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

		modtime += reftime;
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
	register int j;
	auto	FILE	*fpS;
	auto	char	bfr[BUFSIZ],
			x_file[PATHLEN];

	MakeName(x_file, 'x');
	if (!(fpS = fopen (x_file, "w")))
		failed(x_file);
	FPRINTF (fpS, "\001h%05d\n", chksum);
	(void) rewind (fpT);
	for (j = 1; j < lines; j++) {
		(void) fgets (bfr, sizeof(bfr), fpT);
		(void) fputs (bfr, fpS);
	}
	FCLOSE(fpS);
	if (chmod(x_file, s_mode) < 0
	 || rename(x_file, s_file) < 0)
		perror(x_file);
	SHOW ("** %d lines processed\n", lines);
	(void) fflush (stdout);
}

/*
 * Modify the check-in date in a single SCCS s-file:
 */
static
ProcessFile(modtime, reftime)
time_t	modtime,	/* file's mod-time */
	reftime;	/* min-estimate of delay between clock and file-system*/
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
	Critical(TRUE);
	if (fpS = fopen (s_file, "r")) {
		while (fgets (bfr, sizeof(bfr), fpS)) {
			lines++;
			len = strlen(bfr);
			if (!changed && TestDelta(bfr)) {
				if (! EditDelta(bfr, modtime, reftime, &tfix))
					break;
				changed++;
			}
			if (strncmp(bfr, "\001h", 2)) {
				for (j = 0; j < len; j++)
					chksum += (bfr[j] & 0xff);
				(void) fputs (bfr, fpT);
			}
		}
		FCLOSE (fpS);

		if (changed)
			EditFile(lines);
	} else {
		TELL ("** could not open \"%s\"\n", s_file);
	}
	Critical(FALSE);
}

/*
 * Performs the 'mkdir()' using the proper ownership/mode.
 */
static
MkDir()
{
	int	old_mask = umask(0);
	if (mkdir(d_path, d_mode) < 0)
		failed(d_path);
	(void)umask(old_mask);
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
	register char	*s;
	auto	time_t	put_time,
			ref_time = time(0);
	struct	stat	sb;
	auto	char	temp[BUFSIZ],
			*put_verb,
			put_opts[BUFSIZ];
	auto	int	put_flag = TRUE;

	ShowedIt = FALSE;
	(void)strcpy(s_file, name2sccs(name, FALSE));
	(void)strcpy(g_file, sccs2name(name, FALSE));

	/* The file must exist; otherwise we give up! */
	if ((put_time = isFILE(g_file, &s_mode)) == 0)
		failed(g_file);

	/* It is possible (particularly on Apollo ring) for our clock time
	 * to be earlier than the file's mod-time.
	 */
	if ((ref_time -= put_time) <= 0)
		ref_time--;	/* negative iff clock < file */
	else
		ref_time = 0;

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
				/*
				 * If this is not running in set-uid mode, make
				 * the directory with group-permissions so that
				 * the owner of the parent directory can still
				 * put files in it too.
				 *
				 * patch: should verify group-compatibility
				 */
				if (getuid() != d_user
				 || getgid() != d_group)
					d_mode |= (S_IWRITE>>3);
					/* force to group-writeable */
				if (getuid() == geteuid())
					d_user = getuid();
				if (for_user2(MkDir, d_user, d_group) < 0)
					failed("mkdir");
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
		put_verb = DELTA_TOOL;
		catarg(strcpy(put_opts, delta_opts), s_file);
		put_flag = FALSE;
	} else {
		put_verb = ADMIN_TOOL;
		FORMAT(temp, "-i%s", g_file);
		catarg(strcpy(put_opts, admin_opts), temp);
		catarg(put_opts, s_file);
	}

	if (VERBOSE) shoarg(stdout, put_verb, put_opts);
	if (!no_op) {
		if (execute(put_verb, put_opts) < 0)
			failed(put_verb);
		ProcessFile(put_time, ref_time);
		if (put_flag && !geteuid()) {
			if (chown(s_file, d_user, d_group) < 0)
				failed("chown");
		}
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

	COPY(username, getuser());

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

/*
 * We have our own 'failed()' to ensure that we clear critical-zone
 */
failed(s)
char	*s;
{
	Critical(-1);
	perror(s);
	(void)exit(FAIL);
}
