#ifndef	lint
static	char	Id[] = "$Header: /users/source/archives/sccs_tools.vcs/src/putdelta/src/RCS/sccsput.c,v 6.0 1991/10/24 09:11:44 ste_cm Rel $";
#endif

/*
 * Title:	sccsput.c (sccs put-tree)
 * Author:	T.E.Dickey
 * Created:	08 May 1990 (from sccsput.sh and rcsput.c)
 * Modified:
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
#include	<rcsdefs.h>
#include	<sccsdefs.h>
extern	FILE	*popen();

#define	isDIR(mode)	((mode & S_IFMT) == S_IFDIR)
#define	isFILE(mode)	((mode & S_IFMT) == S_IFREG)
#define	VERBOSE		if (!quiet) PRINTF

static	char	diff_opts[BUFSIZ];
static	char	*verb = "putdelta";
static	char	comment[BUFSIZ];
static	FILE	*log_fp;
static	int	a_opt;		/* all-directory scan */
static	int	no_op;		/* no-op mode */
static	char	*pager;		/* nonzero if we don't cat diffs */
static	int	Force, force;
static	char	*k_opt	= "";
static	char	*r_opt	= "";
static	int	quiet;
static	int	found_diffs;	/* true iff we keep logfile */

static
cat2fp(
_ARX(FILE *,	fp)
_AR1(char *,	name)
	)
_DCL(FILE *,	fp)
_DCL(char *,	name)
{
	auto	FILE	*ifp;
	auto	char	t[BUFSIZ];
	auto	size_t	n;

	if (ifp = fopen(name, "r")) {
		while ((n = fread(t, sizeof(char), sizeof(t), ifp)) > 0)
			if (fwrite(t, sizeof(char), n, fp) != n)
				break;
		FCLOSE(ifp);
	}
}

static
pipe2file(
_ARX(char *,	cmd)
_AR1(char *,	name)
	)
_DCL(char *,	cmd)
_DCL(char *,	name)
{
	auto	FILE	*ifp, *ofp;
	auto	char	buffer[BUFSIZ];
	auto	int	empty	= TRUE;
	auto	size_t	n;

	if (!tmpnam(name) || !(ofp = fopen(name,"w")))
		failed("tmpnam");
	if (!quiet) {
		FORMAT(buffer, "%s > ", cmd);
		shoarg(stdout, buffer, name);
	}
	if (!(ifp = popen(bldcmd(buffer, cmd, sizeof(buffer)), "r")))
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
different(
_ARX(char *,	working)
_AR1(char *,	archive)
	)
_DCL(char *,	working)
_DCL(char *,	archive)
{
	auto	char	buffer[BUFSIZ],
			in_diff[MAXPATHLEN],
			out_diff[MAXPATHLEN];
	auto	int	changed;

	*buffer = EOS;
	catarg(buffer, "get");
	catarg(buffer, "-p");
	if (quiet)	catarg(buffer, "-s");
	if (*k_opt)	catarg(buffer, k_opt);
	if (*r_opt)	catarg(buffer, r_opt);
	catarg(buffer, archive);
	(void)pipe2file(buffer, in_diff);

	*buffer = EOS;
	catarg(buffer, "diff");
	(void)strcat(buffer, diff_opts);
	catarg(buffer, in_diff);
	catarg(buffer, working);
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
			VERBOSE("... appending to logfile");
			cat2fp(log_fp, out_diff);
			if (!found_diffs && filesize(out_diff) > 0)
				found_diffs = TRUE;
		}
	} else {
		VERBOSE("*** no differences found ***\n");
	}

	(void)unlink(in_diff);
	(void)unlink(out_diff);
	return (changed);
}

static
checkin(
_ARX(char *,	path)
_AR1(char *,	name)
	)
_DCL(char *,	path)
_DCL(char *,	name)
{
	auto	char	args[BUFSIZ];
	auto	char	*working = sccs2name(name,FALSE);
	auto	char	*archive = name2sccs(name,FALSE);
	auto	int	first;

	if (first = (filesize(archive) < 0)) {
		if (!Force && !istextfile(working)) {
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
			if (!Force && !different(working, archive))
				return;
			first = FALSE;
		}
	}

	*args = EOS;
	if (quiet)	catarg(args, "-s");
	if (force)	catarg(args, "-f");
	if (*k_opt)	catarg(args, k_opt);
	if (*r_opt)	catarg(args, r_opt);
	if (*comment)	catarg(args, comment);
	catarg(args, working);

	if (!no_op) {
		PRINTF("*** %s \"%s\"\n",
			first	? "Initial SCCS insertion of"
				: "Applying SCCS delta to",
			name);
		if (!quiet) shoarg(stdout, verb, args);
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
WALK_FUNC(scan_tree)
{
	auto	char	tmp[MAXPATHLEN],
			*s = pathcat(tmp, path, name);

	if (sp == 0 || readable < 0) {
		readable = -1;
		perror(name);
		if (!Force)
			exit(FAIL);
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
		checkin(path,name);
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

usage(
_AR1(int,	option))
_DCL(int,	option)
{
	static	char	*tbl[] = {
 "Usage: sccsput [options] files_or_directories"
,""
,"Options"
,"  -a       process all directories, including those beginning with \".\""
,"  -b       (passed to \"diff\")"
,"  -c       send differences to terminal without $PAGER filtering"
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
	register int	j;
	for (j = 0; j < sizeof(tbl)/sizeof(tbl[0]); j++)
		FPRINTF(stderr, "%s\n", tbl[j]);
	if (option == '?')
		(void)system("putdelta -?");
	exit(FAIL);
}

/*ARGSUSED*/
_MAIN
{
	auto	char	deferred[BUFSIZ],
			*logname;
#define	DEFERRED	strcat(strcpy(deferred, "-"), optarg)

	register int	j;

	revert("not a setuid-program");	/* ensure this is not misused */
	track_wd((char *)0);
	pager = dftenv("more -l", "PAGER");
	while ((j = getopt(argc, argv, "abcfhkl:nr:sy:D:FT:")) != EOF) {
		switch (j) {
		case 'a':	a_opt = TRUE;			break;
		case 'b':	catarg(diff_opts, "-b");	break;
		case 'c':	pager = 0;			break;
		case 'f':	force = TRUE;			break;
		case 'h':	catarg(diff_opts, "-h");	break;
		case 'k':	k_opt = "-k";			break;
		case 'l':	found_diffs = filesize(logname = optarg) > 0;
				if (!(log_fp = fopen(optarg, "a+")))
					usage(0);
				break;
		case 'n':	no_op = TRUE;			break;
		case 'r':	FORMAT(deferred, "-r%s", optarg);
				r_opt = stralloc(deferred);
				break;
		case 's':	quiet = TRUE;			break;
		case 'y':	FORMAT(comment, "-y%.*s",
					sizeof(comment)-3, optarg);
				break;
		case 'D':	catarg(diff_opts, DEFERRED);	break;
		case 'F':	Force = TRUE;			break;
		case 'T':	verb = optarg;			break;
		default:	usage(j);
		}
	}

	if (optind < argc) {
		while (optind < argc)
			do_arg(argv[optind++]);
	} else
		do_arg(".");

	if (log_fp != 0) {
		FCLOSE(log_fp);
		if (!found_diffs)
			(void)unlink(logname);
	}

	exit(SUCCESS);
	/*NOTREACHED*/
}
