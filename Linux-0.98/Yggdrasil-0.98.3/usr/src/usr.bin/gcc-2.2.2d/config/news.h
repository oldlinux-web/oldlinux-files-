/* Definitions of target machine for GNU compiler.  SONY NEWS-OS 3.0 version.
   Copyright (C) 1987, 1989 Free Software Foundation, Inc.

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

#ifndef USE_GAS
/* This controls conditionals in m68k.h.  */
#define MOTOROLA		/* Use Motorola syntax rather than "MIT" */
#define SGS_NO_LI		/* Suppress jump table label usage */
#endif

#include "m68k.h"

/* See m68k.h.  7 means 68020 with 68881.  */

#define TARGET_DEFAULT 7

/* Define __HAVE_68881__ in preprocessor, unless -msoft-float is specified.
   This will control the use of inline 68881 insns in certain macros.  */

#define CPP_SPEC "%{!msoft-float:-D__HAVE_68881__}"

/* Names to predefine in the preprocessor for this target machine.  */
/* These are the ones defined by Sony, plus mc68000 for uniformity with
   GCC on other 68000 systems.  */

#define CPP_PREDEFINES "-Dunix -Dbsd43 -Dsony -Dsony_news -Dmc68000 -Dmc68020 -Dnews700"

/* These conditionals tested for different submodels,
   but they were incorrect since they tested the host rather than the target.
   The choice of model shouldn't actually matter.  */

#if 0
#ifdef news800
#define CPP_PREDEFINES "-Dunix -Dbsd43 -Dsony -Dsony_news -Dmc68000 -Dmc68020 -Dnews800"
#endif
#ifdef news900
#define CPP_PREDEFINES "-Dunix -Dbsd43 -Dsony -Dsony_news -Dmc68000 -Dmc68020 -Dnews900"
#endif
#ifdef news1500
#define CPP_PREDEFINES "-Dunix -Dbsd43 -Dsony -Dsony_news -Dmc68000 -Dmc68020 -Dmc68030 -Dnews1500"
#endif
#ifdef news1700
#define CPP_PREDEFINES "-Dunix -Dbsd43 -Dsony -Dsony_news -Dmc68000 -Dmc68020 -Dmc68030 -Dnews1700"
#endif
#ifdef news1800
#define CPP_PREDEFINES "-Dunix -Dbsd43 -Dsony -Dsony_news -Dmc68000 -Dmc68020 -Dmc68030 -Dnews1800"
#endif
#ifdef news1900
#define CPP_PREDEFINES "-Dunix -Dbsd43 -Dsony -Dsony_news -Dmc68000 -Dmc68020 -Dmc68030 -Dnews1900"
#endif
#endif

/* Link with libg.a when debugging, for dbx's sake.  */

#define LIB_SPEC "%{g:-lg} %{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p} "

/* This is BSD, so it wants DBX format.  */

#define DBX_DEBUGGING_INFO

#if 0
/* This is to be compatible with types.h.
   It was found to be necessary with Newsos 3.  */

#define SIZE_TYPE "long int"
#endif

/* Override parts of m68k.h to fit Sony's assembler syntax.  */

#undef BIGGEST_ALIGNMENT
#undef CALL_USED_REGISTERS
#undef FUNCTION_VALUE
#undef LIBCALL_VALUE
#undef FUNCTION_PROFILER

#ifdef MOTOROLA
#undef FUNCTION_PROLOGUE
#undef FUNCTION_EPILOGUE
#undef REGISTER_NAMES
#undef ASM_OUTPUT_REG_PUSH
#undef ASM_OUTPUT_REG_POP
#undef ASM_OUTPUT_DOUBLE
#undef ASM_OUTPUT_SKIP
#undef ASM_FORMAT_PRIVATE_NAME
#undef PRINT_OPERAND
#undef PRINT_OPERAND_ADDRESS
#endif  

#undef ASM_OUTPUT_ALIGN

/* There is no point aligning anything to a rounder boundary than this.  */
#define BIGGEST_ALIGNMENT 32

/* A bitfield declared as `int' forces `int' alignment for the struct.  */
#define PCC_BITFIELD_TYPE_MATTERS 1
  
