/*
 * Title:	getdelta.c (get an sccs-delta)
 * Author:	T.E.Dickey
 * Created:	26 Mar 1986 (as a procedure)
 * Modified:
 *		06 Dec 2019, use DYN-strings to replace catarg, and provide
 *			     for sccs-wrapper.
 *		30 Apr 2002, use sccsyear() to isolate against buggy data from
 *			     SunOS 4.x
 *		21 Apr 2002, don't add 1900 to year for packdate().
 *		30 Jun 2000, if -f option is given, do not exit with error-code.
 *		27 Jun 2000, Y2K fix. 
 *		07 Apr 2000, if only a cutoff date is given, set retrieved file
 *			     modification time.
 *		07 Feb 2000, don't use variable named 'new', since gcc 2.95.2
 *			     generates incorrect code for it (sometimes).
 *		27 Jun 1999, correct uninitialized get_opts[] - worked on SunOS.
 *		14 Oct 1995, allow 14-character s-filenames
 *		08 Sep 1995, get CmVision file-mode, if present.
 *		07 Sep 1995, added processing for CmVision binary-files.
 *		16 Mar 1995, allow -r, -s options to repeat (use last).
 *		19 Jul 1994, added "-p" option.
 *		18 Jul 1994, corrected 'bump()' using 'vercmp()'.
 *		15 Jul 1994, use 'sccspath()'
 *		13 Jul 1994, Linux/gcc warnings
 *		23 Sep 1993, SunOS/gcc warnings
 *		17 Dec 1991, typo in 'catarg()' call for "-r".
 *		18 Nov 1991, use 'catarg()' for building 'get_opts[]'
 *		24 Oct 1991, converted to ANSI
 *		19 Jul 1991, corrected logic, allowing arguments of the form
 *			     "SCCS/s.file". Modified version-compare logic to
 *			     allow versions to 'bump' to higher level, rather
 *			     than simply match for equality.
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
#define	CHR_PTYPES
#define	STR_PTYPES
#define	TIM_PTYPES
#include	<ptypes.h>
#include	<dyn_str.h>
#include	<sccsdefs.h>

MODULE_ID("$Id: getdelta.c,v 6.31 2019/12/06 22:14:52 tom Exp $")

/* local definitions */
#define	NAMELEN		80	/* length of tokens in sccs-header */
#define	TELL	if (!silent) PRINTF

#ifndef	GET_TOOL
#define	GET_TOOL	"get"
#endif

#define	CTL_A	'\001'

#define	S_MODE(s) ((s & 0555) | writable)

/************************************************************************
 *	local data							*
 ************************************************************************/

static time_t opt_c = 0;
static mode_t s_mode;		/* protection of s-file */
static mode_t writable = 0;	/* nonzero to make g-file writable */

static int silent = FALSE;	/* "-s" option */
static int force = FALSE;	/* "-f" option */
static int lockit = FALSE;	/* "-e" option */
static int noop = FALSE;	/* "-n" option */
static int piped = FALSE;	/* "-p" option */

static char *sid = NULL;

/************************************************************************
 *	local procedures						*
 ************************************************************************/

static void
GiveUp(void)
{
    exit(force ? EXIT_SUCCESS : EXIT_FAILURE);
}

static int
Dots(char *s)
{
    int count = 0;
    while (*s)
	if (*s++ == '.')
	    count++;
    return (count);
}

/*
 * Compare the SCCS version string against the '-r' option's value for equality
 */
static int
same(char *version)
{
    int code = 0;
    size_t len = strlen(sid), cmp = strlen(version);

    if (len == cmp) {
	code = !strcmp(sid, version);
    } else if ((len < cmp) && version[len] == '.') {
	if ((Dots(sid) + 1) == Dots(version))
	    code = !strncmp(sid, version, len);
    }
    return (code);
}

/*
 * Compare the SCCS version string against the '-r' option's value, allowing it
 * to 'bump' up to a higher version.
 */
static int
bump(char *version)
{
    int code;

    if (sid == NULL) {		/* match first "R.L" */
	char *s;
	int dot = 0;
	for (s = version; *s; s++)
	    if (*s == '.')
		dot++;
	return (dot == 1);
    } else if (!(code = same(version))) {
	code = !vercmp(version, sid, 1);
    }
    return (code);
}

#ifdef CMV_PATH
static int file_is_binary;
static int file_is_SCCS;
static char cmv_binary[] = "\\\\\\\\";

/*
 * Inspect the file to see if it's a binary file.  If so, we must suppress
 * keyword-expansion for the current file.  We "know" that s-files that hold
 * binary data begin with 4 backslashes.  If we look for the first text line in
 * the tip-version, that's enough to find the 'cmv_binary' string.
 */
