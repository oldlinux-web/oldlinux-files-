/* Read and manage MIPS symbol tables from object modules.
   Source originally from hartzell@boulder.colorado.edu
   Rewritten by: meissner@osf.org
   Copyright (C) 1991 Free Software Foundation, Inc.

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

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <a.out.h>
#include "config.h"

#ifdef __STDC__
typedef void *PTR_T;
typedef const void *CPTR_T;
#define __proto(x) x
#else

#if defined(_STDIO_H_) || defined(__STDIO_H__)		/* Ultrix 4.0, SGI */
typedef void *PTR_T;
typedef void *CPTR_T;

#else
typedef char *PTR_T;					/* Ultrix 3.1 */
typedef char *CPTR_T;
#endif

#define __proto(x) ()
#define const
#endif

#define uchar	unsigned char
#define ushort	unsigned short
#define uint	unsigned int
#define ulong	unsigned long

/* Do to size_t being defined in sys/types.h and different
   in stddef.h, we have to do this by hand.....  Note, these
   types are correct for MIPS based systems, and may not be
   correct for other systems.  */

#define size_t		uint
#define ptrdiff_t	int


/* Redefinition of of storage classes as an enumeration for better
   debugging.  */

#ifndef stStaParam
#define stStaParam	16	/* Fortran static parameters */
#endif

#ifndef btVoid
#define btVoid		26	/* void basic type */
#endif

typedef enum sc {
  sc_Nil	 = scNil,	  /* no storage class */
  sc_Text	 = scText,	  /* text symbol */
  sc_Data	 = scData,	  /* initialized data symbol */
  sc_Bss	 = scBss,	  /* un-initialized data symbol */
  sc_Register	 = scRegister,	  /* value of symbol is register number */
  sc_Abs	 = scAbs,	  /* value of symbol is absolute */
  sc_Undefined	 = scUndefined,	  /* who knows? */
  sc_CdbLocal	 = scCdbLocal,	  /* variable's value is IN se->va.?? */
  sc_Bits	 = scBits,	  /* this is a bit field */
  sc_CdbSystem	 = scCdbSystem,	  /* var's value is IN CDB's address space */
  sc_RegImage	 = scRegImage,	  /* register value saved on stack */
  sc_Info	 = scInfo,	  /* symbol contains debugger information */
  sc_UserStruct	 = scUserStruct,  /* addr in struct user for current process */
  sc_SData	 = scSData,	  /* load time only small data */
  sc_SBss	 = scSBss,	  /* load time only small common */
  sc_RData	 = scRData,	  /* load time only read only data */
  sc_Var	 = scVar,	  /* Var parameter (fortran,pascal) */
  sc_Common	 = scCommon,	  /* common variable */
  sc_SCommon	 = scSCommon,	  /* small common */
  sc_VarRegister = scVarRegister, /* Var parameter in a register */
  sc_Variant	 = scVariant,	  /* Variant record */
  sc_SUndefined	 = scSUndefined,  /* small undefined(external) data */
  sc_Init	 = scInit,	  /* .init section symbol */
  sc_Max	 = scMax	  /* Max storage class+1 */
} sc_t;

/* Redefinition of symbol type.  */

typedef enum st {
  st_Nil	= stNil,	/* Nuthin' special */
  st_Global	= stGlobal,	/* external symbol */
  st_Static	= stStatic,	/* static */
  st_Param	= stParam,	/* procedure argument */
  st_Local	= stLocal,	/* local variable */
  st_Label	= stLabel,	/* label */
  st_Proc	= stProc,	/*     "      "	 Procedure */
  st_Block	= stBlock,	/* beginning of block */
  st_End	= stEnd,	/* end (of anything) */
  st_Member	= stMember,	/* member (of anything	- struct/union/enum */
  st_Typedef	= stTypedef,	/* type definition */
  st_File	= stFile,	/* file name */
  st_RegReloc	= stRegReloc,	/* register relocation */
  st_Forward	= stForward,	/* forwarding address */
  st_StaticProc	= stStaticProc,	/* load time only static procs */
  st_StaParam	= stStaParam,	/* Fortran static parameters */
  st_Constant	= stConstant,	/* const */
  st_Str	= stStr,	/* string */
  st_Number	= stNumber,	/* pure number (ie. 4 NOR 2+2) */
  st_Expr	= stExpr,	/* 2+2 vs. 4 */
  st_Type	= stType,	/* post-coercion SER */
  st_Max	= stMax		/* max type+1 */
} st_t;

/* Redefinition of type qualifiers.  */

typedef enum tq {
  tq_Nil	= tqNil,	/* bt is what you see */
  tq_Ptr	= tqPtr,	/* pointer */
  tq_Proc	= tqProc,	/* procedure */
  tq_Array	= tqArray,	/* duh */
  tq_Far	= tqFar,	/* longer addressing - 8086/8 land */
  tq_Vol	= tqVol,	/* volatile */
  tq_Max	= tqMax		/* Max type qualifier+1 */
} tq_t;

/* Redefinition of basic types.  */

typedef enum bt {
  bt_Nil	= btNil,	/* undefined */
  bt_Adr	= btAdr,	/* address - integer same size as pointer */
  bt_Char	= btChar,	/* character */
  bt_UChar	= btUChar,	/* unsigned character */
  bt_Short	= btShort,	/* short */
  bt_UShort	= btUShort,	/* unsigned short */
  bt_Int	= btInt,	/* int */
  bt_UInt	= btUInt,	/* unsigned int */
  bt_Long	= btLong,	/* long */
  bt_ULong	= btULong,	/* unsigned long */
  bt_Float	= btFloat,	/* float (real) */
  bt_Double	= btDouble,	/* Double (real) */
  bt_Struct	= btStruct,	/* Structure (Record) */
  bt_Union	= btUnion,	/* Union (variant) */
  bt_Enum	= btEnum,	/* Enumerated */
  bt_Typedef	= btTypedef,	/* defined via a typedef, isymRef points */
  bt_Range	= btRange,	/* subrange of int */
  bt_Set	= btSet,	/* pascal sets */
  bt_Complex	= btComplex,	/* fortran complex */
  bt_DComplex	= btDComplex,	/* fortran double complex */
  bt_Indirect	= btIndirect,	/* forward or unnamed typedef */
  bt_FixedDec	= btFixedDec,	/* Fixed Decimal */
  bt_FloatDec	= btFloatDec,	/* Float Decimal */
  bt_String	= btString,	/* Varying Length Character String */
  bt_Bit	= btBit,	/* Aligned Bit String */
  bt_Picture	= btPicture,	/* Picture */
  bt_Void	= btVoid,	/* void */
  bt_Max	= btMax		/* Max basic type+1 */
} bt_t;

