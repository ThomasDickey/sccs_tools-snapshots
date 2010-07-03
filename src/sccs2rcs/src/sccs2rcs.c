/*
 * SCCSTORCS - build RCS file from SCCS file preserving deltas.
 * Author: Ken Greer
 *
 * $Log: sccs2rcs.c,v $
 * Revision 6.10  2010/07/03 17:14:18  tom
 * stricter gcc warnings
 *
 * Revision 6.9  2004/03/08 01:45:59  tom
 * remove K&R support, indent'd
 *
 * Revision 6.8  2002/07/06 16:18:09  tom
 * char-subscript warning (gcc/solaris)
 *
 * Revision 6.7  2002/07/05 13:42:07  tom
 * gcc warning
 *
 * Revision 6.6  2001/12/11 15:04:07  tom
 * change interface to rcs_dir()
 *
 * Revision 6.5  1995/05/13 23:19:39  tom
 * use MODULE_ID
 *
 * Revision 6.4  1995/05/13 23:19:39  tom
 * setmtime
 *
 * Revision 6.3  1994/07/19  15:27:34  tom
 * require repeating "-e" option to enable Log-comment editing.
 *
 * Revision 6.1  1993/09/23  20:21:51  dickey
 * gcc warnings
 *
 * Revision 6.0  93/04/29  12:30:16  ste_cm
 * BASELINE Wed May  5 11:05:31 1993 -- TD_LIB #12
 * 
 * Revision 5.3  93/04/29  12:30:16  dickey
 * missed an option
 * 
 * Revision 5.2  93/04/29  10:11:34  dickey
 * provided "-q" option for CI-tool
 * 
 * Revision 5.1  93/04/29  09:06:28  dickey
 * shorten-filenames
 * 
 * Revision 5.0  91/10/24  09:17:22  ste_cm
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

#define	CHR_PTYPES
#define	STR_PTYPES
#include "ptypes.h"
#include "rcsdefs.h"
#include "sccsdefs.h"
#include <errno.h>

MODULE_ID("$Id: sccs2rcs.c,v 6.10 2010/07/03 17:14:18 tom Exp $")

#define SOH	001		/* SCCS lines start with SOH (Control-A) */
#define RCS	"rcs"
#define GET	"getdelta -f"
#define CI	"checkin -f"
#define CO	"checkout"
#define	WHOAMI	"sccs2rcs"

#define prefix(a, b)	(strncmp(a, b, strlen(a)) == 0)
#define null(str)	((str) == NULL ? "<null>\n" : (str))

static char *workfile;		/* set iff we infer working file */
static char comments[80];	/* set in 'find_comment()' */
static char *comment_opt;	/* set by '-c' option */
static int edit_lines;		/* total lines written to temp-file */

int
  edit_key = FALSE;		/* edit 'sccs-id' keywords */
int quiet = FALSE;
int trace = FALSE;		/* just show what would be done, don't run commands */
int verbose = FALSE;		/* Print commands before executing */

typedef struct delta {
    char *author;
    char *revision;
    char *commentary;
    struct delta *next;
} DELTA;

typedef struct userlist {
    char *user;
    struct userlist *next;
} USERLIST;

typedef struct header {
    DELTA *deltas;
    USERLIST *userlist;
    char *description;
} HEADER;

static void build_new_rcs_file(HEADER *, char *);
static void print_header(char *, HEADER *);

static void
quit2(const char *fmt, const char *args)
{
    FPRINTF(stderr, "%s: ", WHOAMI);
    FPRINTF(stderr, fmt, args);
    exit(FAIL);
    /*NOTREACHED */
}

static void
quit(const char *fmt)
{
    FPRINTF(stderr, "%s: ", WHOAMI);
    FPRINTF(stderr, fmt);
    exit(FAIL);
    /*NOTREACHED */
}

static void *
xalloc(unsigned size)
{
    void *p;
    if ((p = malloc(size)) == NULL)
	quit("Out of Memory.\n");
    return (p);
}

#define	XALLOC(cast)	(cast *)xalloc(sizeof(cast))

/*
 * Allocate space for string and copy str to it.
 */
static char *
string(char *str)
{
    char *p = xalloc((unsigned) (strlen(str) + 1));
    return (strcpy(p, str));
}

/*
 * Return pointer to the final file name in a path.
 * I.e. sname ("/foo/baz/mumble") returns a pointer to "mumble".
 */
static char *
sname(char *s)
{
    char *p;

    for (p = s; *p;)
	if (*p++ == '/')
	    s = p;
    return (s);
}

static DELTA *
new_delta(char *line)
{
    DELTA *delta;
    char rev[32];
    char author[32];

    if (sscanf(line, "%*s %*s %s %*s %*s %s", rev, author) < 1)
	quit("delta format");
    delta = XALLOC(DELTA);
    delta->author = string(author);
    delta->revision = string(rev);
    delta->commentary = NULL;
    return (delta);
}

