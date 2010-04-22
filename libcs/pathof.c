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
 * pathof - find path of a component needed by a program
 *
 * char *pathof(component)
 * char *component;
 *
 * Where component is of the form "<prog>.<subprog>" or "<prog>.<subdir>"
 *
 * Examples of component are:
 *      "sup.libpath"
 *      "csh.profiledir"
 *      "login.mkplan"
 *
 * If component is not found, pathof() returns NULL.  If component
 * is found, a copy of the path for that component is salloc()ed and
 * a pointer to the copy is returned.
 *
 **********************************************************************
 * HISTORY
 * $Log:	pathof.c,v $
 * Revision 1.2  90/12/11  17:57:44  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 30-Apr-88  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Created.
 *
 **********************************************************************
 */
#include <stdio.h>

char *pathof(component)
char *component;
{
    FILE *f;
    char *p, *q, buf[BUFSIZ];
    char *salloc();
    int len;

    if ((f = fopen("/usr/lib/paths", "r")) == NULL)
	return(NULL);
    len = strlen(component);
    while ((p = fgets(buf, BUFSIZ, f)) != NULL) {
	if (index("#;:\n", buf[0]) != NULL)
	    continue;
	if (buf[len] != ' ' && buf[len] != '\t')
	    continue;
	if (strncasecmp(buf, component, len) != 0)
	    continue;
	p = buf + len + 1;
	while (*p == ' ' || *p == '\t')
	    p++;
	q = p;
	while (*q != ' ' && *q != '\t' && *q != '\n' && *q != '\0')
	    q++;
	*q = '\0';
	break;
    }
    (void) fclose(f);
    return(p ? salloc(p) : NULL);
}
