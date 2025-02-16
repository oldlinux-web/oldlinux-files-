#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#endif

#ifndef L_SET
# define L_SET SEEK_SET
#endif

#define	POSIX_UTIME

/* Some things that need to be defined in order to make code written for
   BSD Unix compile under System V Unix.  */

/*#include <memory.h>*/
#define bcmp(b1,b2,len)		memcmp(b1,b2,len)
#define bcopy(src,dst,len)	memcpy(dst,src,len)
#define bzero(s,n)		memset(s,0,n)

#include <string.h>
#define index(s,c)		strchr(s,c)
#define rindex(s,c)		strrchr(s,c)

/*
 * Might not need these. Leave them out for now.
 *
#ifdef SEEK_SET
#	ifndef L_SET
#		define L_SET SEEK_SET
#	endif
# endif

#ifdef SEEK_CUR
#	ifndef L_INCR
#		define L_INCR SEEK_CUR
#	endif
# endif
 */

#ifndef DONTDECLARE_MALLOC
extern PTR  EXFUN(malloc,(unsigned));
extern PTR  EXFUN(realloc, (PTR, unsigned));
extern void EXFUN(free,(PTR));
#endif

/* EXACT TYPES */
typedef char int8e_type;
typedef unsigned char uint8e_type;
typedef short int16e_type;
typedef unsigned short uint16e_type;
typedef int int32e_type;
typedef unsigned int uint32e_type;

/* CORRECT SIZE OR GREATER */
typedef char int8_type;
typedef unsigned char uint8_type;
typedef short int16_type;
typedef unsigned short uint16_type;
typedef int int32_type;
typedef unsigned int uint32_type;

#include "fopen-same.h"
