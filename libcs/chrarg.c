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
/*  chrarg  --  parse character and return its index
 *
 *  Usage:  i = chrarg (ptr,brk,prompt,legals,defalt);
 *	int i;
 *	char **ptr,*brk,*prompt,*legals,defalt;
 *
 *  Chrarg will parse an argument from the string pointed to by "ptr",
 *  bumping ptr to point to the next argument.  The first character
 *  of the arg will be searched for in "legals", and its index
 *  returned; if it is not found, or if there is no argument, then
 *  getchr() will be used to ask the user for a character.
 *  "Brk" is the list of characters which may terminate an argument;
 *  if it is 0, then " " is used.
 *
 *  HISTORY
 * $Log:	chrarg.c,v $
 * Revision 1.2  90/12/11  17:50:49  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Now puts output on stderr.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.
 *
 */

#include <stdio.h>

int strcmp(), getchr();
char *index(),*nxtarg();

int chrarg (ptr,brk,prompt,legals,defalt)
char **ptr, *brk, *prompt, *legals, defalt;
{
	register int i;
	register char *arg,*p;

	i = -1;			/* bogus value */
	fflush (stdout);

	arg = nxtarg (ptr,brk);	/* parse argument */

	if (*arg) {		/* there was an arg */
		p = index (legals,*arg);
		if (p) {
			i = p - legals;
		} 
		else if (strcmp("?",arg) != 0) {
			fprintf (stderr,"%s: not valid.  ",arg);
		}
	}

	if (i < 0) {
		i = getchr (prompt,legals,defalt);
	}

	return (i);
}
