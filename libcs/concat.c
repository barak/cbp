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
 * subroutine for concatenating strings into a buffer
 *
 * char *concat(buf, buflen, ptr1, ptr2, ..., NULL) { char *ep; return(ep); }
 * char *buf, *ptr1, *ptr2, ...;
 * int buflen;
 *
 * "buflen" should be sizeof("buf")
 * "buf" will be terminated by a null byte
 * "concat" will return a pointer to the null byte, if return is non-null
 *
 * concat will return null(0) under any of the following conditions:
 *    1) buf is null
 *    2) buflen <= 0
 *    3) buf was not large enough to hold the contents of all the ptrs.
 *
 **********************************************************************
 * HISTORY
 * $Log:	concat.c,v $
 * Revision 2.3  90/12/11  17:51:13  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * Revision 2.2  88/12/13  13:51:06  gm0w
 * 	Created.
 * 	[88/12/04            gm0w]
 * 
 **********************************************************************
 */

#include <stdio.h>
#include <varargs.h>

char *vconcat();

/*VARARGS2*/
char *
concat(buf, buflen, va_alist)
char *buf;
int buflen;
va_dcl
{
    char *ptr;
    va_list ap;

    va_start(ap);
    ptr = vconcat(buf, buflen, ap);
    va_end(ap);
    return(ptr);
}

char *
vconcat(buf, buflen, ap)
char *buf;
int buflen;
va_list ap;
{
    register char *arg, *ptr, *ep;

    if (buf == NULL)
	return(NULL);
    if (buflen <= 0)
	return(NULL);
    ptr = buf;
    *ptr = '\0';
    ep = buf + buflen;
    while (ptr != NULL && (arg = va_arg(ap, char *)) != NULL)
	while (*ptr = *arg++)
	    if (++ptr == ep) {
		ptr = NULL;
		break;
	    }
    return(ptr);
}
