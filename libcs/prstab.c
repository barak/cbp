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
/*  prstab, fprstab  --  print list of strings
 *
 *  Usage:  prstab (table);
 *	    fprstab (file,table);
 *	char **table;
 *	FILE *file;
 *
 *  table is an array of pointers to strings, ending with a 0
 *  value.  This is the same format as "stablk" tables.
 *
 *  Prstab will attempt to print the strings in a concise format,
 *  using multiple columns if its heuristics indicate that this is
 *  desirable.
 *  Fprstab is the same, but you can specify the file instead of using
 *  stdout.
 *
 *  The heuristics are these:  assume that each column must be at
 *  least as wide as the longest string plus three blanks.  Figure
 *  out how many columns can fit on a line, and suppose that we use
 *  that many columns.  This represents the "widest" useable format.
 *  Now, see if this is too wide.  This means that there are just a
 *  few strings, and we would like them to be printed in fewer columns,
 *  with each column being a little bit longer.  The heuristic rule is
 *  that we will always use at least some minimum number of rows (8)
 *  if there are at least that many strings.
 *
 *  HISTORY
 * $Log:	prstab.c,v $
 * Revision 1.2  90/12/11  17:57:52  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 28-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Modified for 4.2 BSD.  Also added fprstab() routine.
 *
 * 15-Mar-83  Steven Shafer (sas) at Carnegie-Mellon University
 *	Fixed two bugs:  printing extra spaces at end of last column, and
 *	dividing by zero if ncol = 0 (i.e. very long string).
 *
 * 16-Apr-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created.
 *
 */

#include <stdio.h>

#define SPACE 5			/* min. space between columns */
#define MAXCOLS 71		/* max. cols on line */
#define MINROWS 8		/* min. rows to be printed */

prstab (list)
char **list;
{
	fprstab (stdout,list);
}

fprstab (file,list)
FILE *file;
char **list;
{
	register int nelem;	/* # elements in list */
	register int maxwidth;	/* widest element */
	register int i,l;	/* temps */
	register int row,col;	/* current position */
	register int nrow,ncol;	/* desired format */
	char format[20];	/* format for printing strings */

	maxwidth = 0;
	for (i=0; list[i]; i++) {
		l = strlen (list[i]);
		if (l > maxwidth)  maxwidth = l;
	}

	nelem = i;
	if (nelem <= 0)  return;

	ncol = MAXCOLS / (maxwidth + SPACE);
	if (ncol < 1)  ncol = 1;	/* for very long strings */
	if (ncol > (nelem + MINROWS - 1) / MINROWS)
		ncol = (nelem + MINROWS - 1) / MINROWS;
	nrow = (nelem + ncol - 1) / ncol;

	sprintf (format,"%%-%ds",maxwidth+SPACE);

	for (row=0; row<nrow; row++) {
		fprintf (file,"\t");
		for (col=0; col<ncol; col++) {
			i = row + (col * nrow);
			if (i < nelem) {
				if (col < ncol - 1) {
					fprintf (file,format,list[i]);
				}
				else {
					fprintf (file,"%s",list[i]);
				}
			}
		}
		fprintf (file,"\n");
	}
}