static char *
concat(char *old_str, char *str)
{
    int len;
    char *newstring;

    if (old_str == NULL)
	return (string(str));

    len = strlen(old_str) + strlen(str);
    newstring = xalloc((unsigned) (len + 1));
    (void) strcat(strcpy(newstring, old_str), str);
    free(old_str);
    return (newstring);
}

static void
trimtail(char *line)
{
    char *p = line;
    while (*p)
	p++;
    while (p > line && p[-1] <= ' ')
	p--;
    *p = '\0';
}

static USERLIST *
collect_userlist(FILE *fd)
{
    char line[128];
    USERLIST *userlist = NULL, *newuser;
    while (fgets(line, sizeof line, fd)) {
	if (line[0] == SOH && line[1] == 'U')	/* End of userlist */
	    break;
	trimtail(line);
	newuser = XALLOC(USERLIST);
	newuser->user = string(line);
	newuser->next = userlist;
	userlist = newuser;
    }
    return (userlist);
}

static HEADER *
collect_header(FILE *fd)
{
    DELTA *head = NULL, *delta = NULL;
    USERLIST *userlist = NULL;
    static HEADER header;
    char line[BUFSIZ], *description = NULL;
    while (fgets(line, sizeof line, fd)) {
	if (line[0] != SOH)
	    continue;
	if (line[1] == 'I')	/* The first INCLUDE */
	    break;
	switch (line[1]) {
	case 'd':		/* New delta */
	    delta = new_delta(line);
	    delta->next = head;
	    head = delta;
	    break;
	case 'c':		/* Commentary */
	    delta->commentary = concat(delta->commentary, &line[3]);
	    break;
	case 'u':
	    userlist = collect_userlist(fd);
	    break;
	case 't':
	    while (fgets(line, sizeof line, fd) && !prefix("\1T", line))
		description = concat(description, line);
	}
    }
    header.userlist = userlist;
    header.deltas = head;
    header.description = description;
    return (&header);
}

static int
invoke(const char *command, const char *args)
{
    if (trace || verbose)
	shoarg(stdout, command, args);
    return (trace ? 0 : execute(command, args));
}

/*
 * Build an initiate a command via a pipe
 */
static FILE *
to_pipe(char *command, char *args)
{
    char temp[BUFSIZ];

    if (trace || verbose)
	shoarg(stdout, command, args);
    return trace
	? 0
	: popen(bldcmd2(temp, command, args, sizeof(temp)), "w");
}

static void
RCSarg(char *command)
{
    *command = EOS;
    if (quiet)
	catarg(command, "-q");
}

/*
 * See if a file exists
 */
static int
fexists(char *name)
{
    struct stat sb;

    return (stat(name, &sb) >= 0
	    || (sb.st_mode & S_IFMT) == S_IFREG);
}

/*
 * Convert SCCS file to RCS file
 */
static HEADER *
read_sccs(char *sccsfile)
{
    HEADER *header;
    FILE *fd;

    if (!fexists(workfile = sccs2name(sccsfile, FALSE)))
	workfile = 0;
    sccsfile = name2sccs(sccsfile, FALSE);
    if ((fd = fopen(sccsfile, "r")) == NULL) {
	FPRINTF(stderr, "%s: cannot open.\n", sccsfile);
	return (NULL);
    }
    header = collect_header(fd);
    FCLOSE(fd);
    if (trace || verbose > 1)
	print_header(sccsfile, header);
    build_new_rcs_file(header, sccsfile);
    return (header);
}

static int
install_userlist(USERLIST * userlist, char *rcsfile)
{
    char command[BUFSIZ];
    int count;
    if (userlist == NULL)
	return (0);
    RCSarg(command);
    (void) strcat(command, "-a");
    for (count = 0; userlist; userlist = userlist->next, count++) {
	if (count > 0)
	    (void) strcat(command, ",");
	(void) strcat(command, userlist->user);
    }
    (void) strcat(command, " ");
    catarg(command, rcsfile);
    return (invoke(RCS, command));
}

