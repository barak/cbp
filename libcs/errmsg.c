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
/*****************************************************************
 * HISTORY
 * $Log:	errmsg.c,v $
 * Revision 1.2  90/12/11  17:52:09  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 04-Mar-85  Rudy Nedved (ern) at Carnegie-Mellon University
 *	Create a CMU version of the BBN errmsg routine from scratch. It
 *	differs from the BBN errmsg routine in the fact that it uses a
 *	negative value to indicate using the current errno value...the
 *	BBN uses a negative OR zero value.
 */

extern int	errno;
extern int	sys_nerr;
extern char	*sys_errlist[];

static char *itoa(p,n)
char *p;
unsigned n;
{
    if (n >= 10)
	p =itoa(p,n/10);
    *p++ = (n%10)+'0';
    return(p);
}

char *errmsg(cod)
int cod;
{
	static char unkmsg[] = "Unknown error ";
	static char unk[sizeof(unkmsg)+11];		/* trust us */

	if (cod < 0) cod = errno;

	if((cod >= 0) && (cod < sys_nerr))
	    return(sys_errlist[cod]);

	strcpy(unk,unkmsg);
	*itoa(&unk[sizeof(unkmsg)-1],cod) = '\0';

	return(unk);
}
