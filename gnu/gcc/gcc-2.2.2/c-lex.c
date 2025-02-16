/* Lexical analyzer for C and Objective C.
   Copyright (C) 1987, 1988, 1989, 1992 Free Software Foundation, Inc.

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
#include <errno.h>
#include <setjmp.h>

#include "config.h"
#include "rtl.h"
#include "tree.h"
#include "input.h"
#include "c-lex.h"
#include "c-tree.h"
#include "flags.h"
#include "c-parse.h"

#ifdef MULTIBYTE_CHARS
#include <stdlib.h>
#include <locale.h>
#endif

#ifndef errno
extern int errno;
#endif

/* The elements of `ridpointers' are identifier nodes
   for the reserved type names and storage classes.
   It is indexed by a RID_... value.  */
tree ridpointers[(int) RID_MAX];

/* Cause the `yydebug' variable to be defined.  */
#define YYDEBUG 1

/* the declaration found for the last IDENTIFIER token read in.
   yylex must look this up to detect typedefs, which get token type TYPENAME,
   so it is left around in case the identifier is not a typedef but is
   used in a context which makes it a reference to a variable.  */
tree lastiddecl;

/* Nonzero enables objc features.  */

int doing_objc_thang;

extern tree lookup_interface ();

extern int yydebug;

/* File used for outputting assembler code.  */
extern FILE *asm_out_file;

#ifndef WCHAR_TYPE_SIZE
#ifdef INT_TYPE_SIZE
#define WCHAR_TYPE_SIZE INT_TYPE_SIZE
#else
#define WCHAR_TYPE_SIZE	BITS_PER_WORD
#endif
#endif

/* Number of bytes in a wide character.  */
#define WCHAR_BYTES (WCHAR_TYPE_SIZE / BITS_PER_UNIT)

static int maxtoken;		/* Current nominal length of token buffer.  */
char *token_buffer;	/* Pointer to token buffer.
			   Actual allocated length is maxtoken + 2.
			   This is not static because objc-parse.y uses it.  */

/* Nonzero if end-of-file has been seen on input.  */
static int end_of_file;

/* Buffered-back input character; faster than using ungetc.  */
static int nextchar = -1;

int check_newline ();

/* Nonzero tells yylex to ignore \ in string constants.  */
static int ignore_escape_flag = 0;

/* C code produced by gperf version 2.5 (GNU C++ version) */
/* Command-line: gperf -p -j1 -i 1 -g -o -t -N is_reserved_word -k1,3,$ c-parse.gperf  */ 
struct resword { char *name; short token; enum rid rid; };

#define TOTAL_KEYWORDS 53
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 13
#define MIN_HASH_VALUE 7
#define MAX_HASH_VALUE 102
/* maximum key range = 96, duplicates = 0 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash (str, len)
     register char *str;
     register int unsigned len;
{
  static unsigned char asso_values[] =
    {
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103, 103, 103, 103, 103, 103,
     103, 103, 103, 103, 103,   1, 103,   2,   1,  24,
       1,   5,  19,  39,  16,  13, 103,   1,  25,   1,
      34,  34,  24, 103,  13,  12,   1,  45,  24,   7,
     103, 103,   2, 103, 103, 103, 103, 103,
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 3:
        hval += asso_values[str[2]];
      case 2:
      case 1:
        hval += asso_values[str[0]];
    }
  return hval + asso_values[str[len - 1]];
}

#ifdef __GNUC__
__inline
#endif
struct resword *
is_reserved_word (str, len)
     register char *str;
     register unsigned int len;
{
  static struct resword wordlist[] =
    {
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"asm",  ASM_KEYWORD, NORID},
      {"",}, 
      {"__asm",  ASM_KEYWORD, NORID},
      {"",}, 
      {"__asm__",  ASM_KEYWORD, NORID},
      {"break",  BREAK, NORID},
      {"__typeof__",  TYPEOF, NORID},
      {"",}, 
      {"__alignof__",  ALIGNOF, NORID},
      {"",}, 
      {"__attribute__",  ATTRIBUTE, NORID},
      {"int",  TYPESPEC, RID_INT},
      {"__attribute",  ATTRIBUTE, NORID},
      {"__extension__",  EXTENSION, NORID},
      {"",}, 
      {"__signed",  TYPESPEC, RID_SIGNED},
      {"",}, 
      {"__signed__",  TYPESPEC, RID_SIGNED},
      {"__inline__",  SCSPEC, RID_INLINE},
      {"else",  ELSE, NORID},
      {"__inline",  SCSPEC, RID_INLINE},
      {"default",  DEFAULT, NORID},
      {"__typeof",  TYPEOF, NORID},
      {"while",  WHILE, NORID},
      {"__alignof",  ALIGNOF, NORID},
      {"struct",  STRUCT, NORID},
      {"__const",  TYPE_QUAL, RID_CONST},
      {"if",  IF, NORID},
      {"__const__",  TYPE_QUAL, RID_CONST},
      {"__label__",  LABEL, NORID},
      {"do",  DO, NORID},
      {"__volatile__",  TYPE_QUAL, RID_VOLATILE},
      {"sizeof",  SIZEOF, NORID},
      {"__volatile",  TYPE_QUAL, RID_VOLATILE},
      {"auto",  SCSPEC, RID_AUTO},
      {"void",  TYPESPEC, RID_VOID},
      {"char",  TYPESPEC, RID_CHAR},
      {"static",  SCSPEC, RID_STATIC},
      {"case",  CASE, NORID},
      {"extern",  SCSPEC, RID_EXTERN},
      {"switch",  SWITCH, NORID},
      {"for",  FOR, NORID},
      {"inline",  SCSPEC, RID_INLINE},
      {"typeof",  TYPEOF, NORID},
      {"typedef",  SCSPEC, RID_TYPEDEF},
      {"short",  TYPESPEC, RID_SHORT},
      {"",}, 
      {"return",  RETURN, NORID},
      {"enum",  ENUM, NORID},
      {"",}, 
      {"double",  TYPESPEC, RID_DOUBLE},
      {"signed",  TYPESPEC, RID_SIGNED},
      {"float",  TYPESPEC, RID_FLOAT},
      {"",}, {"",}, 
      {"volatile",  TYPE_QUAL, RID_VOLATILE},
      {"",}, 
      {"const",  TYPE_QUAL, RID_CONST},
      {"",}, 
      {"unsigned",  TYPESPEC, RID_UNSIGNED},
      {"",}, {"",}, {"",}, {"",}, 
      {"continue",  CONTINUE, NORID},
      {"",}, 
      {"register",  SCSPEC, RID_REGISTER},
      {"",}, {"",}, {"",}, {"",}, 
      {"goto",  GOTO, NORID},
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, {"",}, 
      
      {"union",  UNION, NORID},
      {"",}, {"",}, {"",}, {"",}, 
      {"long",  TYPESPEC, RID_LONG},
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register char *s = wordlist[key].name;

          if (*s == *str && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}

/* Return something to represent absolute declarators containing a *.
   TARGET is the absolute declarator that the * contains.
   TYPE_QUALS is a list of modifiers such as const or volatile
   to apply to the pointer type, represented as identifiers.

   We return an INDIRECT_REF whose "contents" are TARGET
   and whose type is the modifier list.  */

