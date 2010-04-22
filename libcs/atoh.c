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
/*  atoh  --  convert ascii to hexidecimal
 *
 *  Usage:  i = atoh (string);
 *	unsigned int i;
 *	char *string;
 *
 *  Atoo converts the value contained in "string" into an
 *  unsigned integer, assuming that the value represents
 *  a hexidecimal number.
 *
 * HISTORY
 * $Log:	atoh.c,v $
 * Revision 1.4  92/06/15  14:40:08  chs
 * 	Made CMUCS conditional unconditional.
 * 	[92/06/15  14:39:02  chs]
 * 
 * Revision 1.3  91/07/03  12:55:45  neek
 * 	skip leading "0x" or "0X" if present.
 * 	[91/07/03            neek]
 * 
 * Revision 1.2  90/12/11  17:50:07  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.
 *
 */

unsigned int atoh(ap)
char *ap;
{
	register char *p;
	register unsigned int n;
	register int digit,lcase;

	p = ap;
	n = 0;
	while(*p == ' ' || *p == '	')
		p++;

	if(*p == '0' && ((*(p+1) == 'x') || (*(p+1) == 'X')))
		p+=2;

	while ((digit = (*p >= '0' && *p <= '9')) ||
		(lcase = (*p >= 'a' && *p <= 'f')) ||
		(*p >= 'A' && *p <= 'F')) {
		n *= 16;
		if (digit)	n += *p++ - '0';
		else if (lcase)	n += 10 + (*p++ - 'a');
		else		n += 10 + (*p++ - 'A');
	}
	return(n);
}