/* NEWS makes d2, d3, fp2 and fp3 unsaved registers, unlike the Sun system.  */
  
#define CALL_USED_REGISTERS \
 {1, 1, 1, 1, 0, 0, 0, 0, \
  1, 1, 0, 0, 0, 0, 0, 1, \
  1, 1, 1, 1, 0, 0, 0, 0}

/* NEWS returns floats and doubles in fp0, not d0/d1.  */

#define FUNCTION_VALUE(VALTYPE,FUNC) LIBCALL_VALUE (TYPE_MODE (VALTYPE))

#define LIBCALL_VALUE(MODE) \
 gen_rtx (REG, (MODE), ((TARGET_68881 && ((MODE) == SFmode || (MODE) == DFmode)) ? 16 : 0))

#define ASM_OUTPUT_ALIGN(FILE,LOG)	\
  fprintf (FILE, "\t.align %d\n", (LOG))

#ifdef MOTOROLA

/* Don't try to define `gcc_compiled.' since the assembler does not
   accept symbols with periods.  This is no real loss since GDB only
   really needs it for parms passed in registers.  */
#define ASM_IDENTIFY_GCC(FILE)

#define FUNCTION_PROLOGUE(FILE, SIZE)     \
{ register int regno;						\
  register int mask = 0;					\
  extern char call_used_regs[];					\
  int fsize = ((SIZE) + 3) & -4;				\
  if (frame_pointer_needed)					\
    { if (fsize < 0x8000)			                \
        fprintf (FILE, "\tlink fp,#%d\n", -fsize);		\
      else if (TARGET_68020)                                    \
        fprintf (FILE, "\tlink.l fp,#%d\n", -fsize);            \
      else							\
	fprintf (FILE, "\tlink fp,#0\n\tsub.l #%d,sp\n", fsize); }  \
  for (regno = 16; regno < FIRST_PSEUDO_REGISTER; regno++)	\
    if (regs_ever_live[regno] && ! call_used_regs[regno])	\
       mask |= 1 << (regno - 16);				\
  if (mask != 0)						\
    fprintf (FILE, "\tfmovem.x #0x%x,-(sp)\n", mask & 0xff);    \
  mask = 0;							\
  for (regno = 0; regno < 16; regno++)				\
    if (regs_ever_live[regno] && ! call_used_regs[regno])	\
       mask |= 1 << (15 - regno);				\
  if (frame_pointer_needed)					\
    mask &= ~ (1 << (15-FRAME_POINTER_REGNUM));			\
  if (exact_log2 (mask) >= 0)					\
    fprintf (FILE, "\tmove.l %s,-(sp)\n", reg_names[15 - exact_log2 (mask)]);  \
  else if (mask) fprintf (FILE, "\tmovem.l #0x%x,-(sp)\n", mask); }

#define FUNCTION_PROFILER(FILE, LABEL_NO) \
   fprintf (FILE, "\tmove.l #LP%d,d0\n\tjsr mcount\n", (LABEL_NO));