tree
make_pointer_declarator (type_quals, target)
     tree type_quals, target;
{
  return build1 (INDIRECT_REF, type_quals, target);
}

void
init_lex ()
{
  /* Make identifier nodes long enough for the language-specific slots.  */
  set_identifier_size (sizeof (struct lang_identifier));

  /* Start it at 0, because check_newline is called at the very beginning
     and will increment it to 1.  */
  lineno = 0;

#ifdef MULTIBYTE_CHARS
  /* Change to the native locale for multibyte conversions.  */
  setlocale (LC_CTYPE, "");
#endif

  maxtoken = 40;
  token_buffer = (char *) xmalloc (maxtoken + 2);

  ridpointers[(int) RID_INT] = get_identifier ("int");
  ridpointers[(int) RID_CHAR] = get_identifier ("char");
  ridpointers[(int) RID_VOID] = get_identifier ("void");
  ridpointers[(int) RID_FLOAT] = get_identifier ("float");
  ridpointers[(int) RID_DOUBLE] = get_identifier ("double");
  ridpointers[(int) RID_SHORT] = get_identifier ("short");
  ridpointers[(int) RID_LONG] = get_identifier ("long");
  ridpointers[(int) RID_UNSIGNED] = get_identifier ("unsigned");
  ridpointers[(int) RID_SIGNED] = get_identifier ("signed");
  ridpointers[(int) RID_INLINE] = get_identifier ("inline");
  ridpointers[(int) RID_CONST] = get_identifier ("const");
  ridpointers[(int) RID_VOLATILE] = get_identifier ("volatile");
  ridpointers[(int) RID_AUTO] = get_identifier ("auto");
  ridpointers[(int) RID_STATIC] = get_identifier ("static");
  ridpointers[(int) RID_EXTERN] = get_identifier ("extern");
  ridpointers[(int) RID_TYPEDEF] = get_identifier ("typedef");
  ridpointers[(int) RID_REGISTER] = get_identifier ("register");

  /* Some options inhibit certain reserved words.
     Clear those words out of the hash table so they won't be recognized.  */
#define UNSET_RESERVED_WORD(STRING) \
  do { struct resword *s = is_reserved_word (STRING, sizeof (STRING) - 1); \
       if (s) s->name = ""; } while (0)

  if (flag_traditional)
    {
      UNSET_RESERVED_WORD ("const");
      UNSET_RESERVED_WORD ("volatile");
      UNSET_RESERVED_WORD ("typeof");
      UNSET_RESERVED_WORD ("signed");
      UNSET_RESERVED_WORD ("inline");
    }
  if (flag_no_asm)
    {
      UNSET_RESERVED_WORD ("asm");
      UNSET_RESERVED_WORD ("typeof");
      UNSET_RESERVED_WORD ("inline");
    }
}

void
reinit_parse_for_function ()
{
}

/* Function used when yydebug is set, to print a token in more detail.  */

void
yyprint (file, yychar, yylval)
     FILE *file;
     int yychar;
     YYSTYPE yylval;
{
  tree t;
  switch (yychar)
    {
    case IDENTIFIER:
    case TYPENAME:
      t = yylval.ttype;
      if (IDENTIFIER_POINTER (t))
	fprintf (file, " `%s'", IDENTIFIER_POINTER (t));
      break;

    case CONSTANT:
      t = yylval.ttype;
      if (TREE_CODE (t) == INTEGER_CST)
	fprintf (file, " 0x%8x%8x", TREE_INT_CST_HIGH (t),
		 TREE_INT_CST_LOW (t));
      break;
    }
}


/* If C is not whitespace, return C.
   Otherwise skip whitespace and return first nonwhite char read.  */

static int
skip_white_space (c)
     register int c;
{
  static int newline_warning = 0;

  for (;;)
    {
      switch (c)
	{
	  /* We don't recognize comments here, because
	     cpp output can include / and * consecutively as operators.
	     Also, there's no need, since cpp removes all comments.  */

	case '\n':
	  c = check_newline ();
	  break;

	case ' ':
	case '\t':
	case '\f':
	case '\v':
	case '\b':
	  c = getc (finput);
	  break;

	case '\r':
	  /* ANSI C says the effects of a carriage return in a source file
	     are undefined.  */
	  if (pedantic && !newline_warning)
	    {
	      warning ("carriage return in source file");
	      warning ("(we only warn about the first carriage return)");
	      newline_warning = 1;
	    }
	  c = getc (finput);
	  break;

	case '\\':
	  c = getc (finput);
	  if (c == '\n')
	    lineno++;
	  else
	    error ("stray '\\' in program");
	  c = getc (finput);
	  break;

	default:
	  return (c);
	}
    }
}

/* Skips all of the white space at the current location in the input file.
   Must use and reset nextchar if it has the next character.  */

void
position_after_white_space ()
{
  register int c;

  if (nextchar != -1)
    c = nextchar, nextchar = -1;
  else
    c = getc (finput);

  ungetc (skip_white_space (c), finput);
}

/* Make the token buffer longer, preserving the data in it.
   P should point to just beyond the last valid character in the old buffer.
   The value we return is a pointer to the new buffer
   at a place corresponding to P.  */

static char *
extend_token_buffer (p)
     char *p;
{
  int offset = p - token_buffer;

  maxtoken = maxtoken * 2 + 10;
  token_buffer = (char *) xrealloc (token_buffer, maxtoken + 2);

  return token_buffer + offset;
}

/* At the beginning of a line, increment the line number
   and process any #-directive on this line.
   If the line is a #-directive, read the entire line and return a newline.
   Otherwise, return the line's first non-whitespace character.  */

