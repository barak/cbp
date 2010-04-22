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
 *  atot - convert ascii string to time
 *
 **********************************************************************
 * HISTORY
 * $Log:	atot.c,v $
 * Revision 1.2  90/12/11  17:50:25  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * 09-Oct-85  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Fixed previous edit.  Need to zero time fields of tm struct
 *	after parsedate() because it sets them to -1, which is
 *	considered bad form by gtime().
 *
 * 30-Apr-85  Steven Shafer (sas) at Carnegie-Mellon University
 *	Adapted for 4.2 BSD UNIX.  Changed to new timeval struct, use
 *	gettimeofday(), and new parsedate().
 *
 * 03-Jan-80  Mike Accetta (mja) at Carnegie-Mellon University
 *	Created.
 *
 **********************************************************************
 *
 *  Usage:  t = atot (date);
 *	time_t t;
 *	char *date;
 *
 *     Converts a date string to its corresponding UNIX date/time
 *  format using pdate().  The month, day and year default to
 *  today, hours, minutes and seconds default to 0.
 *
 *     Returns converted time or -1 on error (bad date string).
 */

#include <sys/types.h>
#include <sys/time.h>
#include <c.h>
#include <libc.h>

extern time_t gtime();

time_t atot(str)
char *str;
{

    struct timeval now;
    struct timezone zone;
    register struct tm *tm;

    gettimeofday (&now,&zone);
    tm = localtime(&now.tv_sec);
    tm->tm_sec = tm->tm_min = tm->tm_hour = 0;
    if (parsedate(str, tm, 1, 0, 0) < 0)
	return(CERROR);
    if (tm->tm_sec == -1 && tm->tm_min == -1 && tm->tm_hour == -1)
	tm->tm_sec = tm->tm_min = tm->tm_hour = 0;
    return(gtime(tm));

}
