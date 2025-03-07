
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

#ifndef LINT
__attribute__((unused)) static const char *rcsid = "$Id: prtrace.c,v 1.1.1.1 1998/12/29 21:36:11 paradis Exp $";
#endif

/*
 * $Log: prtrace.c,v $
 * Revision 1.1.1.1  1998/12/29 21:36:11  paradis
 * Initial CVS checkin
 *
 * Revision 1.2  1995/10/31  16:45:05  cruz
 *  Moved variable prtrace_enable from initdata.c
 *
 * Revision 1.1  1995/10/18  14:17:38  cruz
 * Initial revision
 *
 *
 */

#include "lib.h"

#ifdef TRACE_ENABLE

/*
 * Tracing with debug prints using the PRTRACE
 * macros (see prtrace.h) can be performed by
 * two different means. (1) Defining PRTRACE
 * will build in the tracing at compile time.
 * (2) Defining TRACE_ENABLE will also build
 * in tracing at compile time.  However, most
 * of it can be enabled or disabled at run
 * time with the debug monitor prtrace command.
 */
int prtrace_enable;

#define MAX_VAR_TABLE_ENTRIES 10

typedef struct {
    char *varname;
    ui   *varaddr;
    VARIABLE_VALUES *value_table;
} VARIABLE;


extern ui arp_debug, ip_debug, tftp_debug, udp_debug;
extern VARIABLE_VALUES arp_debug_table[], ip_debug_table[], tftp_debug_table[], udp_debug_table[];

static VARIABLE var_table[MAX_VAR_TABLE_ENTRIES];
static ui var_table_idx;  /* Initialized to 0 */
static int initialized;   /* Initialized to 0 */

/*
 *  Used for adding variables to the list of variable that can be
 *  accessed from the user interface.  The arguments are: a string
 *  to be associated with the variable name, the address of that
 *  variable and a table that maps values in that variable to a
 *  string
 */
static BOOLEAN add_variable(char *varname, ui *varaddr, VARIABLE_VALUES *val_table)
{
    if (var_table_idx >= MAX_VAR_TABLE_ENTRIES)
      return FALSE;

    var_table[var_table_idx].varname = varname;
    var_table[var_table_idx].varaddr = varaddr;
    var_table[var_table_idx].value_table = val_table;
    var_table_idx++;
    return TRUE;
}


/*
 *  Searches the list of all variables available to see if
 *  the specified name is one of the variables that can be
 *  accessed from the user interface.
 */
static VARIABLE * find_var(char *varname)
{
    int i;
    for (i=0; i < var_table_idx; i++) {
        if (strcmp(varname, var_table[i].varname) == 0)
          return (&var_table[i]);
    }
    return NULL;
}

/*
 *  Prints a list of all the variables that can be accessed
 *  from the user interface.
 */
static void print_variables()
{
    int i;

    printf("Current variables: ");
    for (i=0; i < var_table_idx; i++) {
        if ((i % 3) == 0)
          printf("\n    ");
        printf("%s ", var_table[i].varname);
    }
    printf("\n");
}

/*
 *  Searches the list of possible values for a variable and returns the
 *  table entry containing the string describing that value.
 */
static VARIABLE_VALUES *find_value(VARIABLE_VALUES *val_table, char *valuename)
{
   int i = 0;
   while (val_table[i].valuename != NULL) {
       if (strcmp(valuename, val_table[i].valuename) == 0)
         return (&val_table[i]);
       i++;
   }
   return NULL;
}

/*
 *  Prints the names of the values set in the specified variable.
 *  Each bit that is ON on the variable must have a valuename associated
 *  with it.
 */
static void print_current_var_values(VARIABLE *var)
{
   int index = 1;
   int count = 0;
   ui value;
   VARIABLE_VALUES *val_table;

   val_table = var->value_table;
   value = *(var->varaddr);

   printf("\n%s = ", var->varname);
   while (value) {
       if (value & 1) {
           if (count && ((count % 3) == 0))
             printf("\n    ");
           printf("%s ", val_table[index].valuename);
           count++;
       }
       index++;
       value >>=1;
   }
   if (!count)
     printf("DISABLED");
   printf("\n\n");
}

/*
 *  Prints all the possible values a variable can have.  This
 *  requires a table of values to be defined for each variable.
 */
static void print_possible_var_values(VARIABLE *var)
{
   int index = 0;
   VARIABLE_VALUES *val_table;

   val_table = var->value_table;
   printf("Possible values: \n");
   while (val_table && (val_table[index].valuename != NULL)) {
       printf("    %s\n", val_table[index].valuename);
       index++;
   }
   printf("\n");
}


/*
 *  This routine adds the debugging variables for various modules of the
 *  debug monitor.  The variables and corresponding table values are
 *  declared in the module in which they are used.
 */
static void debug_init()
{

    arp_debug = ip_debug = udp_debug = tftp_debug = 0;

    /*  Add debugging variables for IP, TFTP, UDP and ARP module. */
    add_variable("ARP_DEBUG", &arp_debug, arp_debug_table);
    add_variable("IP_DEBUG", &ip_debug, ip_debug_table);
    add_variable("UDP_DEBUG", &udp_debug, udp_debug_table);
    add_variable("TFTP_DEBUG", &tftp_debug, tftp_debug_table);
}

/*
 *  Top level routine for setting and listing internal variables that
 *  we want to make visible and accessible from the user interface.
 */
void setvar (int argc, char **argv)
{
  int index, index2;
  VARIABLE *var;
  VARIABLE_VALUES *var_value;

  if (!initialized) {
      debug_init();
      initialized = TRUE;
  }

  if (argc < 2) {
      print_variables();
      return;
  }

  if ((var = find_var(argv[1])) == NULL) {
      printf("Unknown variable: %s\n", argv[1]);
      return;
  }

  if (argc < 3) {
      print_current_var_values(var);
      print_possible_var_values(var);
      return;
  }

  /*
   *  See if the value that the user entered is in the list of possible values.
   */
  if ((var_value = find_value(var->value_table, argv[2])) == NULL) {
      printf("Unknown setting %s for variable: %s\n", argv[2], argv[1]);
      return;
  }

  /*
   *  The user specified value is in the list.  Add the value to the variable if
   *  not disabling.
   */
  if (var_value->value)
    *(var->varaddr) |= var_value->value;
  else
    *(var->varaddr) = 0;

  print_current_var_values(var);
}

#endif /* TRACE_ENABLE */