int
check_newline ()
{
  register int c;
  register int token;

  lineno++;

  /* Read first nonwhite char on the line.  */

  c = getc (finput);
  while (c == ' ' || c == '\t')
    c = getc (finput);

  if (c != '#')
    {
      /* If not #, return it so caller will use it.  */
      return c;
    }

  /* Read first nonwhite char after the `#'.  */

  c = getc (finput);
  while (c == ' ' || c == '\t')
    c = getc (finput);

  /* If a letter follows, then if the word here is `line', skip
     it and ignore it; otherwise, ignore the line, with an error
     if the word isn't `pragma', `ident', `define', or `undef'.  */

  if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    {
      if (c == 'p')
	{
	  if (getc (finput) == 'r'
	      && getc (finput) == 'a'
	      && getc (finput) == 'g'
	      && getc (finput) == 'm'
	      && getc (finput) == 'a'
	      && ((c = getc (finput)) == ' ' || c == '\t' || c == '\n'))
	    {
#ifdef HANDLE_PRAGMA
	      HANDLE_PRAGMA (finput);
#endif /* HANDLE_PRAGMA */
	      goto skipline;
	    }
	}

      else if (c == 'd')
	{
	  if (getc (finput) == 'e'
	      && getc (finput) == 'f'
	      && getc (finput) == 'i'
	      && getc (finput) == 'n'
	      && getc (finput) == 'e'
	      && ((c = getc (finput)) == ' ' || c == '\t' || c == '\n'))
	    {
#ifdef DWARF_DEBUGGING_INFO
	      if ((debug_info_level == DINFO_LEVEL_VERBOSE)
		  && (write_symbols == DWARF_DEBUG))
	        dwarfout_define (lineno, get_directive_line (finput));
#endif /* DWARF_DEBUGGING_INFO */
	      goto skipline;
	    }
	}
      else if (c == 'u')
	{
	  if (getc (finput) == 'n'
	      && getc (finput) == 'd'
	      && getc (finput) == 'e'
	      && getc (finput) == 'f'
	      && ((c = getc (finput)) == ' ' || c == '\t' || c == '\n'))
	    {
#ifdef DWARF_DEBUGGING_INFO
	      if ((debug_info_level == DINFO_LEVEL_VERBOSE)
		  && (write_symbols == DWARF_DEBUG))
	        dwarfout_undef (lineno, get_directive_line (finput));
#endif /* DWARF_DEBUGGING_INFO */
	      goto skipline;
	    }
	}
      else if (c == 'l')
	{
	  if (getc (finput) == 'i'
	      && getc (finput) == 'n'
	      && getc (finput) == 'e'
	      && ((c = getc (finput)) == ' ' || c == '\t'))
	    goto linenum;
	}
      else if (c == 'i')
	{
	  if (getc (finput) == 'd'
	      && getc (finput) == 'e'
	      && getc (finput) == 'n'
	      && getc (finput) == 't'
	      && ((c = getc (finput)) == ' ' || c == '\t'))
	    {
	      /* #ident.  The pedantic warning is now in cccp.c.  */

	      /* Here we have just seen `#ident '.
		 A string constant should follow.  */

	      while (c == ' ' || c == '\t')
		c = getc (finput);

	      /* If no argument, ignore the line.  */
	      if (c == '\n')
		return c;

	      ungetc (c, finput);
	      token = yylex ();
	      if (token != STRING
		  || TREE_CODE (yylval.ttype) != STRING_CST)
		{
		  error ("invalid #ident");
		  goto skipline;
		}

	      if (!flag_no_ident)
		{
#ifdef ASM_OUTPUT_IDENT
		  ASM_OUTPUT_IDENT (asm_out_file, TREE_STRING_POINTER (yylval.ttype));
#endif
		}

	      /* Skip the rest of this line.  */
	      goto skipline;
	    }
	}

      error ("undefined or invalid # directive");
      goto skipline;
    }

linenum:
  /* Here we have either `#line' or `# <nonletter>'.
     In either case, it should be a line number; a digit should follow.  */

  while (c == ' ' || c == '\t')
    c = getc (finput);

  /* If the # is the only nonwhite char on the line,
     just ignore it.  Check the new newline.  */
  if (c == '\n')
    return c;

  /* Something follows the #; read a token.  */

  ungetc (c, finput);
  token = yylex ();

  if (token == CONSTANT
      && TREE_CODE (yylval.ttype) == INTEGER_CST)
    {
      int old_lineno = lineno;
      int used_up = 0;
      /* subtract one, because it is the following line that
	 gets the specified number */

      int l = TREE_INT_CST_LOW (yylval.ttype) - 1;

      /* Is this the last nonwhite stuff on the line?  */
      c = getc (finput);
      while (c == ' ' || c == '\t')
	c = getc (finput);
      if (c == '\n')
	{
	  /* No more: store the line number and check following line.  */
	  lineno = l;
	  return c;
	}
      ungetc (c, finput);

      /* More follows: it must be a string constant (filename).  */

      /* Read the string constant, but don't treat \ as special.  */
      ignore_escape_flag = 1;
      token = yylex ();
      ignore_escape_flag = 0;

      if (token != STRING || TREE_CODE (yylval.ttype) != STRING_CST)
	{
	  error ("invalid #line");
	  goto skipline;
	}

      input_filename
	= (char *) permalloc (TREE_STRING_LENGTH (yylval.ttype) + 1);
      strcpy (input_filename, TREE_STRING_POINTER (yylval.ttype));
      lineno = l;

      /* Each change of file name
	 reinitializes whether we are now in a system header.  */
      in_system_header = 0;

      if (main_input_filename == 0)
	main_input_filename = input_filename;

      /* Is this the last nonwhite stuff on the line?  */
      c = getc (finput);
      while (c == ' ' || c == '\t')
	c = getc (finput);
      if (c == '\n')
	return c;
      ungetc (c, finput);

      token = yylex ();
      used_up = 0;

      /* `1' after file name means entering new file.
	 `2' after file name means just left a file.  */

      if (token == CONSTANT
	  && TREE_CODE (yylval.ttype) == INTEGER_CST)
	{
	  if (TREE_INT_CST_LOW (yylval.ttype) == 1)
	    {
	      /* Pushing to a new file.  */
	      struct file_stack *p
		= (struct file_stack *) xmalloc (sizeof (struct file_stack));
	      input_file_stack->line = old_lineno;
	      p->next = input_file_stack;
	      p->name = input_filename;
	      input_file_stack = p;
	      input_file_stack_tick++;
#ifdef DWARF_DEBUGGING_INFO
	      if (debug_info_level == DINFO_LEVEL_VERBOSE
		  && write_symbols == DWARF_DEBUG)
		dwarfout_start_new_source_file (input_filename);
#endif /* DWARF_DEBUGGING_INFO */

	      used_up = 1;
	    }
	  else if (TREE_INT_CST_LOW (yylval.ttype) == 2)
	    {
	      /* Popping out of a file.  */
	      if (input_file_stack->next)
		{
		  struct file_stack *p = input_file_stack;
		  input_file_stack = p->next;
		  free (p);
		  input_file_stack_tick++;
#ifdef DWARF_DEBUGGING_INFO
		  if (debug_info_level == DINFO_LEVEL_VERBOSE
		      && write_symbols == DWARF_DEBUG)
		    dwarfout_resume_previous_source_file (input_file_stack->line);
#endif /* DWARF_DEBUGGING_INFO */
		}
	      else
		error ("#-lines for entering and leaving files don't match");

	      used_up = 1;
	    }
	}

      /* If we have handled a `1' or a `2',
	 see if there is another number to read.  */
      if (used_up)
	{
	  /* Is this the last nonwhite stuff on the line?  */
	  c = getc (finput);
	  while (c == ' ' || c == '\t')
	    c = getc (finput);
	  if (c == '\n')
	    return c;
	  ungetc (c, finput);

	  token = yylex ();
	  used_up = 0;
	}

      /* `3' after file name means this is a system header file.  */

      if (token == CONSTANT
	  && TREE_CODE (yylval.ttype) == INTEGER_CST
	  && TREE_INT_CST_LOW (yylval.ttype) == 3)
	in_system_header = 1;
    }
  else
    error ("invalid #-line");

  /* skip the rest of this line.  */
 skipline:
  if (c == '\n')
    return c;
  while ((c = getc (finput)) != EOF && c != '\n');
  return c;
}

