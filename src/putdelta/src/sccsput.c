/*
 * Title:	sccsput.c (sccs put-tree)
 * Author:	T.E.Dickey
 * Created:	08 May 1990 (from sccsput.sh and rcsput.c)
 * Modified:
 *		06 Dec 2019, use DYN-strings to replace catarg.
 *		14 Oct 1995, allow archive leafname to be limited to 14-chars
 *		27 May 1994, added "-e" and "-C" options.  Made verbosity
 *			more consistent with CM_TOOLS.
 *		23 Sep 1993, gcc warnings
 *		24 Oct 1991, converted to ANSI
 *		13 Sep 1991, use common 'filesize()'
 *		24 Jul 1991, corrected size of 'comment[]'
 *		22 Jul 1991, cleanup use of 'catarg()'
 *		19 Jul 1991, accept "-r" option (for diff and putdelta)
 *		18 Jul 1991, renamed "-f" option to "-F", added new "-f" to
 *			pass-down to 'putdelta'.  Also, pass-thru "-k" to
 *			'putdelta'.
 *		25 Jun 1991, added "-D", "-T" options.  Automatically unlink
 *			log-file if it is empty (may retain prior file if one
 *			exists).
 *		20 Jun 1991, use 'shoarg()'.
 *		07 Jun 1991, use 'getopt()'; cleanup option processing,
 *			including those that we pass-thru to 'putdelta'
 *			corrected pathname of "-l" value
 *		
 * Function:	Use 'putdelta' to archive one or more files from the
 *		SCCS-directory which is located in the current working
 *		directory, and then, to set the delta date of the  checked-in
 *		files according to the last modification date (rather than the
 *		current date, as SCCS assumes).
 *
 * Options:	see 'usage()'
 */

#define	STR_PTYPES
#include	<ptypes.h>
#include	<dyn_str.h>
#include	<rcsdefs.h>
#include	<sccsdefs.h>

MODULE_ID("$Id: sccsput.c,v 6.14 2019/12/06 22:29:50 tom Exp $")

#define	DEBUG		if (debug) PRINTF
#define	VERBOSE		if (!quiet) PRINTF

static ARGV *diff_opts;
static const char *verb = "putdelta";
static char comment[BUFSIZ];
static FILE *log_fp;
static int a_opt;		/* all-directory scan */
static int no_op;		/* no-op mode */
static const char *pager;	/* nonzero if we don't cat diffs */
static int Force, force;
static int e_opt;
static const char *k_opt = "";
static const char *r_opt = "";
static char *LogName = NULL;
static int debug;
static int quiet;
static int found_diffs;		/* true iff we keep logfile */

static void
ExitProgram(int code)
{
    if (log_fp != 0) {
	FCLOSE(log_fp);
	if (!found_diffs)
	    (void) unlink(LogName);
    }
    exit(code);
}

static void
cat2fp(FILE *fp, const char *name)
{
    FILE *ifp;
    char t[BUFSIZ];
    size_t n;

    if ((ifp = fopen(name, "r")) != NULL) {
	while ((n = fread(t, sizeof(char), sizeof(t), ifp)) > 0)
	    if (fwrite(t, sizeof(char), n, fp) != n)
		  break;
	FCLOSE(ifp);
    }
}

static int
pipe2file(DYN * cmd, char *name)
{
    FILE *ifp, *ofp;
    char buffer[BUFSIZ];
    int empty = TRUE;
    size_t n;

    if (!tmpnam(name))
	failed("tmpnam");
    if (!(ofp = fopen(name, "w")))
	failed("tmpnam-open");
    if (debug) {
	FORMAT(buffer, "%s > ", dyn_string(cmd));
	shoarg(stdout, buffer, name);
    }
    if (!(ifp = popen(dyn_string(cmd), "r")))
	failed("popen");
    /* copy the result to a file so we can send it two places */
    while ((n = fread(buffer, sizeof(char), sizeof(buffer), ifp)) > 0) {
	if (fwrite(buffer, sizeof(char), n, ofp) != n)
	      break;
	empty = FALSE;
    }
    (void) pclose(ifp);
    FCLOSE(ofp);
    return (!empty);
}

