/*
 *       Copyright 1988 by Evans & Sutherland Computer Corporation,
 *                          Salt Lake City, Utah
 *  Portions Copyright 1989 by the Massachusetts Institute of Technology
 *                        Cambridge, Massachusetts
 *
 * Copyright 1992 Claude Lecommandeur.
 */

/***********************************************************************
 *
 * $XConsortium: parse.c,v 1.52 91/07/12 09:59:37 dave Exp $
 *
 * parse the .twmrc file
 *
 * 17-Nov-87 Thomas E. LaStrange       File created
 * 10-Oct-90 David M. Sternlicht       Storing saved colors on root
 *
 * Do the necessary modification to be integrated in ctwm.
 * Can no longer be used for the standard twm.
 *
 * 22-April-92 Claude Lecommandeur.
 *
 ***********************************************************************/

#include "ctwm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#ifdef USEM4
# include <sys/types.h>
# include <sys/wait.h>
#endif

#include "ctwm_atoms.h"
#include "screen.h"
#include "parse.h"
#include "parse_int.h"
#include "deftwmrc.h"
#ifdef SOUNDS
#  include "sound.h"
#endif

#ifndef SYSTEM_INIT_FILE
#error "No SYSTEM_INIT_FILE set"
#endif

static bool ParseStringList(const char **sl);

/*
 * With current bison, this is defined in the gram.tab.h, so this causes
 * a warning for redundant declaration.  With older bisons and byacc,
 * it's not, so taking it out causes a warning for implicit declaration.
 * A little looking around doesn't show any handy #define's we could use
 * to be sure of the difference.  This should quiet it down on gcc/clang
 * anyway...
 */
#ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wredundant-decls"
extern int yyparse(void);
# pragma GCC diagnostic pop
#else
extern int yyparse(void);
#endif

static FILE *twmrc;

static int ptr = 0;
static int len = 0;
#define BUF_LEN 300
static char buff[BUF_LEN + 1];
static const char **stringListSource, *currentString;

/*
 * While there are (were) referenced in a number of places through the
 * file, overflowlen is initialized to 0, only possibly changed in
 * twmUnput(), and unless it's non-zero, neither is otherwise touched.
 * So this is purely a twmUnput()-related var, and with flex, never used
 * for anything.
 */
#ifdef NON_FLEX_LEX
static char overflowbuff[20];           /* really only need one */
static int overflowlen;
#endif

int ConstrainedMoveTime = 400;          /* milliseconds, event times */
bool ParseError;                        /* error parsing the .twmrc file */
int RaiseDelay = 0;                     /* msec, for AutoRaise */
int (*twmInputFunc)(void);              /* used in lexer */

static int twmrc_lineno;


/* lex plumbing funcs */
static bool doparse(int (*ifunc)(void), const char *srctypename,
                    const char *srcname);

static int twmStringListInput(void);
#ifndef USEM4
static int twmFileInput(void);
#else
static int m4twmFileInput(void);
#endif

#if defined(YYDEBUG) && YYDEBUG
int yydebug = 1;
#endif


/*
 * Principle entry point from top-level code to parse the config file
 */
