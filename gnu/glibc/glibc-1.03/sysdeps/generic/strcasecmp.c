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
#include <string.h>
#include <ctype.h>


/* Compare S1 and S2, ignoring case, returning less than, equal to or
   greater than zero if S1 is lexiographically less than,
   equal to or greater than S2.  */
int
DEFUN(strcasecmp, (s1, s2), CONST char *s1 AND CONST char *s2)
{
  register CONST unsigned char *p1 = (CONST unsigned char *) s1;
  register CONST unsigned char *p2 = (CONST unsigned char *) s2;
  unsigned char c1, c2;

  if (p1 == p2)
    return 0;

  do
    {
      c1 = tolower(*p1++);
      c2 = tolower(*p2++);
      if (c1 != c2)
	break;
    }
  while (c1 != '\0');

  if (c1 == '\0')
    return -1;
  else if (c2 == '\0')
    return 1;
  return c1 - c2;
}
