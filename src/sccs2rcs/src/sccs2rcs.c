#ifndef	lint
static char *RCSid =
"$Header: /users/source/archives/sccs_tools.vcs/src/sccs2rcs/src/RCS/sccs2rcs.c,v 5.0 1991/10/24 09:17:22 ste_cm Rel $";
#endif

/*
 * SCCSTORCS - build RCS file from SCCS file preserving deltas.
 * Author: Ken Greer
 *
 * $Log: sccs2rcs.c,v $
 * Revision 5.0  1991/10/24 09:17:22  ste_cm
 * BASELINE Mon Jul 20 12:41:28 1992 -- CM_TOOLS #11
 *
 * Revision 4.0  91/10/24  09:17:22  ste_cm
 * BASELINE Tue Dec 17 11:56:35 1991
 * 
 * Revision 3.7  91/10/24  09:17:22  dickey
 * compile against CM_TOOLS #10
 * 
 * Revision 3.6  91/07/24  11:49:53  dickey
 * use name2sccs/sccs2name to simplify/standardize pathname translation
 * 
 * Revision 3.5  91/07/24  11:39:29  dickey
 * quieted the 'rcs' command also
 * 
 * Revision 3.4  91/07/24  11:24:47  dickey
 * corrected logic (introduced in last version) when invoking 'ci' to add
 * a new delta. tuned verbosity.
 * 
 * Revision 3.3  91/07/24  09:24:49  dickey
 * use catarg/shoarg/bldcmd2/execute to simplify the process of escaping
 * spaces.
 * 
 * Revision 3.2  91/07/24  08:29:59  dickey
 * test for exceptions to error-failure after 'mkdir()'
 * 
 * Revision 3.1  91/07/24  08:20:12  dickey
 * lint (apollo sr10.3); also corrected code that passes quiet-option to CI
 * and CO (GET was ok)
 * 
 * Revision 3.0  91/05/23  09:09:16  ste_cm
 * BASELINE Tue Jun 18 08:04:39 1991 -- apollo sr10.3
 * 
 * Revision 2.4  91/05/23  09:09:16  dickey
 * apollo sr10.3 cpp complains about endif-tags
 * 
 * Revision 2.3  90/06/22  07:46:59  dickey
 * altered interface to 'name2rcs()'
 * 
 * Revision 2.2  89/10/10  15:26:37  dickey
 * use RCS_DIR environment variable ('rcs_dir()' function) where needed to
 * make this work better with CM_TOOLS
 * 
 * Revision 2.1  89/10/05  10:53:38  dickey
 * changed ident-keyword to 'Id' from 'Header'
 * 
 * Revision 2.0  89/04/28  14:48:28  ste_cm
 * BASELINE Mon Jul 10 09:22:04 EDT 1989
 * 
 * Revision 1.11  89/04/28  14:48:28  dickey
 * "toupper" is more portable than "_toupper"
 * 
 * Revision 1.10  89/03/23  13:32:25  dickey
 * translate only one Log-keyword per file
 * 
 * Revision 1.9  89/03/23  10:37:26  dickey
 * require a colon after reserved-words before splitting a Log-line.
 * 
 * Revision 1.8  89/03/23  08:14:08  dickey
 * added '-c' option (to pass-thru to 'rcs'), refined code which splits line
 * after Log-keyword.
 * 
 * Revision 1.7  89/03/22  15:04:39  dickey
 * added code to support "-e" option (edit sccs keywords, changing them to
 * RCS keywords).
 * 
 * Revision 1.6  89/03/22  10:37:00  dickey
 * linted, use "ptypes.h" and 'getopt()'.
 * added -e option (not done)
 * 
 * Revision 1.5  89/03/20  11:05:07  dickey
 * rewrote, making this smart enough to preserve checkin-dates, and to work
 * with the conventional RCS and sccs directory convention.  Renamed to avoid
 * confusion with the dumb version.
 * 
 * Revision 1.4  84/10/17  21:12:11  root
 * FROM_KEYS
 * 
 * Revision 1.4  84/10/17  21:12:11  root
 * Added check for having multiple deltas in a row for the same revision.
 * --ks
 * 
 * Revision 1.3  84/10/17  20:53:18  root
 * Put in SCCS string in comment for telling who checked it in..
 * --ks
 * 
 * Revision 1.2  84/10/17  12:22:14  root
 * Fixed the case when a delta was removed.
 * Also, use -f on checkin so comments are kept even if the file
 * didn't change between deltas.
 * --ks
 * 
 * Revision 1.1  84/10/07  14:59:47  root
 * Initial revision
 * 
 * Revision 1.2  83/03/27  11:21:17  root
 * Returns non-zero exit codes on soft errors also.
 * 
 * Revision 1.1  83/03/24  14:33:24  root
 * Initial revision
 * 
 */