bool
ParseTwmrc(char *filename)
{
	int i;
	char *home = NULL;
	int homelen = 0;
	char *cp = NULL;
	char tmpfilename[257];

	/*
	 * Check for the twmrc file in the following order:
	 *   0.  -f filename.#
	 *   1.  -f filename
	 *   2.  .ctwmrc.#
	 *   3.  .ctwmrc
	 *   4.  .twmrc.#
	 *   5.  .twmrc
	 *   6.  system.ctwmrc
	 */
	for(twmrc = NULL, i = 0; !twmrc && i < 7; i++) {
		switch(i) {
			case 0:                       /* -f filename.# */
				if(filename) {
					cp = tmpfilename;
					sprintf(tmpfilename, "%s.%d", filename, Scr->screen);
				}
				else {
					cp = filename;
				}
				break;

			case 1:                       /* -f filename */
				cp = filename;
				break;

			case 2:                       /* ~/.ctwmrc.screennum */
				if(!filename) {
					home = getenv("HOME");
					if(home) {
						homelen = strlen(home);
						cp = tmpfilename;
						sprintf(tmpfilename, "%s/.ctwmrc.%d",
						        home, Scr->screen);
						break;
					}
				}
				continue;

			case 3:                       /* ~/.ctwmrc */
				if(home) {
					tmpfilename[homelen + 8] = '\0';
				}
				break;

			case 4:                       /* ~/.twmrc.screennum */
				if(!filename) {
					home = getenv("HOME");
					if(home) {
						homelen = strlen(home);
						cp = tmpfilename;
						sprintf(tmpfilename, "%s/.twmrc.%d",
						        home, Scr->screen);
						break;
					}
				}
				continue;

			case 5:                       /* ~/.twmrc */
				if(home) {
					tmpfilename[homelen + 7] = '\0'; /* C.L. */
				}
				break;

			case 6:                       /* system.twmrc */
				cp = SYSTEM_INIT_FILE;
				break;
		}

		if(cp) {
			twmrc = fopen(cp, "r");
		}
	}

	if(twmrc) {
		bool status;
#ifdef USEM4
		FILE *raw = NULL;
#endif

		/*
		 * If we wound up opening a config file that wasn't the filename
		 * we were passed, make sure the user knows about it.
		 */
		if(filename && strncmp(cp, filename, strlen(filename)) != 0) {
			fprintf(stderr,
			        "%s:  unable to open twmrc file %s, using %s instead\n",
			        ProgramName, filename, cp);
		}


		/*
		 * Kick off the parsing, however we do it.
		 */
#ifdef USEM4
		if(CLarg.GoThroughM4) {
			/*
			 * Hold onto raw filehandle so we can fclose() it below, and
			 * swap twmrc over to the output from m4
			 */
			raw = twmrc;
			twmrc = start_m4(raw);
		}
		status = doparse(m4twmFileInput, "file", cp);
		wait(0);
		fclose(twmrc);
		if(raw) {
			fclose(raw);
		}
#else
		status = doparse(twmFileInput, "file", cp);
		fclose(twmrc);
#endif

		/* And we're done */
		return status;
	}
	else {
		/*
		 * Couldn't find anything to open, fall back to our builtin
		 * config.
		 */
		if(filename) {
			fprintf(stderr,
			        "%s:  unable to open twmrc file %s, using built-in defaults instead\n",
			        ProgramName, filename);
		}
		return ParseStringList(defTwmrc);
	}

	/* NOTREACHED */
}

static bool
ParseStringList(const char **sl)
{
	stringListSource = sl;
	currentString = *sl;
	return doparse(twmStringListInput, "string list", NULL);
}


/*
 * Util used throughout the code (possibly often wrongly?)
 */
void twmrc_error_prefix(void)
{
	fprintf(stderr, "%s:  line %d:  ", ProgramName, twmrc_lineno);
}



/*
 * Everything below here is related to plumbing and firing off lex/yacc
 */


/*
 * Backend func that takes an input-providing func and hooks it up to the
 * lex/yacc parser to do the work
 */
static bool
doparse(int (*ifunc)(void), const char *srctypename,
        const char *srcname)
{
	ptr = 0;
	len = 0;
	twmrc_lineno = 0;
	ParseError = false;
	twmInputFunc = ifunc;
#ifdef NON_FLEX_LEX
	overflowlen = 0;
#endif

	yyparse();

	if(ParseError) {
		fprintf(stderr, "%s:  errors found in twm %s",
		        ProgramName, srctypename);
		if(srcname) {
			fprintf(stderr, " \"%s\"", srcname);
		}
		fprintf(stderr, "\n");
	}
	return !(ParseError);
}


/*
 * Various input routines for the lexer for the various sources of
 * config.
 */

#ifndef USEM4
#include <ctype.h>

/* This has Tom's include() funtionality.  This is utterly useless if you
 * can use m4 for the same thing.               Chris P. Ross */

#define MAX_INCLUDES 10

static struct incl {
	FILE *fp;
	char *name;
	int lineno;
} rc_includes[MAX_INCLUDES];
static int include_file = 0;


