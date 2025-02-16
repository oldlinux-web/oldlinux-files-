/* Definitions of target machine for GNU compiler.
   Commodore Amiga A3000UX version.

   Copyright (C) 1991 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "m68kv4.h"

/* Alter assembler syntax for fsgldiv.  */

#define FSGLDIV_USE_S

/* Names to predefine in the preprocessor for this target machine.  For the
   Amiga, these definitions match those of the native AT&T compiler.  Note
   that we override the definition in m68kv4.h, where SVR4 is defined and
   AMIX isn't. */

#undef CPP_PREDEFINES
#define CPP_PREDEFINES \
  "-Dm68k -Dunix -DAMIX -Amachine(m68k) -Acpu(m68k) -Asystem(unix) -Alint(off)"

/* This is the library routine that is used to transfer control from
   the trampoline to the actual nested function.  FIXME:  This needs to
   be implemented still.  -fnf */

#undef TRANSFER_FROM_TRAMPOLINE

/* At end of a switch table, define LDnnn iff the symbol LInnn was defined.
   Some SGS assemblers have a bug such that "Lnnn-LInnn-2.b(pc,d0.l*2)"
   fails to assemble.  Luckily "Lnnn(pc,d0.l*2)" produces the results
   we want.  This difference can be accommodated by making the assembler
   define such "LDnnn" to be either "Lnnn-LInnn-2.b", "Lnnn", or any other
   string, as necessary.  This is accomplished via the ASM_OUTPUT_CASE_END
   macro. (the Amiga assembler has this bug) */

#undef ASM_OUTPUT_CASE_END
#define ASM_OUTPUT_CASE_END(FILE,NUM,TABLE)				\
do {									\
  if (switch_table_difference_label_flag)				\
    asm_fprintf ((FILE), "%s %LLD%d,%LL%d\n", SET_ASM_OP, (NUM), (NUM));\
  switch_table_difference_label_flag = 0;				\
} while (0)

int switch_table_difference_label_flag;

/* This says how to output assembler code to declare an
   uninitialized external linkage data object.  Under SVR4,
   the linker seems to want the alignment of data objects
   to depend on their types.  We do exactly that here.
   [This macro overrides the one in svr4.h because the amix assembler
    has a minimum default alignment of 4, and will not accept any
    explicit alignment smaller than this.  -fnf] */

#undef ASM_OUTPUT_ALIGNED_COMMON
#define ASM_OUTPUT_ALIGNED_COMMON(FILE, NAME, SIZE, ALIGN)		\
do {									\
  fputs ("\t.comm\t", (FILE));						\
  assemble_name ((FILE), (NAME));					\
  fprintf ((FILE), ",%u,%u\n", (SIZE), MAX ((ALIGN) / BITS_PER_UNIT, 4)); \
} while (0)

/* This says how to output assembler code to declare an
   uninitialized internal linkage data object.  Under SVR4,
   the linker seems to want the alignment of data objects
   to depend on their types.  We do exactly that here.
   [This macro overrides the one in svr4.h because the amix assembler
    has a minimum default alignment of 4, and will not accept any
    explicit alignment smaller than this.  -fnf] */

#undef ASM_OUTPUT_ALIGNED_LOCAL
#define ASM_OUTPUT_ALIGNED_LOCAL(FILE, NAME, SIZE, ALIGN)		\
do {									\
  fprintf ((FILE), "%s\t%s,%u,%u\n",					\
	   BSS_ASM_OP, (NAME), (SIZE), MAX ((ALIGN) / BITS_PER_UNIT, 4)); \
} while (0)

/* This definition of ASM_OUTPUT_ASCII is the same as the one in m68ksgs.h,
   which has been overridden by the one in svr4.h.  However, we can't use
   the one in svr4.h because the amix assembler croaks on some of the
   strings that it emits (such as .string "\"%s\"\n"). */

#undef ASM_OUTPUT_ASCII
#define ASM_OUTPUT_ASCII(FILE,PTR,LEN)				\
{								\
  register int sp = 0, lp = 0, ch;				\
  fprintf ((FILE), "\t%s ", BYTE_ASM_OP);				\
  do {								\
    ch = (PTR)[sp];						\
    if (ch > ' ' && ! (ch & 0x80) && ch != '\\')		\
      {								\
	fprintf ((FILE), "'%c", ch);				\
      }								\
    else							\
      {								\
	fprintf ((FILE), "0x%x", ch);				\
      }								\
    if (++sp < (LEN))						\
      {								\
	if ((sp % 10) == 0)					\
	  {							\
	    fprintf ((FILE), "\n\t%s ", BYTE_ASM_OP);		\
	  }							\
	else							\
	  {							\
	    putc (',', (FILE));					\
	  }							\
      }								\
  } while (sp < (LEN));						\
  putc ('\n', (FILE));						\
}
