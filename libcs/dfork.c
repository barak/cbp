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
#include <c.h>
/*
 * This routine does a double fork so that the (grand)child won't
 * hang on exit because the (grand)parent never wait(2)ed for it.
 * Note: grandchild will be inherited by init (proc 1) on exit.
 **********************************************************************
 * HISTORY
 * $Log:	dfork.c,v $
 * Revision 1.2  90/12/11  17:51:32  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 30-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Adapted for 4.2 BSD UNIX.  Inserted READ and WRITE macros here
 *	rather than using <modes.h>, which is obsolete.
 *
 * 13-Mar-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Made code more careful about closing pipes in cases of system
 *	call failures.
 *
 */

#define READ 0
#define WRITE 1

dfork() {
    int pid;
    int fildes[2];

    if (pipe(fildes) == CERROR)
	return(CERROR);
    if ((pid = fork()) == CERROR) {
	close(fildes[READ]);
	close(fildes[WRITE]);
	return(CERROR);
    }
    if (pid) {
	close(fildes[WRITE]);
	while (wait(0) != pid);
	if (read(fildes[READ], &pid, sizeof(int)) != sizeof(int))
	    pid = CERROR;
	close(fildes[READ]);
	return(pid);
    } else {
	close(fildes[READ]);
	if ((pid = fork()) == CERROR)
	    exit(CERROR);
	if (pid) {
	    write(fildes[WRITE], &pid, sizeof(int));
	    close(fildes[WRITE]);
	    exit(0);
	}
	close(fildes[WRITE]);
	return(0);
    }
}
