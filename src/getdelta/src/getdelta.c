#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/sccs_tools.vcs/src/getdelta/src/RCS/getdelta.c,v 3.11 1991/06/28 16:58:07 ste_cm Exp $";
#endif

/*
 * Title:	getdelta.c (get an sccs-delta)
 * Author:	T.E.Dickey
 * Created:	26 Mar 1986 (as a procedure)
 * Modified:
 *		27 Jun 1991, added "-b" option.
 *		24 Jun 1991, Test directory permissions for archive- and
 *			     working-file.
 *		20 Jun 1991, pass-thru "-e" option. Use 'shoarg()'.
 *			     Use 'sccs2name()' and 'name2sccs()'.
 *		22 Mar 1989, corrected 'same()' code so we can get correct
 *			     date from branches.
 *		02 Sep 1988, dropped "-d" option and GET_PATH.
 *		09 Aug 1988, corrected overlapping "-s", "-k" options.
 *		29 Jul 1988, renamed from 'sccsbase'.  Preserve executable-mode
 *			     of extracted file (see 'putdelta').
 *		10 Jun 1988, recoded so that "get" is invoked from this module
 *			     rather than from shell script.
 *		20 May 1988, port to Apollo bsd4.2 (DOMAIN/IX SR9.7).
 *		21 Apr 1988, Apollo SR9.7 does not need 'utime()' fix.
 *		23 Nov 1987, fix for APOLLO 'utime()'.
 *		12 May 1986, added '-d' option
 *		01 May 1986, recoded to be a program.
 *
 * Function:	Extract a specified SCCS-delta of a given file, altering the
 *		modification date to correspond with the delta-date.
 *
 * Options:	(see 'usage()')
 */

#define	ACC_PTYPES
#define	STR_PTYPES
#include	"ptypes.h"
#include	"sccsdefs.h"

#include	<ctype.h>
#include	<time.h>
extern	long	packdate();
extern	char	*ctime();
extern	char	*pathcat();
extern	char	*relpath();
extern	time_t	cutoff();

extern	char	*optarg;
extern	int	optind;

/* local definitions */
#define	NAMELEN		80	/* length of tokens in sccs-header */
#define	YELL	(void) FPRINTF(stderr,
#define	TELL	if (!silent) PRINTF(

#ifndef	GET_TOOL
#define	GET_TOOL	"get"
#endif

/************************************************************************
 *	local data							*
 ************************************************************************/

static	time_t	opt_c	= 0;

static	int	writeable = 0,		/* nonzero to make g-file writeable */
		s_mode,			/* protection of s-file */
		silent	= FALSE,	/* "-s" option */
		force	= FALSE,	/* "-f" option */
		lockit	= FALSE,	/* "-e" option */
		noop	= FALSE;	/* "-n" option */

static	char	*sid	= NULL,
		get_opts[BUFSIZ],
		bfr[BUFSIZ];

/************************************************************************
 *	local procedures						*
 ************************************************************************/

/*
 * Compare the SCCS version string against the '-r' option's value
 */
static
int
same (version)
char	*version;
{
	int	code	= (sid == NULL);

	if (!code) {
		register size_t	len = strlen (sid),
				cmp = strlen (version),
				dot;
		if (len == cmp)
			code = ! strcmp(sid,version);
		else if (len < cmp) {
			for (cmp = dot = 0; sid[cmp]; cmp++)
				if (sid[cmp] == '.')
					dot++;
			if ((dot == 0 || dot == 2)	/* "R" or "R.L.B" */
			&&  (version[len] == '.'))
				code = ! strncmp(sid,version,len);
		}
	}
	return (code);
}

/*
 * Process a single file:
 */
static
PostProcess (name, s_file)
char	*name, *s_file;
{
	FILE	*fp;
	time_t	date	= 0;
	int	got	= FALSE,
		year, mon, mday,
		hour, min, sec, new, old;
	char	version[NAMELEN], pgmr[NAMELEN];

	static	char	fmt[] = "\001d D %s %d/%d/%d %d:%d:%d %s %d %d";

	if (fp = fopen (s_file, "r")) {
		newzone(5,0,FALSE);	/* interpret in EST/EDT zone */
		while (fgets(bfr, sizeof(bfr), fp) && *bfr == '\001') {
			if (sscanf (bfr, fmt, version,
					&year, &mon,  &mday,
					&hour, &min, &sec,
					pgmr, &new, &old) > 0) {
				date = packdate(1900+year, mon, mday, hour, min, sec);
				if (opt_c) {
					if (date > opt_c)	continue;
					else if (got = !sid)	break;
				}
				if (got = same(version))	break;
			}
		}
		(void)fclose (fp);
		oldzone();		/* restore caller's time zone */
		if (got) {
			YELL "** %s %s: %s", name, version, ctime(&date));
		} else {
			if (sid) {
				TELL "** no match for sid=%s\n", sid);
			} else if (opt_c) {
				TELL "** no deltas before cutoff\n");
				if (date)
					TELL "** oldest date was %s", ctime(&date));
			}
		}
	} else
		TELL "** could not open \"%s\"\n", s_file);

	if (!noop && got) {
		(void)chmod(name, s_mode);
		if (setmtime(name, date) < 0)
			YELL "%s: cannot set time\n", name);
		else if (lockit && !geteuid()) {
			char	p_file[BUFSIZ];
			char	*s = strrchr(strcpy(p_file, s_file), '/');
			if (s != 0)
				s++;
			else
				s = p_file;
			*s = 'p';
			if (chown(p_file, getuid(), getgid()) < 0)
				failed("chown");
		}
	}
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
			if (mode_)
				*mode_ = (sb.st_mode & 0555) | writeable;
			return (TRUE);
		} else {
			YELL "?? \"%s\" is not a file\n", name);
			(void)exit(FAIL);
			/*NOTREACHED*/
		}
	}
	return (FALSE);
}