static int
different(const char *working, const char *archive)
{
    static char format[] = "------- %s -------\n";
    ARGV *args;
    DYN *str;
    char in_diff[MAXPATHLEN];
    char out_diff[MAXPATHLEN];
    int changed;

    args = sccs_argv("get");
    argv_append(&args, "-p");
    if (!debug)
	argv_append(&args, "-s");
    if (*k_opt)
	argv_append(&args, k_opt);
    if (*r_opt)
	argv_append(&args, r_opt);
    argv_append(&args, archive);
    str = argv_flatten(args);
    (void) pipe2file(str, in_diff);
    argv_free(&args);
    dyn_free(str);

    args = argv_init1("diff");
    argv_merge(&args, diff_opts);
    argv_append(&args, in_diff);
    argv_append(&args, working);
    str = argv_flatten(args);
    changed = pipe2file(str, out_diff);
    argv_free(&args);
    dyn_free(str);

    if (changed) {
	if (!quiet) {
	    if (pager == 0) {
		PRINTF(format, working);
		cat2fp(stdout, out_diff);
	    } else {
		if (execute(pager, out_diff) < 0)
		    failed(pager);
	    }
	}
	if (log_fp != 0) {
	    VERBOSE("... appending to logfile");
	    FPRINTF(log_fp, format, working);
	    cat2fp(log_fp, out_diff);
	    if (!found_diffs && filesize(out_diff) > 0)
		found_diffs = TRUE;
	}
    } else {
	VERBOSE("*** no differences found ***\n");
    }

    (void) unlink(in_diff);
    (void) unlink(out_diff);
    if (e_opt && !changed)
	ExitProgram(FAIL);
    return (changed);
}

static int
ok_file(const char *name)
{
    if (!Force && !istextfile(name)) {
	PRINTF("*** \"%s\" does not seem to be a text file\n", name);
	return FALSE;
    }
    return TRUE;
}

static void
SccsPut(const char *path, const char *name)
{
    ARGV *args;
    const char *working = sccs2name(name, FALSE);
    const char *archive = name2sccs(name, FALSE);
    int first;
#if defined(S_FILES_14)
    char temp[MAXPATHLEN];
#endif

    /*
     * Start by assuming that if we find a non-empty file with the correct
     * name for an archive, that this isn't the first delta.
     */
    if (filesize(archive) >= 0) {
	first = FALSE;
    }
#if defined(S_FILES_14)
    else if (fleaf14(strcpy(temp, archive))
	     && filesize(temp) >= 0) {
	archive = temp;
	first = FALSE;
    }
#endif
    else {
	first = TRUE;
    }

    /*
     * For the initial insertion, verify that the file looks like text.
     * Otherwise we'll catch garbage files when differencing them.
     *
     * If we've found something that looks like an archive, check if it's
     * got at least one version.
     */
    if (first) {
	if (!ok_file(working))
	    return;
    } else {
	time_t date;
	const char *vers, *locker;
	sccslast(path, working, &vers, &date, &locker);
	if (*vers == '?') {
	    first = TRUE;	/* no revisions present */
	    if (!ok_file(working))
		return;
	} else {
	    if (!Force && !different(working, archive))
		return;
	}
    }

    args = argv_init1(verb);
    if (!debug)
	argv_append(&args, "-s");
    if (force)
	argv_append(&args, "-f");
    if (*k_opt)
	argv_append(&args, k_opt);
    if (*r_opt)
	argv_append(&args, r_opt);
    if (*comment)
	argv_append(&args, comment);
    argv_append(&args, working);

    if (!no_op) {
	VERBOSE("*** %s \"%s\"\n",
		first ? "Initial SCCS insertion of"
		: "Applying SCCS delta to",
		name);
	if (debug)
	    show_argv(stdout, argv_values(args));
	if (executev(argv_values(args)) < 0)
	    failed(working);
    } else {
	VERBOSE("--- %s \"%s\"\n",
		first ? "This would be initial for"
		: "Delta would be applied to",
		name);
    }
    argv_free(&args);
}

