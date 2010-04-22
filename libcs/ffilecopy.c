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
/*  ffilecopy  --  very fast buffered file copy
 *
 *  Usage:  i = ffilecopy (here,there)
 *	int i;
 *	FILE *here, *there;
 *
 *  Ffilecopy is a very fast routine to copy the rest of a buffered
 *  input file to a buffered output file.  Here and there are open
 *  buffers for reading and writing (respectively); ffilecopy
 *  performs a file-copy faster than you should expect to do it
 *  yourself.  Ffilecopy returns 0 if everything was OK; EOF if
 *  there was any error.  Normally, the input file will be left in
 *  EOF state (feof(here) will return TRUE), and the output file will be
 *  flushed (i.e. all data on the file rather in the core buffer).
 *  It is not necessary to flush the output file before ffilecopy.
 *
 *  HISTORY
 * $Log:	ffilecopy.c,v $
 * Revision 1.2  90/12/11  17:52:41  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.
 *
 */

#include <stdio.h>
int filecopy();

int ffilecopy (here,there)
FILE *here, *there;
{
	register int i, herefile, therefile;

	herefile = fileno(here);
	therefile = fileno(there);

	if (fflush (there) == EOF)		/* flush pending output */
		return (EOF);

	if ((here->_cnt) > 0) {			/* flush buffered input */
		i = write (therefile, here->_ptr, here->_cnt);
		if (i != here->_cnt)  return (EOF);
		here->_ptr = here->_base;
		here->_cnt = 0;
	}

	i = filecopy (herefile, therefile);	/* fast file copy */
	if (i < 0)  return (EOF);

	(here->_flag) |= _IOEOF;		/* indicate EOF */

	return (0);
}
