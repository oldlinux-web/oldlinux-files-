/* Copyright (C) 1991 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <ansidecl.h>
#include <errno.h>
#include <sys/stat.h>

/* Create a device file named PATH, with permission and special bits MODE
   and device number DEV (which can be constructed from major and minor
   device numbers with the `makedev' macro above).  */
int
DEFUN(__mknod, (path, mode, dev),
      CONST char *path AND int mode AND int dev)
{
  errno = ENOSYS;
  return -1;
}


#ifdef	 HAVE_GNU_LD

#include <gnu-stabs.h>

stub_warning(__mknod);

#endif	/* GNU stabs.  */