/*ARGSUSED*/
static int
WALK_FUNC(scan_tree)
{
    char tmp[MAXPATHLEN];
    char *s = pathcat(tmp, path, name);

    (void) level;

    if (sp == 0 || readable < 0) {
	readable = -1;
	perror(name);
	if (!Force)
	    exit(FAIL);
    } else if (isDIR(sp->st_mode)) {
	abspath(s);		/* get rid of "." and ".." names */
	if (!a_opt && *pathleaf(s) == '.')
	    readable = -1;
	else if (sameleaf(s, sccs_dir((char *) 0, (char *) 0))
		 || sameleaf(s, rcs_dir(NULL, NULL)))
	    readable = -1;
	else if (debug)
	    track_wd(path);
    } else if (isFILE(sp->st_mode)) {
	if (debug)
	    track_wd(path);
	SccsPut(path, name);
    } else
	readable = -1;

    return (readable);
}

static void
do_arg(const char *name)
{
    (void) walktree((char *) 0, name, scan_tree, "r", 0);
}

static void
usage(int option)
{
    static const char *tbl[] =
    {
	"Usage: sccsput [options] files_or_directories"
	,""
	,"Options"
	,"  -a       process all directories, including those beginning with \".\""
	,"  -b       (passed to \"diff\")"
	,"  -C       (passed to \"diff\")"
	,"  -c       send differences to terminal without $PAGER filtering"
	,"  -e       error if no differences"
	,"  -f       force (passed to putdelta)"
	,"  -h       (passed to \"diff\")"
	,"  -k       suppress keyword expansion from archive before differences"
	,"           (also passed to putdelta; implies file has unexpanded keywords)"
	,"  -l file  write all differences to log-file"
	,"  -n       compute differences only, don't try to archive"
	,"  -r SID   specify SCCS-sid (version)"
	,"  -s       silent (passed to putdelta)"
	,"  -y text  comment (passed to putdelta)"
	,"  -D opts  special options to pass to \"diff\""
	,"  -F       force (initial) insertion even if file appears to be non-text"
	,"  -T tool  specify a checkin-tool other than \"putdelta\""
	,""
    };
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    if (option == '?') {
	ARGV *args = argv_init1("putdelta");
	argv_append(&args, "-?");
	executev(argv_values(args));
    }
    exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
    char deferred[BUFSIZ];
#define	DEFERRED	strcat(strcpy(deferred, "-"), optarg)

    int j;

    revert("not a setuid-program");	/* ensure this is not misused */
    pager = dftenv("more -l", "PAGER");
    debug = sccs_debug();
    while ((j = getopt(argc, argv, "abCcefhkl:nr:sy:D:FT:")) != EOF) {
	switch (j) {
	case 'a':
	    a_opt = TRUE;
	    break;
	case 'b':
	    argv_append(&diff_opts, "-b");
	    break;
	case 'C':
	    argv_append(&diff_opts, "-c");
	    break;
	case 'c':
	    pager = 0;
	    break;
	case 'e':
	    e_opt = TRUE;
	    break;
	case 'f':
	    force = TRUE;
	    break;
	case 'h':
	    argv_append(&diff_opts, "-h");
	    break;
	case 'k':
	    k_opt = "-k";
	    break;
	case 'l':
	    found_diffs = filesize(LogName = optarg) > 0;
	    if (!(log_fp = fopen(optarg, "a+")))
		usage(0);
	    break;
	case 'n':
	    no_op = TRUE;
	    break;
	case 'r':
	    FORMAT(deferred, "-r%s", optarg);
	    r_opt = stralloc(deferred);
	    break;
	case 's':
	    quiet = TRUE;
	    break;
	case 'y':
	    FORMAT(comment, "-y%.*s",
		   (int) (sizeof(comment) - 3), optarg);
	    break;
	case 'D':
	    argv_append(&diff_opts, DEFERRED);
	    break;
	case 'F':
	    Force = TRUE;
	    break;
	case 'T':
	    verb = optarg;
	    break;
	default:
	    usage(j);
	}
    }

    if (debug)
	track_wd((char *) 0);
    if (optind < argc) {
	while (optind < argc)
	    do_arg(argv[optind++]);
    } else {
	do_arg(".");
    }

    ExitProgram(SUCCESS);
    /*NOTREACHED */
}
