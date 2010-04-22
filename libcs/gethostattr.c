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
 * gethostattr(ptrs, ptrcnt)
 * char *ptrs[];
 * int ptrcnt;
 *
 * This routine takes a pointer to an array of ptrcnt character pointers.
 * It will place a pointer to each attribute of the current host into
 * this array.  The pointers are pointing to a static buffer, so they
 * should be copied if necessary.  The routine returns -1 on error, or
 * the number of pointers placed in the array.  In addition, the pointer
 * after the last pointer used is set to NULL if there is room in the
 * array.
 *
 **********************************************************************
 * HISTORY
 * $Log:	gethostattr.c,v $
 * Revision 1.5  91/07/02  18:42:19  neek
 * 	force gethostattr() to read the complete line of attributes,
 * 	not to stop if comes across a null (::) attribute.
 * 	[91/07/02            neek]
 * 
 * Revision 1.4  90/12/11  17:54:30  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 08-Nov-85  Jonathan McElravy (jm) at Carnegie-Mellon University
 *	  Added an fclose to close attributes file on success.
 *
 * 12-Oct-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Created.
 *
 **********************************************************************
 */

#include <stdio.h>
#include <libc.h>

static char buf[BUFSIZ];

gethostattr(ptrs, ptrcnt)
char *ptrs[];
int ptrcnt;
{
    char hname[256], *p, *q;
    int count;
    FILE *f;

    if (gethostname(hname, 256) < 0)
	return(-1);
    if ((f = fopen("/etc/attributes", "r")) == NULL)
	return(-1);
    while (p = fgets(buf, BUFSIZ, f)) {
	if (strcasecmp(nxtarg(&p, ":"), hname)) continue;
	fclose(f);
	count = 0;
	while (1)
	{
		if( (*(q = nxtarg(&p, ":\n"))) == 0 ) 
		{
			/* watch out for empty attributes, ie "::" */	
			if( _argbreak == ':'  ) 
				continue;
			else
				break;
		}
	    if (count < ptrcnt)
		ptrs[count] = q;
	    count++;
	}
	if (count < ptrcnt)
	    ptrs[count] = NULL;
	return(count);
    }
    fclose(f);
    return(-1);
}