#define FUNCTION_EPILOGUE(FILE, SIZE) \
{ register int regno;						\
  register int mask, fmask;					\
  register int nregs;						\
  int offset, foffset;						\
  extern char call_used_regs[];					\
  int fsize = ((SIZE) + 3) & -4;				\
  int big = 0;							\
  nregs = 0;  fmask = 0;					\
  for (regno = 16; regno < FIRST_PSEUDO_REGISTER; regno++)	\
    if (regs_ever_live[regno] && ! call_used_regs[regno])	\
      { nregs++; fmask |= 1 << (23 - regno); }			\
  foffset = nregs * 12;						\
  nregs = 0;  mask = 0;						\
  if (frame_pointer_needed) regs_ever_live[FRAME_POINTER_REGNUM] = 0; \
  for (regno = 0; regno < 16; regno++)				\
    if (regs_ever_live[regno] && ! call_used_regs[regno])	\
      { nregs++; mask |= 1 << regno; }				\
  offset = foffset + nregs * 4;					\
  if (offset + fsize >= 0x8000 					\
      && frame_pointer_needed					\
      && (mask || fmask))					\
    { fprintf (FILE, "\tmove.l #%d,a0\n", -fsize);		\
      fsize = 0, big = 1; }					\
  if (exact_log2 (mask) >= 0) {					\
    if (big)							\
      fprintf (FILE, "\tmove.l (-%d,fp,a0.l),%s\n",		\
	       offset + fsize, reg_names[exact_log2 (mask)]);	\
    else if (! frame_pointer_needed)				\
      fprintf (FILE, "\tmove.l (sp)+,%s\n",			\
	       reg_names[exact_log2 (mask)]);			\
    else							\
      fprintf (FILE, "\tmove.l (-%d,fp),%s\n",			\
	       offset + fsize, reg_names[exact_log2 (mask)]); }	\
  else if (mask) {						\
    if (big)							\
      fprintf (FILE, "\tmovem.l (-%d,fp,a0.l),#0x%x\n",		\
	       offset + fsize, mask);				\
    else if (! frame_pointer_needed)				\
      fprintf (FILE, "\tmovem.l (sp)+,#0x%x\n", mask);		\
    else							\
      fprintf (FILE, "\tmovem.l (-%d,fp),#0x%x\n",		\
	       offset + fsize, mask); }				\
  if (fmask) {							\
    if (big)							\
      fprintf (FILE, "\tfmovem.x (-%d,fp,a0.l),#0x%x\n",	\
	       foffset + fsize, fmask);				\
    else if (! frame_pointer_needed)				\
      fprintf (FILE, "\tfmovem.x (sp)+,#0x%x\n", fmask);	\
    else							\
      fprintf (FILE, "\tfmovem.x (-%d,fp),#0x%x\n",		\
	       foffset + fsize, fmask); }			\
  if (frame_pointer_needed)					\
    fprintf (FILE, "\tunlk fp\n");				\
  if (current_function_pops_args)				\
    fprintf (FILE, "\trtd #%d\n", current_function_pops_args);	\
  else fprintf (FILE, "\trts\n"); }

/* Difference from m68k.h is in `fp' instead of `a6'.  */

#define REGISTER_NAMES \
{"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",	\
 "a0", "a1", "a2", "a3", "a4", "a5", "fp", "sp",	\
 "fp0", "fp1", "fp2", "fp3", "fp4", "fp5", "fp6", "fp7"}

/* This is how to output an insn to push a register on the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_PUSH(FILE,REGNO)  \
  fprintf (FILE, "\tmove.l %s,-(sp)\n", reg_names[REGNO])

/* This is how to output an insn to pop a register from the stack.
   It need not be very fast code.  */

#define ASM_OUTPUT_REG_POP(FILE,REGNO)  \
  fprintf (FILE, "\tmove.l (sp)+,%s\n", reg_names[REGNO])
  
#define ASM_OUTPUT_DOUBLE(FILE,VALUE)  \
  fprintf (FILE, "\t.double 0d%.20e\n", (VALUE))

#define ASM_OUTPUT_SKIP(FILE,SIZE)  \
  fprintf (FILE, "\t.space %u\n", (SIZE))

/* Store in OUTPUT a string (made with alloca) containing
   an assembler-name for a local static variable named NAME.
   LABELNO is an integer which is different for each call.  */

#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)	\
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 13),	\
  sprintf ((OUTPUT), "%s$$$%d", (NAME), (LABELNO)))

