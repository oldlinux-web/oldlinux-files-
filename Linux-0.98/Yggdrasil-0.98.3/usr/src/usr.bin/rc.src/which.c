/* which.c: check to see if a file is executable.

   This function was originally written with Maarten Litmaath's which.c as
   a template, but was changed in order to accomodate the possibility of
   rc's running setuid or the possibility of executing files not in the
   primary group. Much of this file has been re-vamped by Paul Haahr.
   I re-re-vamped the functions that Paul supplied to correct minor bugs
   and to strip out unneeded functionality.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <errno.h>
#include "rc.h"
#include "hash.h"
#include "utils.h"
#include "builtins.h"

#define X_USR 0100
#define X_GRP 0010
#define X_OTH 0001
#define X_ALL (X_USR|X_GRP|X_OTH)

extern int stat(const char *, struct stat *);

static boolean initialized = FALSE;
static int uid, gid;

#ifdef NGROUPS
static int ngroups, gidset[NGROUPS];

/* determine whether gid lies in gidset */

static int ingidset(int gid) {
	int i;
	for (i = 0; i < ngroups; i++)
		if (gid == gidset[i])
			return 1;
	return 0;
}
#endif

/*
   A home-grown access/stat. Does the right thing for group-executable files.
   Returns a boolean result instead of this -1 nonsense.
*/

static boolean rc_access(char *path, boolean verbose) {
	struct stat st;
	int mask;

	if (stat(path, &st) != 0) {
		if (verbose) /* verbose flag only set for absolute pathname */
			uerror(path);
		return FALSE;
	}

	if (uid == 0)
		mask = X_ALL;
	else if (uid == st.st_uid)
		mask = X_USR;
#ifdef NGROUPS
	else if (gid == st.st_gid || ingidset(st.st_gid))
#else
	else if (gid == st.st_gid)
#endif
		mask = X_GRP;
	else
		mask = X_OTH;

	if (((st.st_mode & S_IFMT) == S_IFREG) && (st.st_mode & mask))
		return TRUE;

	errno = EACCES;
	if (verbose)
		uerror(path);
	return FALSE;
}

/* return a full pathname by searching $path, and by checking the status of the file */

char *which(char *name, boolean verbose) {
	static char *test = NULL;
	static int testlen = 0;
	List *path;
	int len;

	if (name == NULL)	/* no filename? can happen with "> foo" as a command */
		return NULL;

	if (!initialized) {
		initialized = TRUE;
		uid = geteuid();
		gid = getegid();
#ifdef NGROUPS
		ngroups = getgroups(NGROUPS, gidset);
#endif
	}

	if (isabsolute(name)) /* absolute pathname? */
		return rc_access(name, verbose) ? name : NULL;

	len = strlen(name);
	for (path = varlookup("path"); path != NULL; path = path->n) {
		int need = strlen(path->w) + len + 2; /* one for null terminator, one for the '/' */
		if (testlen < need) {
			efree(test);
			test = ealloc(testlen = need);
		}
		if (*path->w == '\0') {
			strcpy(test, name);
		} else {
			strcpy(test, path->w);
			strcat(test, "/");
			strcat(test, name);
		}
		if (rc_access(test, FALSE))
			return test;
	}
	if (verbose)
		fprint(2, "%s not found\n", name);
	return NULL;
}
