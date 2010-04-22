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
/*  getsearch  --  ask user for string in table
 *
 *  Usage:  i = getsearch (prompt,table,defalt)
 *	int i;
 *	char *prompt, **table, *defalt;
 *
 *  Getsearch prints the messge:   prompt  [defalt]
 *  and asks the user to type in a line.  This input text
 *  is compared to all the strings in the table to see which
 *  (if any) it matches; the stabsearch() routine is used
 *  for the matching.  If the string is ambiguous or invalid
 *  (i.e. matches zero strings, or more than one), the cycle
 *  is repeated.  When a valid string is typed, the index
 *  of the string it matches is returned.  If the user just
 *  types carriage return, the default string is used for matching.
 *  The default value is also used on error or EOF in standard input.
 *  The string table may be declared in this way:
 * 	char *table[] = {"string1","string2",...,0};
 *
 *  HISTORY
 * $Log:	getsearch.c,v $
 * Revision 1.2  90/12/11  17:55:23  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Now uses stderr for output.
 *
 * 23-Oct-82  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added code to use default value on EOF or error in standard input.
 *
 * 23-Jan-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created.  This is just an alternative to getstab, but with funny
 *	heuristic string-matching and the nifty "May I suggest..." stuff.
 *
 */

#include <strings.h>
#include <stdio.h>
#include <c.h>

int getsearch (prompt,table,defalt)
char *prompt, **table, *defalt;
{
	char input[200];
	register int ix;

	fflush (stdout);
	do {
		fprintf (stderr,"%s  [%s]  ",prompt, defalt);
		fflush (stderr);
		if (gets (input) == NULL)  strcpy (input,defalt);
		if (*input == '\0')  strcpy (input,defalt);
		ix = stabsearch (input,table,FALSE);
	} 
	while (ix < 0);
}
