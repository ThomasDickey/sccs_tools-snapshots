/*
 * Title:	fixsccs.c (sccs put-tree)
 * Author:	T.E.Dickey
 * Created:	07 Jun 1994
 * Modified:
 *
 * Function:
 *		Touchs sccs files so that their modification times match the
 *		checkin-time recorded for the tip version.  This is used to
 *		accommodate SunOS's make facility, which has a get-from-sccs
 * 		rule. Normally, 'putdelta' does this; 'fixsccs' is used to
 *		handle the cases when someone bypasses 'putdelta' and uses
 *		'admin' or 'delta' directly.
 *
 * Options:	see 'usage()'
 */

#define	STR_PTYPES
#include	<ptypes.h>
#include	<sccsdefs.h>
#include	<time.h>	/* declares 'ctime()' */

MODULE_ID("$Id: fixsccs.c,v 6.14 2025/01/07 01:03:39 tom Exp $")

static int a_opt;		/* all-directory scan */
static int no_op;		/* no-op mode */
static int quiet;
static int fixed;

static void
FixSCCS(const char *path, const char *archive, Stat_t * sp)
{
    time_t date;
    const char *vers;
    const char *locker;

    sccslast(path, sccs2name(archive, TRUE), &vers, &date, &locker);
    if (*vers != '?'
	&& sp->st_mtime != date) {
	if (!quiet) {
	    PRINTF("%s %s\n", archive, vers);
	    PRINTF("   now: %s", ctime(&(sp->st_mtime)));
	    PRINTF("   set: %s", ctime(&date));
	}
	if (!no_op) {
	    if (setmtime(archive, date, (time_t) 0) < 0)
		failed(archive);
	}
	fixed++;
    }
}

/*ARGSUSED*/
static int
WALK_FUNC(scan_tree)
{
    static char prefix[] = SCCS_PREFIX;
    char tmp[MAXPATHLEN];
    char *leaf;

    (void) level;

    leaf = pathleaf(pathcat(tmp, path, name));

    if (sp == NULL || readable < 0) {
	readable = -1;
	perror(name);
    } else if (isDIR(sp->st_mode)) {
	if (!a_opt && !dotname(leaf) && *leaf == '.')
	    readable = -1;
	else if (!quiet)
	    track_wd(path);
    } else if (!strncmp(leaf, prefix, sizeof(prefix) - sizeof(char))
	       && isFILE(sp->st_mode)) {
	if (!quiet)
	    track_wd(path);
	FixSCCS(path, tmp, sp);
    } else
	readable = -1;

    return (readable);
}

static void
do_arg(const char *name)
{
    (void) walktree((char *) 0, name, scan_tree, "r", 0);
}

static
void
usage(void)
{
    static const char *tbl[] =
    {
	"Usage: fixsccs [options] files_or_directories"
	,""
	,"Sets SCCS archive file times to match the checkin time of the tip version."
	,""
	,"Options"
	,"  -a       process all directories, including those beginning with \".\""
	,"  -n       scan archives only, don't try to set mod-times"
	,"  -q       quiet"
	,""
    };
    unsigned j;
    for (j = 0; j < sizeof(tbl) / sizeof(tbl[0]); j++)
	FPRINTF(stderr, "%s\n", tbl[j]);
    exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
    register int j;

    while ((j = getopt(argc, argv, "anq")) != EOF) {
	switch (j) {
	case 'a':
	    a_opt = TRUE;
	    break;
	case 'n':
	    no_op = TRUE;
	    break;
	case 'q':
	    quiet = TRUE;
	    break;
	default:
	    usage();
	}
    }

    if (!quiet)
	track_wd((char *) 0);
    if (optind < argc) {
	while (optind < argc)
	    do_arg(argv[optind++]);
    } else
	do_arg(".");

    if (fixed)
	PRINTF("%d files %sfixed\n", fixed, no_op ? "would be " : "");
    else
	PRINTF("No files %sfixed\n", no_op ? "would be " : "");
    exit(SUCCESS);
    /*NOTREACHED */
}
