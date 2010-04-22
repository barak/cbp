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
/*
 *  quit  --  print message and exit
 *
 *  Usage:  quit (status,format [,arg]...);
 *	int status;
 *	(... format and arg[s] make up a printf-arglist)
 *
 *  Quit is a way to easily print an arbitrary message and exit.
 *  It is most useful for error exits from a program:
 *	if (open (...) < 0) then quit (1,"Can't open...",file);
 *
 **********************************************************************
 * HISTORY
 * $Log:	quit.c,v $
 * Revision 1.3  90/12/11  17:58:02  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * Revision 1.2  88/12/13  13:52:41  gm0w
 * 	Rewritten to use varargs.
 * 	[88/12/13            gm0w]
 * 
 **********************************************************************
 */

#include <stdio.h>
#include <varargs.h>

quit (status, fmt, va_alist)
int status;
char *fmt;
va_dcl
{
	va_list args;

	fflush(stdout);
	va_start(args);
	(void) vfprintf(stderr, fmt, args);
	va_end(args);
	exit(status);
}
