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
 * Code provided by Robert Elz, who writes:
 *	The "best" way to do mktime I think is based on an idea of Bob
 *	Kridle's (so its said...) from a long time ago. (mtxinu!kridle now).
 *	It does a binary search of the time_t space.  Since time_t's are
 *	just 32 bits, its a max of 32 iterations (even at 64 bits it
 *	would still be very reasonable).
 *
 **********************************************************************
 * HISTORY
 * $Log:	gtime.c,v $
 * Revision 1.8  92/08/24  15:31:47  moore
 * 	Added offtime, which appears to be in original BSD code, but no
 * 	longer in 4.3 BSD Reno.  Also changed tzfile macros to new format.
 * 	[92/08/24  15:31:31  moore]
 * 
 * Revision 1.7  92/06/15  14:40:19  chs
 * 	Made CMUCS conditional unconditional.
 * 	[92/06/15  14:39:13  chs]
 * 
 * Revision 1.6  90/12/11  17:56:08  mja
 * 	Add copyright/disclaimer for distribution.
 * 
 * Revision 1.5  89/02/20  14:38:20  gm0w
 * 	Fixed bug that, when dealing with an extreme value,
 * 	requires an extra trip through the loop.
 * 	[89/02/17            bww]
 * 
 * 13-Apr-87  Bradley White (bww) at Carnegie-Mellon University
 *	Added gtime() inteface for backwards compatibility at CMU.
 *	Changed to do the "right thing" with out of range values (I
 *	haven't read the "mktime" proposal in the draft ANSI C
 *	standard).  Changed to use the correct median value if
 *	time_t is unsigned.  Since kre decided that the median
 *	value for signed time_t's should be zero, I assume he
 *	wanted to allow negative times (the new localtime handles
 *	them correctly).  If so, the number of iterations should be
 *	the same regardless of the time_t representation, hence the
 *	"--bits" when time_t is unsigned (this simplifies termination).
 *	The value of WRONG assumes that time_t is signed, and no
 *	matter what value you pick you will be precluding a valid
 *	time_t (in the case of -1, one second before the epoch [if
 *	you believe in negative times]).  Perhaps ~0 would be
 *	better for everyone.  Also, general cleanup.
 *
 **********************************************************************
 */

#include <sys/types.h>
#include <time.h>
#include <tzfile.h>

#define WRONG	(-1)

#define MLEN(t)	(mon_lengths[isleap((t).tm_year+TM_YEAR_BASE)][(t).tm_mon])

static int mon_lengths[2][MONSPERYEAR] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
	31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


static time_t
timemk(timeptr, funcp, offset)
	struct tm *timeptr;
	struct tm *(*funcp)();
	long offset;
{
	register int direction;
	register int bits;
	time_t t;
	struct tm yourtm, mytm;

	yourtm = *timeptr;
	/*
	 * Correct the tm supplied, in case some
	 * of its values are out of range.
	 */
	while (yourtm.tm_sec >= SECSPERMIN)
		++yourtm.tm_min, yourtm.tm_sec -= SECSPERMIN;
	while (yourtm.tm_sec < 0)
		--yourtm.tm_min, yourtm.tm_sec += SECSPERMIN;
	while (yourtm.tm_min >= MINSPERHOUR)
		++yourtm.tm_hour, yourtm.tm_min -= MINSPERHOUR;
	while (yourtm.tm_min < 0)
		--yourtm.tm_hour, yourtm.tm_min += MINSPERHOUR;
	while (yourtm.tm_hour >= HOURSPERDAY)
		++yourtm.tm_mday, yourtm.tm_hour -= HOURSPERDAY;
	while (yourtm.tm_hour < 0)
		--yourtm.tm_mday, yourtm.tm_hour += HOURSPERDAY;
	while (yourtm.tm_mon >= MONSPERYEAR)
		++yourtm.tm_year, yourtm.tm_mon -= MONSPERYEAR;
	while (yourtm.tm_mon < 0)
		--yourtm.tm_year, yourtm.tm_mon += MONSPERYEAR;
	while (yourtm.tm_mday > MLEN(yourtm)) {
		yourtm.tm_mday -= MLEN(yourtm), ++yourtm.tm_mon;
		if (yourtm.tm_mon >= MONSPERYEAR)
			++yourtm.tm_year, yourtm.tm_mon -= MONSPERYEAR;
	}
	while (yourtm.tm_mday <= 0) {
		--yourtm.tm_mon, yourtm.tm_mday += MLEN(yourtm);
		if (yourtm.tm_mon < 0)
			--yourtm.tm_year, yourtm.tm_mon += MONSPERYEAR;
	}
	/*
	 * Calcluate the number of magnitude bits in a time_t
	 * (this works regardless of whether time_t is signed
	 * or unsigned, though lint complains if unsigned - we
	 * are of course assuming two's-complement representation).
	 */
	for (bits = 0, t = 1; t > 0; ++bits, t <<= 1)
		;
	/*
	 * If time_t is signed, then 0 is the median value,
	 * if time_t is unsigned, then 1 << bits-1 is median.
	 */
	t = (t < 0) ? 0 : ((time_t) 1 << --bits);
	for (;;) {
		mytm = *((*funcp)(&t, offset));
		if ((direction = (mytm.tm_year - yourtm.tm_year)) == 0
		&& (direction = (mytm.tm_mon - yourtm.tm_mon)) == 0
		&& (direction = (mytm.tm_mday - yourtm.tm_mday)) == 0
		&& (direction = (mytm.tm_hour - yourtm.tm_hour)) == 0
		&& (direction = (mytm.tm_min - yourtm.tm_min)) == 0
		&& (direction = (mytm.tm_sec - yourtm.tm_sec)) == 0) {
			*timeptr = mytm;
			return t;
		}
		if (bits-- < 0) {
			*timeptr = yourtm;	/* restore "original" value */
			return WRONG;
		}
		if (bits < 0)
			t--;
		else if (direction > 0)
			t -= (time_t) 1 << bits;
		else
			t += (time_t) 1 << bits;
	}
}


time_t
timelocal(timeptr)
	struct tm *timeptr;
{
	extern struct tm *localtime();

	return timemk(timeptr, localtime, 0L);
}


time_t
timegm(timeptr)
	struct tm *timeptr;
{
	extern struct tm *gmtime();

	return timemk(timeptr, gmtime, 0L);
}


/* This routine was in the original 4.3 ctime collection.
 * but when I picked up the 4.3 reno, it had disappeared.
 * In the meantime, several routines, like those found in
 * this libcs collection have grown to expect it.  So I
 * rewrote it using mktime. Dale Moore.
 */
struct tm *
offtime (clock, offset)
	time_t * clock;
	long offset;
{
	struct tm * tmp = localtime(clock);
	time_t t;

	tmp->tm_sec += offset;  /* should we worry about over/underflow? */
	t = mktime(tmp);
	return localtime(&t);
}

time_t
timeoff(timeptr, offset)
	struct tm *timeptr;
	long offset;
{

	return timemk(timeptr, offtime, offset);
}


/*
 * the following is supplied for compatibility with
 * previous versions of the CMUCS runtime library.
 */
long
gtime(tm)
	struct tm *tm;
{
	time_t t;

	if ((t = timelocal(tm)) == WRONG)
		return -1L;
	return (long) t;
}