static void
CheckForBinary(char *s_file)
{
    FILE *fp;
    int state = 0;
    int delete = 0;

    file_is_binary = FALSE;
    file_is_SCCS = TRUE;
    if ((fp = fopen(s_file, "r")) != NULL) {
	char buf[BUFSIZ];
	while (fgets(buf, sizeof(buf), fp) != 0) {
	    if (state == 0) {
		if (buf[0] != CTL_A
		    || buf[1] != 'h') {
		    file_is_binary = TRUE;
		    file_is_SCCS = FALSE;
		    break;
		}
		state = 1;
	    } else if (buf[0] == CTL_A) {
		switch (buf[1]) {
		case 'E':
		    if (delete == atoi(buf + 2))
			delete = 0;
		    break;
		case 'I':
		    state = 2;
		    break;
		case 'D':
		    if (!delete)
			delete = atoi(buf + 2);
		    break;
		}
	    } else if (state == 2 && !delete) {
		if (!strncmp(buf, cmv_binary, 4))
		    file_is_binary = TRUE;
		break;
	    }
	}
    }
}

/*
 * If the current file is a CmVision binary-file, de-hexify it by converting
 * backslash sequences to characters.
 */
static void
DeHexify(char *name)
{
    FILE *ifp;
    FILE *ofp;
    Stat_t sb;
    char *temp;
    char dirname[MAXPATHLEN];
    char buffer[1024];
    int first = TRUE;
    static char hex[] = "0123456789ABCDEF";

    (void) strcpy(dirname, pathhead(name, &sb));
    temp = tempnam(dirname, "get");
    if ((ifp = fopen(name, "r")) == 0)
	failed(name);

    if ((ofp = fopen(temp, "w")) == 0)
	failed(temp);

    /* we're reading lines no longer than 256 characters */
    while (fgets(buffer, sizeof(buffer), ifp)) {
	char *s = buffer;
	if (first) {
	    if (strncmp(buffer, cmv_binary, 4)) {
		FPRINTF(stderr, "? not a binary: %s\n", name);
		fclose(ifp);
		fclose(ofp);
		remove(temp);
		return;
	    }
	    first = FALSE;
	    s += 4;
	}
	while (*s != EOS) {
	    int c;
	    if ((c = *s++) == '\\') {
		char *a = strchr(hex, *s++);
		char *b = strchr(hex, *s++);
		if (a == 0
		    || b == 0) {
		    FPRINTF(stderr, "? non-hex\n");
		    GiveUp();
		}
		c = (int) (((a - hex) << 4) + (b - hex));
		if (c == 'J' && *s == '\n')
		    break;
	    }
	    fputc(c, ofp);
	    if (ferror(ofp))
		failed(temp);
	}
    }
    fclose(ifp);
    fclose(ofp);
    if (rename(temp, name) < 0)
	failed(name);
    free(temp);
}
#endif

static char *
LeafOf(char *name)
{
    char *leaf = fleaf(name);
    if (leaf == 0)
	leaf = name;
    return leaf;
}

/*
 * Process a single file by finding the check-in date and using that to
 * timestamp the file.
 */
