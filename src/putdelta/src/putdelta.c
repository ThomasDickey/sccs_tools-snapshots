#ifndef	lint
static	char	sccs_id[] = "@(#)putdelta.c	1.3 88/09/02 09:29:56";
#endif	lint

/*
 * Title:	putdelta.c (create new or initial sccs delta)
 * Author:	T.E.Dickey
 * Created:	25 Apr 1986
 * Modified:
 *		02 Sep 1988, use 'sccs_dir()'
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
 * Options:	s	(silent) suppress all but essential messages reporting
 *			updates which were made.
 *		y note	specifies delta-comment
 */

#include	"ptypes.h"

#include	<ctype.h>
#include	<time.h>
#include	<signal.h>
extern	struct	tm *localtime();
extern	FILE	*tmpfile();
extern	long	packdate();
extern	char	*getuser();
extern	char	*sccs_dir();
extern	char	*strcat();
extern	char	*strcpy();
extern	char	*strncpy();
extern	char	*strrchr();
extern	int	localzone;

extern	char	*optarg;
extern	int	optind;
extern	int	errno;
extern	char	*sys_errlist[];

/* local declarations: */
#define	NAMELEN		80	/* length of tokens in sccs-header */
#define	TIMEZONE	5	/* time-zone we use to store dates */

#define	SHOW	if (ShowIt(TRUE))  PRINTF
#define	TELL	if (ShowIt(FALSE)) PRINTF
#define	WARN	TELL ("?? %s\n", sys_errlist[errno])

static	FILE	*fpT;
static	int	silent	= FALSE,
		ShowedIt;
static	char	username[NAMELEN],
		admin_opts[BUFSIZ],
		delta_opts[BUFSIZ];

#define	HDR_DELTA	"\001d D "
#define	LEN_DELTA	5	/* strlen(HDR_DELTA) */

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

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static
usage ()
{
	PRINTF ("usage: putdelta [-s -y<comment>] files\n");
	(void) exit (1);
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
			exit(1);
			/*NOTREACHED*/
		}
	}
	return (0);
}

/*
 * See if the name corresponds to an sccs "s." file
 */
static
char	*
isSCCS(name)
char	*name;
{
	register char *s = strrchr(name, '/');

	if (!s) s = name;
	else	s++;
	if (!strncmp(s, "s.", 2))
		return (s+2);
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
				FORMAT(bfr, fmt_lock,
					rev_code, NextDelta(rev_code),
					username, rev_date, rev_time);
				new_lock = TRUE;
				break;
			}
		}
		(void)fclose(fp);
	}

	/* if we found a correctly-formatted delta in the s-file, lock it */
	if ((new_lock != FALSE) && (fp = fopen(p_file, "a+"))) {
		(void)fputs(bfr, fp);
		(void)fclose(fp);
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
	int	(*sv_int)();
	int	(*sv_quit)();
	register int j;
	char	bfr[BUFSIZ];

	if (chmod(s_file, 0644)) {
		   TELL ("** cannot write-enable \"%s\"\n", s_file);
		   return;
	}
	sv_int  = signal (SIGINT, SIG_IGN);
	sv_quit = signal (SIGQUIT, SIG_IGN);
	fpS = fopen (s_file, "w");
	(void) fprintf (fpS, "\001h%05d\n", chksum);
	(void) rewind (fpT);
	for (j = 1; j < lines; j++) {
		(void) fgets (bfr, sizeof(bfr), fpT);
		(void) fputs (bfr, fpS);
	}
	(void) fclose (fpS);
	(void) chmod (s_file, s_mode);
	SHOW ("** %d lines processed\n", lines);
	(void) fflush (stdout);
	(void) signal (SIGINT,  sv_int);
	(void) signal (SIGQUIT, sv_quit);
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
 * If the SCCS-directory does not exist, make it.
 */
static
MakeDirectory()
{
	struct	stat	sb;
	char	path[BUFSIZ],
		*s;

	if (s = strrchr(s_file, '/')) {
		(strncpy(path, s_file, s - s_file))[s - s_file] = '\0';

		if (stat(path, &sb) >= 0)
			return ((sb.st_mode & S_IFMT) == S_IFDIR);
		else {
			TELL("** make directory %s\n", path);
			if (mkdir(path, 0755) < 0) {
				WARN;
				return (FALSE);
			}
		}
	}		/* ...else... user is putting files in "." */
	return (TRUE);
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
	time_t		modtime;
	register char	*s;
	static	char	temp[BUFSIZ],
			*put_verb,
			put_opts[BUFSIZ];

	ShowedIt = FALSE;
	/* Construct the name of the s-file */
	if (s = isSCCS(name)) {
		(void)strcpy(s_file, name);
		(void)strcpy(g_file, name = s);
	} else {
		FORMAT(s_file, "%s/s.%s", sccs_dir(), strcpy(g_file, name));
	}

	/* The file must exist; otherwise we give up! */
	if ((modtime = isFILE(g_file, &s_mode)) == 0) {
		WARN;
		return;
	}

	/* Construct the name of the corresponding p-file */
	if (s = strrchr(strcpy(p_file, s_file), '/'))
		*(++s) = 'p';
	else
		*p_file = 'p';

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
		if (!MakeDirectory())
			return;
	}
	TELL("** %s %s\n", put_verb, put_opts);

	if (execute(put_verb, put_opts) < 0)
		return;

	ProcessFile(modtime);
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
	while ((j = getopt(argc, argv, "sy:")) != EOF)
		switch (j) {
		case 's':
			silent	= TRUE;
			catarg(admin_opts, "-s");
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
	exit(0);
	/*NOTREACHED*/
}

failed(s)
char	*s;
{
	perror(s);
	exit(1);
}