/* Redefinition of the language codes.  */

typedef enum lang {
  lang_C	 = langC,
  lang_Pascal	 = langPascal,
  lang_Fortran	 = langFortran,
  lang_Assembler = langAssembler,
  lang_Machine	 = langMachine,
  lang_Nil	 = langNil,
  lang_Ada	 = langAda,
  lang_Pl1	 = langPl1,
  lang_Cobol	 = langCobol
} lang_t;

/* Redefinition of the debug level codes.  */

typedef enum glevel {
  glevel_0	= GLEVEL_0,
  glevel_1	= GLEVEL_1,
  glevel_2	= GLEVEL_2,
  glevel_3	= GLEVEL_3
} glevel_t;


/* Keep track of the active scopes.  */
typedef struct scope {
  struct scope *prev;		/* previous scope */
  ulong open_sym;		/* symbol opening scope */
  sc_t sc;			/* storage class */
  st_t st;			/* symbol type */
} scope_t;

struct filehdr global_hdr;	/* a.out header */

int	 errors		= 0;	/* # of errors */
int	 want_aux	= 0;	/* print aux table */
int	 want_line	= 0;	/* print line numbers */
int	 want_rfd	= 0;	/* print relative file desc's */
int	 want_scope	= 0;	/* print scopes for every symbol */
int	 tfile_fd;		/* file descriptor of .T file */
off_t	 tfile_offset;		/* current offset in .T file */
scope_t	*cur_scope	= 0;	/* list of active scopes */
scope_t	*free_scope	= 0;	/* list of freed scopes */
HDRR	 sym_hdr;		/* symbolic header */
char	*l_strings;		/* local strings */
char	*e_strings;		/* external strings */
SYMR	*l_symbols;		/* local symbols */
EXTR	*e_symbols;		/* external symbols */
LINER	*lines;			/* line numbers */
DNR	*dense_nums;		/* dense numbers */
OPTR	*opt_symbols;		/* optimization symbols */
AUXU	*aux_symbols;		/* Auxiliary symbols */
char	*aux_used;		/* map of which aux syms are used */
FDR	*file_desc;		/* file tables */
ulong	*rfile_desc;		/* relative file tables */
PDR	*proc_desc;		/* procedure tables */

/* Forward reference for functions.  */
PTR_T read_seek		__proto((PTR_T, size_t, off_t, const char *));
void  read_tfile	__proto((void));
void  print_global_hdr	__proto((struct filehdr *));
void  print_sym_hdr	__proto((HDRR *));
void  print_file_desc	__proto((FDR *, int));
void  print_symbol	__proto((SYMR *, int, char *, AUXU *, int));
void  print_aux		__proto((AUXU, int, int));
void  emit_aggregate	__proto((char *, AUXU, AUXU, const char *));
char *st_to_string	__proto((st_t));
char *sc_to_string	__proto((sc_t));
char *glevel_to_string	__proto((glevel_t));
char *lang_to_string	__proto((lang_t));
char *type_to_string	__proto((AUXU *, int));

extern PTR_T	malloc	__proto((size_t));
extern PTR_T	calloc	__proto((size_t, size_t));
extern PTR_T	realloc	__proto((PTR_T, size_t));
extern void	free	__proto((PTR_T));
extern char    *ctime	__proto((time_t *));

extern char *optarg;
extern int   optind;
extern int   opterr;

/* Create a table of debugging stab-codes and corresponding names.  */

#define __define_stab(NAME, CODE, STRING) {(int)CODE, STRING},
struct {short code; char string[10];} stab_names[]  = {
#include "stab.def"
#undef __define_stab
};


/* Read some bytes at a specified location, and return a pointer.  */

PTR_T
read_seek (ptr, size, offset, context)
     PTR_T ptr;			/* pointer to buffer or NULL */
     size_t size;		/* # bytes to read */
     off_t offset;		/* offset to read at */
     const char *context;	/* context for error message */
{
  long read_size = 0;

  if (size == 0)		/* nothing to read */
    return ptr;

  if ((ptr == (PTR_T)0 && (ptr = malloc (size)) == (PTR_T)0)
      || (tfile_offset != offset && lseek (tfile_fd, offset, 0) == -1)
      || (read_size = read (tfile_fd, ptr, size)) < 0)
    {
      perror (context);
      exit (1);
    }

  if (read_size != size)
    {
      fprintf (stderr, "%s: read %ld bytes, expected %ld bytes\n",
	       context, read_size, (long) size);
      exit (1);
    }

  tfile_offset = offset + size;
  return ptr;
}


/* Convert language code to string format.  */

char *
lang_to_string (lang)
     lang_t lang;
{
  switch (lang)
    {
    case langC:		return "C";
    case langPascal:	return "Pascal";
    case langFortran:	return "Fortran";
    case langAssembler:	return "Assembler";
    case langMachine:	return "Machine";
    case langNil:	return "Nil";
    case langAda:	return "Ada";
    case langPl1:	return "Pl1";
    case langCobol:	return "Cobol";
    }

  return "Unknown language";
}


/* Convert storage class to string.  */

char *
sc_to_string(storage_class)
     sc_t storage_class;
{
  switch(storage_class)
    {
    case sc_Nil:	 return "Nil";
    case sc_Text:	 return "Text";
    case sc_Data:	 return "Data";
    case sc_Bss:	 return "Bss";
    case sc_Register:	 return "Register";
    case sc_Abs:	 return "Abs";
    case sc_Undefined:	 return "Undefined";
    case sc_CdbLocal:	 return "CdbLocal";
    case sc_Bits:	 return "Bits";
    case sc_CdbSystem:	 return "CdbSystem";
    case sc_RegImage:	 return "RegImage";
    case sc_Info:	 return "Info";
    case sc_UserStruct:	 return "UserStruct";
    case sc_SData:	 return "SData";
    case sc_SBss:	 return "SBss";
    case sc_RData:	 return "RData";
    case sc_Var:	 return "Var";
    case sc_Common:	 return "Common";
    case sc_SCommon:	 return "SCommon";
    case sc_VarRegister: return "VarRegister";
    case sc_Variant:	 return "Variant";
    case sc_SUndefined:	 return "SUndefined";
    case sc_Init:	 return "Init";
    case sc_Max:	 return "Max";
    }

  return "???";
}