#define	STR_PTYPES
#include "ptypes.h"
#include "rcsdefs.h"
#include "sccsdefs.h"
#include <ctype.h>
#include <errno.h>
extern	FILE	*tmpfile();

#define SOH	001		/* SCCS lines start with SOH (Control-A) */
#define RCS	"rcs"
#define GET	"getdelta -f"
#define CI	"checkin -f"
#define CO	"checkout"
#define	WHOAMI	"sccs2rcs"

#define prefix(a, b)	(strncmp(a, b, strlen(a)) == 0)
#define null(str)	((str) == NULL ? "<null>\n" : (str))

#define	WARN	FPRINTF(stderr,

static	char	*workfile;	/* set iff we infer working file */
static	char	comments[80];	/* set in 'find_comment()' */
static	char	*comment_opt;	/* set by '-c' option */
static	int	edit_lines;	/* total lines written to temp-file */

int
    edit_key = FALSE,	/* edit 'sccs-id' keywords */
    quiet = FALSE,
    trace = FALSE,	/* just show what would be done, don't run commands */
    verbose = FALSE;	/* Print commands before executing */

typedef struct delta
{
    char *revision;
    char *commentary;
    struct delta *next;
} DELTA;

typedef struct userlist
{
    char *user;
    struct userlist *next;
} USERLIST;

typedef struct header
{
    DELTA *deltas;
    USERLIST *userlist;
    char  *description;
} HEADER;


quit2(fmt, args)
char *fmt,*args;
{
	WARN "%s: ", WHOAMI);
	WARN fmt, args);
	exit(FAIL);
	/*NOTREACHED*/
}

quit (fmt)
char *fmt;
{
	WARN "%s: ", WHOAMI);
	WARN fmt);
	exit (FAIL);
	/*NOTREACHED*/
}

char *
xalloc (size)
unsigned size;
{
    char *p;
    if ((p = malloc (size)) == NULL)
	quit ("Out of Memory.\n");
    return (p);
}

#ifdef	lint
#define	XALLOC(cast)	((cast *)0)
#else
#define	XALLOC(cast)	(cast *)xalloc(sizeof(cast))
#endif

/*
 * Allocate space for string and copy str to it.
 */
char *
string (str)
char *str;
{
    register char *p = xalloc ((unsigned) (strlen (str) + 1));
    return (strcpy (p, str));
}

/*
 * Return pointer to the final file name in a path.
 * I.e. sname ("/foo/baz/mumble") returns a pointer to "mumble".
 */
char *
sname (s)
register char *s;
{
    register char *p;

    for (p = s; *p;)
       if (*p++ == '/')
	   s = p;
    return (s);
}

DELTA *
new_delta (line)
char *line;
{
    register DELTA *delta;
    char rev[32];

    if (sscanf (line, "%*s %*s %s", rev) < 1)
	quit("delta format");
    delta = XALLOC(DELTA);
    delta -> revision = string (rev);
    delta -> commentary = NULL;
    return (delta);
}

char *
concat (old_str, str)
char *old_str, *str;
{
    register int len;
    register char *newstring;

    if (old_str == NULL)
	return (string (str));

    len = strlen (old_str) + strlen (str);
    newstring = xalloc ((unsigned) (len + 1));
    (void)strcat (strcpy (newstring, old_str), str);
    free (old_str);
    return (newstring);
}

trimtail (line)
char *line;
{
    register char *p = line;
    while (*p) p++;
    while (p > line && p[-1] <= ' ')
	p--;
    *p = '\0';
}

USERLIST *
collect_userlist (fd)
FILE *fd;
{
    char line[128];
    USERLIST *userlist = NULL, *newuser;
    while (fgets (line, sizeof line, fd))
    {
	if (line[0] == SOH && line[1] == 'U')	/* End of userlist */
	    break;
	trimtail (line);
	newuser = XALLOC(USERLIST);
	newuser -> user = string (line);
	newuser -> next = userlist;
	userlist = newuser;
    }
    return (userlist);
}