#define isalnum(char) ((char >= 'a' && char <= 'z') || (char >= 'A' && char <= 'Z') || (char >= '0' && char <= '9'))
#define isdigit(char) (char >= '0' && char <= '9')
#define ENDFILE -1  /* token that represents end-of-file */

/* Read an escape sequence, returning its equivalent as a character,
   or -1 if it is backslash-newline.  */

static int
readescape ()
{
  register int c = getc (finput);
  register int code;
  register unsigned count;
  unsigned firstdig;

  switch (c)
    {
    case 'x':
      if (warn_traditional)
	warning ("the meaning of `\\x' varies with -traditional");

      if (flag_traditional)
	return c;

      code = 0;
      count = 0;
      while (1)
	{
	  c = getc (finput);
	  if (!(c >= 'a' && c <= 'f')
	      && !(c >= 'A' && c <= 'F')
	      && !(c >= '0' && c <= '9'))
	    {
	      ungetc (c, finput);
	      break;
	    }
	  code *= 16;
	  if (c >= 'a' && c <= 'f')
	    code += c - 'a' + 10;
	  if (c >= 'A' && c <= 'F')
	    code += c - 'A' + 10;
	  if (c >= '0' && c <= '9')
	    code += c - '0';
	  if (count == 0)
	    firstdig = code;
	  count++;
	}
      if (count == 0)
	error ("\\x used with no following hex digits");
      else if ((count - 1) * 4 >= TYPE_PRECISION (integer_type_node)
	       || (count > 1
		   && ((1 << (TYPE_PRECISION (integer_type_node) - (count - 1) * 4))
		       <= firstdig)))
	pedwarn ("hex escape out of range");
      return code;

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':
      code = 0;
      count = 0;
      while ((c <= '7') && (c >= '0') && (count++ < 3))
	{
	  code = (code * 8) + (c - '0');
	  c = getc (finput);
	}
      ungetc (c, finput);
      return code;

    case '\\': case '\'': case '"':
      return c;

    case '\n':
      lineno++;
      return -1;

    case 'n':
      return TARGET_NEWLINE;

    case 't':
      return TARGET_TAB;

    case 'r':
      return TARGET_CR;

    case 'f':
      return TARGET_FF;

    case 'b':
      return TARGET_BS;

    case 'a':
      if (warn_traditional)
	warning ("the meaning of `\\a' varies with -traditional");

      if (flag_traditional)
	return c;
      return TARGET_BELL;

    case 'v':
#if 0 /* Vertical tab is present in common usage compilers.  */
      if (flag_traditional)
	return c;
#endif
      return TARGET_VT;

    case 'E':
      pedwarn ("non-ANSI-standard escape sequence, `\\E'");
      return 033;

    case '?':
      return c;

      /* `\(', etc, are used at beginning of line to avoid confusing Emacs.  */
    case '(':
    case '{':
    case '[':
      if (pedantic)
	pedwarn ("non-ANSI escape sequence `\\%c'", c);
      return c;
    }
  if (c >= 040 && c <= 0177)
    pedwarn ("unknown escape sequence `\\%c'", c);
  else
    pedwarn ("unknown escape sequence: `\\' followed by char code 0x%x", c);
  return c;
}

void
yyerror (string)
     char *string;
{
  char buf[200];

  strcpy (buf, string);

  /* We can't print string and character constants well
     because the token_buffer contains the result of processing escapes.  */
  if (end_of_file)
    strcat (buf, " at end of input");
  else if (token_buffer[0] == 0)
    strcat (buf, " at null character");
  else if (token_buffer[0] == '"')
    strcat (buf, " before string constant");
  else if (token_buffer[0] == '\'')
    strcat (buf, " before character constant");
  else if (token_buffer[0] < 040 || (unsigned char) token_buffer[0] >= 0177)
    sprintf (buf + strlen (buf), " before character 0%o",
	     (unsigned char) token_buffer[0]);
  else
    strcat (buf, " before `%s'");

  error (buf, token_buffer);
}

#if 0

struct try_type
{
  tree *node_var;
  char unsigned_flag;
  char long_flag;
  char long_long_flag;
};

struct try_type type_sequence[] = 
{
  { &integer_type_node, 0, 0, 0},
  { &unsigned_type_node, 1, 0, 0},
  { &long_integer_type_node, 0, 1, 0},
  { &long_unsigned_type_node, 1, 1, 0},
  { &long_long_integer_type_node, 0, 1, 1},
  { &long_long_unsigned_type_node, 1, 1, 1}
};
#endif /* 0 */