#define PRINT_OPERAND(FILE, X, CODE)  \
{ if (CODE == '.') fprintf (FILE, ".");					\
  else if (CODE == '#') fprintf (FILE, "#");				\
  else if (CODE == '-') fprintf (FILE, "-(sp)");			\
  else if (CODE == '+') fprintf (FILE, "(sp)+");			\
  else if (CODE == '@') fprintf (FILE, "(sp)");				\
  else if (CODE == '!') fprintf (FILE, "fpcr");				\
  else if (CODE == '$') {if (TARGET_68040_ONLY) fprintf (FILE, "s");}	\
  else if (CODE == '&') {if (TARGET_68040_ONLY) fprintf (FILE, "d");}	\
  else if (GET_CODE (X) == REG)						\
    fprintf (FILE, "%s", reg_names[REGNO (X)]);				\
  else if (GET_CODE (X) == MEM)						\
    output_address (XEXP (X, 0));					\
  else if (GET_CODE (X) == CONST_DOUBLE && GET_MODE (X) == SFmode)	\
    { union { double d; int i[2]; } u;					\
      union { float f; int i; } u1;					\
      u.i[0] = CONST_DOUBLE_LOW (X); u.i[1] = CONST_DOUBLE_HIGH (X);	\
      u1.f = u.d;							\
      if (CODE == 'f')							\
        fprintf (FILE, "#0f%.9e", u1.f);				\
      else								\
        fprintf (FILE, "#0x%x", u1.i); }				\
  else if (GET_CODE (X) == CONST_DOUBLE && GET_MODE (X) != DImode)	\
    { union { double d; int i[2]; } u;					\
      u.i[0] = CONST_DOUBLE_LOW (X); u.i[1] = CONST_DOUBLE_HIGH (X);	\
      fprintf (FILE, "#0d%.20e", u.d); }				\
  else if (CODE == 'b') output_addr_const (FILE, X);			\
  else { putc ('#', FILE); output_addr_const (FILE, X); }}