static void
PostProcess(char *name, char *s_file)
{
    FILE *fp;
    time_t date = 0;
    int got = FALSE, year = 0, mon = 0, mday = 0, hour = 0, min = 0, sec = 0;
    char version[NAMELEN];
    char bfr[BUFSIZ];
    char bfr2[BUFSIZ];
    char *s;

    if ((fp = fopen(s_file, "r")) != NULL) {
	newzone(5, 0, FALSE);	/* interpret in EST/EDT zone */
	while ((s = fgets(bfr, sizeof(bfr), fp)) != 0
	       && (*s++ == CTL_A)) {
	    if (sscanf(s, "d D %s %[^/]/%d/%d %d:%d:%d ",
		       version,
		       bfr2, &mon, &mday,
		       &hour, &min, &sec) == 7
		&& ((year = sccsyear(bfr2)) > 0)) {
		date = packdate(year, mon, mday, hour, min, sec);
#ifdef CMV_PATH			/* for CmVision */
		if ((s = fgets(bfr, sizeof(bfr), fp)) != 0
		    && (*s++ == CTL_A)
		    && !strncmp(s, "c ", 2)
		    && (s = strstr(s, "\\\001O")) != 0) {
		    char *base = s;
		    unsigned long num;

		    if ((s = strstr(base, ":P")) != 0
			&& sscanf(s, ":P%lo:", &num))
			s_mode = (mode_t) S_MODE(num);
		    if ((s = strstr(base, ":M")) != 0
			&& sscanf(s, ":M%lu:", &num))
			date = (time_t) num;
		}
#endif
		if (opt_c) {
		    if (date > opt_c)
			continue;
		    else if ((got = !sid) != 0)
			break;
		}
		if ((got = bump(version)) != 0) {
		    break;
		}
	    }
	}
	(void) fclose(fp);
	oldzone();		/* restore caller's time zone */
	if (got || (opt_c && date && (date < opt_c))) {
	    FPRINTF(stderr, "** %s %s: %s", name, version, ctime(&date));
	    got = TRUE;
	} else {
	    if (sid) {
		TELL("** no match for sid=%s\n", sid);
	    } else if (opt_c) {
		TELL("** no deltas before cutoff\n");
		if (date)
		    TELL("** oldest date was %s", ctime(&date));
	    }
	}
    } else
	TELL("** could not open \"%s\"\n", s_file);

    if (!noop && got) {
#if defined(S_FILES_14)
	/*
	 * If we extracted a shortened (14-char) name, try to rename it
	 * to the longer name supplied by the user.
	 */
	s = LeafOf(s_file) + 2;
	if (strcmp(s, LeafOf(name))) {
	    char temp[MAXPATHLEN];
	    (void) strcpy(LeafOf(strcpy(temp, name)), s);
	    (void) rename(temp, name);
	}
#endif /* S_FILES_14 */
#ifdef CMV_PATH
	if (file_is_binary)
	    DeHexify(name);
#endif
	(void) chmod(name, s_mode);
	if (setmtime(name, date, (time_t) 0) < 0)
	    FPRINTF(stderr, "%s: cannot set time\n", name);
	else if (lockit && !geteuid()) {
	    char p_file[MAXPATHLEN];
	    *LeafOf(strcpy(p_file, s_file)) = 'p';
	    if (chown(p_file, getuid(), getgid()) < 0)
		failed("chown");
	}
    }
}

/*
 * See if the specified file exists.  If so, verify that it is indeed a file.
 */
static int
is_a_file(char *name, mode_t *mode_)
{
    Stat_t sb;

    if (stat(name, &sb) >= 0) {
	if ((sb.st_mode & S_IFMT) == S_IFREG) {
	    if (mode_)
		*mode_ = S_MODE(sb.st_mode);
	    return (TRUE);
	} else {
	    FPRINTF(stderr, "?? \"%s\" is not a file\n", name);
	    GiveUp();
	    /*NOTREACHED */
	}
    }
    return (FALSE);
}

/*
 * See if we have permission to write in the directory given by 'name'
 */
static int
Permitted(char *name, int read_only)
{
    char path[MAXPATHLEN];
    int mode = X_OK | R_OK;
    char *mark = fleaf_delim(pathcat(path, ".", name));

    if (mark != 0)
	*mark = EOS;
    if (!read_only)
	mode |= W_OK;

    if (access(path, mode) < 0)
	failed(path);
    return TRUE;
}

/*
 * Process a single file.  If we are given the name of a non-sccs file, compute
 * the name of the corresponding sccs file.  Otherwise, compute the name of the
 * file to be checked-out from the sccs file name.
 */
