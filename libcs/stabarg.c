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
/*  stabarg  --  parse string from string table
 *
 *  Usage:  i = stabarg (ptr,brk,prompt,table,defalt);
 *	int i;
 *	char **ptr,*brk,*prompt,**table,*defalt;
 *
 *  Stabarg parses an argument from the string pointed to by "ptr",
 *  and bumps ptr to point to the next argument in the string.
 *  The argument thus parsed is looked up in the string array
 *  "table", and its index is returned.  If the argument is ambiguous
 *  or not found in the table, getstab() is used to ask the user
 *  for a string and look it up.
 *  "Brk" is the list of characters which may terminate an argument;
 *  if 0, then " " is used.
 *
 *  The string table might be declared this way:
 *    char *table[] {"one","two","three",0};
 *  The last entry must be a zero.
 *
 *  HISTORY
 * $Log:	stabarg.c,v $
 * Revision 1.2  90/12/11  17:59:25  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Adapted for VAX from PDP-11.
 *
 */

#include <libc.h>

int stabarg (ptr,brk,prompt,table,defalt)
char **ptr,*brk,*prompt,**table,*defalt;
{
	register char *arg;
	register int i;

	arg = nxtarg (ptr,brk);		/* parse an argument */
	fflush (stdout);

	if (*arg) {			/* if there's an argument */
		i = stablk (arg,table,0);
	} 
	else {
		i = -1;
	}

	if (i < 0)  i = getstab (prompt,table,defalt);

	return (i);
}
