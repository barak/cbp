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
/*  fwantread  --  attempt to open file for input
 *
 *  Usage:  f = fwantread (path,file,fullname,prompt);
 *	FILE *f;
 *	char *path,*file,*fullname,*prompt;
 *
 *  Fwantread will search through the specified path for the
 *  specified file, attempting to open it for input if it is
 *  found.  If no such file is found, the user is given
 *  an opportunity to enter a new file name (after the prompt is
 *  printed).  The new file will then be sought using the same
 *  path.
 *  If the path is the null string, the file will be searched for
 *  with no prefix.  If the file name is null, the user will be
 *  prompted for a file name immediately.  The user can always
 *  abort fwantread by typing just a carriage return to the prompt.
 *  Fwantread will put the name of the successfully opened file into
 *  the "fullname" string (which therefore must be long enough to hold a
 *  complete file name), and return its file pointer; if no file
 *  is successfully found, 0 is returned.
 *
 *  HISTORY
 * $Log:	fwantread.c,v $
 * Revision 1.2  90/12/11  17:53:24  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 21-Oct-81  Fil Alleva (faa) at Carnegie-Mellon University
 *	Fixed bug which caused an infinite loop when getstr() got
 *	an EOT error and returned NULL. The error return was ignored
 *	and the value of "answer" was not changed which caused the loop.
 *
 * 28-Aug-80  Mike Accetta (mja) at Carnegie-Mellon University
 *	Fixed bug which used the "file" string to hold name typed at terminal
 *	even though "file" may have been passed as a constant.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.  Nobody can anticipate if it's a win or a lose
 *	to use path-searching; we'll have to see.
 *
 */

#include <stdio.h>

int strcmp();
FILE *fopenp();
char *getstr();

FILE *fwantread (path,file,fullname,prompt)
char *path,*file,*fullname,*prompt;
{
	register FILE *value;
	char myfile[200], *retval;

	if (*file == '\0') {
		getstr (prompt,"no file",myfile);
		if (strcmp(myfile,"no file") == 0)  return (0);
	}
	else
	    strcpy(myfile, file);

	do {
		value = fopenp (path,myfile,fullname,"r");
		if (value == 0) {
			if (*path && (*myfile != '/')) {
				sprintf (fullname,"Want to read %s in path \"%s\"",myfile,path);
			}
			else {
				sprintf (fullname,"Want to read %s",myfile);
			}
			perror (fullname);
			retval = getstr (prompt,"no file",myfile);
			if ((strcmp(myfile,"no file") == 0) || retval == NULL)
			    return (0);
		}
	} 
	while (value == 0);

	return (value);
}
