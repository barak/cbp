/*
 * Copyright (c) 1990,1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software_Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * varargs versions of printf routines
 *
 **********************************************************************
 * HISTORY
 * $Log:	vprintf.c,v $
 * Revision 2.9  93/10/09  17:34:24  mja
 * 	Use _ptr rather than _cnt to compute remaining length in vsnprintf() since
 * 	the RT _doprnt() uses fwrite() in some cases and it doesn't update _cnt
 * 	when attempting to write beyond the end of the buffer.
 * 	[93/10/09  17:34:12  mja]
 * 
 * Revision 2.8  92/06/16  09:48:12  chs
 * 	Put all but the vsn version under CMUCS conditional so they won't
 * 	be compiled under OSF.
 * 	[92/06/16  09:47:52  chs]
 * 
 * Revision 2.7  92/03/19  17:29:43  mja
 * 	Only apply the doprnt_va re-mapping under CMUCS; update copyright.
 * 	[92/03/19  17:26:11  mja]
 * 
 * Revision 2.6  90/12/11  18:00:40  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * Revision 2.5  89/09/08  18:15:55  mbj
 * 	Use _doprnt() for the Multimax (an "old" architecture).
 * 	[89/09/08            mbj]
 * 
 * Revision 2.4  89/08/03  14:40:10  mja
 * 	Add vsnprintf() routine.
 * 	[89/07/12            mja]
 * 
 * 	Terminate vsprintf() string with null byte.
 * 	[89/04/21            mja]
 * 
 * 	Change to use new hidden name for _doprnt on MIPS.
 * 	[89/04/18            mja]
 * 
 * Revision 2.3  89/06/10  14:13:43  gm0w
 * 	Added putc of NULL byte to vsprintf.
 * 	[89/06/10            gm0w]
 * 
 * Revision 2.2  88/12/13  13:53:17  gm0w
 * 	From Brad White.
 * 	[88/12/13            gm0w]
 * 
 **********************************************************************
 */
#include <stdio.h>
#include <varargs.h>

#if	!defined(vax) && !defined(sun3) && !defined(ibmrt) && !defined(multimax)
#if	defined(CMUCS)
/* 
 *  No new architectures make _doprnt() visible since much old broken code
 *  tries to call it without having used <varargs.h> to set up the
 *  argument list (and excluding it from the library thus makes this obvious
 *  at link time).  Instead they provide the identical interface under
 *  _doprnt_va() and we use that here instead.
 */
#define	_doprnt	_doprnt_va
#endif
#endif

#ifdef CMUCS
int
vprintf(fmt, args)
	char *fmt;
	va_list args;
{
	_doprnt(fmt, args, stdout);
	return (ferror(stdout) ? EOF : 0);
}

int
vfprintf(f, fmt, args)
	FILE *f;
	char *fmt;
	va_list args;
{
	_doprnt(fmt, args, f);
	return (ferror(f) ? EOF : 0);
}

int
vsprintf(s, fmt, args)
	char *s, *fmt;
	va_list args;
{
	FILE fakebuf;

	fakebuf._flag = _IOSTRG;	/* no _IOWRT: avoid stdio bug */
	fakebuf._ptr = s;
	fakebuf._cnt = 32767;
	_doprnt(fmt, args, &fakebuf);
	putc('\0', &fakebuf);
	return (strlen(s));
}
#endif	/* CMUCS */

int
vsnprintf(s, n, fmt, args)
	char *s, *fmt;
	va_list args;
{
	FILE fakebuf;

	fakebuf._flag = _IOSTRG;	/* no _IOWRT: avoid stdio bug */
	fakebuf._ptr = s;
	fakebuf._cnt = n-1;
	_doprnt(fmt, args, &fakebuf);
	fakebuf._cnt = n-(fakebuf._ptr-s);
	if (fakebuf._cnt > 0)		 /* paranoia */
	    putc('\0', &fakebuf);
	if (fakebuf._cnt<0)
	    fakebuf._cnt = 0;
	return (n-fakebuf._cnt-1);
}
