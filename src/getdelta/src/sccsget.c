#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/sccs_tools.vcs/src/getdelta/src/RCS/sccsget.c,v 6.0 1991/10/24 08:53:07 ste_cm Rel $";
#endif

/*
 * Title:	sccsget.c (sccs get-tree)
 * Author:	T.E.Dickey
 * Created:	23 May 1991 (from sccsget.sh and rcsget.c)
 * Modified:
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
#include	<rcsdefs.h>
#include	<sccsdefs.h>
extern	FILE	*popen();

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
checkout(
_AR1(char *,	name))
_DCL(char *,	name)
{
	auto	char	args[BUFSIZ];
	auto	char	*working = sccs2name(name,FALSE);

	catarg(strcpy(args, get_opts), working);
	if (!quiet) shoarg(stdout, verb, args);
	if (!no_op) {
		if (execute(verb, args) < 0)
			failed(working);
	}
}

static
fexists(
_AR1(char *,	name))
_DCL(char *,	name)
{
	struct	stat	sb;
	return (stat(name, &sb) >= 0) && isFILE(sb.st_mode);
}

/*ARGSUSED*/
static
WALK_FUNC(scan_tree)
{
	auto	char	tmp[BUFSIZ],
			*s = pathcat(tmp, path, name);

	if (sp == 0 || readable < 0) {
		if (!fexists(name2sccs(name,FALSE))) {
			readable = -1;
			perror(name);
			if (!force)
				exit(FAIL);
		}
		track_wd(path);
		checkout(name);
	} else if (isDIR(sp->st_mode)) {
		abspath(s);		/* get rid of "." and ".." names */
		if (!a_opt && *pathleaf(s) == '.')
			readable = -1;
		else if (sameleaf(s, sccs_dir())
		    ||	 sameleaf(s, rcs_dir()))
			readable = -1;
		else
			track_wd(path);
	} else if (isFILE(sp->st_mode)) {
		track_wd(path);
		checkout(name);
	} else
		readable = -1;

	return(readable);
}

static
do_arg(
_AR1(char *,	name))
_DCL(char *,	name)
{
	(void)walktree((char *)0, name, scan_tree, "r", 0);
}

usage(_AR0)
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

/*ARGSUSED*/
_MAIN
{
	register int	j;

	track_wd((char *)0);
	while ((j = getopt(argc, argv, "ac:efknr:s")) != EOF) {
		char	tmp[BUFSIZ];
		FORMAT(tmp, "-%c", j);
#define	OPT	strcat(tmp, optarg)
		switch (j) {
		case 'a':			 a_opt	= TRUE;	break;
		case 'c': catarg(get_opts, OPT);		break;
		case 'f':			 force	= TRUE;
		case 'e':
		case 'k': catarg(get_opts, tmp);		break;
		case 'n':			 no_op	= TRUE;	break;
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
