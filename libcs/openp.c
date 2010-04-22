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
/*  openp, fopenp  --  search pathlist and open file
 *
 *  Usage:
 *	i = openp (path,file,complete,flags,mode)
 *	f = fopenp (path,file,complete,type)
 *	int i,flags,mode;
 *	FILE *f;
 *	char *path,*file,*complete,*type;
 *
 *  Openp searches for "file" in the pathlist "path";
 *  when the file is found and can be opened by open()
 *  with the specified "flags" and "mode", then the full filename
 *  is copied into "complete" and openp returns the file
 *  descriptor.  If no such file is found, openp returns -1.
 *  Fopenp performs the same function, using fopen() instead
 *  of open() and type instead of flags/mode; it returns 0 if no
 *  file is found.
 *
 *  HISTORY
 * $Log:	openp.c,v $
 * Revision 1.2  90/12/11  17:57:28  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 30-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Adapted for 4.2 BSD UNIX.  Added new parameter to openp.c;
 *	changed names of flags, mode, and type parameters to reflect
 *	current manual entries for open and fopen.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.
 *
 */

#include <stdio.h>

int open();
int searchp();

static int flgs,mod,value;
static char *ftyp;
static FILE *fvalue;

static int func (fnam)
char *fnam;
{
	value = open (fnam,flgs,mod);
	return (value < 0);
}

static int ffunc (fnam)
char *fnam;
{
	fvalue = fopen (fnam,ftyp);
	return (fvalue == 0);
}

int openp (path,file,complete,flags,mode)
char *path, *file, *complete;
int flags, mode;
{
	register char *p;
	flgs = flags;
	mod = mode;
	if (searchp(path,file,complete,func) < 0)  return (-1);
	return (value);
}

FILE *fopenp (path,file,complete,ftype)
char *path, *file, *complete, *ftype;
{
	register char *p;
	ftyp = ftype;
	if (searchp(path,file,complete,ffunc) < 0)  return (0);
	return (fvalue);
}
