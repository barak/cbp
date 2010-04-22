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
/*  movefile  --  change name of file
 *
 *  Usage:  int movefile (oldname,newname);
 *    char *oldname,*newname;
 *
 *  Movefile attempts to change the name of a file.
 *  It tries to perform link and unlink, since this is
 *  the fastest way to rename a file.  However, this
 *  will fail in certain circumstances (e.g. if the old and
 *  new names specify different mounted devices).  So, if
 *  the link/unlink fails, movefile will perform opens
 *  and make a fast copy of the file using filecopy.
 *
 *  If movefile is successful, the original file will be
 *  unlinked (removed), and the new file will have the
 *  same mode as the original.  Movefile will return a 0.
 *  If unsuccessful, the original file will remain, but
 *  any file with the same name as the desired file will
 *  be deleted.
 *  Movefile will return -1 if unsuccessful.
 *
 *  Movefile is useful for any program which creates a temporary
 *  file, then wishes to overwrite (or create) another file
 *  using that temp file.
 *
 *  HISTORY
 * $Log:	movefile.c,v $
 * Revision 1.2  90/12/11  17:57:02  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 03-Jun-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Changed for 4.2 BSD UNIX to use new rename() and open() calls.
 *	Name changed from "rename" to "movefile" to avoid conflict with
 *	new rename() system call.
 *
 * 18-Feb-83  Steven Shafer (sas) at Carnegie-Mellon University
 *	Fixed bug in call to "link" which prevented it from ever working.
 *
 * 10-Dec-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created.
 *
 */

#include "sys/types.h"
#include "sys/stat.h"
#include "sys/file.h"

int movefile (from,to)
char *from, *to;
{
	register int f,t;	/* file descriptors */
	register int x;		/* system call status */
	struct stat statbuf;	/* for old file mode */

	if (rename(from,to) >= 0) {	/* link successful */
		return (0);
	}

	x = stat (from,&statbuf);	/* we need the old file mode */
	if (x<0)	return (-1);

	f = open (from,0);	/* read original */
	if (f<0)	return (-1);
	t = open (to,(O_WRONLY|O_CREAT|O_TRUNC),statbuf.st_mode & 07777);	/* create dest. file */
	if (t<0)	{close(f);	return (-1);}

	x = filecopy (f,t);		/* fast file copy */

	close (f);
	close (t);
	if (x >= 0)	unlink (from);
	return (x);
}
