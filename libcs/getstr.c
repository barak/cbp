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
/*  getstr --  prompt user for a string
 *
 *  Usage:  p = getstr (prompt,defalt,answer);
 *	char *p,*prompt,*defalt,*answer;
 *
 *  Getstr prints this message:  prompt  [defalt]
 *  and accepts a line of input from the user.  This line is
 *  entered into "answer", which must be a big char array;
 *  if the user types just carriage return, then the string
 *  "defalt" is copied into answer.
 *  Value returned by getstr is just the same as answer,
 *  i.e. pointer to result string.
 *  The default value is used on error or EOF in the standard input.
 *
 *  HISTORY
 * $Log:	getstr.c,v $
 * Revision 1.2  90/12/11  17:56:00  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Now uses stderr for output.
 *
 * 23-Oct-82  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added code to copy default to answer (in addition to Fil's code to
 *	return NULL) on error or EOF in the standard input.
 *
 * 21-Oct-80  Fil Alleva (faa) at Carnegie-Mellon University
 *	Getstr() now percuolates any errors from gets() up to the calling
 *	routine.
 *
 * 19-May-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Increased buffer size to 4000 characters.  Why not?
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.  Mike thinks a 0 pointer for the default should
 *	print no default (i.e. not even braces); I'm not sure I like the idea
 *	of a routine that doesn't explicitly tell you what happens if you
 *	just hit Carriage Return.
 *
 */

#include <stdio.h>

char *getstr (prompt,defalt,answer)
char *prompt,*defalt,*answer;
{
	char defbuf[4000];
	register char *retval;

	fflush (stdout);
	fprintf (stderr,"%s  [%s]  ",prompt,defalt);
	fflush (stderr);
	strcpy (defbuf,defalt);
	retval = (char *) gets (answer);
	if (retval == NULL || *answer == '\0')  strcpy (answer,defbuf);
	if (retval == NULL)
	    return (retval);
	else
	    return (answer);
}
