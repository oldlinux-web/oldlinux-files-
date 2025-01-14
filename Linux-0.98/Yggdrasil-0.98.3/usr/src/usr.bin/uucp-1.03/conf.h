/* conf.h */
/* Configuration header file for Taylor UUCP.
   Generated on Sun Oct 11 04:49:21 GMT 1992.  */

/* Set MAIL_PROGRAM to a program which takes a mail address as an argument
   and accepts a mail message to send to that address on stdin.  */
#define MAIL_PROGRAM "/bin/mail"

/* Set ECHO_PROGRAM to a program which echoes its arguments; if echo
   is a shell builtin you can just use "echo".  */
#define ECHO_PROGRAM "/bin/echo"

/* The following macros indicate what header files you have.  Set the
   macro to 1 if you have the corresponding header file, or 0 if you
   do not.  */
#define HAVE_STRING_H 1   	/* <string.h> */
#define HAVE_STRINGS_H 1   	/* <strings.h> */
#define HAVE_UNISTD_H 1   	/* <unistd.h> */
#define HAVE_STDLIB_H 1   	/* <stdlib.h> */
#define HAVE_LIMITS_H 1   	/* <limits.h> */
#define HAVE_TIME_H 1   	/* <time.h> */
#define HAVE_SYS_WAIT_H 1   	/* <sys/wait.h> */
#define HAVE_SYS_IOCTL_H 1   	/* <sys/ioctl.h> */
#define HAVE_DIRENT_H 1   	/* <dirent.h> */
#define HAVE_MEMORY_H 1   	/* <memory.h> */
#define HAVE_SYS_PARAM_H 1   	/* <sys/param.h> */
#define HAVE_UTIME_H 1   	/* <utime.h> */
#define HAVE_FCNTL_H 1   	/* <fcntl.h> */
#define HAVE_SYS_FILE_H 1   	/* <sys/file.h> */
#define HAVE_LIBC_H 0   	/* <libc.h> */
#define HAVE_SYSEXITS_H 0   	/* <sysexits.h> */
#define HAVE_POLL_H 0   	/* <poll.h> */
#define HAVE_STROPTS_H 0   	/* <stropts.h> */

/* Set SIGtype to the return type of a signal handler.  On newer systems
   this will be void; some older systems use int.  */
#define SIGtype int

/* Set HAVE_TIME_T to 1 if time_t is defined in <time.h>, as required by
   the ANSI C standard.  */
#define HAVE_TIME_T 1

/* Set HAVE_SYS_TIME_T to 1 if time_t is defined in <sys/types.h>;  this
   is only checked if HAVE_TIME_T is 0.  */
#define HAVE_SYS_TIME_T 1

/* Set HAVE_SYS_TIME_AND_TIME_H to 1 if <time.h> and <sys/time.h> can both
   be included in a single source file; if you don't have either or both of
   them, it doesn't matter what you set this to.  */
#define HAVE_SYS_TIME_AND_TIME_H 1

/* Set HAVE_TERMIOS_AND_SYS_IOCTL_H to 1 if <termios.h> and <sys/ioctl.h>
   can both be included in a single source file; if you don't have either
   or both of them, it doesn't matter what you set this to.  */
#define HAVE_TERMIOS_AND_SYS_IOCTL_H 1

/* If you are configuring by hand, you should set one of the terminal
   driver options in policy.h.  If you are autoconfiguring, the script
   will check whether your system defines CBREAK, which is a terminal
   setting; if your system supports CBREAK, and you don't set a terminal
   driver in policy.h, the code will assume that you have a BSD style
   terminal driver.  */
#define HAVE_CBREAK 0

/* The package needs several standard types.  If you are using the
   configure script, it will look in standard places for these types,
   and give default definitions for them here if it doesn't find them.
   The default definitions should work on most systems, but you may
   want to check them.  If you are configuring by hand, you will have
   to figure out whether the types are defined on your system, and
   what they should be defined to.

   Each of the types should be defined using #define.  For example,
   #define pid_t int
   */

/* The type pid_t is used to hold a process ID number.  It is normally
   defined in <sys/types.h>.  This is the type returned by the
   functions fork or getpid.  Usually int will work fine.  */
/* A definition of pid_t was found on your system.  */

/* The type uid_t is used to hold a user ID number.  It is normally
   defined in <sys/types.h>.  This is the type returned by the getuid
   function.  Usually int will work fine.  */
/* A definition of uid_t was found on your system.  */

/* The type gid_t is used to hold a group ID number.  It is sometimes
   defined in <sys/types.h>.  This is the type returned by the getgid
   function.  Usually int will work fine.  */
/* A definition of gid_t was found on your system.  */