int
yylex ()
{
  register int c;
  register char *p;
  register int value;
  int wide_flag = 0;

  if (nextchar >= 0)
    c = nextchar, nextchar = -1;
  else
    c = getc (finput);

  /* Effectively do c = skip_white_space (c)
     but do it faster in the usual cases.  */
  while (1)
    switch (c)
      {
      case ' ':
      case '\t':
      case '\f':
      case '\v':
      case '\b':
	c = getc (finput);
	break;

      case '\r':
	/* Call skip_white_space so we can warn if appropriate.  */

      case '\n':
      case '/':
      case '\\':
	c = skip_white_space (c);
      default:
	goto found_nonwhite;
      }
 found_nonwhite:

  token_buffer[0] = c;
  token_buffer[1] = 0;

/*  yylloc.first_line = lineno; */

  switch (c)
    {
    case EOF:
      end_of_file = 1;
      token_buffer[0] = 0;
      value = ENDFILE;
      break;

    case '$':
      if (dollars_in_ident)
	goto letter;
      return '$';

    case 'L':
      /* Capital L may start a wide-string or wide-character constant.  */
      {
	register int c = getc (finput);
	if (c == '\'')
	  {
	    wide_flag = 1;
	    goto char_constant;
	  }
	if (c == '"')
	  {
	    wide_flag = 1;
	    goto string_constant;
	  }
	ungetc (c, finput);
      }
      goto letter;

    case '@':
      if (!doing_objc_thang)
	{
	  value = c;
	  break;
	}
      p = token_buffer;
      *p++ = '@';
      c = getc (finput);
      while (isalnum (c) || c == '_')
	{
	  if (p >= token_buffer + maxtoken)
	    p = extend_token_buffer (p);

	  *p++ = c;
	  c = getc (finput);
	}

      *p = 0;
      nextchar = c;
      value = recognize_objc_keyword (token_buffer + 1);
      if (value != 0)
	break;
      error ("invalid Objective C keyword `%s'", token_buffer);
      /* Cause a syntax error--1 is not a valid token type.  */
      value = 1;
      break;

    case 'A':  case 'B':  case 'C':  case 'D':  case 'E':
    case 'F':  case 'G':  case 'H':  case 'I':  case 'J':
    case 'K':		  case 'M':  case 'N':  case 'O':
    case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':
    case 'U':  case 'V':  case 'W':  case 'X':  case 'Y':
    case 'Z':
    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
    case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
    case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
    case 'p':  case 'q':  case 'r':  case 's':  case 't':
    case 'u':  case 'v':  case 'w':  case 'x':  case 'y':
    case 'z':
    case '_':
    letter:
      p = token_buffer;
      while (isalnum (c) || c == '_' || c == '$' || c == '@')
	{
	  if (p >= token_buffer + maxtoken)
	    p = extend_token_buffer (p);
	  if (c == '$' && ! dollars_in_ident)
	    break;

	  *p++ = c;
	  c = getc (finput);
	}

      *p = 0;
      nextchar = c;

      value = IDENTIFIER;
      yylval.itype = 0;

      /* Try to recognize a keyword.  Uses minimum-perfect hash function */

      {
	register struct resword *ptr;

	if (ptr = is_reserved_word (token_buffer, p - token_buffer))
	  {
	    if (ptr->rid)
	      yylval.ttype = ridpointers[(int) ptr->rid];
	    value = (int) ptr->token;

	    /* Even if we decided to recognize asm, still perhaps warn.  */
	    if (pedantic
		&& (value == ASM_KEYWORD || value == TYPEOF
		    || ptr->rid == RID_INLINE)
		&& token_buffer[0] != '_')
	      pedwarn ("ANSI does not permit the keyword `%s'",
		       token_buffer);
	  }
      }

      /* If we did not find a keyword, look for an identifier
	 (or a typename).  */

      if (value == IDENTIFIER)
	{
          yylval.ttype = get_identifier (token_buffer);
	  lastiddecl = lookup_name (yylval.ttype);

	  if (lastiddecl != 0 && TREE_CODE (lastiddecl) == TYPE_DECL)
	    value = TYPENAME;
	  /* A user-invisible read-only initialized variable
	     should be replaced by its value.
	     We handle only strings since that's the only case used in C.  */
	  else if (lastiddecl != 0 && TREE_CODE (lastiddecl) == VAR_DECL
		   && DECL_IGNORED_P (lastiddecl)
		   && TREE_READONLY (lastiddecl)
		   && DECL_INITIAL (lastiddecl) != 0
		   && TREE_CODE (DECL_INITIAL (lastiddecl)) == STRING_CST)
	    {
	      yylval.ttype = DECL_INITIAL (lastiddecl);
	      value = STRING;
	    }
          else if (doing_objc_thang)
            {
	      tree objc_interface_decl = lookup_interface (yylval.ttype);

	      if (objc_interface_decl)
		{
		  value = CLASSNAME;
		  yylval.ttype = objc_interface_decl;
		}
	    }
	}

      break;

    case '0':  case '1':  case '2':  case '3':  case '4':
    case '5':  case '6':  case '7':  case '8':  case '9':
    case '.':
      {
	int base = 10;
	int count = 0;
	int largest_digit = 0;
	int numdigits = 0;
	/* for multi-precision arithmetic,
	   we store only 8 live bits in each short,
	   giving us 64 bits of reliable precision */
	short shorts[8];
	int overflow = 0;

	enum anon1 { NOT_FLOAT, AFTER_POINT, TOO_MANY_POINTS} floatflag
	  = NOT_FLOAT;

	for (count = 0; count < 8; count++)
	  shorts[count] = 0;

	p = token_buffer;
	*p++ = c;

	if (c == '0')
	  {
	    *p++ = (c = getc (finput));
	    if ((c == 'x') || (c == 'X'))
	      {
		base = 16;
		*p++ = (c = getc (finput));
	      }
	    /* Leading 0 forces octal unless the 0 is the only digit.  */
	    else if (c >= '0' && c <= '9')
	      {
		base = 8;
		numdigits++;
	      }
	    else
	      numdigits++;
	  }

	/* Read all the digits-and-decimal-points.  */

	while (c == '.'
	       || (isalnum (c) && (c != 'l') && (c != 'L')
		   && (c != 'u') && (c != 'U')
		   && (floatflag == NOT_FLOAT || ((c != 'f') && (c != 'F')))))
	  {
	    if (c == '.')
	      {
		if (base == 16)
		  error ("floating constant may not be in radix 16");
		if (floatflag == AFTER_POINT)
		  {
		    error ("malformed floating constant");
		    floatflag = TOO_MANY_POINTS;
		  }
		else
		  floatflag = AFTER_POINT;

		base = 10;
		*p++ = c = getc (finput);
		/* Accept '.' as the start of a floating-point number
		   only when it is followed by a digit.
		   Otherwise, unread the following non-digit
		   and use the '.' as a structural token.  */
		if (p == token_buffer + 2 && !isdigit (c))
		  {
		    if (c == '.')
		      {
			c = getc (finput);
			if (c == '.')
			  {
			    *p++ = c;
			    *p = 0;
			    return ELLIPSIS;
			  }
			error ("parse error at `..'");
		      }
		    ungetc (c, finput);
		    token_buffer[1] = 0;
		    value = '.';
		    goto done;
		  }
	      }
	    else
	      {
		/* It is not a decimal point.
		   It should be a digit (perhaps a hex digit).  */

		if (isdigit (c))
		  {
		    c = c - '0';
		  }
		else if (base <= 10)
		  {
		    if ((c&~040) == 'E')
		      {
			base = 10;
			floatflag = AFTER_POINT;
			break;   /* start of exponent */
		      }
		    error ("nondigits in number and not hexadecimal");
		    c = 0;
		  }
		else if (c >= 'a')
		  {
		    c = c - 'a' + 10;
		  }
		else
		  {
		    c = c - 'A' + 10;
		  }
		if (c >= largest_digit)
		  largest_digit = c;
		numdigits++;

		for (count = 0; count < 8; count++)
		  {
		    shorts[count] *= base;
		    if (count)
		      {
			shorts[count] += (shorts[count-1] >> 8);
			shorts[count-1] &= (1<<8)-1;
		      }
		    else shorts[0] += c;
		  }

		if (shorts[7] >= 1<<8
		    || shorts[7] < - (1 << 8))
		  overflow = TRUE;

		if (p >= token_buffer + maxtoken - 3)
		  p = extend_token_buffer (p);
		*p++ = (c = getc (finput));
	      }
	  }

	if (numdigits == 0)
	  error ("numeric constant with no digits");

	if (largest_digit >= base)
	  error ("numeric constant contains digits beyond the radix");

	/* Remove terminating char from the token buffer and delimit the string */
	*--p = 0;

	if (floatflag != NOT_FLOAT)
	  {
	    tree type = double_type_node;
	    char f_seen = 0;
	    char l_seen = 0;
	    REAL_VALUE_TYPE value;
	    jmp_buf handler;

	    /* Read explicit exponent if any, and put it in tokenbuf.  */

	    if ((c == 'e') || (c == 'E'))
	      {
		if (p >= token_buffer + maxtoken - 3)
		  p = extend_token_buffer (p);
		*p++ = c;
		c = getc (finput);
		if ((c == '+') || (c == '-'))
		  {
		    *p++ = c;
		    c = getc (finput);
		  }
		if (! isdigit (c))
		  error ("floating constant exponent has no digits");
	        while (isdigit (c))
		  {
		    if (p >= token_buffer + maxtoken - 3)
		      p = extend_token_buffer (p);
		    *p++ = c;
		    c = getc (finput);
		  }
	      }

	    *p = 0;
	    errno = 0;

	    /* Convert string to a double, checking for overflow.  */
	    if (setjmp (handler))
	      {
		error ("floating constant out of range");
		value = dconst0;
	      }
	    else
	      {
		set_float_handler (handler);
		value = REAL_VALUE_ATOF (token_buffer);
		set_float_handler (0);
	      }
#ifdef ERANGE
	    if (errno == ERANGE && !flag_traditional && pedantic)
	      {
		char *p1 = token_buffer;
		/* Check for "0.0" and variants;
		   Sunos 4 spuriously returns ERANGE for them.  */
		while (*p1 == '0') p1++;
		if (*p1 == '.')
		  {
		    p1++;
		    while (*p1 == '0') p1++;
		  }
		if (*p1 == 'e' || *p1 == 'E')
		  {
		    /* with significand==0, ignore the exponent */
		    p1++;
		    while (*p1 != 0) p1++;
		  }
		/* ERANGE is also reported for underflow,
		   so test the value to distinguish overflow from that.  */
		if (*p1 != 0 && (value > 1.0 || value < -1.0))
		  pedwarn ("floating point number exceeds range of `double'");
	      }
#endif

	    /* Read the suffixes to choose a data type.  */
	    while (1)
	      {
		if (c == 'f' || c == 'F')
		  {
		    if (f_seen)
		      error ("two `f's in floating constant");
		    else
		      {
			f_seen = 1;
			type = float_type_node;
			value = REAL_VALUE_TRUNCATE (TYPE_MODE (type), value);
			if (REAL_VALUE_ISINF (value) && pedantic)
			  pedwarn ("floating point number exceeds range of `float'");
		      }
		  }
		else if (c == 'l' || c == 'L')
		  {
		    if (l_seen)
		      error ("two `l's in floating constant");
		    l_seen = 1;
		    type = long_double_type_node;
		  }
		else
		  {
		    if (isalnum (c))
		      {
			error ("garbage at end of number");
			while (isalnum (c))
			  {
			    if (p >= token_buffer + maxtoken - 3)
			      p = extend_token_buffer (p);
			    *p++ = c;
			    c = getc (finput);
			  }
		      }
		    break;
		  }
		if (p >= token_buffer + maxtoken - 3)
		  p = extend_token_buffer (p);
		*p++ = c;
		c = getc (finput);
	      }

	    /* Create a node with determined type and value.  */
	    yylval.ttype = build_real (type, value);

	    ungetc (c, finput);
	    *p = 0;
	  }
	else
	  {
	    tree traditional_type, ansi_type, type;
	    int spec_unsigned = 0;
	    int spec_long = 0;
	    int spec_long_long = 0;
	    int bytes, warn, i;

	    while (1)
	      {
		if (c == 'u' || c == 'U')
		  {
		    if (spec_unsigned)
		      error ("two `u's in integer constant");
		    spec_unsigned = 1;
		  }
		else if (c == 'l' || c == 'L')
		  {
		    if (spec_long)
		      {
			if (spec_long_long)
			  error ("three `l's in integer constant");
			else if (pedantic)
			  pedwarn ("ANSI C forbids long long integer constants");
			spec_long_long = 1;
		      }
		    spec_long = 1;
		  }
		else
		  {
		    if (isalnum (c))
		      {
			error ("garbage at end of number");
			while (isalnum (c))
			  {
			    if (p >= token_buffer + maxtoken - 3)
			      p = extend_token_buffer (p);
			    *p++ = c;
			    c = getc (finput);
			  }
		      }
		    break;
		  }
		if (p >= token_buffer + maxtoken - 3)
		  p = extend_token_buffer (p);
		*p++ = c;
		c = getc (finput);
	      }

	    ungetc (c, finput);

	    /* If the constant is not long long and it won't fit in an
	       unsigned long, or if the constant is long long and won't fit
	       in an unsigned long long, then warn that the constant is out
	       of range.  */

	    /* ??? This assumes that long long and long integer types are
	       a multiple of 8 bits.  This better than the original code
	       though which assumed that long was exactly 32 bits and long
	       long was exactly 64 bits.  */

	    if (spec_long_long)
	      bytes = TYPE_PRECISION (long_long_integer_type_node) / 8;
	    else
	      bytes = TYPE_PRECISION (long_integer_type_node) / 8;

	    if (bytes <= 8)
	      {
		warn = overflow;
		for (i = bytes; i < 8; i++)
		  if (shorts[i])
		    {
		      /* If LL was not used, then clear any excess precision.
			 This is equivalent to the original code, but it is
			 not clear why this is being done.  Perhaps to prevent
			 ANSI programs from creating long long constants
			 by accident?  */
		      if (! spec_long_long)
			shorts[i] = 0;
		      warn = 1;
		    }
		if (warn)
		  pedwarn ("integer constant out of range");
	      }
	    else if (overflow)
	      pedwarn ("integer constant larger than compiler can handle");

	    /* If it overflowed our internal buffer, then make it unsigned.
	       We can't distinguish based on the tree node because
	       any integer constant fits any long long type.  */
	    if (overflow)
	      spec_unsigned = 1;

	    /* This is simplified by the fact that our constant
	       is always positive.  */
	    /* The casts in the following statement should not be
	       needed, but they get around bugs in some C compilers.  */
	    yylval.ttype
	      = (build_int_2
		 ((((long)shorts[3]<<24) + ((long)shorts[2]<<16)
		   + ((long)shorts[1]<<8) + (long)shorts[0]),
		  (((long)shorts[7]<<24) + ((long)shorts[6]<<16)
		   + ((long)shorts[5]<<8) + (long)shorts[4])));

#if 0
	    /* Find the first allowable type that the value fits in.  */
	    type = 0;
	    for (i = 0; i < sizeof (type_sequence) / sizeof (type_sequence[0]);
		 i++)
	      if (!(spec_long && !type_sequence[i].long_flag)
		  && !(spec_long_long && !type_sequence[i].long_long_flag)
		  && !(spec_unsigned && !type_sequence[i].unsigned_flag)
		  /* A decimal constant can't be unsigned int
		     unless explicitly specified.  */
		  && !(base == 10 && !spec_unsigned
		       && *type_sequence[i].node_var == unsigned_type_node))
		if (int_fits_type_p (yylval.ttype, *type_sequence[i].node_var))
		  {
		    type = *type_sequence[i].node_var;
		    break;
		  }
	    if (flag_traditional && type == long_unsigned_type_node
		&& !spec_unsigned)
	      type = long_integer_type_node;
	      
	    if (type == 0)
	      {
		type = long_long_integer_type_node;
		warning ("integer constant out of range");
	      }

	    /* Warn about some cases where the type of a given constant
	       changes from traditional C to ANSI C.  */
	    if (warn_traditional)
	      {
		tree other_type = 0;

		/* This computation is the same as the previous one
		   except that flag_traditional is used backwards.  */
		for (i = 0; i < sizeof (type_sequence) / sizeof (type_sequence[0]);
		     i++)
		  if (!(spec_long && !type_sequence[i].long_flag)
		      && !(spec_long_long && !type_sequence[i].long_long_flag)
		      && !(spec_unsigned && !type_sequence[i].unsigned_flag)
		      /* A decimal constant can't be unsigned int
			 unless explicitly specified.  */
		      && !(base == 10 && !spec_unsigned
			   && *type_sequence[i].node_var == unsigned_type_node))
		    if (int_fits_type_p (yylval.ttype, *type_sequence[i].node_var))
		      {
			other_type = *type_sequence[i].node_var;
			break;
		      }
		if (!flag_traditional && type == long_unsigned_type_node
		    && !spec_unsigned)
		  type = long_integer_type_node;
	      
		if (other_type != 0 && other_type != type)
		  {
		    if (flag_traditional)
		      warning ("type of integer constant would be different without -traditional");
		    else
		      warning ("type of integer constant would be different with -traditional");
		  }
	      }

#else /* 1 */
	    /* If warn_traditional, calculate both the ANSI type and the
	       traditional type, then see if they disagree.
	       Otherwise, calculate only the type for the dialect in use.  */
	    if (warn_traditional || flag_traditional)
	      {
		/* Calculate the traditional type.  */
		/* Traditionally, any constant is signed;
		   but if unsigned is specified explicitly, obey that.
		   Use the smallest size with the right number of bits,
		   except for one special case with decimal constants.  */
		if (! spec_long && base != 10
		    && int_fits_type_p (yylval.ttype, unsigned_type_node))
		  traditional_type = (spec_unsigned ? unsigned_type_node
				      : integer_type_node);
		/* A decimal constant must be long
		   if it does not fit in type int.
		   I think this is independent of whether
		   the constant is signed.  */
		else if (! spec_long && base == 10
			 && int_fits_type_p (yylval.ttype, integer_type_node))
		  traditional_type = (spec_unsigned ? unsigned_type_node
				      : integer_type_node);
		else if (! spec_long_long
			 && int_fits_type_p (yylval.ttype,
					     long_unsigned_type_node))
		  traditional_type = (spec_unsigned ? long_unsigned_type_node
				      : long_integer_type_node);
		else
		  traditional_type = (spec_unsigned
				      ? long_long_unsigned_type_node
				      : long_long_integer_type_node);
	      }
	    if (warn_traditional || ! flag_traditional)
	      {
		/* Calculate the ANSI type.  */
		if (! spec_long && ! spec_unsigned
		    && int_fits_type_p (yylval.ttype, integer_type_node))
		  ansi_type = integer_type_node;
		else if (! spec_long && (base != 10 || spec_unsigned)
			 && int_fits_type_p (yylval.ttype, unsigned_type_node))
		  ansi_type = unsigned_type_node;
		else if (! spec_unsigned && !spec_long_long
			 && int_fits_type_p (yylval.ttype, long_integer_type_node))
		  ansi_type = long_integer_type_node;
		else if (! spec_long_long
			 && int_fits_type_p (yylval.ttype,
					     long_unsigned_type_node))
		  ansi_type = long_unsigned_type_node;
		else if (! spec_unsigned
			 && int_fits_type_p (yylval.ttype,
					     long_long_integer_type_node))
		  ansi_type = long_long_integer_type_node;
		else
		  ansi_type = long_long_unsigned_type_node;
	      }

	    type = flag_traditional ? traditional_type : ansi_type;

	    if (warn_traditional && traditional_type != ansi_type)
	      {
		if (TYPE_PRECISION (traditional_type)
		    != TYPE_PRECISION (ansi_type))
		  warning ("width of integer constant changes with -traditional");
		else if (TREE_UNSIGNED (traditional_type)
			 != TREE_UNSIGNED (ansi_type))
		  warning ("integer constant is unsigned in ANSI C, signed with -traditional");
		else
		  warning ("width of integer constant may change on other systems with -traditional");
	      }
#endif

	    if (!flag_traditional && !int_fits_type_p (yylval.ttype, type))
	      pedwarn ("integer constant out of range");

	    TREE_TYPE (yylval.ttype) = type;
	    *p = 0;
	  }

	value = CONSTANT; break;
      }

    case '\'':
    char_constant:
      {
	register int result = 0;
	register num_chars = 0;
	unsigned width = TYPE_PRECISION (char_type_node);
	int max_chars;

	if (wide_flag)
	  {
	    width = WCHAR_TYPE_SIZE;
#ifdef MULTIBYTE_CHARS
	    max_chars = MB_CUR_MAX;
#else
	    max_chars = 1;
#endif
	  }
	else
	  max_chars = TYPE_PRECISION (integer_type_node) / width;

	while (1)
	  {
	  tryagain:

	    c = getc (finput);

	    if (c == '\'' || c == EOF)
	      break;

	    if (c == '\\')
	      {
		c = readescape ();
		if (c < 0)
		  goto tryagain;
		if (width < HOST_BITS_PER_INT
		    && (unsigned) c >= (1 << width))
		  pedwarn ("escape sequence out of range for character");
	      }
	    else if (c == '\n')
	      {
		if (pedantic)
		  pedwarn ("ANSI C forbids newline in character constant");
		lineno++;
	      }

	    num_chars++;
	    if (num_chars > maxtoken - 4)
	      extend_token_buffer (token_buffer);

	    token_buffer[num_chars] = c;

	    /* Merge character into result; ignore excess chars.  */
	    if (num_chars < max_chars + 1)
	      {
		if (width < HOST_BITS_PER_INT)
		  result = (result << width) | (c & ((1 << width) - 1));
		else
		  result = c;
	      }
	  }

	token_buffer[num_chars + 1] = '\'';
	token_buffer[num_chars + 2] = 0;

	if (c != '\'')
	  error ("malformatted character constant");
	else if (num_chars == 0)
	  error ("empty character constant");
	else if (num_chars > max_chars)
	  {
	    num_chars = max_chars;
	    error ("character constant too long");
	  }
	else if (num_chars != 1 && ! flag_traditional)
	  warning ("multi-character character constant");

	/* If char type is signed, sign-extend the constant.  */
	if (! wide_flag)
	  {
	    int num_bits = num_chars * width;
	    if (TREE_UNSIGNED (char_type_node)
		|| ((result >> (num_bits - 1)) & 1) == 0)
	      yylval.ttype
		= build_int_2 (result & ((unsigned) ~0
					 >> (HOST_BITS_PER_INT - num_bits)),
			       0);
	    else
	      yylval.ttype
		= build_int_2 (result | ~((unsigned) ~0
					  >> (HOST_BITS_PER_INT - num_bits)),
			       -1);
	  }
	else
	  {
#ifdef MULTIBYTE_CHARS
	    /* Set the initial shift state and convert the next sequence.  */
	    result = 0;
	    /* In all locales L'\0' is zero and mbtowc will return zero,
	       so don't use it.  */
	    if (num_chars > 1
		|| (num_chars == 1 && token_buffer[1] != '\0'))
	      {
		wchar_t wc;
		(void) mbtowc (NULL, NULL, 0);
		if (mbtowc (& wc, token_buffer + 1, num_chars) == num_chars)
		  result = wc;
		else
		  warning ("Ignoring invalid multibyte character");
	      }
#endif
	    yylval.ttype = build_int_2 (result, 0);
	  }

	TREE_TYPE (yylval.ttype) = integer_type_node;
	value = CONSTANT;
	break;
      }

    case '"':
    string_constant:
      {
	c = getc (finput);
	p = token_buffer + 1;

	while (c != '"' && c >= 0)
	  {
	    /* ignore_escape_flag is set for reading the filename in #line.  */
	    if (!ignore_escape_flag && c == '\\')
	      {
		c = readescape ();
		if (c < 0)
		  goto skipnewline;
		if (!wide_flag
		    && TYPE_PRECISION (char_type_node) < HOST_BITS_PER_INT
		    && c >= (1 << TYPE_PRECISION (char_type_node)))
		  pedwarn ("escape sequence out of range for character");
	      }
	    else if (c == '\n')
	      {
		if (pedantic)
		  pedwarn ("ANSI C forbids newline in string constant");
		lineno++;
	      }

	    if (p == token_buffer + maxtoken)
	      p = extend_token_buffer (p);
	    *p++ = c;

	  skipnewline:
	    c = getc (finput);
	  }
	*p = 0;

	/* We have read the entire constant.
	   Construct a STRING_CST for the result.  */

	if (wide_flag)
	  {
	    /* If this is a L"..." wide-string, convert the multibyte string
	       to a wide character string.  */
	    char *widep = (char *) alloca ((p - token_buffer) * WCHAR_BYTES);
	    int len;

#ifdef MULTIBYTE_CHARS
	    len = mbstowcs ((wchar_t *) widep, token_buffer + 1, p - token_buffer);
	    if ((unsigned) len >= (p - token_buffer))
	      {
		warning ("Ignoring invalid multibyte string");
		len = 0;
	      }
	    bzero (widep + (len * WCHAR_BYTES), WCHAR_BYTES);
#else
	    {
	      union { long l; char c[sizeof (long)]; } u;
	      int big_endian;
	      char *wp, *cp;

	      /* Determine whether host is little or big endian.  */
	      u.l = 1;
	      big_endian = u.c[sizeof (long) - 1];
	      wp = widep + (big_endian ? WCHAR_BYTES - 1 : 0);

	      bzero (widep, (p - token_buffer) * WCHAR_BYTES);
	      for (cp = token_buffer + 1; cp < p; cp++)
		*wp = *cp, wp += WCHAR_BYTES;
	      len = p - token_buffer - 1;
	    }
#endif
	    yylval.ttype = build_string ((len + 1) * WCHAR_BYTES, widep);
	    TREE_TYPE (yylval.ttype) = wchar_array_type_node;
	  }
	else
	  {
	    yylval.ttype = build_string (p - token_buffer, token_buffer + 1);
	    TREE_TYPE (yylval.ttype) = char_array_type_node;
	  }

	*p++ = '"';
	*p = 0;

	value = STRING; break;
      }

    case '+':
    case '-':
    case '&':
    case '|':
    case '<':
    case '>':
    case '*':
    case '/':
    case '%':
    case '^':
    case '!':
    case '=':
      {
	register int c1;

      combine:

	switch (c)
	  {
	  case '+':
	    yylval.code = PLUS_EXPR; break;
	  case '-':
	    yylval.code = MINUS_EXPR; break;
	  case '&':
	    yylval.code = BIT_AND_EXPR; break;
	  case '|':
	    yylval.code = BIT_IOR_EXPR; break;
	  case '*':
	    yylval.code = MULT_EXPR; break;
	  case '/':
	    yylval.code = TRUNC_DIV_EXPR; break;
	  case '%':
	    yylval.code = TRUNC_MOD_EXPR; break;
	  case '^':
	    yylval.code = BIT_XOR_EXPR; break;
	  case LSHIFT:
	    yylval.code = LSHIFT_EXPR; break;
	  case RSHIFT:
	    yylval.code = RSHIFT_EXPR; break;
	  case '<':
	    yylval.code = LT_EXPR; break;
	  case '>':
	    yylval.code = GT_EXPR; break;
	  }

	token_buffer[1] = c1 = getc (finput);
	token_buffer[2] = 0;

	if (c1 == '=')
	  {
	    switch (c)
	      {
	      case '<':
		value = ARITHCOMPARE; yylval.code = LE_EXPR; goto done;
	      case '>':
		value = ARITHCOMPARE; yylval.code = GE_EXPR; goto done;
	      case '!':
		value = EQCOMPARE; yylval.code = NE_EXPR; goto done;
	      case '=':
		value = EQCOMPARE; yylval.code = EQ_EXPR; goto done;
	      }
	    value = ASSIGN; goto done;
	  }
	else if (c == c1)
	  switch (c)
	    {
	    case '+':
	      value = PLUSPLUS; goto done;
	    case '-':
	      value = MINUSMINUS; goto done;
	    case '&':
	      value = ANDAND; goto done;
	    case '|':
	      value = OROR; goto done;
	    case '<':
	      c = LSHIFT;
	      goto combine;
	    case '>':
	      c = RSHIFT;
	      goto combine;
	    }
	else if ((c == '-') && (c1 == '>'))
	  { value = POINTSAT; goto done; }
	ungetc (c1, finput);
	token_buffer[1] = 0;

	if ((c == '<') || (c == '>'))
	  value = ARITHCOMPARE;
	else value = c;
	goto done;
      }

    case 0:
      /* Don't make yyparse think this is eof.  */
      value = 1;
      break;

    default:
      value = c;
    }

done:
/*  yylloc.last_line = lineno; */

  return value;
}

/* Sets the value of the 'yydebug' variable to VALUE.
   This is a function so we don't have to have YYDEBUG defined
   in order to build the compiler.  */

void
set_yydebug (value)
     int value;
{
#if YYDEBUG != 0
  yydebug = value;
#else
  warning ("YYDEBUG not defined.");
#endif
}