static void
DoFile(char *name, ARGV * options)
{
    int ok = TRUE;
    char *working = sccs2name(name, FALSE);
    char *archive = name2sccs(name, FALSE);
    char old_wd[MAXPATHLEN];
    char new_wd[MAXPATHLEN];
    char s_file[MAXPATHLEN];
    char buffer[MAXPATHLEN];
    char *s;
    ARGV *args;

    if ((!piped && !Permitted(working, FALSE))
	|| !Permitted(archive, !lockit))
	return;

    args = argv_init();
    argv_merge(&args, options);

    /*
     * SCCS 'get' extracts only into the current directory.  Perform a
     * 'chdir()' to accommodate this if necessary.
     */
    (void) strcpy(buffer, working);
    if ((s = fleaf_delim(buffer)) != NULL) {
	if (!getwd(old_wd))
	    failed("getwd");
	*s = EOS;
	name = ++s;
	abspath(pathcat(new_wd, old_wd, buffer));
	abspath(strcpy(s_file, name2sccs(name, FALSE)));
	if (!silent) {
	    char temp[MAXPATHLEN];
	    shoarg(stdout, "cd", relpath(temp, old_wd, new_wd));
	}
	if (chdir(new_wd) < 0)
	    failed(new_wd);
	(void) relpath(s_file, new_wd, s_file);
    } else {
	name = working;
	*old_wd = *new_wd = EOS;
	(void) strcpy(s_file, archive);
    }

    /*
     * Check for the file first by its given name, then with a 14-character
     * limit, to see if we'll be able to extract it.
     */
    if (is_a_file(s_file, &s_mode)
#if defined(S_FILES_14)
	|| (fleaf14(s_file) && is_a_file(s_file, &s_mode))
#endif
	) {
#ifdef CMV_PATH
	CheckForBinary(s_file);
	/*
	 * If it doesn't look like an s-file, it's probably a binary
	 * file. In that case, the date and mode information is stored
	 * in one of the r-files, and it's a lot more work to get it.
	 */
	if (!file_is_SCCS) {
	    FPRINTF(stderr, "? file is not stored in SCCS-form: %s\n", name);
	    GiveUp();
	}
	if (file_is_binary && !writable)
	    argv_append(&args, "-k");
#endif
	if (is_a_file(name, (mode_t *) 0)) {
	    if (piped) {
		;
	    } else if (force) {
		if (!noop && (unlink(name) < 0))
		    failed(name);
	    } else {
		FPRINTF(stderr, "?? \"%s\" already exists\n", name);
		ok = FALSE;
	    }
	}
    } else {
	FPRINTF(stderr, "?? \"%s\" not found\n", s_file);
	ok = FALSE;
    }

    /*
     * Process the file if we found no errors
     */
    if (ok) {
	ARGV *get = sccs_argv(GET_TOOL);
	argv_merge(&get, args);
	argv_append(&get, s_file);
	if (!silent)
	    show_argv(stdout, argv_values(get));
	if (!noop) {
	    newzone(0, 0, FALSE);	/* do this in GMT zone */
	    if (executev(argv_values(get)) < 0)
		failed(name);
	}
	if (!piped)
	    PostProcess(name, s_file);
	argv_free(&get);
    }
    argv_free(&args);

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

static void
usage(void)
{
    static const char *msg[] =
    {
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
	,"  -p      pipe the file to standard output (passed to \"get\")"
	,"  -r SID  (a la \"get\") recognizes SCCS-sid description"
	,"  -s      make the operation less noisy (passed to \"get\")"
    };
    unsigned j;
    for (j = 0; j < sizeof(msg) / sizeof(msg[0]); j++)
	FPRINTF(stderr, "%s\n", msg[j]);
    (void) exit(EXIT_FAILURE);
    /*NOTREACHED */
}

/************************************************************************
 *	public entrypoints						*
 ************************************************************************/
/*ARGSUSED*/
_MAIN
{
    char temp[BUFSIZ];
    char r_opt[BUFSIZ];
    int j, k;
    ARGV *get_opts = argv_init();

    oldzone();

    *r_opt = EOS;
    while ((j = getopt(argc, argv, "bc:efknpr:s")) != EOF) {
	switch (j) {
	    /* options interpreted & pass through to "get" */
	case 'r':
	    sid = stralloc(optarg);
	    if ((k = (int) strlen(sid) - 1) > 0)
		if (sid[k] == '.')
		    sid[k] = EOS;
	    TELL("sid: %s\n", sid);
	    FORMAT(r_opt, "-r%s", sid);
	    break;

	case 'c':
	    FORMAT(temp, "-c%s", optarg);
	    k = optind;
	    opt_c = cutoff(argc, argv);
	    while (k < optind)
		(void) strcat(strcat(temp, " "), argv[k++]);
	    argv_append(&get_opts, temp);
	    TELL("cutoff: %s\n", ctime(&opt_c));
	    break;

	case 'p':
	    piped = TRUE;
	    argv_append(&get_opts, "-p");
	    break;

	case 's':
	    silent = TRUE;
	    break;

	case 'b':
	case 'e':
	    lockit = TRUE;
	case 'k':
	    FORMAT(temp, "-%c", j);
	    argv_append(&get_opts, temp);
	    writable = S_IWRITE;
	    break;

	    /* options belonging to this program only */
	case 'n':
	    noop = TRUE;
	    break;
	case 'f':
	    force = TRUE;
	    break;
	default:
	    usage();
	}
    }

    /* Use 'get' options once-only, since it complains otherwise */
    if (*r_opt != EOS)
	argv_append(&get_opts, r_opt);
    if (silent)
	argv_append(&get_opts, "-s");

    if (optind < argc) {
	for (j = optind; j < argc; j++)
	    DoFile(argv[j], get_opts);
    } else
	usage();
    (void) exit(EXIT_SUCCESS);
    /*NOTREACHED */
}
