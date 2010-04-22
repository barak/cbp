/*
 * Copyright (c) 1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND CARNEGIE MELLON UNIVERSITY
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO EVENT
 * SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Users of this software agree to return to Carnegie Mellon any
 * improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 *
 * Export of this software is permitted only after complying with the
 * regulations of the U.S. Deptartment of Commerce relating to the
 * Export of Technical Data.
 */
/*  stablk  --  string table lookup
 *
 *  Usage:  i = stablk (arg,table,quiet);
 *
 *	int i;
 *	char *arg,**table;
 *	int quiet;
 *
 *  Stablk looks for a string in "table" which matches
 *  "arg".  Table is declared like this:
 *    char *table[] = {"string1","string2",...,0};
 *  Each string in the table is checked via stablk() to determine
 *  if its initial characters match arg.  If exactly one such
 *  string matches arg, then the index of that string is returned.
 *  If none match arg, or if several match, then -1 (respectively -2)
 *  is returned.  Also, for either of these errors, if quiet is
 *  FALSE, the user will be asked if he wants a list of the possible
 *  strings.  In the case of multiple matches, the matching strings
 *  will be marked specially.
 *
 *  HISTORY
 * $Log:	stablk.c,v $
 * Revision 1.2  90/12/11  17:59:32  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Now puts output on std. error using fprintf and
 *	fprstab.
 *
 * 08-Sep-81  Steven Shafer (sas) at Carnegie-Mellon University
 *	Now handles case of multiple exact matches just like case of
 *	multiple initial-substring matches:  returns -2 if "quiet", else
 *	asks user which one (as if it matters).
 *
 * 19-May-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added exactmatch and code to recognize exact match in case of
 *	ambiguity from initial prefix matching.
 *
 * 16-Apr-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Changed listing code to use prstab() instead of just printing
 *	table -- this uses multiple columns when appropriate.  To do this,
 *	it was necessary to add the "matches" array.  Too bad!
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX from Ken Greer's routine.  The error messages are
 *	different now.
 *
 *  Originally from klg (Ken Greer) on IUS/SUS UNIX.
 */

#include <c.h>
#include <strings.h>
#include <stdio.h>

#define NOTFOUND -1
#define AMBIGUOUS -2
#define MAXSTRINGS 500

int stlmatch();
int strcmp();

int stablk (arg,table,quiet)
char *arg, **table;
int quiet;
{
	register int i,ix,count;
	int wantlist;
	char *matches[MAXSTRINGS];
	int exactmatch;

	count = 0;
	exactmatch = 0;
	for (i=0; table[i] != 0 && exactmatch == 0; i++) {
		if (stlmatch (table[i],arg)) {
			ix = i;		/* index of last match */
			matches[count++] = table[i];
			if (strcmp(table[i],arg) == 0)  exactmatch = 1;
		}
	}
	matches[count] = 0;

	if (exactmatch) {	/* i-th entry is exact match */
		--i;		/* (actually, i-1th entry) */
		matches[0] = table[i];
		count = 1;
		for (i=i+1; table[i] != 0; i++) {
			if (strcmp(table[i],arg) == 0)  {
				matches[count++] = table[i];
				ix = i;
			}
		}
		matches[count] = 0;
	}

	if (count == 1)  return (ix);

	if (!quiet) {
		if (strcmp(arg,"?") == 0) {
			wantlist = TRUE;
		}
		else {
			fprintf (stderr,"%s is %s.  ",arg,(count ? "ambiguous" : "unknown"));
			wantlist = getbool ("Do you want a list?",TRUE);
		}
		if (wantlist) {
			fprintf (stderr,"Must match one of these:\n");
			if (count)  fprstab (stderr,matches);
			else	    fprstab (stderr,table);
		}
	}
	return (count ? AMBIGUOUS : NOTFOUND);
}