HEADER *
collect_header (fd)
FILE *fd;
{
    DELTA *head = NULL, *delta;
    USERLIST *userlist = NULL;
    static HEADER header;
    char line[BUFSIZ], *description = NULL;
    while (fgets (line, sizeof line, fd))
    {
	if (line[0] != SOH)
	    continue;
	if (line[1] == 'I')		/* The first INCLUDE */
	    break;
	switch (line[1])
	{
	    case 'd': 		       /* New delta */
		delta = new_delta (line);
		delta->next = head;
		head = delta;
		break;
	    case 'c': 		       /* Commentary */
		delta -> commentary = concat (delta -> commentary, &line[3]);
		break;
	    case 'u':
		userlist = collect_userlist (fd);
		break;
	    case 't':
		while (fgets (line, sizeof line, fd) && !prefix("\1T", line))
		    description = concat (description, line);
	}
    }
    header.userlist = userlist;
    header.deltas = head;
    header.description = description;
    return (&header);
}

static
invoke(command, args)
char	*command, *args;
{
	if (trace || verbose)
		shoarg (stdout, command, args);
	return (trace ? 0 : execute (command, args));
}


/*
 * Build an initiate a command via a pipe
 */
static
FILE *
to_pipe(command, args)
char	*command, *args;
{
	extern FILE *popen();
	char	temp[BUFSIZ];

	if (trace || verbose)
		shoarg (stdout, command, args);
	return trace
		? 0
		: popen(bldcmd2(temp, command, args, sizeof(temp)), "w");
}

static
RCSarg(command)
char	*command;
{
	*command = EOS;
	if (quiet) catarg(command, "-q");
}

/*
 * See if a file exists
 */
fexists(name)
char	*name;
{
	struct	stat	sb;

	return (stat(name, &sb) >= 0
	||  (sb.st_mode & S_IFMT) == S_IFREG);
}

/*
 * Convert SCCS file to RCS file
 */
HEADER *
read_sccs (sccsfile)
char *sccsfile;
{
    HEADER *header;
    FILE *fd;

    if (!fexists(workfile = sccs2name(sccsfile, FALSE)))
	workfile = 0;
    sccsfile = name2sccs(sccsfile, FALSE);
    if ((fd = fopen (sccsfile, "r")) == NULL)
    {
	WARN "%s: cannot open.\n", sccsfile);
	return (NULL);
    }
    header = collect_header (fd);
    FCLOSE (fd);
    if (trace || verbose > 1)
	print_header (sccsfile, header);
    build_new_rcs_file (header, sccsfile);
    return (header);
}

install_userlist (userlist, rcsfile)
register USERLIST *userlist;
char *rcsfile;
{
    char command[BUFSIZ];
    int count;
    if (userlist == NULL)
	return (0);
    RCSarg(command);
    (void)strcat (command, "-a");
    for (count = 0; userlist; userlist = userlist -> next, count++)
    {
	if (count > 0)
	    (void)strcat (command, ",");
	(void)strcat (command, userlist -> user);
    }
    (void)strcat(command, " ");
    catarg(command, rcsfile);
    return (invoke(RCS, command));
}

initialize_rcsfile (description, rcsfile)
char *description, *rcsfile;
{
    char command[BUFSIZ];
    FILE *pd;

    if (mkdir(rcs_dir(), 0755) < 0) {	/* forces common naming convention */
	if (errno != EEXIST)
	    failed("mkdir");
    }

    RCSarg(command);
    catarg(command, "-i");
    catarg(command, "-U");

    if (comment_opt) {
	char temp[BUFSIZ];
	FORMAT(temp, "-c%s", comment_opt);
	catarg(command, temp);
    }
    catarg(command, rcsfile);
    if (pd = to_pipe(RCS, command)) {
	FPRINTF (pd, "%s", description ? description : "\n");
	return (pclose (pd));
    } else if (trace) {
	PRINTF ("Description:\n%s\n", null(description));
	return (0);
    } else
	return (-1);
}

install_deltas (delta, sccsfile, rcsfile)
register DELTA *delta;
char *sccsfile, *rcsfile;
{
    FILE *pd;
    char command[BUFSIZ];
    char version[BUFSIZ];

    for (; delta; delta = delta -> next)
    {
	FORMAT(version, "-r%s", delta->revision);
	if (trace || verbose > 1)
	    PRINTF("# installing delta %s\n", version);

	/*
	 * Get the SCCS file.
	 */

	*command = EOS;
	if (quiet)	catarg(command, "-s");
	catarg(command, version);
	catarg(command, sccsfile);
	if (invoke(GET, command) < 0)
		return (-1);

	RCSarg(command);
	catarg(command, version);
	catarg(command, rcsfile);

	if (pd = to_pipe(CI, command)) {
	    FPRINTF (pd, delta->commentary);
	    if (pclose (pd) < 0)
		return (-1);
	} else if (trace) {
	    PRINTF("Commentary:\n%s\n", null(delta->commentary));
	} else
	    return (-1);
    }
    return (0);
}

