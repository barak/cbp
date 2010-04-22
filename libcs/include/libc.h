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
 **********************************************************************
 * Revision 1.14  94/01/13  15:53:07  moore
 **********************************************************************
 */

#ifndef	_LIBC_H_
#define	_LIBC_H_ 1

#include <sys/types.h>
#ifdef	CMUCS
#include <sysent.h>
#endif	/* CMUCS */
#include <ctype.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>

#ifdef	OSF
#define	const
#endif	/* OSF */

/*  CMU stdio additions */
#if defined(__STDC__)
extern FILE *fopenp(const char*, const char*, char*, char*);
extern FILE *fwantread(const char*, const char*, const char*, const char*);
extern FILE *fwantwrite(const char*, const char*, const char*, const char*,
			int);
#else	/* __STDC__ */
extern FILE *fopenp();
extern FILE *fwantread();
extern FILE *fwantwrite();
#endif	/* __STDC__ */


/* CMU string routines */

extern char _argbreak;

#if defined(__STDC__)
extern char* foldup(char*, const char*);
extern char* folddown(char*, const char*);
extern char* sindex(const char*, const char*);  /* Use strstr! */
extern char* skipto(const char*, const char*);
extern char* skipover(const char*, const char*);
extern char* nxtarg(char**, const char*);
extern char* getstr(const char*, char*, char*);
extern int getstab(const char*, const char**, const char*);
extern int getsearch(const char*, const char**, const char*);
extern char* strarg(const char**, const char*, const char*, char*, char*);
extern int stabarg(const char**, const char*, const char*, const char**,
		   const char*);
extern int searcharg(const char**, const char*, const char*, const char**,
		     const char*);
extern int getint(const char*, int, int, int);
extern int intarg(const char**, const char*, const char*, int, int, int);
extern long getlong(const char*, long, long, long);
extern long longarg(const char**, const char*, const char*, long, long, long);
extern short getshort(const char*, short, short, short);
extern short shortarg(const char**, const char*, const char*,
		      short, short, short);
extern float getfloat(const char*, float, float, float);
extern float floatarg(const char**, const char*, const char*,
		      float, float, float);
extern double getdouble(const char*, double, double, double);
extern double doublearg(const char**, const char*, const char*,
			double, double, double);
extern unsigned int getoct(const char*, unsigned int, unsigned int,
			   unsigned int);
extern unsigned int octarg(const char**, const char*, const char*,
			   unsigned int, unsigned int, unsigned int);
extern unsigned int gethex(const char*, unsigned int, unsigned int,
			   unsigned int);
extern unsigned int hexarg(const char**, const char*, const char*,
			   unsigned int, unsigned int, unsigned int);
extern unsigned int atoo(const char*);
extern unsigned int atoh(const char*);
extern char *salloc(const char*); /* Use strdup */
extern char *concat(const char*, int, ...);
#else	/* __STDC__ */
extern char *foldup(), *folddown();
extern char *sindex(), *skipto(), *skipover(), *nxtarg();
extern char *getstr(), *strarg();
extern long getlong(), longarg();
extern short getshort(), shortarg();
extern float getfloat(), floatarg();
extern double getdouble(), doublearg();
extern unsigned int getoct(), octarg(), gethex(), hexarg();
extern unsigned int atoo(), atoh();
extern char *salloc();
extern char *concat();
#endif	/* __STDC__ */

/* CMU library routines */
#if defined(__STDC__)
extern char *getname(int);
extern char *pathof(char *);
extern char *errmsg(int);
#else	/* __STDC__ */
extern char *getname();
extern char *pathof();
extern char *errmsg();
#endif	/* __STDC__ */

/*  CMU time additions */
#if defined(__STDC__)
extern long gtime(const struct tm*);
extern long atot(const char*);
extern char *fdate(const char*, const char*, const struct tm*);
#else	/* __STDC__ */
extern long gtime();
extern long atot();
extern char *fdate();
#endif	/* __STDC__ */

#ifdef	OSF
#undef const
#endif	/* OSF */

#ifdef	_POSIX_SOURCE
#include <unistd.h>
#include <stdlib.h>
#else	/* _POSIX_SOURCE */
/* 4.3 BSD standard library routines; taken from man(3) */
#if defined(__STDC__)
typedef int (*PFI)();
#if defined(c_plusplus)
typedef int (*PFI2)(...);
#endif /* c_plusplus */
extern void bcopy(const void *, void *, int);
extern int bcmp(const void *, const void *, int);
extern void bzero(void *, int);
extern int ffs(int);
extern char *crypt(const char *, const char *);
extern void setkey(char *);
extern void encrypt(char *, int);
extern char *ecvt(double, int, int *, int *);
extern char *fcvt(double, int, int *, int *);
extern char *gcvt(double, int, char *);
extern int execl(const char *, ...);
extern int execlp(const char *, ...);
extern int execv(const char *, const char **);
extern int execvp(const char *, const char **);
extern int execle(const char *, ...);
extern int exect(const char *, const char **, const char **);
extern char *getlogin(void);
extern int getopt(int, char * const [], const char *);
extern char *getpass(const char *);
extern char *getusershell(void);
extern void setusershell(void);
extern void endusershell(void);
extern char *getwd(char *);
extern int initgroups(const char *, gid_t);
extern void *alloca(int);
extern char *mktemp(char *);
extern int mkstemp(char *);
extern void monitor(PFI, PFI, short *, int, int);
extern void monstartup(PFI, PFI);
extern void moncontrol(int);
extern int pause(void);
extern long random(void);
extern int srandom(int);
extern void *initstate(unsigned, void *, int);
extern void *setstate(void *);
extern int rcmd(char **, int, const char *, const char *, const char *, int);
extern int rresvport(int *);
extern int ruserok(char *, int, const char *, const char *);
extern char *re_comp(char *);
extern int re_exec(char *);
extern int rexec(char **, int, const char *, const char *, const char *,
		 int *);
extern int setuid(uid_t);
extern int seteuid(uid_t);
extern int setruid(uid_t);
extern int setgid(gid_t);
extern int setegid(gid_t);
extern int setrgid(gid_t);
extern void sleep(unsigned);
extern void swab(void *, void *, int);
extern char *ttyname(int);
extern int isatty(int);
extern int ttyslot(void);
extern unsigned ualarm(unsigned, unsigned);
extern void usleep(unsigned);
#else	/* __STDC__ */
extern char *crypt();
extern char *ecvt();
extern char *fcvt();
extern char *gcvt();
extern char *getlogin();
extern char *getpass();
extern char *getusershell();
extern char *getwd();
extern char *alloca();
extern char *mktemp();
extern long random();
extern char *initstate();
extern char *setstate();
extern char *re_comp();
extern char *ttyname();
extern unsigned ualarm();
#endif	/* __STDC__ */
#endif	/* _POSIX_SOURCE */
#endif	/* not _LIBC_H_ */
