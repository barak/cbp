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
 *	bsearch -- generic binary search, like qsort.
 *
 **********************************************************************
 * HISTORY
 * $Log:	bsearch.c,v $
 * Revision 1.3  90/12/11  17:50:40  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 09-May-87  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Added comment that a return of -1 is possible.
 *
 * 03-Feb-86  David Nichols (nichols) at Carnegie-Mellon University
 *	Created.
 *
 **********************************************************************
 */

/* Bsearch does a binary search on the array.  It returns either the index
   of the matching element, the index of the largest element less than the
   key, if such an element exists, otherwise returns -1 if not.  The caller
   must use the compar function to determine if the key was found or not
   when the index returned is non-negative. */
int bsearch (base, nel, width, key, compar)
    char *base;			/* start of the array to search */
    int nel;			/* size of the array */
    int width;			/* size of an element of the array */
    char *key;			/* pointer to a key to find */
    int (*compar)();		/* comparison routine like in qsort */
{
    int     l = 0;
    int     u = nel - 1;
    int     m;
    int     r;

    /* Invariant: key > all elements in [0..l) and key < all elements in
       (u..nel-1]. */
    while (l <= u) {
	m = (l + u) / 2;
	r = (*compar) (key, base + (m * width));
	if (r == 0)
	    return m;
	else if (r < 0)
	    u = m - 1;
	else
	    l = m + 1;
    }
    return u;			/* Which should == l - 1 */
}
