/* Definitions of target machine for GNU compiler, for AMD Am29000 CPU, Unix.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Contributed by Richard Kenner (kenner@nyu.edu)

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* This is mostly the same as a29k.h, except that we define unix instead of
   EPI and define unix-style machine names.  */

#include "a29k.h"

/* Set our default target to be the 29050; that is the more interesting chip
   for Unix systems.  */

#undef TARGET_DEFAULT
#define TARGET_DEFAULT (1+2+8+64)

#undef CPP_PREDEFINES
#define CPP_PREDEFINES "-Dam29k -Da29k -Dam29000"

#undef CPP_SPEC
#define CPP_SPEC "%{!m29000:-Dam29050 -D__am29050__}"

#undef LINK_SPEC
#define LINK_SPEC "-c /usr/lib/default.ld"

/* We can't say "-lgcc" due to a bug in gld for now.  */
#define LINK_LIBGCC_SPECIAL

/* For some systems, it is best if double-word objects are aligned on a 
   doubleword boundary.  We want to maintain compatibility with MetaWare in
   a29k.h, but do not feel constrained to do so here.  */

#undef BIGGEST_ALIGNMENT
#define BIGGEST_ALIGNMENT 64

#if 0 /* This would be needed except that the 29k doesn't have strict
	 alignment requirements.  */

#define FUNCTION_ARG_BOUNDARY(MODE, TYPE)				\
  (((TYPE) != 0)							\
	? ((TYPE_ALIGN(TYPE) <= PARM_BOUNDARY)				\
		? PARM_BOUNDARY						\
		: TYPE_ALIGN(TYPE))					\
	: ((GET_MODE_ALIGNMENT(MODE) <= PARM_BOUNDARY)			\
		? PARM_BOUNDARY						\
		: GET_MODE_ALIGNMENT(MODE)))
#endif