static int
initialize_rcsfile(char *description, char *rcsfile)
{
    char command[BUFSIZ];
    FILE *pd;

    if (mkdir(rcs_dir(NULL, NULL), 0755) < 0) {		/* forces common naming convention */
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
    if ((pd = to_pipe(RCS, command)) != NULL) {
	FPRINTF(pd, "%s", description ? description : "\n");
	return (pclose(pd));
    } else if (trace) {
	PRINTF("Description:\n%s\n", null(description));
	return (0);
    } else
	return (-1);
}

static int
install_deltas(DELTA * delta, char *sccsfile, char *rcsfile)
{
    FILE *pd;
    char command[BUFSIZ];
    char author[BUFSIZ];
    char version[BUFSIZ];

    for (; delta; delta = delta->next) {
	FORMAT(author, "-w%s", delta->author);
	FORMAT(version, "-r%s", delta->revision);
	if (trace || verbose > 1)
	    PRINTF("# installing delta %s\n", version);

	/*
	 * Get the SCCS file.
	 */

	*command = EOS;
	if (quiet)
	    catarg(command, "-s");
	catarg(command, version);
	catarg(command, sccsfile);
	if (invoke(GET, command) < 0)
	    return (-1);

	RCSarg(command);
	catarg(command, author);
	if (quiet)
	    catarg(command, "-q");
	catarg(command, version);
	catarg(command, rcsfile);

	if ((pd = to_pipe(CI, command)) != NULL) {
	    FPRINTF(pd, delta->commentary);
	    if (pclose(pd) < 0)
		return (-1);
	} else if (trace) {
	    PRINTF("Commentary:\n%s\n", null(delta->commentary));
	} else
	    return (-1);
    }
    return (0);
}

static int
finalize_rcsfile(char *rcsfile)
{
    char command[BUFSIZ];
    RCSarg(command);
    catarg(command, "-L");
    catarg(command, rcsfile);
    return (invoke(RCS, command));
}

/*
 * This is called from 'rcsparse_str()' to store comment-text
 */
static void
store_comment(int c)
{
    int len = strlen(comments);
    comments[len++] = c;
    comments[len] = EOS;
}

/*
 * Find the string we are using for a comment-string
 */
static void
find_comment(char *filename)
{
    int header = TRUE;
    char key[80];
    char *rcsfile;
    int code = S_FAIL;
    char *s = 0;

    if (comment_opt) {
	(void) strcpy(comments, comment_opt);
	return;
    }
    comments[0] = EOS;
    if (!rcsopen(rcsfile = name2rcs(filename, FALSE), -verbose, TRUE))
	quit2("Could not find archive %s\n", rcsfile);
    while (header && (s = rcsread(s, code))) {
	s = rcsparse_id(key, s);
	switch (code = rcskeys(key)) {
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
static int
edit_what(char *s)
{
    char tmp[BUFSIZ];
    char *t;
    size_t len;
    int changes = FALSE;

    while ((t = strchr(s, '@')) != NULL) {
	if (!strncmp(t, "@(#)", len = 4)
	    && t[len] != '"') {
	    s = t;
	    len = strlen(t = strcpy(tmp, t + len)) - 1;
	    while ((len != 0)
		   && (ispunct(UCH(t[len]))
		       || isspace(UCH(t[len]))))
		len--;
	    len++;
	    if ((t = strchr(tmp, '"'))
		&& (t - tmp < (int) len))
		len = t - tmp;
	    if (len > 0) {
		*s++ = '$';
		/* break up literal because of rcs */
		(void) strcpy(s, "Id$");
		s += strlen(s);
		(void) strcpy(s, tmp + len);
		changes = TRUE;
	    } else
		s++;
	} else {
	    s = t + 1;
	}
    }
    return (changes);
}

static int
match(const char *s, const char *t)
{
    char *base = s;

    while (*t) {
	if (*s++ != *t++)
	    return (0);
    }
    if (isalnum(UCH(*s)))
	return (0);
    return (s - base);
}

#define	SKIP(s)		while(isspace(UCH(*s)))	s++
#define	MATCH(t)	(len = match(tmp+(s-base),t))

/*
 * Search for places where we can put a Log-keyword.  This is when we find (in
 * sequence) the comment-prefix, at least one of the keywords (see MATCH), and
 * a colon.
 */
static int
edit_log(char *s)
{
    char tmp[BUFSIZ];
    char *base;
    int len;
    int ok = FALSE;
    char *t = comments;

    SKIP(s);			/* be tolerant about leading blanks */
    SKIP(t);
    while (*t) {
	if (isspace(UCH(*s)) && isspace(UCH(*t))) {
	    SKIP(s);
	    SKIP(t);
	    continue;
	}
	if (*s++ != *t++)
	    return (0);		/* does not match comment-prefix */
    }

    /* make a copy in uppercase to simplify matching */
    for (t = base = s; (tmp[t - s] = *t) != EOS; t++)
	if (isalpha(UCH(*t)) && islower(UCH(*t)))
	    tmp[t - s] = toupper(*t);

    while (*s) {
	SKIP(s);
	if (MATCH("LAST")
	    || MATCH("MODIFIED")
	    || MATCH("REVISED")
	    || MATCH("REVISION")
	    || MATCH("UPDATED")
	    || MATCH("UPDATE")) {
	    ok = TRUE;
	    s += len;
	    SKIP(s);
	} else {
	    if (*s == ':') {
		s++;
		break;
	    }
	    return (FALSE);	/* colon makes it unambiguous */
	}
    }
    if (ok) {
	(void) strcpy(tmp, s);
	(void) strcpy(base, "$");	/* split keyword to avoid rcs */
	(void) strcat(base, "Log$");
	if (*tmp != '\n') {
	    edit_lines++;	/* split the line */
	    (void) strcat(strcat(base, "\n"), comments);
	}
	for (t = tmp; (*t == ' ') || (*t == '\t'); t++) ;
	(void) strcat(base, t);
    }
    return (ok);
}

/*
 * Make a new revision (on the main trunk) which has the sccs keywords
 * substituted into RCS keywords.
 */
static void
edit_keywords(char *filename)
{
    char bfr[BUFSIZ];
    char tmp[BUFSIZ];
    FILE *fpT;
    FILE *fpS;
    struct stat sb;
    int log_tag = FALSE;
    int changes = 0;

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
    if ((fpS = fopen(filename, "r")) != NULL) {
	while (fgets(bfr, sizeof(bfr), fpS)) {
	    edit_lines++;
	    changes += edit_what(bfr);
	    if (!log_tag && (edit_key > 1) && edit_log(bfr)) {
		log_tag = TRUE;
		changes++;
	    }
	    (void) fputs(bfr, fpT);
	}
	FCLOSE(fpS);
    }
    if (changes) {
	if (copyback(fpT, filename, (int) sb.st_mode, edit_lines))
	    if (setmtime(filename, sb.st_mtime, sb.st_atime) < 0)
		failed("(setmtime)");
	if (verbose) {
	    (void) invoke("rcsdiff", filename);
	}
	*bfr = EOS;
	FORMAT(tmp, "-m%s keywords", WHOAMI);
	if (quiet)
	    catarg(bfr, "-q");
	catarg(bfr, tmp);
	catarg(bfr, filename);
	(void) invoke(CI, bfr);
    }
    FCLOSE(fpT);
}

static void
build_new_rcs_file(HEADER * header, char *sccsfile)
{
    char *rcsfile = &(sname(sccsfile))[2];

    if (initialize_rcsfile(header->description, rcsfile))
	quit2("Error initializing new rcs file %s\n", rcsfile);

    if (install_userlist(header->userlist, rcsfile))
	quit2("Error installing user access list to rcs file %s\n", rcsfile);

    if (install_deltas(header->deltas, sccsfile, rcsfile))
	quit2("Error installing delta to rcs file %s\n", rcsfile);

    if (edit_key)
	edit_keywords(rcsfile);

    if (finalize_rcsfile(rcsfile))
	quit2("Error setting defaults to rcs file %s\n", rcsfile);
}

static void
print_header(char *sccsfile, HEADER * header)
{
    DELTA *d;
    USERLIST *u;

    PRINTF("\n%s:\n", sccsfile);
    PRINTF("------------------------------------------------------------\n");
    if (header->description)
	PRINTF("Descriptive text:\n%s", header->description);

    if (header->userlist) {
	PRINTF("\nUser access list:\n");
	for (u = header->userlist; u; u = u->next)
	    PRINTF("%s\n", u->user);
    }

    for (d = header->deltas; d; d = d->next) {
	PRINTF("\nAuthor:  %s", d->author);
	PRINTF("\nRelease: %s\n", d->revision);
	PRINTF("Commentary:\n%s", d->commentary);
    }
    PRINTF("------------------------------------------------------------\n");
}

/*ARGSUSED*/
_MAIN
{
    int errors = 0;
    int j;

    while ((j = getopt(argc, argv, "c:eqtv")) != EOF)
	switch (j) {
	case 'c':
	    comment_opt = optarg;
	    break;
	case 'e':		/* once to edit Id-string */
	    edit_key++;		/* repeat to edit Log-comment */
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
	    FPRINTF(stderr, "Unknown switch \"%s\".\n", argv[1]);
	    exit(FAIL);
	    /*NOTREACHED */
	}

    if (argc <= 1)
	quit2("Usage: %s [-cTEXT -e -t -v -q] s.file ...\n", WHOAMI);

    for (j = optind; j < argc; j++) {
	char *sccsfile = argv[j];
	if (read_sccs(sccsfile) != NULL) {
	    if (workfile) {	/* restore original working file */
		char command[BUFSIZ];
		RCSarg(command);
		catarg(command, workfile);
		(void) invoke(CO, command);
	    }
	} else
	    errors++;
    }
    exit(errors ? FAIL : SUCCESS);
    /*NOTREACHED */
}