/* Convert symbol type to string.  */

char *
st_to_string(symbol_type)
     st_t symbol_type;
{
  switch(symbol_type)
    {
    case st_Nil:	return "Nil";
    case st_Global:	return "Global";
    case st_Static:	return "Static";
    case st_Param:	return "Param";
    case st_Local:	return "Local";
    case st_Label:	return "Label";
    case st_Proc:	return "Proc";
    case st_Block:	return "Block";
    case st_End:	return "End";
    case st_Member:	return "Member";
    case st_Typedef:	return "Typedef";
    case st_File:	return "File";
    case st_RegReloc:	return "RegReloc";
    case st_Forward:	return "Forward";
    case st_StaticProc:	return "StaticProc";
    case st_Constant:	return "Constant";
    case st_StaParam:	return "StaticParam";
    case st_Str:	return "String";
    case st_Number:	return "Number";
    case st_Expr:	return "Expr";
    case st_Type:	return "Type";
    case st_Max:	return "Max";
    }

  return "???";
}


/* Convert debug level to string.  */

char *
glevel_to_string (g_level)
     glevel_t g_level;
{
  switch(g_level)
    {
    case GLEVEL_0: return "G0";
    case GLEVEL_1: return "G1";
    case GLEVEL_2: return "G2";
    case GLEVEL_3: return "G3";
    }

  return "??";
}
     

/* Convert the type information to string format.  */

char *
type_to_string (aux_ptr, index)
     AUXU *aux_ptr;
     int index;
{
  AUXU u;
  struct qual {
    tq_t type;
    int  low_bound;
    int  high_bound;
    int  stride;
  } qualifiers[7];

  bt_t basic_type;
  int i;
  static char buffer1[1024];
  static char buffer2[1024];
  char *p1 = buffer1;
  char *p2 = buffer2;
  char *used_ptr = aux_used + (aux_ptr - aux_symbols);

  for (i = 0; i < 7; i++)
    {
      qualifiers[i].low_bound = 0;
      qualifiers[i].high_bound = 0;
      qualifiers[i].stride = 0;
    }

  used_ptr[index] = 1;
  u = aux_ptr[index++];
  if (u.isym == -1)
    return "-1 (no type)";

  basic_type = (bt_t) u.ti.bt;
  qualifiers[0].type = (tq_t) u.ti.tq0;
  qualifiers[1].type = (tq_t) u.ti.tq1;
  qualifiers[2].type = (tq_t) u.ti.tq2;
  qualifiers[3].type = (tq_t) u.ti.tq3;
  qualifiers[4].type = (tq_t) u.ti.tq4;
  qualifiers[5].type = (tq_t) u.ti.tq5;
  qualifiers[6].type = tq_Nil;

  /*
   * Go get the basic type.
   */
  switch (basic_type)
    {
    case bt_Nil:		/* undefined */
      strcpy (p1, "nil");
      break;

    case bt_Adr:		/* address - integer same size as pointer */
      strcpy (p1, "address");
      break;

    case bt_Char:		/* character */
      strcpy (p1, "char");
      break;

    case bt_UChar:		/* unsigned character */
      strcpy (p1, "unsigned char");
      break;

    case bt_Short:		/* short */
      strcpy (p1, "short");
      break;

    case bt_UShort:		/* unsigned short */
      strcpy (p1, "unsigned short");
      break;

    case bt_Int:		/* int */
      strcpy (p1, "int");
      break;

    case bt_UInt:		/* unsigned int */
      strcpy (p1, "unsigned int");
      break;

    case bt_Long:		/* long */
      strcpy (p1, "long");
      break;

    case bt_ULong:		/* unsigned long */
      strcpy (p1, "unsigned long");
      break;

    case bt_Float:		/* float (real) */
      strcpy (p1, "float");
      break;

    case bt_Double:		/* Double (real) */
      strcpy (p1, "double");
      break;

      /* Structures add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to struct def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case bt_Struct:		/* Structure (Record) */
      emit_aggregate (p1, aux_ptr[index], aux_ptr[index+1], "struct");
      used_ptr[index] = 1;
      if (aux_ptr[index].rndx.rfd == ST_RFDESCAPE)
	used_ptr[++index] = 1;

      index++;			/* skip aux words */
      break;

      /* Unions add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to union def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case bt_Union:		/* Union */
      emit_aggregate (p1, aux_ptr[index], aux_ptr[index+1], "union");
      used_ptr[index] = 1;
      if (aux_ptr[index].rndx.rfd == ST_RFDESCAPE)
	used_ptr[++index] = 1;

      index++;			/* skip aux words */
      break;

      /* Enumerations add 1-2 aux words:
	 1st word is [ST_RFDESCAPE, offset] pointer to enum def;
	 2nd word is file index if 1st word rfd is ST_RFDESCAPE.  */

    case bt_Enum:		/* Enumeration */
      emit_aggregate (p1, aux_ptr[index], aux_ptr[index+1], "enum");
      used_ptr[index] = 1;
      if (aux_ptr[index].rndx.rfd == ST_RFDESCAPE)
	used_ptr[++index] = 1;

      index++;			/* skip aux words */
      break;

    case bt_Typedef:		/* defined via a typedef, isymRef points */
      strcpy (p1, "typedef");
      break;

    case bt_Range:		/* subrange of int */
      strcpy (p1, "subrange");
      break;

    case bt_Set:		/* pascal sets */
      strcpy (p1, "set");
      break;

    case bt_Complex:		/* fortran complex */
      strcpy (p1, "complex");
      break;

    case bt_DComplex:		/* fortran double complex */
      strcpy (p1, "double complex");
      break;

    case bt_Indirect:		/* forward or unnamed typedef */
      strcpy (p1, "forward/unamed typedef");
      break;

    case bt_FixedDec:		/* Fixed Decimal */
      strcpy (p1, "fixed decimal");
      break;

    case bt_FloatDec:		/* Float Decimal */
      strcpy (p1, "float decimal");
      break;

    case bt_String:		/* Varying Length Character String */
      strcpy (p1, "string");
      break;

    case bt_Bit:		/* Aligned Bit String */
      strcpy (p1, "bit");
      break;

    case bt_Picture:		/* Picture */
      strcpy (p1, "picture");
      break;

    case bt_Void:		/* Void */
      strcpy (p1, "void");
      break;

    default:
      sprintf (p1, "Unknown basic type %d", (int) basic_type);
      break;
    }

  p1 += strlen (buffer1);

  /*
   * If this is a bitfield, get the bitsize.
   */
  if (u.ti.fBitfield)
    {
      int bitsize;

      used_ptr[index] = 1;
      bitsize = aux_ptr[index++].width;
      sprintf (p1, " : %d", bitsize);
      p1 += strlen (buffer1);
    }


  /*
   * Deal with any qualifiers.
   */
  if (qualifiers[0].type != tq_Nil)
    {
      /*
       * Snarf up any array bounds in the correct order.  Arrays
       * store 5 successive words in the aux. table:
       *	word 0	RNDXR to type of the bounds (ie, int)
       *	word 1	Current file descriptor index
       *	word 2	low bound
       *	word 3	high bound (or -1 if [])
       *	word 4	stride size in bits
       */
      for (i = 0; i < 7; i++)
	{
	  if (qualifiers[i].type == tq_Array)
	    {
	      qualifiers[i].low_bound  = aux_ptr[index+2].dnLow;
	      qualifiers[i].high_bound = aux_ptr[index+3].dnHigh;
	      qualifiers[i].stride     = aux_ptr[index+4].width;
	      used_ptr[index] = 1;
	      used_ptr[index+1] = 1;
	      used_ptr[index+2] = 1;
	      used_ptr[index+3] = 1;
	      used_ptr[index+4] = 1;
	      index += 5;
	    }
	}

      /*
       * Now print out the qualifiers.
       */
      for (i = 0; i < 6; i++)
	{
	  switch (qualifiers[i].type)
	    {
	    case tq_Nil:
	    case tq_Max:
	      break;

	    case tq_Ptr:
	      strcpy (p2, "ptr to ");
	      p2 += sizeof ("ptr to ")-1;
	      break;

	    case tq_Vol:
	      strcpy (p2, "volatile ");
	      p2 += sizeof ("volatile ")-1;
	      break;

	    case tq_Far:
	      strcpy (p2, "far ");
	      p2 += sizeof ("far ")-1;
	      break;

	    case tq_Proc:
	      strcpy (p2, "func. ret. ");
	      p2 += sizeof ("func. ret. ");
	      break;

	    case tq_Array:
	      {
		int first_array = i;
		int j;

		/* Print array bounds reversed (ie, in the order the C
		   programmer writes them).  C is such a fun language.... */

		while (i < 5 && qualifiers[i+1].type == tq_Array)
		  i++;

		for (j = i; j >= first_array; j--)
		  {
		    strcpy (p2, "array [");
		    p2 += sizeof ("array [")-1;
		    if (qualifiers[j].low_bound != 0)
		      sprintf (p2,
			       "%ld:%ld {%ld bits}",
			       (long) qualifiers[j].low_bound,
			       (long) qualifiers[j].high_bound,
			       (long) qualifiers[j].stride);

		    else if (qualifiers[j].high_bound != -1)
		      sprintf (p2,
			       "%ld {%ld bits}",
			       (long) (qualifiers[j].high_bound + 1),
			       (long) (qualifiers[j].stride));

		    else
		      sprintf (p2, " {%ld bits}", (long) (qualifiers[j].stride));

		    p2 += strlen (p2);
		    strcpy (p2, "] of ");
		    p2 += sizeof ("] of ")-1;
		  }
	      }
	      break;
	    }
	}
    }

  strcpy (p2, buffer1);
  return buffer2;
}


