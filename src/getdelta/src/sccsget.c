/*
 * Title:	sccsget.c (sccs get-tree)
 * Author:	T.E.Dickey
 * Created:	23 May 1991 (from sccsget.sh and rcsget.c)
 * Modified:
 *		06 Dec 2019, use DYN-strings to replace catarg
 *		19 Jul 1994, added dir-only logic, made this work with the
 *			     SCCS_VAULT config, and defined repeated "-n"
 *			     option for debugging.  Use 'sccs_debug()'.
 *		23 Sep 1993, gcc warnings
 *		24 Oct 1991, converted to ANSI
 *		20 Jun 1991, pass-thru "-e" option, use 'shoarg()'
 *		
 * Function:	Use SCCS-get to checkout one or more files from the sccs-
 *		directory which is located in the current working directory.
 *		Then, use 'sccsdate' to set the modification date of the
 *		checked-out files according to the last delta date (rather than
 *		the current date, as SCCS assumes).
 *
 *		Options are designed to feed-thru to 'get(1)'.
 *
 * Environment:
 *		SCCS_DIR- name of sccs directory
 *
 * Options:	see 'usage()'
 */

#define	STR_PTYPES
#include	<ptypes.h>
#include	<dyn_str.h>
#include	<rcsdefs.h>
#include	<sccsdefs.h>

MODULE_ID("$Id: sccsget.c,v 6.13 2021/01/10 19:52:14 tom Exp $")

static ARGV *get_opts;
static const char *verb = "getdelta";
static int a_opt;		/* all-directory scan */
static int no_op;		/* no-op mode */
static int force;
static int debug;
static int dir_only;		/* expand names via archive-directories */
static const char *working_path;	/* used to resolve SCCS working/archive dirs */

static void
checkout(const char *name)
{
    ARGV *args;
    char *working = sccs2name(name, FALSE);

    if (!strncmp("p.", pathleaf(name), 2)) {
	return;
    }
    args = argv_init1(verb);
    argv_merge(&args, get_opts);
    if (no_op > 1)
	argv_append(&args, "-n");
    argv_append(&args, working);
    if (debug)
	show_argv(stdout, argv_values(args));
    if (no_op != 1) {
	if (executev(argv_values(args)) < 0)
	    failed(working);
    }
    argv_free(&args);
}

static void
SetWd(const char *path)
{
    if (debug)
	shoarg(stdout, "chdir", path);
    if (no_op != 1) {
	if (chdir(path) < 0)
	    failed(path);
    }
}

static int
fexists(const char *name)
{
    Stat_t sb;
    return (stat_file(name, &sb) >= 0);
}

/*ARGSUSED*/
static int
WALK_FUNC(do_archive)
{
    const char *archive = name2sccs(name, FALSE);

    (void) sp;
    (void) level;

    if (fexists(archive)) {
	if (debug)
	    track_wd(path);
	if (strcmp(path, working_path)) {
	    if (debug)
		PRINTF("\n");
	    SetWd(working_path);
	}
	checkout(name);
	if (strcmp(path, working_path))
	    SetWd(path);
    }

    return (readable);
}

/*ARGSUSED*/
static int
WALK_FUNC(scan_tree)
{
    char tmp1[MAXPATHLEN];
    char vault[MAXPATHLEN];
    char *s = pathcat(tmp1, path, name);
    Stat_t sb;

    (void) level;

    working_path = path;
    if (sp == 0 || readable < 0) {
	if (!dir_only) {
	    if (!fexists(name2sccs(name, FALSE))) {
		readable = -1;
		perror(name);
		if (!force)
		    exit(FAIL);
	    }
	    if (debug)
		track_wd(path);
	    checkout(name);
	}
    } else if (isDIR(sp->st_mode)) {
	abspath(s);		/* get rid of "." and ".." names */
	(void) strcpy(vault, sccs_dir(path, "."));
	if (!a_opt && *pathleaf(s) == '.') {
	    readable = -1;
	} else if (sameleaf(s, sccs_dir((char *) 0, (char *) 0))) {
	    /*
	     * patch: this assumes that even SCCS_VAULT will have
	     * leafnames that can be identified.
	     */
	    readable = -1;	/* require special scanning */
	    if (dir_only && (stat_dir(vault, &sb) >= 0)) {
		if (debug)
		    track_wd(vault);
		(void) walktree(path, vault, do_archive, "r", 0);
	    }
	} else if (sameleaf(s, rcs_dir(NULL, NULL))) {
	    readable = -1;
	}
	if (readable >= 0) {
	    if (debug)
		track_wd(path);
	}
    } else if (isFILE(sp->st_mode)) {
	if (!dir_only) {
	    if (debug)
		track_wd(path);
	    checkout(name);
	}
    } else {
	readable = -1;
    }

    return (readable);
}

static void
do_arg(const char *name)
{
    Stat_t sb;
    dir_only = (stat_dir(name, &sb) >= 0)
	|| sameleaf(sccs_dir((char *) 0, (char *) 0), name);
    (void) walktree((char *) 0, name, scan_tree, "r", 0);
}

static void
usage(void)
{
    static const char *tbl[] =
    {
	"Usage: sccsget [options] files_or_directories"
	,""
	,"Options"
	,"  -a       process all directories, including those beginning with \".\""
	,"  -n       traverse tree only, don't really extract (repeat to see lower"
	,"           level trace from getdelta)"
	,""
	,"Other options are passed to getdelta:"
	,"  -c DATE  specify cutoff date"
	,"  -e       extract for editing (suppress keywords, passed to \"get\")"
	,"  -k       suppress keyword expansion"
	,"  -r SID   specify revision"
    };
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    FFLUSH(stderr);		/* for CLIX */
    exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
    int j;

    debug = sccs_debug();
    if (debug)
	track_wd((char *) 0);

    get_opts = argv_init();
    while ((j = getopt(argc, argv, "ac:efknr:s")) != EOF) {
	char tmp[BUFSIZ];
	FORMAT(tmp, "-%c", j);
#define	OPT	strcat(tmp, optarg)
	switch (j) {
	case 'a':
	    a_opt = TRUE;
	    break;
	case 'c':
	    argv_append(&get_opts, OPT);
	    break;
	case 'f':
	    force = TRUE;
	    /*FALLTHRU */
	case 'e':
	    /*FALLTHRU */
	case 'k':
	    argv_append(&get_opts, tmp);
	    break;
	case 'n':
	    no_op++;
	    break;
	case 'r':
	    argv_append(&get_opts, OPT);
	    break;
	default:
	    usage();
	}
    }
    if (!debug)
	argv_append(&get_opts, "-s");

    if (optind < argc) {
	for (j = optind; j < argc; j++)
	    do_arg(argv[j]);
    } else {
	do_arg(".");
    }

    exit(SUCCESS);
    /*NOTREACHED */
}
