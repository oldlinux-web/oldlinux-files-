/* Support for GDB maintenance commands.
   Copyright (C) 1992 Free Software Foundation, Inc.
   Written by Fred Fish at Cygnus Support.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#include "defs.h"

#if MAINTENANCE_CMDS	/* Entire file goes away if not including maint cmds */

#include <signal.h>
#include "command.h"
#include "gdbcmd.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "demangle.h"

static void
maintenance_command PARAMS ((char *, int));

static void
maintenance_dump_me PARAMS ((char *, int));

static void
maintenance_demangle PARAMS ((char *, int));

/*

LOCAL FUNCTION

	maintenance_command -- access the maintenance subcommands

SYNOPSIS

	void maintenance_command (char *args, int from_tty)

DESCRIPTION

*/

static void
maintenance_command (args, from_tty)
     char *args;
     int from_tty;
{
  printf ("\"maintenance\" must be followed by the name of a maintenance command.\n");
  help_list (maintenancelist, "maintenance ", -1, stdout);
}


/* ARGSUSED */
static void
maintenance_dump_me (args, from_tty)
     char *args;
     int from_tty;
{
  if (query ("Should GDB dump core? "))
    {
      signal (SIGQUIT, SIG_DFL);
      kill (getpid (), SIGQUIT);
    }
}

/*  Someday we should allow demangling for things other than just
    explicit strings.  For example, we might want to be able to
    specify the address of a string in either GDB's process space
    or the debuggee's process space, and have gdb fetch and demangle
    that string.  If we have a char* pointer "ptr" that points to
    a string, we might want to be able to given just the name and
    have GDB demangle and print what it points to, etc.  (FIXME) */

static void
maintenance_demangle (args, from_tty)
     char *args;
     int from_tty;
{
  char *demangled;

  if (args == NULL || *args == '\0')
    {
      printf ("\"maintenance demangle\" takes an argument to demangle.\n");
    }
  else
    {
      demangled = cplus_demangle (args, DMGL_ANSI | DMGL_PARAMS);
      if (demangled != NULL)
	{
	  printf ("%s\n", demangled);
	  free (demangled);
	}
      else
	{
	  printf ("Can't demangle \"%s\"\n", args);
	}
    }
}

/* The "maintenance info" command is defined as a prefix, with allow_unknown 0.
   Therefore, its own definition is called only for "maintenance info" with
   no args.  */

/* ARGSUSED */
static void
maintenance_info_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  printf ("\"maintenance info\" must be followed by the name of an info command.\n");
  help_list (maintenanceinfolist, "maintenance info ", -1, stdout);
}

/* The "maintenance print" command is defined as a prefix, with allow_unknown
   0.  Therefore, its own definition is called only for "maintenance print"
   with no args.  */

/* ARGSUSED */
static void
maintenance_print_command (arg, from_tty)
     char *arg;
     int from_tty;
{
  printf ("\"maintenance print\" must be followed by the name of a print command.\n");
  help_list (maintenanceprintlist, "maintenance print ", -1, stdout);
}

/*

GLOBAL FUNCTION

	_initialize_maint_cmds -- initialize the process file system stuff

SYNOPSIS

	void _initialize_maint_cmds (void)

DESCRIPTION

	Do required initializations during gdb startup for using the
	/proc file system interface.

*/


void
_initialize_maint_cmds ()
{
  add_prefix_cmd ("maintenance", class_maintenance, maintenance_command,
		  "Commands for use by GDB maintainers.\n\
Includes commands to dump specific internal GDB structures in\n\
a human readable form, to cause GDB to deliberately dump core,\n\
to test internal functions such as the C++ demangler, etc.",
		  &maintenancelist, "maintenance ", 0,
		  &cmdlist);

  add_com_alias ("mt", "maintenance", class_maintenance, 0);

  add_prefix_cmd ("info", class_info, maintenance_info_command,
		  "Commands for showing things about the program being debugged.",
		  &maintenanceinfolist, "maintenance info ", 0,
		  &maintenancelist);

  add_prefix_cmd ("print", class_maintenance, maintenance_print_command,
		  "Maintenance command for printing GDB internal state.",
		  &maintenanceprintlist, "maintenance print ", 0,
		  &maintenancelist);

  add_cmd ("dump-me", class_maintenance, maintenance_dump_me,
	   "Get fatal error; make debugger dump its core.\n\
GDB sets it's handling of SIGQUIT back to SIG_DFL and then sends\n\
itself a SIGQUIT signal.",
	   &maintenancelist);

  add_cmd ("demangle", class_maintenance, maintenance_demangle,
	   "Demangle a C++ mangled name.\n\
Call internal GDB demangler routine to demangle a C++ link name\n\
and prints the result.",
	   &maintenancelist);

  add_cmd ("type", class_maintenance, maintenance_print_type,
	   "Print a type chain for a given symbol.\n\
For each node in a type chain, print the raw data for each member of\n\
the type structure, and the interpretation of the data.",
	   &maintenanceprintlist);

  add_cmd ("symbols", class_maintenance, maintenance_print_symbols,
	   "Print dump of current symbol definitions.\n\
Entries in the full symbol table are dumped to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's symbols.",
	   &maintenanceprintlist);

  add_cmd ("msymbols", class_maintenance, maintenance_print_msymbols,
	   "Print dump of current minimal symbol definitions.\n\
Entries in the minimal symbol table are dumped to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's minimal symbols.",
	   &maintenanceprintlist);

  add_cmd ("psymbols", class_maintenance, maintenance_print_psymbols,
	   "Print dump of current partial symbol definitions.\n\
Entries in the partial symbol table are dumped to file OUTFILE.\n\
If a SOURCE file is specified, dump only that file's partial symbols.",
	   &maintenanceprintlist);

  add_cmd ("objfiles", class_maintenance, maintenance_print_objfiles,
	   "Print dump of current object file definitions.",
	   &maintenanceprintlist);

}

#endif	/* MAINTENANCE_CMDS */
