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
/*  wantwrite  --  attempt to open file for output
 *
 *  Usage:  i = wantwrite (path,file,fullname,prompt,warn);
 *    int i,warn;
 *    char *path,*file,*fullname,*prompt;
 *
 *  Wantwrite will attempt to open "file" for output somewhere in
 *  the pathlist "path".  If no such file can be created, the user
 *  is given an oportuity to enter a new file name (after the
 *  prompt is printed).  The new file will then be sought using
 *  the same path.
 *  If the path is the null string, the file will be created with
 *  no prefix to the file name.  If the file name is null, the
 *  user will be prompted for a file name immediately.  The user
 *  can always abort wantwrite by typing just a carriage return
 *  to the prompt.
 *  Wantwrite will put the name of the successfully created file
 *  into the "fullname" string (which must therefore be long enough to
 *  hold a complete file name), and return its file descriptor;
 *  if no file is created, -1 will be returned.
 *  Wantwrite examines each entry in the path, to see if the
 *  desired file can be created there.  If "warn" is true, 
 *  wantwrite will first check to ensure that no such file
 *  already exists (if it does, the user is allowed to keep it).
 *  If no such file exists (or the user wants to delete it), then
 *  wantwrite attempts to create the file.  If it is unsuccessful,
 *  the next entry in the pathlist is examined in the same way.
 *
 *  HISTORY
 * $Log:	wantwrite.c,v $
 * Revision 1.2  90/12/11  18:01:02  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 30-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Adapted for 4.2 BSD UNIX:  New open call, output moved to stderr.
 *
 * 21-Oct-81  Fil Alleva (faa) at Carnegie-Mellon University
 *	Fixed bug which caused an infinite loop when getstr() got
 *	an EOT error and returned NULL. The error return was ignored
 *	and the value of "answer" was not changed which caused the loop.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Rewritten for VAX.
 *
 */

#include <stdio.h>
#include <sys/file.h>

int strcmp();
int creat();
int searchp();
char *getstr();

static int warnflag;
static int fildes;

static int func (fnam)
char *fnam;
{		/* attempt to create fnam */
	register int goahead;
	goahead = 1;
	if (warnflag) {
		fildes = open (fnam,O_WRONLY,0);
		if (fildes >= 0) {
			close (fildes);
			fprintf (stderr,"%s already exists!  ",fnam);
			goahead = getbool ("Delete old file?",0);
		}
	}
	if (goahead) {
		fildes = open (fnam,(O_WRONLY|O_CREAT|O_TRUNC),0644);
		if (fildes < 0) {
			goahead = 0;
		}
	}
	return (!goahead);
}

int wantwrite (path,file,fullname,prompt,warn)
char *path,*file,*fullname,*prompt;
int warn;
{
	register int i;
	char myfile [2000], *retval;

	fflush (stdout);
	if (*file == '\0') {
		getstr (prompt,"no file",myfile);
		if (strcmp(myfile,"no file") == 0)  return (-1);
	}
	else strcpy (myfile,file);

	warnflag = warn;
	do {
		i = searchp (path,myfile,fullname,func);
		if (i < 0) {
			if (*path && (*myfile != '/')) {
				fprintf (stderr,"%s in path \"%s\":  Can't create.\n",myfile,path);
			} 
			else {
				perror (myfile);
			}
			retval = getstr (prompt,"no file",myfile);
			if ((strcmp(myfile,"no file") == 0) || retval == NULL)
			    return (-1);
		}
	} 
	while (i < 0);

	return (fildes);
}