/*
 * See if we have permission to write in the directory given by 'name'
 */
static
Permitted(name, read_only)
char	*name;
{
	char	path[BUFSIZ];
	int	mode	= X_OK | R_OK;
	char	*mark	= strrchr(pathcat(path, ".", name), '/');

	if (mark != 0)	*mark = EOS;
	if (!read_only)	mode |= W_OK;

	if (access(path, mode) < 0)
		failed(path);
	return TRUE;
}

/*
 * Process a single file.  If we are given the name of a non-sccs file, compute
 * the name of the corresponding sccs file.  Otherwise, compute the name of the
 * file to be checked-out from the sccs file name.
 */
DoFile (name, s_file)
char	*name, *s_file;
{
	auto	int	ok	= TRUE;
	auto	char	*working = sccs2name(name, FALSE);
	auto	char	*archive = name2sccs(name, FALSE);
	auto	char	old_wd[BUFSIZ],
			new_wd[BUFSIZ],
			buffer[BUFSIZ],
			*s;

	if (!Permitted(working, FALSE) || !Permitted(archive, !lockit))
		return;

	/*
	 * SCCS 'get' extracts only into the current directory.  Perform a
	 * 'chdir()' to accommodate this if necessary.
	 */
	(void)strcpy(buffer, working);
	if (s = strrchr(buffer, '/')) {
		if (!getwd(old_wd))
			failed("getwd");
		*s = EOS;
		name = ++s;
		abspath(pathcat(new_wd, old_wd, buffer));
		abspath(strcpy(s_file, name2sccs(name, FALSE)));
		if (!silent) {
			char	temp[BUFSIZ];
			shoarg(stdout, "cd", relpath(temp, old_wd, new_wd));
		}
		if (chdir(new_wd) < 0)
			failed(new_wd);
		(void)relpath(s_file, new_wd, s_file);
	} else {
		*old_wd = *new_wd = EOS;
		(void)strcpy(s_file, archive);
	}

	/*
	 * Check to see if we think that we can extract the file
	 */
	if (isFILE(s_file,&s_mode) > 0) {
		if (isFILE(name,(int *)0)) {
			if (force) {
				if (!noop && (unlink(name) < 0))
					failed(name);
			} else {
				YELL "?? \"%s\" already exists\n", name);
				ok = FALSE;
			}
		}
	} else {
		YELL "?? \"%s\" not found\n", s_file);
		ok = FALSE;
	}

	/*
	 * Process the file if we found no errors
	 */
	if (ok) {
		if (!silent) shoarg(stdout, GET_TOOL, get_opts);
		if (!noop) {
			newzone(0,0,FALSE);	/* do this in GMT zone */
			if (execute(GET_TOOL, get_opts) < 0)
				failed(name);
		}
		PostProcess(name, s_file);
	}

	/*
	 * Restore the working-directory if we had to alter it.
	 */
	if (*old_wd != EOS) {
		if (!silent)
			shoarg(stdout, "cd", relpath(old_wd, new_wd, old_wd));
		if (chdir(old_wd) < 0)
			failed("chdir");
	}
}

static
usage ()
{
	static	char	*msg[] = {
 "Usage: getdelta [options] files"
,""
,"Options:"
,"  -b      branch (passed to \"get\")"
,"  -c DATE (a la \"get\") recognizes SCCS cutoff-date description, forces"
,"          this instead to use the last date before the cutoff, or the oldest"
,"          delta-date."
,"  -e      extract for editing (suppress keywords, passed to \"get\")"
,"  -f      force overwrite of existing file"
,"  -k      extract without keyword expansion (passed to \"get\")"
,"  -n      noop: show states we would do"
,"  -r SID  (a la \"get\") recognizes SCCS-sid description"
,"  -s      make the operation less noisy (passed to \"get\")"
	};
	register int	j;
	for (j = 0; j < sizeof(msg)/sizeof(msg[0]); j++)
		YELL "%s\n", msg[j]);
	(void)exit(FAIL);
	/*NOTREACHED*/
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
failed(s)
char	*s;
{
	perror(s);
	(void)exit(FAIL);
	/*NOTREACHED*/
}

main (argc, argv)
char	*argv[];
{
	register int	j, k;
	register char	*s;

	oldzone();
	s = get_opts;

	while ((j = getopt(argc, argv, "c:efknr:s")) != EOF) {
		switch (j) {
		/* options interpreted & pass through to "get" */
		case 'r':
			sid = optarg;
			TELL "sid: %s\n", sid);
			FORMAT(s, "-r%s ", sid);
			break;
		case 'c':
			FORMAT(s, "-c %s ", optarg);
			k = optind;
			opt_c = cutoff(argc, argv);
			while (k < optind) {
				s += strlen(s);
				FORMAT(s, "%s ", argv[k++]);
			}
			TELL "cutoff: %s\n", ctime(&opt_c));
			break;
		case 's':
			silent	= TRUE;
			FORMAT(s, "-%c ", j);
			break;
		case 'b':
		case 'e':
			lockit	= TRUE;
		case 'k':
			FORMAT(s, "-%c ", j);
			writeable = S_IWRITE;
			break;
		/* options belonging to this program only */
		case 'n':	noop	 = TRUE;	break;
		case 'f':	force	 = TRUE;	break;
		default:	usage();
		}
		s += strlen(s);
	}
	for (j = optind; j < argc; j++)
		DoFile (argv[j], s);
	(void)exit(SUCCESS);
	/*NOTREACHED*/
}