#define PRINT_OPERAND_ADDRESS(FILE, ADDR)  \
{ register rtx reg1, reg2, breg, ireg;					\
  register rtx addr = ADDR;						\
  rtx offset;								\
  switch (GET_CODE (addr))						\
    {									\
    case REG:								\
      fprintf (FILE, "(%s)", reg_names[REGNO (addr)]);			\
      break;								\
    case PRE_DEC:							\
      fprintf (FILE, "-(%s)", reg_names[REGNO (XEXP (addr, 0))]);	\
      break;								\
    case POST_INC:							\
      fprintf (FILE, "(%s)+", reg_names[REGNO (XEXP (addr, 0))]);	\
      break;								\
    case PLUS:								\
      reg1 = 0;	reg2 = 0;						\
      ireg = 0;	breg = 0;						\
      offset = 0;							\
      if (CONSTANT_ADDRESS_P (XEXP (addr, 0)))				\
	{								\
	  offset = XEXP (addr, 0);					\
	  addr = XEXP (addr, 1);					\
	}								\
      else if (CONSTANT_ADDRESS_P (XEXP (addr, 1)))			\
	{								\
	  offset = XEXP (addr, 1);					\
	  addr = XEXP (addr, 0);					\
	}								\
      if (GET_CODE (addr) != PLUS) ;					\
      else if (GET_CODE (XEXP (addr, 0)) == SIGN_EXTEND)		\
	{								\
	  reg1 = XEXP (addr, 0);					\
	  addr = XEXP (addr, 1);					\
	}								\
      else if (GET_CODE (XEXP (addr, 1)) == SIGN_EXTEND)		\
	{								\
	  reg1 = XEXP (addr, 1);					\
	  addr = XEXP (addr, 0);					\
	}								\
      else if (GET_CODE (XEXP (addr, 0)) == MULT)			\
	{								\
	  reg1 = XEXP (addr, 0);					\
	  addr = XEXP (addr, 1);					\
	}								\
      else if (GET_CODE (XEXP (addr, 1)) == MULT)			\
	{								\
	  reg1 = XEXP (addr, 1);					\
	  addr = XEXP (addr, 0);					\
	}								\
      else if (GET_CODE (XEXP (addr, 0)) == REG)			\
	{								\
	  reg1 = XEXP (addr, 0);					\
	  addr = XEXP (addr, 1);					\
	}								\
      else if (GET_CODE (XEXP (addr, 1)) == REG)			\
	{								\
	  reg1 = XEXP (addr, 1);					\
	  addr = XEXP (addr, 0);					\
	}								\
      if (GET_CODE (addr) == REG || GET_CODE (addr) == MULT		\
	  || GET_CODE (addr) == SIGN_EXTEND)				\
	{ if (reg1 == 0) reg1 = addr; else reg2 = addr; addr = 0; }	\
      if (offset != 0) { if (addr != 0) abort (); addr = offset; }	\
      if ((reg1 && (GET_CODE (reg1) == SIGN_EXTEND			\
		    || GET_CODE (reg1) == MULT))			\
	  || (reg2 != 0 && REGNO_OK_FOR_BASE_P (REGNO (reg2))))		\
	{ breg = reg2; ireg = reg1; }					\
      else if (reg1 != 0 && REGNO_OK_FOR_BASE_P (REGNO (reg1)))		\
	{ breg = reg1; ireg = reg2; }					\
      if (ireg != 0 && breg == 0 && GET_CODE (addr) == LABEL_REF)	\
        { int scale = 1;						\
	  if (GET_CODE (ireg) == MULT)					\
	    { scale = INTVAL (XEXP (ireg, 1));				\
	      ireg = XEXP (ireg, 0); }					\
	  if (GET_CODE (ireg) == SIGN_EXTEND)				\
	    fprintf (FILE, "(L%d.b,pc,%s.w",				\
		     CODE_LABEL_NUMBER (XEXP (addr, 0)),		\
		     reg_names[REGNO (XEXP (ireg, 0))]); 		\
	  else								\
	    fprintf (FILE, "(L%d.b,pc,%s.l",				\
		     CODE_LABEL_NUMBER (XEXP (addr, 0)),		\
		     reg_names[REGNO (ireg)]);				\
	  if (scale != 1) fprintf (FILE, "*%d", scale);			\
	  putc (')', FILE);						\
	  break; }							\
      if (breg != 0 && ireg == 0 && GET_CODE (addr) == LABEL_REF)	\
        { fprintf (FILE, "(L%d.b,pc,%s.l",				\
		   CODE_LABEL_NUMBER (XEXP (addr, 0)),			\
		   reg_names[REGNO (breg)]);				\
	  putc (')', FILE);						\
	  break; }							\
      if (ireg != 0 || breg != 0)					\
	{ int scale = 1;						\
	  if (breg == 0)						\
	    abort ();							\
	  if (addr && GET_CODE (addr) == LABEL_REF) abort ();		\
	  fprintf (FILE, "(");						\
	  if (addr != 0) {						\
	    output_addr_const (FILE, addr);				\
	    putc (',', FILE); }						\
	  fprintf (FILE, "%s", reg_names[REGNO (breg)]);		\
	  if (ireg != 0)						\
	    putc (',', FILE);						\
	  if (ireg != 0 && GET_CODE (ireg) == MULT)			\
	    { scale = INTVAL (XEXP (ireg, 1));				\
	      ireg = XEXP (ireg, 0); }					\
	  if (ireg != 0 && GET_CODE (ireg) == SIGN_EXTEND)		\
	    fprintf (FILE, "%s.w", reg_names[REGNO (XEXP (ireg, 0))]);	\
	  else if (ireg != 0)						\
	    fprintf (FILE, "%s.l", reg_names[REGNO (ireg)]);		\
	  if (scale != 1) fprintf (FILE, "*%d", scale);			\
	  putc (')', FILE);						\
	  break;							\
	}								\
      else if (reg1 != 0 && GET_CODE (addr) == LABEL_REF)		\
	{ fprintf (FILE, "(L%d.b,pc,%s.l)",				\
		   CODE_LABEL_NUMBER (XEXP (addr, 0)),			\
		   reg_names[REGNO (reg1)]);				\
	  break; }							\
    default:								\
      if (GET_CODE (addr) == CONST_INT					\
	  && INTVAL (addr) < 0x8000					\
	  && INTVAL (addr) >= -0x8000)					\
	fprintf (FILE, "%d.w", INTVAL (addr));				\
      else								\
        output_addr_const (FILE, addr);					\
    }}

#else /* Using GAS, which uses the MIT assembler syntax, like a Sun.  */

#define FUNCTION_PROFILER(FILE, LABEL_NO) \
   fprintf (FILE, "\tmovl #LP%d,d0\n\tjsr mcount\n", (LABEL_NO));

#endif /* MOTOROLA */
