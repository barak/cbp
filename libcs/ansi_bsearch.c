/*
 * HISTORY
 * $Log:	ansi_bsearch.c,v $
 * Revision 2.2  92/05/19  13:43:25  neek
 * 	This routine is provided to complete the transition
 * 	away from having a non-ansi 'bsearch(3)' routine in
 * 	libcs.a.  The old bsearch() in libcs has been renamed
 * 	to cs_bsearch(). ansi_bsearch() is implemented
 * 	as a cover function to cs_bsearch, but will take
 * 	the same semantics, and return the same value as the
 * 	ANSI version.
 * 	[92/05/19            neek]
 * 
 */

#include <stdio.h>		/* NULL */
#include <sys/types.h>	/* size_t */

/*
 * Perform a binary search.
 * This is a cover function for cs_bsearch(), but has the semantics
 * and return value of the ansi version of the same (bsearch())
 * routine.
 */
void *
ansi_bsearch(key, base0, nmemb, size, compar)
	void *base0;
	size_t nmemb;
	register size_t size;
	void *key;
	register int (*compar)();
{
	int n;

	if( !key ) 
		return( NULL );

	n = cs_bsearch(base0, nmemb, size, key, compar);

	if( n < 0 )
		return( NULL );

	if( (*compar)( key, (char *)base0 + (n*size) ) )
		return( NULL );

	return( (void *)((char *)base0 + (n*size)) );
}

