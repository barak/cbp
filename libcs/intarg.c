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
/*  intarg  --  parse integer argument
 *
 *  Usage:  i = intarg (ptr,brk,prompt,min,max,default)
 *    int i,min,max,default;
 *    char **ptr,*brk,*prompt;
 *
 *  Will attempt to parse an argument from the string pointed to
 *  by "ptr", incrementing ptr to point to the next arg.  If
 *  an arg is found, it is converted into an integer.  If there is
 *  no arg or the value of the arg is not within the range
 *  [min..max], then "getint" is called to ask the user for an
 *  integer value.
 *  "Brk" is the list of characters which may terminate an argument;
 *  if 0, then " " is used.
 *
 *  HISTORY
 * $Log:	intarg.c,v $
 * Revision 1.2  90/12/11  17:56:28  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Now puts output on stderr.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.
 *
 */

#include <ctype.h>
#include <stdio.h>

int getint();
char *nxtarg();
long atol();
int strcmp();

int intarg (ptr,brk,prompt,min,max,defalt)
char **ptr;
char *brk,*prompt;
int min,max,defalt;
{
	register int i;
	register char *arg,*p;

	arg = nxtarg (ptr,brk);
	fflush (stdout);

	if (*arg != '\0') {		/* if there was an arg */
		for (p=arg; *p && (isdigit(*p) || *p == '-' || *p == '+'); p++) ;
		if (*p) {
			if (strcmp(arg,"?") != 0)  fprintf (stderr,"%s not numeric.  ",arg);
		} 
		else {
			i = atol (arg);
			if (i<min || i>max) {
				fprintf (stderr,"%d out of range.  ",i);
			}
		}
	}

	if (*arg == '\0' || *p != '\0' || i<min || i>max) {
		i = getint (prompt,min,max,defalt);
	}

	return (i);
}