finalize_rcsfile (rcsfile)
char *rcsfile;
{
    char command[BUFSIZ];
    RCSarg(command);
    catarg(command, "-L");
    catarg(command, rcsfile);
    return (invoke (RCS, command));
}

/*
 * This is called from 'rcsparse_str()' to store comment-text
 */
store_comment(c)
{
	register int	len = strlen(comments);
	comments[len++] = c;
	comments[len]   = EOS;
}

/*
 * Find the string we are using for a comment-string
 */
find_comment(filename)
char	*filename;
{
	auto	int	header	= TRUE;
	auto	char	key[80];
	auto	char	*rcsfile;
	register char	*s = 0;

	if (comment_opt) {
		(void)strcpy(comments, comment_opt);
		return;
	}
	comments[0] = EOS;
	if (!rcsopen(rcsfile = name2rcs(filename,FALSE), -verbose, TRUE))
		quit2("Could not find archive %s\n", rcsfile);
	while (header && (s = rcsread(s))) {
		s = rcsparse_id(key, s);
		switch(rcskeys(key)) {
		case S_COMMENT:
			s = rcsparse_str(s, store_comment);
		case S_VERS:
			header = FALSE;
			break;
		}
	}
	rcsclose();
}

/*
 * Substitute what-strings to rcs format.  We assume that the keywords are
 * either part of a quoted string, or embedded in a comment.  The logic for
 * '"' is intended to protect against clobbering string literals and comments
 * which are appended after a string literal.
 */
edit_what(s)
char	*s;
{
	auto	 char	tmp[BUFSIZ];
	register char	*t;
	register size_t	len;
	auto	 int	changes = FALSE;

	while (t = strchr(s, '@')) {
		if (!strncmp(t, "@(#)", len = 4)
		&&  t[len] != '"') {
			s = t;
			len = strlen(t = strcpy(tmp, t + len)) - 1;
			while (	(len != 0)
			&&	(ispunct(t[len]) || isspace(t[len]) ) )
					len--;
			len++;
			if (	(t = strchr(tmp, '"'))
			&&	(t-tmp < len) )
				len = t - tmp;
			if (len > 0) {
				*s++ = '$';
				/* break up literal because of rcs */
				(void)strcpy(s, "Id$");
				s += strlen(s);
				(void)strcpy(s, tmp+len);
				changes = TRUE;
			} else
				s++;
		} else {
			s = t + 1;
		}
	}
	return (changes);
}

match(s,t)
char	*s,*t;
{
	register char	*base = s;

	while (*t) {
		if (*s++ != *t++)
			return (0);
	}
	if (isalnum(*s))
		return (0);
	return (s-base);
}

#define	SKIP(s)		while(isspace(*s))	s++;
#define	MATCH(t)	(len = match(tmp+(s-base),t))

/*
 * Search for places where we can put a Log-keyword.  This is when we find (in
 * sequence) the comment-prefix, at least one of the keywords (see MATCH), and
 * a colon.
 */
edit_log(s)
char	*s;
{
	auto	 char	tmp[BUFSIZ];
	auto	 char	*base;
	auto	 int	len;
	auto	 int	ok = FALSE;
	register char	*t = comments;

	SKIP(s);		/* be tolerant about leading blanks */
	SKIP(t);
	while (*t) {
		if (isspace(*s) && isspace(*t)) {
			SKIP(s);
			SKIP(t);
			continue;
		}
		if (*s++ != *t++)
			return(0);	/* does not match comment-prefix */
	}

	/* make a copy in uppercase to simplify matching */
	for (t = base = s; tmp[t-s] = *t; t++)
		if (isalpha(*t) && islower(*t))
			tmp[t-s] = toupper(*t);

	while (*s) {
		SKIP(s);
		if (	MATCH("LAST")
		||	MATCH("MODIFIED")
		||	MATCH("REVISED")
		||	MATCH("REVISION")
		||	MATCH("UPDATED")
		||	MATCH("UPDATE") ) {
			ok = TRUE;
			s += len;
			SKIP(s);
		} else {
			if (*s == ':') {
				s++;
				break;
			}
			return (FALSE);		/* colon makes it unambiguous */
		}
	}
	if (ok) {
		(void)strcpy(tmp, s);
		(void)strcpy(base, "$");	/* split keyword to avoid rcs */
		(void)strcat(base, "Log$");
		if (*tmp != '\n') {
			edit_lines++;		/* split the line */
			(void)strcat(strcat(base, "\n"), comments);
		}
		for (t = tmp; (*t == ' ') || (*t == '\t'); t++);
		(void)strcat(base, t);
	}
	return (ok);
}

