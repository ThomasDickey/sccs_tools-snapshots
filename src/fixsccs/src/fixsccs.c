#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/sccs_tools.vcs/src/fixsccs/src/RCS/fixsccs.c,v 6.5 1994/06/07 15:05:31 dickey Exp $";
#endif

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

static	int	a_opt;		/* all-directory scan */
static	int	no_op;		/* no-op mode */
static	int	quiet;
static	int	fixed;

static
void	FixSCCS(
	_ARX(char *,	path)
	_ARX(char *,	archive)
	_AR1(STAT *,	sp)
		)
	_DCL(char *,	path)
	_DCL(char *,	archive)
	_DCL(STAT *,	sp)
{
	auto	time_t	date;
	auto	char	*vers,
			*locker;

	sccslast(path, sccs2name(archive,TRUE), &vers, &date, &locker);
	if (*vers != '?'
	 && sp->st_mtime != date) {
		if (!quiet) {
			PRINTF("%s %s\n", archive, vers);
			PRINTF("   now: %s", ctime(&(sp->st_mtime)));
			PRINTF("   set: %s", ctime(&date));
		}
		if (!no_op) {
			if (setmtime(archive, date) < 0)
				failed(archive);
		}
		fixed++;
	}
}

/*ARGSUSED*/
static
int	WALK_FUNC(scan_tree)
{
	static	char	prefix[] = SCCS_PREFIX;
	auto	char	tmp[MAXPATHLEN],
			*leaf;

	leaf = pathleaf(pathcat(tmp, path, name));

	if (sp == 0 || readable < 0) {
		readable = -1;
		perror(name);
	} else if (isDIR(sp->st_mode)) {
		if (!a_opt && !dotname(leaf) && *leaf == '.')
			readable = -1;
		else if (!quiet)
			track_wd(path);
	} else if (!strncmp(leaf, prefix, sizeof(prefix)-sizeof(char))
	    &&    isFILE(sp->st_mode)) {
		if (!quiet)
			track_wd(path);
		FixSCCS(path, tmp, sp);
	} else
		readable = -1;

	return(readable);
}

static
void	do_arg(
	_AR1(char *,	name))
	_DCL(char *,	name)
{
	(void)walktree((char *)0, name, scan_tree, "r", 0);
}

static
void	usage(_AR0)
{
	static	char	*tbl[] = {
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
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
	register int	j;

	while ((j = getopt(argc, argv, "anq")) != EOF) {
		switch (j) {
		case 'a':	a_opt = TRUE;			break;
		case 'n':	no_op = TRUE;			break;
		case 'q':	quiet = TRUE;			break;
		default:	usage();
		}
	}

	if (!quiet)
		track_wd((char *)0);
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
	/*NOTREACHED*/
}
