#ifndef	lint
static	char	Id[] = "$Id: sccsput.c,v 2.1 1990/05/08 14:20:59 dickey Exp $";
#endif	lint

/*
 * Title:	sccsput.c (sccs put-tree)
 * Author:	T.E.Dickey
 * Created:	08 May 1990 (from sccsput.sh and rcsput.c)
 * $Log: sccsput.c,v $
 * Revision 2.1  1990/05/08 14:20:59  dickey
 * RCS_BASE
 *
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
#include	"ptypes.h"
#include	"rcsdefs.h"
#include	"sccsdefs.h"
extern	FILE	*popen();
extern	char	*dftenv();
extern	char	*pathcat();
extern	char	*pathleaf();

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)
#define	VERBOSE		if (!quiet) PRINTF

static	char	diff_opts[BUFSIZ];
static	char	*verb = "putdelta";
static	FILE	*log_fp;
static	int	a_opt;		/* all-directory scan */
static	int	no_op;		/* no-op mode */
static	char	*pager;		/* nonzero if we don't cat diffs */
static	int	force;
static	int	quiet;

static
filesize(name)
char	*name;
{
	auto	struct	stat	sb;
	return (stat(name, &sb) >= 0 && (sb.st_mode & S_IFMT) == S_IFREG
		? sb.st_size : -1);
}

static
cat2fp(fp, name)
FILE	*fp;
char	*name;
{
	auto	FILE	*ifp;
	auto	char	t[BUFSIZ];
	auto	int	n;

	if (ifp = fopen(name, "r")) {
		while ((n = fread(t, sizeof(char), sizeof(t), ifp)) > 0)
			if (fwrite(t, sizeof(char), n, fp) != n)
				break;
		FCLOSE(ifp);
	}
}

static
pipe2file(cmd, name)
char	*cmd, *name;
{
	auto	FILE	*ifp, *ofp;
	auto	char	buffer[BUFSIZ];
	auto	int	empty	= TRUE,
			n;

	VERBOSE("%% %s\n", cmd);
	if (!tmpnam(name) || !(ofp = fopen(name,"w")))
		failed("tmpnam");

	if (!(ifp = popen(cmd, "r")))
		failed("popen");
	/* copy the result to a file so we can send it two places */
	while ((n = fread(buffer, sizeof(char), sizeof(buffer), ifp)) > 0) {
		if (fwrite(buffer, sizeof(char), n, ofp) != n)
			break;
		empty = FALSE;
	}
	(void)pclose(ifp);
	FCLOSE(ofp);
	return (!empty);
}

static
different(working, archive)
char	*working;
char	*archive;
{
	auto	char	buffer[BUFSIZ],
			in_diff[BUFSIZ],
			out_diff[BUFSIZ];
	auto	int	changed;

	FORMAT(buffer, "get %s -k -p %s", quiet ? "-s" : "", archive);
	(void)pipe2file(buffer, in_diff);

	FORMAT(buffer, "diff %s %s %s", diff_opts, in_diff, working);
	changed = pipe2file(buffer, out_diff);

	if (changed) {
		if (!quiet) {
			if (pager == 0)
				cat2fp(stdout, out_diff);
			else {
				if (execute(pager, out_diff) < 0)
					failed(pager);
			}
		}
		if (log_fp != 0) {
			PRINTF("appending to logfile");
			cat2fp(log_fp, out_diff);
		}
	} else {
		PRINTF("*** no differences found ***\n");
	}

	(void)unlink(in_diff);
	(void)unlink(out_diff);
	return (changed);
}

static
checkin(path,name)
char	*path;
char	*name;
{
	auto	char	args[BUFSIZ];
	auto	char	*working = sccs2name(name,FALSE);
	auto	char	*archive = name2sccs(name,FALSE);
	auto	int	first;

	if (first = (filesize(archive) < 0)) {
		if (!force && !istextfile(working)) {
			PRINTF("*** \"%s\" does not seem to be a text file\n",
				working);
			return;
		}
		first = TRUE;
	} else {
		auto	time_t	date;
		auto	char	*vers,
				*locker;
		sccslast(path, working, &vers, &date, &locker);
		if (*vers == '?')
			first = TRUE;	/* no revisions present */
		else {
			if (!force && !different(working, archive))
				return;
			first = FALSE;
		}
	}

	(void)strcpy(args, working);

	if (!no_op) {
		PRINTF("*** %s \"%s\"\n",
			first	? "Initial SCCS insertion of"
				: "Applying SCCS delta to",
			name);
		VERBOSE("%% %s %s\n", verb, args);
		if (execute(verb, args) < 0)
			failed(working);
	} else {
		PRINTF("--- %s \"%s\"\n",
			first	? "This would be initial for"
				: "Delta would be applied to",
			name);
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
		checkin(path,name);
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

usage(option)
{
	static	char	*tbl[] = {
 "Usage: sccsput [options] files_or_directories"
,""
,"Options"
,"  -a       process all directories, including those beginning with \".\""
,"  -b       (passed to sccsdiff)"
,"  -h       (passed to sccsdiff)"
,"  -c       send differences to terminal without $PAGER filtering"
,"  -l file  write all differences to logfile"
,"  -n       compute differences only, don't try to archive"
,"  -s       silent"
,""
	};
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	if (option == '?')
		(void)system("checkin -?");
	exit(FAIL);
}

main(argc, argv)
char	*argv[];
{
	register int	j;
	register char	*s;
	auto	 int	had_args = FALSE;

	track_wd((char *)0);
	pager = dftenv("more -l", "PAGER");
	/* patch: getopt? */
	for (j = 1; j < argc; j++) {
		if (*(s = argv[j]) == '-') {
			switch (s[1]) {
			case 'a':	a_opt = TRUE;		break;
			case 'f':	force = TRUE;		break;
			case 'b':
			case 'h':	catarg(diff_opts, s);	break;
			case 'c':	pager = 0;		break;
			case 'n':	no_op = TRUE;		break;
			case 'l':	if (s[2] == EOS)
						s = "logfile";
					if (!(log_fp = fopen(s, "a+")))
						usage(0);
					break;
			case 's':	quiet = TRUE;		break;
			default:	usage(s[1]);
			}
		} else {
			do_arg(s);
			had_args = TRUE;
		}
	}

	if (!had_args)
		do_arg(".");
	exit(SUCCESS);
	/*NOTREACHED*/
}