static int twmFileInput(void)
{
#ifdef NON_FLEX_LEX
	if(overflowlen) {
		return (int) overflowbuff[--overflowlen];
	}
#endif

	while(ptr == len) {
		while(include_file) {
			if(fgets(buff, BUF_LEN, rc_includes[include_file].fp) == NULL) {
				free(rc_includes[include_file].name);
				fclose(rc_includes[include_file].fp);
				twmrc_lineno = rc_includes[include_file--].lineno;
			}
			else {
				break;
			}
		}

		if(!include_file)
			if(fgets(buff, BUF_LEN, twmrc) == NULL) {
				return 0;
			}
		twmrc_lineno++;

		if(strncmp(buff, "include", 7) == 0) {
			/* Whoops, an include file! */
			char *p = buff + 7, *q;
			FILE *fp;

			while(isspace(*p)) {
				p++;
			}
			for(q = p; *q && !isspace(*q); q++) {
				continue;
			}
			*q = 0;

			if((fp = fopen(p, "r")) == NULL) {
				fprintf(stderr, "%s: Unable to open included init file %s\n",
				        ProgramName, p);
				continue;
			}
			if(++include_file >= MAX_INCLUDES) {
				fprintf(stderr, "%s: init file includes nested too deep\n",
				        ProgramName);
				continue;
			}
			rc_includes[include_file].fp = fp;
			rc_includes[include_file].lineno = twmrc_lineno;
			twmrc_lineno = 0;
			rc_includes[include_file].name = strdup(p);
			continue;
		}
		ptr = 0;
		len = strlen(buff);
	}
	return ((int)buff[ptr++]);
}
#else /* USEM4 */
/* If you're going to use m4, use this version instead.  Much simpler.
 * m4 ism's credit to Josh Osborne (stripes) */

static int m4twmFileInput(void)
{
	int line;
	static FILE *cp = NULL;

	if(cp == NULL && CLarg.keepM4_filename) {
		cp = fopen(CLarg.keepM4_filename, "w");
		if(cp == NULL) {
			fprintf(stderr,
			        "%s:  unable to create m4 output %s, ignoring\n",
			        ProgramName, CLarg.keepM4_filename);
			CLarg.keepM4_filename = NULL;
		}
	}

#ifdef NON_FLEX_LEX
	if(overflowlen) {
		return((int) overflowbuff[--overflowlen]);
	}
#endif

	while(ptr == len) {
nextline:
		if(fgets(buff, BUF_LEN, twmrc) == NULL) {
			if(cp) {
				fclose(cp);
			}
			return(0);
		}
		if(cp) {
			fputs(buff, cp);
		}

		if(sscanf(buff, "#line %d", &line)) {
			twmrc_lineno = line - 1;
			goto nextline;
		}
		else {
			twmrc_lineno++;
		}

		ptr = 0;
		len = strlen(buff);
	}
	return ((int)buff[ptr++]);
}
#endif /* USEM4 */


static int twmStringListInput(void)
{
#ifdef NON_FLEX_LEX
	if(overflowlen) {
		return (int) overflowbuff[--overflowlen];
	}
#endif

	/*
	 * return the character currently pointed to
	 */
	if(currentString) {
		unsigned int c = (unsigned int) * currentString++;

		if(c) {
			return c;        /* if non-nul char */
		}
		currentString = *++stringListSource;  /* advance to next bol */
		return '\n';                    /* but say that we hit last eol */
	}
	return 0;                           /* eof */
}



/*
 * unput/output funcs for AT&T lex.  No longer supported, and expected to
 * be GC'd in a release or two.
 */
#ifdef NON_FLEX_LEX

void twmUnput(int c)
{
	if(overflowlen < sizeof overflowbuff) {
		overflowbuff[overflowlen++] = (char) c;
	}
	else {
		twmrc_error_prefix();
		fprintf(stderr, "unable to unput character (%c)\n",
		        c);
	}
}

void TwmOutput(int c)
{
	putchar(c);
}

#endif /* NON_FLEX_LEX */