/*
 * Make a new revision (on the main trunk) which has the sccs keywords
 * substituted into RCS keywords.
 */
edit_keywords(filename)
char	*filename;
{
	auto	char	bfr[BUFSIZ];
	auto	char	tmp[BUFSIZ];
	auto	FILE	*fpT,
			*fpS;
	auto	struct	stat	sb;
	auto	int	log_tag	= FALSE,
			changes = 0;

	edit_lines = 0;
	find_comment(filename);

	RCSarg(bfr);
	catarg(bfr, filename);
	if (invoke(CO, bfr) < 0)
		return;
	if (stat(filename, &sb) < 0)
		failed(filename);
	if (!(fpT = tmpfile()))
		failed("(tmpfile)");
	if (fpS = fopen(filename, "r")) {
		while (fgets(bfr, sizeof(bfr), fpS)) {
			edit_lines++;
			changes += edit_what(bfr);
			if (!log_tag && edit_log(bfr)) {
				log_tag = TRUE;
				changes++;
			}
			(void)fputs(bfr, fpT);
		}
		FCLOSE(fpS);
	}
	if (changes) {
		if (copyback(fpT, filename, (int)sb.st_mode, edit_lines))
			if (setmtime(filename, sb.st_mtime) < 0)
				failed("(setmtime)");
		if (verbose) {
			(void)invoke("rcsdiff", filename);
		}
		*bfr = EOS;
		FORMAT(tmp, "-m%s keywords", WHOAMI);
		catarg(bfr, tmp);
		catarg(bfr, filename);
		(void)invoke(CI, bfr);
	}
	FCLOSE(fpT);
}

build_new_rcs_file (header, sccsfile)
HEADER *header;
char *sccsfile;
{
    char *rcsfile = &(sname (sccsfile))[2];

    if (initialize_rcsfile (header -> description, rcsfile))
	quit2 ("Error initializing new rcs file %s\n", rcsfile);

    if (install_userlist (header -> userlist, rcsfile))
	quit2 ("Error installing user access list to rcs file %s\n", rcsfile);

    if (install_deltas (header -> deltas, sccsfile, rcsfile))
	quit2 ("Error installing delta to rcs file %s\n", rcsfile);

    if (edit_key)
	edit_keywords(rcsfile);

    if (finalize_rcsfile (rcsfile))
	quit2 ("Error setting defaults to rcs file %s\n", rcsfile);
}

print_header (sccsfile, header)
char *sccsfile;
register HEADER *header;
{
    register DELTA *d;
    register USERLIST *u;

    PRINTF ("\n%s:\n", sccsfile);
    PRINTF ("------------------------------------------------------------\n");
    if (header -> description)
	PRINTF ("Descriptive text:\n%s", header -> description);

    if (header -> userlist)
    {
	PRINTF ("\nUser access list:\n");
	for (u = header -> userlist; u; u = u -> next)
	    PRINTF ("%s\n", u -> user);
    }

    for (d = header -> deltas; d; d = d -> next)
    {
	PRINTF ("\nRelease: %s\n", d -> revision);
	PRINTF ("Commentary:\n%s", d -> commentary);
    }
    PRINTF ("------------------------------------------------------------\n");
}

/*ARGSUSED*/
_MAIN
{
	auto	int	errors = 0;
	register int	j;

	while ((j = getopt(argc, argv, "c:eqtv")) != EOF)
		switch (j) {
		case 'c':
			comment_opt = optarg;
			break;
		case 'e':
			edit_key = TRUE;
			break;
		case 'q':
			quiet = TRUE;
			break;
		case 'v':
			if (verbose)
				quiet = FALSE;
			verbose++;
			break;
		case 't': 
			trace = TRUE;
			break;
		default: 
			WARN "Unknown switch \"%s\".\n", argv[1]);
			exit (FAIL);
			/*NOTREACHED*/
		}

	if (argc <= 1)
		quit2 ("Usage: %s [-cTEXT -e -t -v -q] s.file ...\n", WHOAMI);

	for (j = optind; j < argc; j++) {
		auto	char *sccsfile = argv[j];
		if (read_sccs (sccsfile) != NULL) {
			if (workfile) {	/* restore original working file */
				auto	char	command[BUFSIZ];
				RCSarg(command);
				catarg(command, workfile);
				(void)invoke(CO, command);
			}
		} else
			errors++;
	}
	exit (errors ? FAIL : SUCCESS);
	/*NOTREACHED*/
}
