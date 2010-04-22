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
/*  gethex --  prompt user for hexadecimal integer
 *
 *  Usage:  i = gethex (prompt,min,max,defalt)
 *	unsigned int i,min,max,defalt;
 *	char *prompt;
 *
 *  Gethex prints the message:  prompt  (min to max, hexadecimal)  [defalt]
 *  and accepts a line of input from the user.  If the input
 *  is not null or numeric, an error message is printed; otherwise,
 *  the value is converted to an hexadecimal integer (or the value "defalt" is
 *  substituted if the input is null).  Then, the value is
 *  checked to ensure that is lies within the range "min" to "max".
 *  If it does not, an error message is printed.  As long as
 *  errors occur, the cycle is repeated; when a legal value is
 *  entered, this value is returned by gethex.
 *  On error or EOF in the standard input, the default is returned.
 *
 *  HISTORY
 * $Log:	gethex.c,v $
 * Revision 1.2  90/12/11  17:54:22  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Now uses stderr for output.
 *
 * 23-Oct-82  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added code to return default on error or EOF in standard input.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.
 *
 */

#include <stdio.h>
#include <ctype.h>

unsigned int atoh();

unsigned int gethex (prompt,min,max,defalt)
unsigned int min,max,defalt;
char *prompt;
{
	char input [200];
	register char *p;
	register unsigned int i;
	register int err;

	fflush (stdout);
	do {

		fprintf (stderr,"%s  (%s%x to %s%x, hexadecimal)  [%s%x]  ",
			prompt,(min ? "0x" : ""),min,
			(max ? "0x" : ""),max,(defalt ? "0x" : ""),defalt);
		fflush (stderr);

		if (gets (input) == NULL) {
			i = defalt;
			err = (i < min || max < i);
		}
		else {
			err = 0;
			if (input[0]=='0' && (input[1]=='x' || input[1]=='X'))
				strcpy (input,input+2);
			for (p=input; *p && ((*p>='0' && *p<='9') ||
				(*p>='a' && *p<='f') || (*p>='A' && *p<='F'));
				p++) ;
			if (*p) {		/* non-numeric */
				err = 1;
			} 
			else {
				if (*input)	i = atoh (input);
				else		i = defalt;
				err = (i < min || max < i);
			}
		}

		if (err) {
			fprintf (stderr,"Must be an unsigned hexadecimal number between %s%x and %s%x\n",
			(min ? "0x" : ""),min,(max ? "0x" : ""),max);
		}
	} 
	while (err);

	return (i);
}
