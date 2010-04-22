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
/*  run[c], run[c]v, run[c]p, run[c]vp -- exec process and wait for it to exit
 *
 *  Usage:
 *	i = run (file, arg1, arg2, ..., argn, 0);
 *	i = runv (file, arglist);
 *	i = runp (file, arg1, arg2, ..., argn, 0);
 *	i = runvp (file, arglist);
 *	i = runc (func, file, arg1, arg2, ..., argn, 0);
 *	i = runcv (func, file, arglist);
 *	i = runcp (func, file, arg1, arg2, ..., argn, 0);
 *	i = runcvp (func, file, arglist);
 *
 *  Run, runv, runp, runvp and runc, runcv, runcp, runcvp have argument lists
 *  exactly like the corresponding routines, execl, execv, execlp, execvp.  The
 *  run routines perform a fork, then:
 *  IN THE NEW PROCESS, an execl[p] or execv[p] is performed with the specified
 *  arguments (after first invoking the supplied function in the runc* cases).
 *  The process returns with a -1 code if the exec was not successful.
 *  IN THE PARENT PROCESS, the signals SIGQUIT and SIGINT are disabled,
 *  the process waits until the newly forked process exits, the
 *  signals are restored to their original status, and the return
 *  status of the process is analyzed.
 *  All run routines return:  -1 if the exec failed or if the child was
 *  terminated abnormally; otherwise, the exit code of the child is
 *  returned.
 *
 **********************************************************************
 * HISTORY
 * $Log:	run.c,v $
 * Revision 1.5  92/03/24  14:09:46  mja
 * 	Add <sys/time.h> per portability suggestion from Brad.
 * 	[92/03/18  14:40:46  mja]
 * 
 * 	Updated other run variants.
 * 	[91/12/04            danner]
 * 
 * 	Updated runc to use a static array of char * in order to
 * 	use <varargs.h> correctly
 * 	[91/12/04            danner]
 * 
 * Revision 1.4  90/12/11  17:58:10  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * Revision 1.3  90/02/01  16:13:57  mja
 * 	Define new runc(), runcv(), runcp() amd runcvp() entry points;
 * 	remove most lint.
 * 	[90/02/01  15:22:40  mja]
 * 
 * Revision 1.2  89/08/03  14:36:46  mja
 * 	Update run() and runp() to use <varargs.h>.
 * 	[89/04/19            mja]
 * 
 * 23-Sep-86  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Merged old runv and runvp modules.
 *
 * 22-Nov-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Added check and kill if child process was stopped.
 *
 * 30-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Adapted for 4.2 BSD UNIX:  Conforms to new signals and wait.
 *
 * 15-July-82 Mike Accetta (mja) and Neal Friedman (naf)
 *				  at Carnegie-Mellon University
 *	Added a return(-1) if vfork fails.  This should only happen
 *	if there are no more processes available.
 *
 * 28-Jan-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Added setuid and setgid for system programs' use.
 *
 * 21-Jan-80  Steven Shafer (sas) at Carnegie-Mellon University
 *	Changed fork to vfork.
 *
 * 20-Nov-79  Steven Shafer (sas) at Carnegie-Mellon University
 *	Created for VAX.  The proper way to fork-and-execute a system
 *	program is now by "runvp" or "runp", with the program name
 *	(rather than an absolute pathname) as the first argument;
 *	that way, the "PATH" variable in the environment does the right
 *	thing.  Too bad execvp and execlp (hence runvp and runp) don't
 *	accept a pathlist as an explicit argument.
 *
 **********************************************************************
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <signal.h>
#include <varargs.h>

extern uid_t getuid();
extern gid_t getgid();

#define RUN_MAXARGS 1024

int run (name,va_alist)
char *name;
va_dcl
{
	va_list ap;
	int val;
	int i;
  	char *args[RUN_MAXARGS];

	va_start(ap);
	for (i=0; i<RUN_MAXARGS; i++)
	  {
	    args[i] = va_arg(ap,char *);
	    if (args[i] == (char *) 0)
	      break;
	  }

	if (i == RUN_MAXARGS)
	    return -1;

	val = runv (name, args);
	va_end(ap);
	return(val);
}

int runv (name,argv)
char *name,**argv;
{
	return (dorun ((int (*)())0, name, argv, 0));
}

int runp (name,va_alist)
char *name;
va_dcl
{
	va_list ap;
	int val;
	int i;
  	char *args[RUN_MAXARGS];

	va_start(ap);
	for (i=0; i<RUN_MAXARGS; i++)
	  {
	    args[i] = va_arg(ap,char *);
	    if (args[i] == (char *) 0)
	      break;
	  }

	if (i == RUN_MAXARGS)
	    return -1;

	val = runvp (name, args);
	va_end(ap);

	return (val);
}

int runvp (name,argv)
char *name,**argv;
{
	return (dorun ((int (*)())0, name, argv, 1));
}

int runc (func,name,va_alist)
int (*func)();
char *name;
va_dcl
{
	va_list ap;
	int val;
	int i;
  	char *args[RUN_MAXARGS];

	va_start(ap);
	for (i=0; i<RUN_MAXARGS; i++)
	  {
	    args[i] = va_arg(ap,char *);
	    if (args[i] == (char *) 0)
	      break;
	  }

	if (i == RUN_MAXARGS)
	    return -1;

	va_end(ap);
	val = runcv (func, name, args);
	return(val);
}

int runcv (func,name,argv)
int (*func)();
char *name,**argv;
{
	return (dorun (func, name, argv, 0));
}

int runcp (func,name,va_alist)
int (*func)();
char *name;
va_dcl
{
	va_list ap;
	int val;
	int i;
  	char *args[RUN_MAXARGS];

	va_start(ap);
	for (i=0; i<RUN_MAXARGS; i++)
	  {
	    args[i] = va_arg(ap,char *);
	    if (args[i] == (char *) 0)
	      break;
	  }

	if (i == RUN_MAXARGS)
	    return -1;
	val = runcvp (func, name, args);
	va_end(ap);
	return (val);
}

int runcvp (func,name,argv)
int (*func)();
char *name,**argv;
{
	return (dorun (func, name, argv, 1));
}

static
int dorun (func,name,argv,usepath)
int (*func)();
char *name,**argv;
int usepath;
{
	int wpid;
	register int pid;
	struct sigvec ignoresig,intsig,quitsig;
	union wait status;
	int execvp(), execv();
	int (*execrtn)() = usepath ? execvp : execv;

	if ((pid = vfork()) == -1)
		return(-1);	/* no more process's, so exit with error */

	if (pid == 0) {			/* child process */
		if (func)
		    (*func)();
		(void) setgid (getgid());
		(void) setuid (getuid());
		(*execrtn) (name,argv);
		fprintf (stderr,"run: can't exec %s\n",name);
		_exit (0377);
	}

	ignoresig.sv_handler = SIG_IGN;	/* ignore INT and QUIT signals */
	ignoresig.sv_mask = 0;
	ignoresig.sv_onstack = 0;
	(void) sigvec (SIGINT,&ignoresig,&intsig);
	(void) sigvec (SIGQUIT,&ignoresig,&quitsig);
	do {
		wpid = wait3 (&status.w_status, WUNTRACED, (struct rusage *)0);
		if (WIFSTOPPED (status)) {
		    (void) kill (0,SIGTSTP);
		    wpid = 0;
		}
	} while (wpid != pid && wpid != -1);
	(void) sigvec (SIGINT,&intsig,(struct sigvec *)0); /* restore signals */
	(void) sigvec (SIGQUIT,&quitsig,(struct sigvec *)0);

	if (WIFSIGNALED (status) || status.w_retcode == 0377)
		return (-1);

	return (status.w_retcode);
}