/* Print out the global file header for object files.  */

void
print_global_hdr (ptr)
     struct filehdr *ptr;
{
  char *time = ctime ((off_t *)&ptr->f_timdat);
  ushort flags = ptr->f_flags;

  printf("Global file header:\n");
  printf("    %-*s 0x%x\n",    24, "magic number",	     (ushort) ptr->f_magic);
  printf("    %-*s %d\n",      24, "# sections",	     (int)    ptr->f_nscns);
  printf("    %-*s %ld, %s",   24, "timestamp",		     (long)   ptr->f_timdat, time);
  printf("    %-*s %ld\n",     24, "symbolic header offset", (long)   ptr->f_symptr);
  printf("    %-*s %ld\n",     24, "symbolic header size",   (long)   ptr->f_nsyms);
  printf("    %-*s %ld\n",     24, "optional header",	     (long)   ptr->f_opthdr);
  printf("    %-*s 0x%lx",     24, "flags",		     (ushort) flags);

  if ((flags & F_RELFLG) != 0)
    printf (", F_RELFLG");

  if ((flags & F_EXEC) != 0)
    printf (", F_EXEC");

  if ((flags & F_LNNO) != 0)
    printf (", F_LNNO");

  if ((flags & F_LSYMS) != 0)
    printf (", F_LSYMS");

  if ((flags & F_MINMAL) != 0)
    printf (", F_MINMAL");

  if ((flags & F_UPDATE) != 0)
    printf (", F_UPDATE");

  if ((flags & F_SWABD) != 0)
    printf (", F_SWABD");

  if ((flags & F_AR16WR) != 0)
    printf (", F_AR16WR");

  if ((flags & F_AR32WR) != 0)
    printf (", F_AR32WR");

  if ((flags & F_AR32W) != 0)
    printf (", F_AR32W");

  if ((flags & F_PATCH) != 0)
    printf (", F_PATCH/F_NODF");

  printf ("\n\n");
}


/* Print out the symbolic header.  */

