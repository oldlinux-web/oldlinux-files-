/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
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
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Defined in fopen.c.  */
extern int EXFUN(__getmode, (CONST char *mode, __io_mode *mptr));

/* Open a new stream that will read and/or write from the buffer in
   S, which is of LEN bytes.  If the mode indicates appending, the
   buffer pointer is set to point to the first '\0' in the buffer.
   If S is NULL, the buffer is allocated by malloc and will be freed
   when the stream is closed.  The only purpose of this is to write
   things and then read what's been written.  If LEN is zero, writes will
   always return errors and reads will always return end-of-file.

   The stream is set up such that seeks and tells will always fail and
   once the buffer is full of written characters or empty of characters
   to read, attempted writes always return an output error and attempted
   reads always return end-of-file.  */
FILE *
DEFUN(fmemopen, (s, len, mode),
      PTR s AND size_t len AND CONST char *mode)
{
  __io_mode m;
  register FILE *stream;

  if (!__getmode (mode, &m))
    return NULL;

  stream = __newstream ();
  if (stream == NULL)
    return NULL;

  stream->__mode = m;

  /* Input gets EOF.  */
  stream->__room_funcs.__input = NULL;
  /* Output gets error.  */
  stream->__room_funcs.__output = NULL;

  /* Do nothing for close.  */
  stream->__io_funcs.__close = NULL;
  /* Can't seek outside the buffer.  */
  stream->__io_funcs.__seek = NULL;

  stream->__seen = 1;

  stream->__userbuf = s != NULL && len > 0;
  if (s == NULL)
    {
      s = malloc (len);
      if (s == NULL)
	{
	  int save = errno;
	  (void) fclose (stream);
	  errno = save;
	  return NULL;
	}
    }

  stream->__buffer = (char *) s;
  stream->__bufsize = len;

  stream->__bufp = stream->__buffer;
  stream->__get_limit = (stream->__buffer +
			 (stream->__mode.__read ? stream->__bufsize : 0));
  stream->__put_limit = (stream->__buffer +
			 (stream->__mode.__write ? stream->__bufsize : 0));
  stream->__fileno = -1;
  stream->__cookie = &stream->__fileno;

  if (stream->__mode.__append)
    {
      char *p = memchr (stream->__bufp, '\0',
			stream->__get_limit - stream->__bufp);
      if (p == NULL)
	stream->__bufp = stream->__get_limit;
      else
	stream->__bufp = p;
    }
  else if (stream->__mode.__truncate)
    memset ((PTR) stream->__buffer, 0, len);

  return stream;
}