/* The type off_t is used to hold an offset in a file.  It is sometimes
   defined in <sys/types.h>.  This is the type of the second argument to
   the lseek function.  Usually long will work fine.  */
/* A definition of off_t was found on your system.  */

/* Set HAVE_SIG_ATOMIC_T_IN_SIGNAL_H if the type sig_atomic_t is defined
   in <signal.h> as required by ANSI C.  */
#define HAVE_SIG_ATOMIC_T_IN_SIGNAL_H 1

/* Set HAVE_SIG_ATOMIC_T_IN_TYPES_H if the type sig_atomic_t is defined
   in <sys/types.h>.  This is ignored if HAVE_SIG_ATOMIC_T_IN_SIGNAL_H is
   set to 1.  */
#define HAVE_SIG_ATOMIC_T_IN_TYPES_H 0

/* The type sig_atomic_t is used to hold a value which may be
   referenced in a single atomic operation.  If it is not defined in
   either <signal.h> or <sys/types.h>, you may want to give it a
   definition here (if you don't, the code will use char).  If your
   compiler does not support sig_atomic_t, there is no type which is
   really correct; fortunately, for this package it does not really
   matter very much.  */

/* When Taylor UUCP is talking to another instance of itself, it will
   tell the other side the size of a file before it is transferred.
   If the package can determine how much disk space is available, it
   will use this information to avoid filling up the disk.  Define one
   of the following macros to tell the code how to determine the
   amount of available disk space.  It is possible that none of these
   are appropriate; it will do no harm to use none of them, but, of
   course, nothing will then prevent the package from filling up the
   disk.  Note that this space check is only useful when talking to
   another instance of Taylor UUCP.

   FS_STATVFS    the statvfs function
   FS_USG_STATFS the four argument statfs function
   FS_MNTENT     the two argument statfs function with the f_bsize field
   FS_STATFS     the two argument statfs function with the f_fsize field
   FS_GETMNT     the two argument statfs function with the fd_req field
   FS_USTAT      the ustat function with 512 byte blocks.  */
#define FS_MNTENT 

/* Set HAVE_VOID to 1 if the compiler supports declaring functions with
   a return type of void and casting values to void.  */
#define HAVE_VOID 1

/* Set HAVE_UNSIGNED_CHAR to 1 if the compiler supports the type unsigned
   char.  */
#define HAVE_UNSIGNED_CHAR 1

/* Set HAVE_ERRNO_DECLARATION to 1 if errno is declared in <errno.h>.  */
#define HAVE_ERRNO_DECLARATION 1

/* Set COMBINED_UNBLOCK to 1 if the flags O_NONBLOCK and O_NDELAY can
   both be specified at once on a file descriptor.  If your system
   does not support both flags, it doesn't matter what you set this
   to.  */
#define COMBINED_UNBLOCK 1

/* There are now a number of functions to check for.  For each of these,
   the macro HAVE_FUNC should be set to 1 if your system has FUNC.  For
   example, HAVE_STRERROR should be set to 1 if your system has strerror,
   0 otherwise.  */

/* Taylor UUCP provides its own versions of the following functions,
   or knows how to work around their absence.  */
#define HAVE_MEMSET 1
#define HAVE_MEMCMP 0
#define HAVE_MEMCHR 1
#define HAVE_MEMCPY 0
#define HAVE_BCOPY 1
#define HAVE_BCMP 1
#define HAVE_BZERO 1
#define HAVE_MEMMOVE 1
#define HAVE_STRCHR 1
#define HAVE_STRRCHR 1
#define HAVE_INDEX 1
#define HAVE_RINDEX 1
#define HAVE_STRERROR 1
#define HAVE_STRTOL 1
#define HAVE_STRSTR 1
#define HAVE_STRDUP 1
#define HAVE_STRCASECMP 1
#define HAVE_STRICMP 0
#define HAVE_STRLWR 0
#define HAVE_BSEARCH 1
#define HAVE_VFPRINTF 1
#define HAVE_REMOVE 1
#define HAVE_FTRUNCATE 1
#define HAVE_LTRUNC 0
#define HAVE_RENAME 1
#define HAVE_OPENDIR 1
#define HAVE_DUP2 1
#define HAVE_WAITPID 1
#define HAVE_WAIT4 1

/* If you have either sigsetjmp or setret, it will be used instead of
   setjmp.  These functions will only be used if your system restarts
   system calls after interrupts (see HAVE_RESTARTABLE_SYSCALLS,
   below).  */
#define HAVE_SIGSETJMP 1
#define HAVE_SETRET 0

/* The code needs to know what function to use to set a signal
   handler.  If will try to use each of the following functions in
   turn.  If none are available, it will use signal, which is assumed
   to always exist.  */