void
print_sym_hdr (sym_ptr)
     HDRR *sym_ptr;
{
  int width = 20;

  printf("Symbolic header, magic number = 0x%04x, vstamp = %d.%d:\n\n",
	 sym_ptr->magic & 0xffff,
	 (sym_ptr->vstamp & 0xffff) >> 8,
	 sym_ptr->vstamp & 0xff);

  printf("    %-*s %11s %11s %11s\n", width, "Info", "Offset", "Number", "Bytes");
  printf("    %-*s %11s %11s %11s\n", width, "====", "======", "======", "=====\n");

  printf("    %-*s %11ld %11d %11d [%d]\n", width, "Line numbers",
	 sym_ptr->cbLineOffset, sym_ptr->cbLine, sym_ptr->cbLine, sym_ptr->ilineMax);

  printf("    %-*s %11ld %11d %11d\n", width, "Dense numbers",
	 sym_ptr->cbDnOffset, sym_ptr->idnMax, sym_ptr->idnMax * sizeof (DNR));

  printf("    %-*s %11ld %11d %11d\n", width, "Procedures Tables",
	 sym_ptr->cbPdOffset, sym_ptr->ipdMax, sym_ptr->ipdMax * sizeof (PDR));

  printf("    %-*s %11ld %11d %11d\n", width, "Local Symbols",
	 sym_ptr->cbSymOffset, sym_ptr->isymMax, sym_ptr->isymMax * sizeof (SYMR));

  printf("    %-*s %11ld %11d %11d\n", width, "Optimization Symbols",
	 sym_ptr->cbOptOffset, sym_ptr->ioptMax, sym_ptr->ioptMax * sizeof (OPTR));

  printf("    %-*s %11ld %11d %11d\n", width, "Auxiliary Symbols",
	 sym_ptr->cbAuxOffset, sym_ptr->iauxMax, sym_ptr->iauxMax * sizeof (AUXU));

  printf("    %-*s %11ld %11d %11d\n", width, "Local Strings",
	 sym_ptr->cbSsOffset, sym_ptr->issMax, sym_ptr->issMax);

  printf("    %-*s %11ld %11d %11d\n", width, "External Strings",
	 sym_ptr->cbSsExtOffset, sym_ptr->issExtMax, sym_ptr->issExtMax);

  printf("    %-*s %11ld %11d %11d\n", width, "File Tables",
	 sym_ptr->cbFdOffset, sym_ptr->ifdMax, sym_ptr->ifdMax * sizeof (FDR));

  printf("    %-*s %11ld %11d %11d\n", width, "Relative Files",
	 sym_ptr->cbRfdOffset, sym_ptr->crfd, sym_ptr->crfd * sizeof (ulong));

  printf("    %-*s %11ld %11d %11d\n", width, "External Symbols",
	 sym_ptr->cbExtOffset, sym_ptr->iextMax, sym_ptr->iextMax * sizeof (EXTR));
}


/* Print out a symbol.  */

void
print_symbol (sym_ptr, number, strbase, aux_base, ifd)
     SYMR *sym_ptr;
     int number;
     char *strbase;
     AUXU *aux_base;
     int ifd;
{
  sc_t storage_class = (sc_t) sym_ptr->sc;
  st_t symbol_type   = (st_t) sym_ptr->st;
  ulong index	     = sym_ptr->index;
  char *used_ptr     = aux_used + (aux_base - aux_symbols);
  scope_t *scope_ptr;

  printf ("\n    Symbol# %d: \"%s\"\n", number, sym_ptr->iss + strbase);

  if (aux_base != (AUXU *)0 && index != indexNil)
    switch (symbol_type)
      {
      case st_Nil:
      case st_Label:
	break;

      case st_File:
      case st_Block:
	printf ("      End+1 symbol: %ld\n", index);
	if (want_scope)
	  {
	    if (free_scope == (scope_t *)0)
	      scope_ptr = (scope_t *) malloc (sizeof (scope_t));
	    else
	      {
		scope_ptr = free_scope;
		free_scope = scope_ptr->prev;
	      }
	    scope_ptr->open_sym = number;
	    scope_ptr->st = symbol_type;
	    scope_ptr->sc = storage_class;
	    scope_ptr->prev = cur_scope;
	    cur_scope = scope_ptr;
	  }
	break;

      case st_End:
	if (storage_class == sc_Text || storage_class == sc_Info)
	  printf ("      First symbol: %ld\n", index);
	else
	  {
	    used_ptr[index] = 1;
	    printf ("      First symbol: %ld\n", aux_base[index].isym);
	  }

	if (want_scope)
	  {
	    if (cur_scope == (scope_t *)0)
	      printf ("      Can't pop end scope\n");
	    else
	      {
		scope_ptr = cur_scope;
		cur_scope = scope_ptr->prev;
		scope_ptr->prev = free_scope;
		free_scope = scope_ptr;
	      }
	  }
	break;

      case st_Proc:
      case st_StaticProc:
	if (MIPS_IS_STAB(sym_ptr))
	  ;
	else if (ifd == -1)		/* local symbol */
	  {
	    used_ptr[index] = used_ptr[index+1] = 1;
	    printf ("      End+1 symbol: %-7ld   Type:  %s\n",
		    aux_base[index].isym, type_to_string (aux_base, index+1));
	  }
	else			/* global symbol */
	  {
	    used_ptr[index] = 1;
	    printf ("      Type: %s\n",
		    type_to_string (aux_base, index));
	  }

	if (want_scope)
	  {
	    if (free_scope == (scope_t *)0)
	      scope_ptr = (scope_t *) malloc (sizeof (scope_t));
	    else
	      {
		scope_ptr = free_scope;
		free_scope = scope_ptr->prev;
	      }
	    scope_ptr->open_sym = number;
	    scope_ptr->st = symbol_type;
	    scope_ptr->sc = storage_class;
	    scope_ptr->prev = cur_scope;
	    cur_scope = scope_ptr;
	  }
	break;

      default:
	if (!MIPS_IS_STAB (sym_ptr))
	  {
	    used_ptr[index] = 1;
	    printf ("      Type: %s\n",
		    type_to_string (aux_base, index));
	  }
	break;
      }

  if (want_scope)
    {
      printf ("      Scopes:  ");
      if (cur_scope == (scope_t *)0)
	printf (" none\n");
      else
	{
	  for (scope_ptr = cur_scope;
	       scope_ptr != (scope_t *)0;
	       scope_ptr = scope_ptr->prev)
	    {
	      char *class;
	      if (scope_ptr->st == st_Proc || scope_ptr->st == st_StaticProc)
		class = "func.";
	      else if (scope_ptr->st == st_File)
		class = "file";
	      else if (scope_ptr->st == st_Block && scope_ptr->sc == sc_Text)
		class = "block";
	      else if (scope_ptr->st == st_Block && scope_ptr->sc == sc_Info)
		class = "type";
	      else
		class = "???";

	      printf (" %d [%s]", scope_ptr->open_sym, class);
	    }
	  printf ("\n");
	}
    }

  printf ("      Value: %-13ld    ",
	  (long)sym_ptr->value);
  if (ifd == -1)
    printf ("String index: %ld\n", (long)sym_ptr->iss);
  else
    printf ("String index: %-11ld Ifd: %d\n",
	    (long)sym_ptr->iss, ifd);

  printf ("      Symbol type: %-11sStorage class: %-11s",
	  st_to_string (symbol_type), sc_to_string (storage_class));

  if (MIPS_IS_STAB(sym_ptr))
    {
      register int i = sizeof(stab_names) / sizeof(stab_names[0]);
      char *stab_name = "stab";
      short code = MIPS_UNMARK_STAB(sym_ptr->index);
      while (--i >= 0)
	if (stab_names[i].code == code)
	  {
	    stab_name = stab_names[i].string;
	    break;
	  }
      printf ("Index: 0x%lx (%s)\n", (long)sym_ptr->index, stab_name);
    }
  else if (sym_ptr->st == stLabel && sym_ptr->index != indexNil)
    printf ("Index: %ld (line#)\n", (long)sym_ptr->index);
  else
    printf ("Index: %ld\n", (long)sym_ptr->index);

}


