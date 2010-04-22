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
 *  editor  --  fork editor to edit some text file
 *
 *  Usage:
 *	i = editor(file, prompt);
 *	char *file, *prompt;
 *	int i;
 *
 *  The editor() routine is used to fork the user's favorite editor.
 *  There is assumed to be an environment variable named "EDITOR" whose
 *  value is the name of the favored editor.  If the EDITOR parameter is
 *  missing, some default (see DEFAULTED below) is assumed.  The runp()
 *  routine is then used to find this editor on the searchlist specified
 *  by the PATH variable (or the default path).  "file" is the name of
 *  the file to be edited and "prompt" is a string (of any length) which
 *  will be printed in a such a way that the user can see it at least at
 *  the start of the editing session.  editor() returns the value of the
 *  runp() call.
 *
 **********************************************************************
 * HISTORY
 * $Log:	editor.c,v $
 * Revision 1.2  90/12/11  17:51:56  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 22-Nov-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Rewritten for 4.2 BSD UNIX.
 *
 **********************************************************************
 */

#include <libc.h>

#define DEFAULTED "emacs"

int editor(file, prompt)
register char *file, *prompt;
{
	register char *editor;

	if ((editor = getenv("EDITOR")) == NULL)
		editor = DEFAULTED;
	if (*prompt) printf("%s\n", prompt);
	return(runp(editor, editor, file, 0));
}