#define HAVE_SIGACTION 1
#define HAVE_SIGVEC 0
#define HAVE_SIGSET 0

/* The code will try to use each of the following functions in turn
   when blocking signals from delivery.  If none are available, a
   relatively unimportant race condition will exist.  */
#define HAVE_SIGPROCMASK 1
#define HAVE_SIGBLOCK 1
#define HAVE_SIGHOLD 0

/* If you have either of the following functions, it will be used to
   determine the number of file descriptors which may be open.
   Otherwise, the code will use OPEN_MAX if defined, then NOFILE if
   defined, then 20.  */
#define HAVE_GETDTABLESIZE 1
#define HAVE_SYSCONF 1

/* The code will use one of the following functions when detaching
   from a terminal.  One of these must exist.  */
#define HAVE_SETPGRP 1
#define HAVE_SETSID 1

/* If you do not specify the local node name in the main configuration
   file, Taylor UUCP will try to use each of the following functions
   in turn.  If neither is available, you must specify the local node
   name in the configuration file.  */
#define HAVE_GETHOSTNAME 1
#define HAVE_UNAME 1

/* The code will try to use each of the following functions in turn to
   determine the current time.  If none are available, it will use
   time, which is assume to always exist.  */
#define HAVE_GETTIMEOFDAY 1
#define HAVE_FTIME 0    /* Your ftime seems to be buggy.  */

/* If neither gettimeofday nor ftime is available, the code will use
   times (if available) to measure a span of time.  See also the
   discussion of TIMES_TICK in policy.h.  */
#define HAVE_TIMES 1

/* When a chat script requests a pause of less than a second with \p,
   Taylor UUCP will try to use each of the following functions in
   turn.  If none are available, it will sleep for a full second.
   Also, the (non-portable) tstuu program requires either select or
   poll.  */
#define HAVE_NAPMS 0
#define HAVE_NAP 0
#define HAVE_USLEEP 1
#define HAVE_POLL 0
#define HAVE_SELECT 1

/* If the getgrent function is available, it will be used to determine
   all the groups a user belongs to when checking file access
   permissions.  */
#define HAVE_GETGRENT 1

/* If the socket function is available, TCP support code will be
   compiled in.  */
#define HAVE_SOCKET 1

/* The code needs to know to how to get the name of the current
   directory.  If getcwd is available it will be used, otherwise if
   getwd is available it will be used.  Otherwise, set PWD_PROGRAM to
   the name of the program which will print the name of the current
   working directory.  */
#define HAVE_GETCWD 1
#define HAVE_GETWD 1
#define PWD_PROGRAM unused

/* The code needs to know how to create directories.  If you have the
   mkdir function, set HAVE_MKDIR to 1 and replace @UUDIR@ in
   Makefile.in with '# ' (the configure script will set @UUDIR@
   according to the variable UUDIR).  Otherwise, set HAVE_MKDIR to 0,
   remove @UUDIR@ from Makefile.in, and set MKDIR_PROGRAM to the name
   of the program which will create a directory named on the command
   line.  */
#define HAVE_MKDIR 1
#define MKDIR_PROGRAM unused

/* That's the end of the list of the functions.  Now there are a few
   last miscellaneous items.  */

/* On some systems times is declared in <sys/times.h> as returning
   int, so the code cannot safely declare it as returning long.  On
   the other hand, on some systems times will not work unless it is
   declared as returning long.  Set TIMES_DECLARATION_OK to 1 if times
   can be safely declared as returning long.  If you will not be using
   times, it doesn't matter what you set this to.  */
#define TIMES_DECLARATION_OK 1

/* Set HAVE_BSD_PGRP to 1 if your getpgrp call takes 1 argument and
   your setpgrp calls takes 2 argument (on System V they generally
   take no arguments).  You can safely set this to 1 on System V,
   provided the call will compile without any errors.  */
#define HAVE_BSD_PGRP 0

/* Set HAVE_UNION_WAIT to 1 if union wait is defined in the header
   file <sys/wait.h>.  */
#define HAVE_UNION_WAIT 0

/* Define UTIME_NULL_MISSING if utime with a NULL second argument does not
   set the file times to the current time.  */

/* Set HAVE_LONG_NAMES to 1 if the system supports file names longer
   than 14 characters.  */
#define HAVE_LONG_NAMES 1

/* If slow system calls are restarted after interrupts, set
   HAVE_RESTARTABLE_SYSCALLS to 1.  This is ignored if HAVE_SIGACTION
   is 1 or if HAVE_SIGVEC is 1 and SV_INTERRUPT is defined in
   <signal.h>.  In both of these cases system calls can be prevented
   from restarting.  */
#define HAVE_RESTARTABLE_SYSCALLS 0