/* Print out a word from the aux. table in various formats.  */

void
print_aux (u, auxi, used)
     AUXU u;
     int auxi;
     int used;
{
  printf ("\t%s#%-5d %11ld, [%4ld/%7ld], [%2d %1d:%1d %1x:%1x:%1x:%1x:%1x:%1x]\n",
	  (used) ? "  " : "* ",
	  auxi,
	  (long) u.isym,
	  (long) u.rndx.rfd,
	  (long) u.rndx.index,
	  u.ti.bt,
	  u.ti.fBitfield,
	  u.ti.continued,
	  u.ti.tq0,
	  u.ti.tq1,
	  u.ti.tq2,
	  u.ti.tq3,
	  u.ti.tq4,
	  u.ti.tq5);
}


/* Write aggregate information to a string.  */

void
emit_aggregate (string, u, u2, which)
     char *string;
     AUXU u;
     AUXU u2;
     const char *which;
{
  int ifd = u.rndx.rfd;
  int index = u.rndx.index;
  int sym_base, ss_base;
  int name;
  
  if (ifd == ST_RFDESCAPE)
    ifd = u2.isym;

  sym_base = file_desc[ifd].isymBase;
  ss_base  = file_desc[ifd].issBase;
  
  name = (index == indexNil) ? 0 : l_symbols[index + sym_base].iss;
  sprintf (string,
	   "%s %s { ifd = %d, index = %d }",
	   which,
	   (name == 0) ? "/* no name */" : &l_strings[ ss_base + name ],
	   ifd,
	   index);
}


/* Print out information about a file descriptor, and the symbols,
   procedures, and line numbers within it.  */

