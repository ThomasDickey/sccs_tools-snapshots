#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/sccs_tools.vcs/src/getdelta/src/RCS/sccsget.c,v 3.0 1991/05/24 08:13:12 ste_cm Rel $";
#endif

/*
 * Title:	sccsget.c (sccs get-tree)
 * Author:	T.E.Dickey
 * Created:	23 May 1991 (from sccsget.sh and rcsget.c)
 * $Log: sccsget.c,v $
 * Revision 3.0  1991/05/24 08:13:12  ste_cm
 * BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
 *
 *		Revision 2.2  91/05/24  08:13:12  dickey
 *		lint (apollo sr10.3)
 *		
 *		Revision 2.1  91/05/23  13:11:49  dickey
 *		RCS_BASE
 *		
 *		
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
#include	"ptypes.h"
#include	"rcsdefs.h"
#include	"sccsdefs.h"
extern	FILE	*popen();
extern	char	*pathcat();
extern	char	*pathleaf();

extern	char	*optarg;
extern	int	optind;

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)
#define	VERBOSE		if (!quiet) PRINTF

static	char	get_opts[BUFSIZ];
static	char	*verb = "getdelta";
static	int	a_opt;		/* all-directory scan */
static	int	no_op;		/* no-op mode */
static	int	force;
static	int	quiet;

static
checkout(name)
char	*name;
{
	auto	char	args[BUFSIZ];
	auto	char	*working = sccs2name(name,FALSE);

	(void)strcat(strcpy(args, get_opts), working);

	VERBOSE("%% %s %s\n", verb, args);
	if (!no_op) {
		if (execute(verb, args) < 0)
			failed(working);
	}
}

/*ARGSUSED*/
static
scan_tree(path, name, sp, ok_acc, level)
char	*path;
char	*name;
struct	stat	*sp;
{
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);

	if (sp == 0 || ok_acc < 0) {
		ok_acc = -1;
		perror(name);
		if (!force)
			exit(FAIL);
	} else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt && *pathleaf(s) == '.')
			ok_acc = -1;
		else if (sameleaf(s, sccs_dir())
		    ||	 sameleaf(s, rcs_dir()))
			ok_acc = -1;
		else
			track_wd(path);
	} else if (isFILE(sp->st_mode)) {
		track_wd(path);
		checkout(name);
	} else
		ok_acc = -1;

	return(ok_acc);
}

static
do_arg(name)
char	*name;
{
	(void)walktree((char *)0, name, scan_tree, "r", 0);
}

usage()
{
	static	char	*tbl[] = {
 "Usage: sccsput [options] files_or_directories"
,""
,"Options"
,"  -a       process all directories, including those beginning with \".\""
,"  -n       traverse tree only, don't really extract"
,""
,"Other options are passed to getdelta"
	};
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	exit(FAIL);
}

main(argc, argv)
char	*argv[];
{
	register int	j;

	track_wd((char *)0);
	while ((j = getopt(argc, argv, "ac:nfkr:s")) != EOF) {
		char	tmp[BUFSIZ];
		FORMAT(tmp, "-%c", j);
#define	OPT	strcat(tmp, optarg)
		switch (j) {
		case 'a':			 a_opt	= TRUE;	break;
		case 'c': catarg(get_opts, OPT);		break;
		case 'k': catarg(get_opts, tmp);		break;
		case 'n':			 no_op	= TRUE;	break;
		case 'f': catarg(get_opts, tmp); force	= TRUE;	break;
		case 'r': catarg(get_opts, OPT);		break;
		case 's': catarg(get_opts, tmp); quiet	= TRUE;	break;
		default:  usage();
		}
	}

	if (optind < argc) {
		for (j = optind; j < argc; j++)
			do_arg(argv[j]);
	} else
		do_arg(".");

	exit(SUCCESS);
	/*NOTREACHED*/
}
