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
/*  filecopy  --  copy a file from here to there
 *
 *  Usage:  i = filecopy (here,there);
 *	int i, here, there;
 *
 *  Filecopy performs a fast copy of the file "here" to the
 *  file "there".  Here and there are both file descriptors of
 *  open files; here is open for input, and there for output.
 *  Filecopy returns 0 if all is OK; -1 on error.
 *
 *  I have performed some tests for possible improvements to filecopy.
 *  Using a buffer size of 10240 provides about a 1.5 times speedup
 *  over 512 for a file of about 200,000 bytes.  Of course, other
 *  buffer sized should also work; this is a rather arbitrary choice.
 *  I have also tried inserting special startup code to attempt
 *  to align either the input or the output file to lie on a
 *  physical (512-byte) block boundary prior to the big loop,
 *  but this presents only a small (about 5% speedup, so I've
 *  canned that code.  The simple thing seems to be good enough.
 *
 *  HISTORY
 * $Log:	filecopy.c,v $
 * Revision 1.2  90/12/11  17:52:57  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX; same as "filcopy" on PDP-11.  Bigger buffer
 *	size (20 physical blocks) seems to be a big win; aligning things
 *	on block boundaries seems to be a negligible improvement at
 *	considerable cost in complexity.
 *
 */

#define BUFFERSIZE 10240

int filecopy (here,there)
int here,there;
{
	register int kount;
	char buffer[BUFFERSIZE];
	kount = 0;
	while (kount == 0 && (kount=read(here,buffer,BUFFERSIZE)) > 0)
		kount -= write (there,buffer,kount);
	return (kount ? -1 : 0);
}