void
print_file_desc (fdp, number)
     FDR *fdp;
     int number;
{
  char *str_base;
  AUXU *aux_base;
  int symi, pdi;
  int width = 20;
  char *used_base;
  
  str_base = l_strings + fdp->issBase;  
  aux_base = aux_symbols + fdp->iauxBase;
  used_base = aux_used + (aux_base - aux_symbols);

  printf ("\nFile #%d, \"%s\"\n\n", number, str_base + fdp->rss);

  printf ("    Name index  = %-10d Readin      = %s\n",
	  (long) fdp->rss, (fdp->fReadin) ? "Yes" : "No");

  printf ("    Merge       = %-10s Endian      = %s\n",
	  (fdp->fMerge)  ? "Yes" : "No",
	  (fdp->fBigendian) ? "BIG" : "LITTLE");

  printf ("    Debug level = %-10s Language    = %s\n",
	  glevel_to_string (fdp->glevel),
	  lang_to_string((lang_t) fdp->lang));

  printf ("    Adr         = 0x%08lx\n\n", (long) fdp->adr);

  printf("    %-*s %11s %11s %11s %11s\n", width, "Info", "Start", "Number", "Size", "Offset");
  printf("    %-*s %11s %11s %11s %11s\n", width, "====", "=====", "======", "====", "======");

  printf("    %-*s %11lu %11lu %11lu %11lu\n",
	 width, "Local strings",
	 (ulong) fdp->issBase,
	 (ulong) fdp->cbSs,
	 (ulong) fdp->cbSs,
	 (ulong) (fdp->issBase + sym_hdr.cbSsOffset));

  printf("    %-*s %11lu %11u %11u %11lu\n",
	 width, "Local symbols",
	 (ulong) fdp->isymBase,
	 (ulong) fdp->csym,
	 (ulong) (fdp->csym * sizeof (SYMR)),
	 (ulong) (fdp->isymBase * sizeof (SYMR) + sym_hdr.cbSymOffset));

  printf("    %-*s %11lu %11lu %11lu %11lu\n",
	 width, "Line numbers",
	 (ulong) fdp->cbLineOffset,
	 (ulong) fdp->cline,
	 (ulong) fdp->cline,
	 (ulong) (fdp->cbLineOffset + sym_hdr.cbLineOffset));

  printf("    %-*s %11lu %11lu %11lu %11lu\n",
	 width, "Optimization symbols",
	 (ulong) fdp->ioptBase,
	 (ulong) fdp->copt,
	 (ulong) (fdp->copt * sizeof (OPTR)),
	 (ulong) (fdp->ioptBase * sizeof (OPTR) + sym_hdr.cbOptOffset));

  printf("    %-*s %11llu %11lu %11lu %11lu\n",
	 width, "Procedures",
	 (ulong) fdp->ipdFirst,
	 (ulong) fdp->cpd,
	 (ulong) (fdp->cpd * sizeof (PDR)),
	 (ulong) (fdp->ipdFirst * sizeof (PDR) + sym_hdr.cbPdOffset));

  printf("    %-*s %11lu %11lu %11lu %11lu\n",
	 width, "Auxiliary symbols",
	 (ulong) fdp->iauxBase,
	 (ulong) fdp->caux,
	 (ulong) (fdp->caux * sizeof (AUXU)),
	 (ulong) (fdp->iauxBase * sizeof(AUXU) + sym_hdr.cbAuxOffset));

  printf("    %-*s %11lu %11lu %11lu %11lu\n",
	 width, "Relative Files",
	 (ulong) fdp->rfdBase,
	 (ulong) fdp->crfd,
	 (ulong) (fdp->crfd * sizeof (ulong)),
	 (ulong) (fdp->rfdBase * sizeof(ulong) + sym_hdr.cbRfdOffset));


  if (want_scope && cur_scope != (scope_t *)0)
    printf ("\n    Warning scope does not start at 0!\n");

  /* 
   * print the info about the symbol table.
   */
  printf ("\n    There are %lu local symbols, starting at %lu\n",
	  (ulong) fdp->csym,
	  (ulong) (fdp->isymBase + sym_hdr.cbSymOffset));

  for(symi = fdp->isymBase; symi < (fdp->csym + fdp->isymBase); symi++)
    print_symbol (&l_symbols[symi],
		  symi - fdp->isymBase,
		  str_base,
		  aux_base,
		  -1);

  if (want_scope && cur_scope != (scope_t *)0)
    printf ("\n    Warning scope does not end at 0!\n");

  /*
   * print the aux. table if desired.
   */

  if (want_aux && fdp->caux != 0)
    {
      int auxi;

      printf ("\n    There are %lu auxiliary table entries, starting at %lu.\n\n",
	      (ulong) fdp->caux,
	      (ulong) (fdp->iauxBase + sym_hdr.cbAuxOffset));

      for (auxi = fdp->iauxBase; auxi < (fdp->caux + fdp->iauxBase); auxi++)
	print_aux (aux_base[auxi], auxi, used_base[auxi]);
    }

  /*
   * print the relative file descriptors.
   */
  if (want_rfd && fdp->crfd != 0)
    {
      ulong *rfd_ptr, i;

      printf ("\n    There are %lu relative file descriptors, starting at %lu.\n",
	      (ulong) fdp->crfd,
	      (ulong) fdp->rfdBase);

      rfd_ptr = rfile_desc + fdp->rfdBase * sizeof (ulong);
      for (i = 0; i < fdp->crfd; i++)
	{
	  printf ("\t#%-5ld %11ld, 0x%08lx\n", i, *rfd_ptr, *rfd_ptr);
	  rfd_ptr++;
	}
    }

  /* 
   * do the procedure descriptors.
   */
  printf ("\n    There are %lu procedure descriptor entries, ", (ulong) fdp->cpd);
  printf ("starting at %lu.\n", (ulong) fdp->ipdFirst);

  for (pdi = fdp->ipdFirst; pdi < (fdp->cpd + fdp->ipdFirst); pdi++)
    {
      PDR *proc_ptr = &proc_desc[pdi];
      printf ("\n\tProcedure descriptor %d:\n", (pdi - fdp->ipdFirst));

      printf ("\t    Name index   = %-11ld Name          = \"%s\"\n",
	      (long) l_symbols[proc_ptr->isym + fdp->isymBase].iss,
	      l_symbols[proc_ptr->isym + fdp->isymBase].iss + str_base);

      printf ("\t    .mask 0x%08lx,%-9ld .fmask 0x%08lx,%ld\n",
	      (long) proc_ptr->regmask,
	      (long) proc_ptr->regoffset,
	      (long) proc_ptr->fregmask,
	      (long) proc_ptr->fregoffset);

      printf ("\t    .frame $%d,%ld,$%d\n",
	      (int)  proc_ptr->framereg,
	      (long) proc_ptr->frameoffset,
	      (int)  proc_ptr->pcreg);

      printf ("\t    Opt. start   = %-11ld Symbols start = %ld\n",
	      (long) proc_ptr->iopt,
	      (long) proc_ptr->isym);

      printf ("\t    First line # = %-11ld Last line #   = %ld\n",
	      (long) proc_ptr->lnLow,
	      (long) proc_ptr->lnHigh);

      printf ("\t    Line Offset  = %-11ld Address       = 0x%08lx\n",
	      (long) proc_ptr->cbLineOffset,
	      (long) proc_ptr->adr);

      /*
       * print the line number entries.
       */

      if (want_line && fdp->cline != 0)
	{
	  int delta, count;
	  long cur_line = proc_ptr->lnLow;
	  uchar *line_ptr = ((uchar *)lines) + proc_ptr->cbLineOffset;
	  uchar *line_end;

	  if (pdi == fdp->cpd + fdp->ipdFirst - 1)	/* last procedure */
	    line_end = ((uchar *)lines) + fdp->cbLine + fdp->ilineBase;
	  else						/* not last proc. */
	    line_end = ((uchar *)lines) + proc_desc[pdi+1].cbLineOffset;


	  printf ("\n\tThere are %lu bytes holding line numbers, starting at %lu.\n",
		  (ulong) (line_end - line_ptr),
		  (ulong) (fdp->ilineBase + sym_hdr.cbLineOffset));

	  while (line_ptr < line_end)
	    {						/* sign extend nibble */
	      delta = ((*line_ptr >> 4) ^ 0x8) - 0x8;
	      count = (*line_ptr & 0xf) + 1;
	      if (delta != -8)
		line_ptr++;
	      else
		{
		  delta = (((line_ptr[1]) & 0xff) << 8) + ((line_ptr[2]) & 0xff);
		  delta = (delta ^ 0x8000) - 0x8000;
		  line_ptr += 3;
		}

	      cur_line += delta;
	      printf ("\t    Line %11ld,   delta %5d,   count %2d\n",
		      cur_line,
		      delta,
		      count);
	    }
	}
    }
}


/* Read in the portions of the .T file that we will print out.  */

