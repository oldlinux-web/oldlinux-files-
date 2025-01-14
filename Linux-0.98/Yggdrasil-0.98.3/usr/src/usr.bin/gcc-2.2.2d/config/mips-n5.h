/* Definitions of target machine for GNU compiler.  Sony RISC NEWS (mips) System V version.
   Copyright (C) 1992 Free Software Foundation, Inc.

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

#define MIPS_SYSV

#define CPP_PREDEFINES "-Dmips -Dunix -Dhost_mips -Dsony -Dsonyrisc -DMIPSEB -DSYSTYPE_SYSV"

#define MD_STARTFILE_PREFIX "/usr/ccs/lib/"

#define CPP_SPEC "\
%{.cc:	-D__LANGUAGE_C_PLUS_PLUS -D_LANGUAGE_C_PLUS_PLUS} \
%{.cxx:	-D__LANGUAGE_C_PLUS_PLUS -D_LANGUAGE_C_PLUS_PLUS} \
%{.C:	-D__LANGUAGE_C_PLUS_PLUS -D_LANGUAGE_C_PLUS_PLUS} \
%{.m:	-D__LANGUAGE_OBJECTIVE_C -D_LANGUAGE_OBJECTIVE_C} \
%{.S:	-D__LANGUAGE_ASSEMBLY -D_LANGUAGE_ASSEMBLY %{!ansi:-DLANGUAGE_ASSEMBLY}} \
%{!.S:	-D__LANGUAGE_C -D_LANGUAGE_C %{!ansi:-DLANGUAGE_C}}"

#define LINK_SPEC "\
%{G*} \
%{!mgas: %{mips1} %{mips2} %{mips3} \
	 %{EB} %{!EB:-EB} %{EL: %e-EL not supported} \
	 %{bestGnum}}"
		    
#define LIB_SPEC "\
%{ZBSD43: -L/usr/ucblib -lucb -lresolv -lsocket -lnsl} \
-nocount %{p:-lprof1} %{pg:-lprof1} -lc crtn.o%s values-Xt.o%s"

#define STARTFILE_SPEC "%{pg:gcrt0.o%s}%{!pg:%{p:mcrt1.o%s}%{!p:-nocount crt1.o%s -count}}"

#define MACHINE_TYPE "Sony RISC NEWS (SVR4 mips)"

#define NO_LIB_PROTOTYPE

#define NO_DOLLAR_IN_LABEL

#define NM_FLAGS "-Bp"

/* Generate calls to memcpy, etc., not bcopy, etc.  */
#define TARGET_MEM_FUNCTIONS

/* Mips System V.4 doesn't have a getpagesize() function needed by the
   trampoline code, so use the POSIX sysconf function to get it.
   This is only done when compiling the trampoline code.  */

#ifdef  L_trampoline
#include <sys/param.h>
#include <unistd.h>

#ifdef _SC_PAGE_SIZE
#define getpagesize()	sysconf(_SC_PAGE_SIZE)

#else				/* older rev of OS */
#define getpagesize()	(NBPC)
#endif /* !_SC_PAGE_SIZE */
#endif /*  L_trampoline */

#include "mips.h"
