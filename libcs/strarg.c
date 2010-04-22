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
/*  strarg  --  parse a string
 *
 *  Usage:  p = strarg (ptr, brk, prompt, defalt, buf);
 *	char *p, **ptr, *brk, *prompt, *defalt, *buf;
 *
 *  Strarg will parse an argument from the string pointed to by "ptr",
 *  bumping ptr to point to the next argument in the string.
 *  The argument parsed will be copied into "buf".  If there is no
 *  argument, then getstr() will be called, and the value placed into
 *  buf.  In any event, the address of buf is returned as a value.
 *  "Brk" is the list of characters which terminate an argument;
 *  if 0, then " " is used.
 *
 *  HISTORY
 * $Log:	strarg.c,v $
 * Revision 1.2  90/12/11  17:59:59  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.
 *
 */

#include <libc.h>

char *strarg (ptr, brk, prompt, defalt, buf)
char **ptr, *brk, *prompt, *defalt, *buf;
{
	register char *arg;

	arg = nxtarg (ptr,brk);		/* parse an argument */
	fflush (stdout);

	if (*arg && strcmp (arg,"?")!=0)  strcpy (buf,arg);
	else getstr (prompt,defalt,buf);

	return (buf);
}
