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
/*  srchscore --  perform approximate string matching
 *
 *  int srchscore (big,small);
 *  tells how well "small" matches substrings of "big".
 *  The score ranges from 0 to (strlen(small) ** 2).
 *  The score is something like the sum, over the longest
 *  matching contiguous substrings, of the square of the
 *  length of the substring minus some penalty for
 *  not-quite-exactly-matching substrings.  Case is not significant.
 *
 *  The value ranges from 0 to strlen(small)**2.
 *
 * HISTORY
 * $Log:	srchscore.c,v $
 * Revision 1.2  90/12/11  17:59:18  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 06-Feb-81  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added limit before "return" so value is never negative.
 *
 * 23-Jan-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created.  The same as Dave McKeown's matching function from the
 *	PDP-11; only the identifiers have been changed to protect somebody.
 *
 */

#define INSERTCOST 1	/* penalty to insert 1 char in small */
#define DELETECOST 1	/* penalty to delete 1 char from small */
#define CHANGECOST 3	/* penalty to change 1 char in small */

static char *newsmall;	/* next character to match in "small" */

static int smatch (big,small)
char *big,*small;
{
	register char *b,*s;	/* current chars in big and small */
	register int goon;	/* TRUE while still looping */
	int nmatch, penalty;	/* match count; total penalty */

	nmatch = 0;
	penalty = 0;
	goon = 1;
	b = big;
	s = small;

	while (*b && *s && goon) {
		if (*b == *s) {
			/* just do stuff at end of loop */
		}
		else if (*(b+1) == *s) {
			b++;
			penalty += INSERTCOST;
		}
		else if (*b == *(s+1)) {
			s++;
			penalty += DELETECOST;
		}
		else if (*(b+1) == *(s+1)) {
			b++; s++;
			penalty += CHANGECOST;
		}
		else {
			goon = 0;
		}
		if (goon) {	/* something matched */
			b++; s++;
			nmatch++;
		}
	}
	newsmall = s;
	return ((nmatch * nmatch) - penalty);
}

int srchscore (big,small)
char *big,*small;
{
	register int score,bestscore,total;
	char *b,*s,*bestsmall;

	total = 0;

	if (*big) {
		s = small;
		while (*s) {
			bestscore = -1;
			for (b=big; *b; b++) {
				score = smatch (b,s);
				if (score > bestscore) {
					bestscore = score;
					bestsmall = newsmall;
				}
			}
			if (bestsmall > s) {
				total += bestscore;
				s = bestsmall;
			}
			else {
				if (total >= DELETECOST-1)  total -= DELETECOST;
				s++;
			}
		}
	}
	if (total < 0)  total = 0;
	return (total);
}