void
read_tfile __proto((void))
{
  short magic;
  off_t sym_hdr_offset = 0;

  /* Determine if this is a .T file (which has no file header), or some
     sort of object file (which does have a file header) via the magic
     number.  */
  (void) read_seek ((PTR_T) &magic, sizeof (magic), (off_t)0, "Magic number");
  if (magic == MIPSELMAGIC || magic == MIPSEBMAGIC)
    {
      (void) read_seek ((PTR_T) &global_hdr, sizeof (global_hdr), (off_t)0,
			"Global file header");

      print_global_hdr (&global_hdr);

      if (global_hdr.f_symptr == 0)
	{
	  printf ("No symbolic header, Goodbye!\n");
	  exit (1);
	}

      sym_hdr_offset = global_hdr.f_symptr;
    }

  (void) read_seek ((PTR_T) &sym_hdr,
		    sizeof (sym_hdr),
		    sym_hdr_offset,
		    "Symbolic header");

  print_sym_hdr (&sym_hdr);

  lines = (LINER *) read_seek ((PTR_T)0,
			       sym_hdr.cbLine,
			       sym_hdr.cbLineOffset,
			       "Line numbers");

  dense_nums = (DNR *) read_seek ((PTR_T)0,
				  sym_hdr.idnMax * sizeof (DNR),
				  sym_hdr.cbDnOffset,
				  "Dense numbers");

  proc_desc = (PDR *) read_seek ((PTR_T)0,
				 sym_hdr.ipdMax * sizeof (PDR),
				 sym_hdr.cbPdOffset,
				 "Procedure tables");

  l_symbols = (SYMR *) read_seek ((PTR_T)0,
				  sym_hdr.isymMax * sizeof (SYMR),
				  sym_hdr.cbSymOffset,
				  "Local symbols");

  opt_symbols = (OPTR *) read_seek ((PTR_T)0,
				    sym_hdr.ioptMax * sizeof (OPTR),
				    sym_hdr.cbOptOffset,
				    "Optimization symbols");

  aux_symbols = (AUXU *) read_seek ((PTR_T)0,
				    sym_hdr.iauxMax * sizeof (AUXU),
				    sym_hdr.cbAuxOffset,
				    "Auxiliary symbols");

  if (sym_hdr.iauxMax > 0)
    {
      aux_used = calloc (sym_hdr.iauxMax, 1);
      if (aux_used == (char *)0)
	{
	  perror ("calloc");
	  exit (1);
	}
    }

  l_strings = (char *) read_seek ((PTR_T)0,
				  sym_hdr.issMax,
				  sym_hdr.cbSsOffset,
				  "Local string table");

  e_strings = (char *) read_seek ((PTR_T)0,
				  sym_hdr.issExtMax,
				  sym_hdr.cbSsExtOffset,
				  "External string table");

  file_desc = (FDR *) read_seek ((PTR_T)0,
				 sym_hdr.ifdMax * sizeof (FDR),
				 sym_hdr.cbFdOffset,
				 "File tables");

  rfile_desc = (ulong *) read_seek ((PTR_T)0,
				    sym_hdr.crfd * sizeof (ulong),
				    sym_hdr.cbRfdOffset,
				    "Relative file tables");

  e_symbols = (EXTR *) read_seek ((PTR_T)0,
				  sym_hdr.iextMax * sizeof (EXTR),
				  sym_hdr.cbExtOffset,
				  "External symbols");
}



int
main (argc, argv)
     int argc;
     char **argv;
{
  int i, opt;

  /*
   * Process arguments
   */
  while ((opt = getopt (argc, argv, "alrs")) != EOF)
    switch (opt)
      {
      default:	errors++;	break;
      case 'a': want_aux++;	break;	/* print aux table */
      case 'l': want_line++;	break;	/* print line numbers */
      case 'r': want_rfd++;	break;	/* print relative fd's */
      case 's':	want_scope++;	break;	/* print scope info */
      }

  if (errors || optind != argc - 1)
    {
      fprintf (stderr, "Calling Sequence:\n");
      fprintf (stderr, "\t%0 [-alrs] <object-or-T-file>\n", argv[0]);
      fprintf (stderr, "\n");
      fprintf (stderr, "switches:\n");
      fprintf (stderr, "\t-a Print out auxiliary table.\n");
      fprintf (stderr, "\t-l Print out line numbers.\n");
      fprintf (stderr, "\t-r Print out relative file descriptors.\n");
      fprintf (stderr, "\t-s Print out the current scopes for an item.\n");
      return 1;
    }

  /*
   * Open and process the input file.
   */
  tfile_fd = open (argv[optind], O_RDONLY);
  if (tfile_fd < 0)
    {
      perror (argv[optind]);
      return 1;
    }

  read_tfile ();

  /*
   * Print any global aux words if any.
   */
  if (want_aux)
    {
      long last_aux_in_use;

      if (sym_hdr.ifdMax != 0 && file_desc[0].iauxBase != 0)
	{
	  printf ("\nGlobal auxiliary entries before first file:\n");
	  for (i = 0; i < file_desc[0].iauxBase; i++)
	    print_aux (aux_symbols[i], 0, aux_used[i]);
	}

      if (sym_hdr.ifdMax == 0)
	last_aux_in_use = 0;
      else
	last_aux_in_use =
	  file_desc[sym_hdr.ifdMax-1].iauxBase +
	  file_desc[sym_hdr.ifdMax-1].caux - 1;

      if (last_aux_in_use < sym_hdr.iauxMax-1)
	{
	  printf ("\nGlobal auxiliary entries after last file:\n");
	  for (i = last_aux_in_use; i < sym_hdr.iauxMax; i++)
	    print_aux (aux_symbols[i], i - last_aux_in_use, aux_used[i]);
	}
    }

  /*
   * Print the information for each file.
   */
  for (i = 0; i < sym_hdr.ifdMax; i++)
    print_file_desc (&file_desc[i], i);

  /* 
   * Print the external symbols.
   */
  want_scope = 0;		/* scope info is meaning for extern symbols */
  printf ("\nThere are %lu external symbols, starting at %lu\n",
	  (ulong) sym_hdr.iextMax,
	  (ulong) sym_hdr.cbExtOffset);

  for(i = 0; i < sym_hdr.iextMax; i++)
    print_symbol (&e_symbols[i].asym, i, e_strings,
		  aux_symbols + file_desc[e_symbols[i].ifd].iauxBase,
		  e_symbols[i].ifd);

  /*
   * Print unused aux symbols now.
   */

  if (want_aux)
    {
      int first_time = 1;

      for (i = 0; i < sym_hdr.iauxMax; i++)
	{
	  if (! aux_used[i])
	    {
	      if (first_time)
		{
		  printf ("\nThe following auxiliary table entries were unused:\n\n");
		  first_time = 0;
		}

	      printf ("    #%-5d %11ld  0x%08lx  %s\n",
		      i,
		      (long) aux_symbols[i].isym,
		      (long) aux_symbols[i].isym,
		      type_to_string (aux_symbols, i));
	    }
	}
    }

  return 0;
}


void
fancy_abort ()
{
  fprintf (stderr, "mips-tdump internal error");
  exit (1);
}

void
fatal(s)
char *s;
{
  fprintf(stderr, "%s\n", s);
  exit(1);
}

/* Same as `malloc' but report error if no memory available.  */

PTR_T
xmalloc (size)
     unsigned size;
{
  register PTR_T value = malloc (size);
  if (value == 0)
    fatal ("Virtual memory exhausted.");
  return value;
}
