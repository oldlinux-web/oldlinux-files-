#ifndef PRIVATE_H

#define PRIVATE_H

/*
** This header is for use ONLY with the time conversion code.
** There is no guarantee that it will remain unchanged,
** or that it will remain at all.
** Do NOT copy it to any system include directory.
** Thank you!
*/

/*
** ID
*/

#ifndef lint
#ifndef NOID
static char	privatehid[] = "@(#)private.h	7.1";
#endif /* !defined NOID */
#endif /* !defined lint */

/*
** const
*/

#ifndef const
#ifndef __STDC__
#define const
#endif /* !defined __STDC__ */
#endif /* !defined const */

/*
** void
*/

#ifndef void
#ifndef __STDC__
#ifndef vax
#ifndef sun
#define void	char
#endif /* !defined sun */
#endif /* !defined vax */
#endif /* !defined __STDC__ */
#endif /* !defined void */

/*
** P((args))
*/

#ifndef P
#ifdef __STDC__
#define P(x)	x
#else /* !defined __STDC__ */
#define ASTERISK	*
#define P(x)	( /ASTERISK x ASTERISK/ )
#endif /* !defined __STDC__ */
#endif /* !defined P */

/*
** genericptr_t
*/

#ifdef __STDC__
typedef void *		genericptr_t;
#else /* !defined __STDC__ */
typedef char *		genericptr_t;
#endif /* !defined __STDC__ */

#include "sys/types.h"	/* for time_t */
#include "stdio.h"
#include "ctype.h"
#include "errno.h"
#include "string.h"
#ifndef _TIME_
#include "time.h"
#endif /* !defined _TIME_ */

#ifndef remove
extern int	unlink P((const char * filename));
#define remove	unlink
#endif /* !defined remove */

#ifndef FILENAME_MAX

#ifndef MAXPATHLEN
#ifdef unix
#include "sys/param.h"
#endif /* defined unix */
#endif /* !defined MAXPATHLEN */

#ifdef MAXPATHLEN
#define FILENAME_MAX	MAXPATHLEN
#else /* !defined MAXPATHLEN */
#define FILENAME_MAX	1024		/* Pure guesswork */
#endif /* !defined MAXPATHLEN */

#endif /* !defined FILENAME_MAX */

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	0
#endif /* !defined EXIT_SUCCESS */

#ifndef EXIT_FAILURE
#define EXIT_FAILURE	1
#endif /* !defined EXIT_FAILURE */

#ifdef __STDC__

#define alloc_size_t	size_t
#define qsort_size_t	size_t
#define fwrite_size_t	size_t

#else /* !defined __STDC__ */

#ifndef alloc_size_t
#define alloc_size_t	unsigned
#endif /* !defined alloc_size_t */

#ifndef qsort_size_t
#ifdef USG
#define qsort_size_t	unsigned
#else /* !defined USG */
#define qsort_size_t	int
#endif /* !defined USG */
#endif /* !defined qsort_size_t */

#ifndef fwrite_size_t
#define fwrite_size_t	int
#endif /* !defined fwrite_size_t */

#ifndef USG
extern char *		sprintf P((char * buf, const char * format, ...));
#endif /* !defined USG */

#endif /* !defined __STDC__ */

/*
** Ensure that these are declared--redundantly declaring them shouldn't hurt.
*/

extern char *		getenv P((const char * name));
extern genericptr_t	malloc P((alloc_size_t size));
extern genericptr_t	calloc P((alloc_size_t nelem, alloc_size_t elsize));
extern genericptr_t	realloc P((genericptr_t oldptr, alloc_size_t newsize));

#ifdef USG
extern void		exit P((int s));
extern void		qsort P((genericptr_t base, qsort_size_t nelem,
				qsort_size_t elsize, int (*comp)()));
extern void		perror P((const char * string));
extern void		free P((char * buf));
#endif /* defined USG */

#ifndef TRUE
#define TRUE	1
#endif /* !defined TRUE */

#ifndef FALSE
#define FALSE	0
#endif /* !defined FALSE */

/*
** UNIX is a registered trademark of AT&T.
** VAX is a trademark of Digital Equipment Corporation.
*/

#endif /* !defined PRIVATE_H */
