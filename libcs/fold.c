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
/*  fold  --  perform case folding
 *
 *  Usage:  p = foldup (out,in);
 *	    p = folddown (out,in);
 *	char *p,*in,*out;
 *
 *  Fold performs case-folding, moving string "in" to
 *  "out" and folding one case to another en route.
 *  Folding may be upper-to-lower case (folddown) or
 *  lower-to-upper case.
 *  Foldup folds to upper case; folddown folds to lower case.
 *  The same string may be specified as both "in" and "out".
 *  The address of "out" is returned for convenience.
 *
 **********************************************************************
 * HISTORY
 * $Log:	fold.c,v $
 * Revision 1.2  90/12/11  17:53:16  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 23-Sep-86  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Unexported original fold routine.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.  The foldup() and folddown() routines
 *	are new.
 *
 **********************************************************************
 */

typedef enum {FOLDUP, FOLDDOWN} FOLDMODE;

static
char *fold (out,in,whichway)
char *in,*out;
FOLDMODE whichway;
{
	register char *i,*o;
	register char lower;
	char upper;
	int delta;

	switch (whichway)
	{
	case FOLDUP:
		lower = 'a';		/* lower bound of range to change */
		upper = 'z';		/* upper bound of range */
		delta = 'A' - 'a';	/* amount of change */
		break;
	case FOLDDOWN:
		lower = 'A';
		upper = 'Z';
		delta = 'a' - 'A';
	}

	i = in;
	o = out;
	do {
		if (*i >= lower && *i <= upper)		*o++ = *i++ + delta;
		else					*o++ = *i++;
	} 
	while (*i);
	*o = '\0';
	return (out);
}

char *foldup (out,in)
char *in,*out;
{
	return (fold(out,in,FOLDUP));
}

char *folddown (out,in)
char *in,*out;
{
	return (fold(out,in,FOLDDOWN));
}
