/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:01 by OpenVMS SDL EV1-31     */
/* Source:  20-MAR-2000 16:23:09 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]EV_DEF.SDL;1 */
/********************************************************************************************************************************/
/* file:	ev_def.sdl                                                  */
/*                                                                          */
/* Copyright (C) 1990 by                                                    */
/* Digital Equipment Corporation, Maynard, Massachusetts.                   */
/* All rights reserved.                                                     */
/*                                                                          */
/* This software is furnished under a license and may be used and copied    */
/* only  in  accordance  of  the  terms  of  such  license  and with the    */
/* inclusion of the above copyright notice. This software or  any  other    */
/* copies thereof may not be provided or otherwise made available to any    */
/* other person.  No title to and  ownership of the  software is  hereby    */
/* transferred.                                                             */
/*                                                                          */
/* The information in this software is  subject to change without notice    */
/* and  should  not  be  construed  as a commitment by digital equipment    */
/* corporation.                                                             */
/*                                                                          */
/* Digital assumes no responsibility for the use  or  reliability of its    */
/* software on equipment which is not supplied by digital.                  */
/*                                                                          */
/*                                                                          */
/* Abstract:	Environment Variable data structure definitions for         */
/*		Alpha firmware.                                             */
/*                                                                          */
/* Authors:	David Mayo                                                  */
/*		jds - Jim Sawin                                             */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	jrk	29-Apr-1998	Removed saved_value string.                 */
/*	jds	23-May-1991	Added NOMALLOC option defs for ev_write.    */
/*	phk	08-May-1991	Split the action field in wr_ & rd_action fields */
/*	dtm	14-Feb-1991	Add EVNODE size field and SRM/UNUSED attributes */
/*	dtm	20-Dec-1990	Add default value field                     */
/*	dtm	12-Oct-1990	Add ev attributes and validation table formats */
/*	dtm	30-Jul-1990	Initial entry.                              */
/*                                                                          */
/*** MODULE $evdef ***/
#define EV_NAME_LEN 32                  /* max size of an identifier        */
#define EV_VALUE_LEN 256                /* max size of a value + save_value + 2 */
#define EV$K_STRING 0
#define EV$K_INTEGER 1
#define EV$K_LOCAL 0
#define EV$K_GLOBAL 2
#define EV$K_VOLATILE 0
#define EV$K_NONVOLATILE 4
#define EV$K_READ 0
#define EV$K_NOREAD 8
#define EV$K_WRITE 0
#define EV$K_NOWRITE 16
#define EV$K_DELETE 0
#define EV$K_NODELETE 32
#define EV$K_SYSTEM 0
#define EV$K_USER 64
#define EV$K_TABLE 0
#define EV$K_ROUTINE 128
#define EV$K_DEF_STRING 0
#define EV$K_DEF_INTEGER 256
#define EV$K_USER_DEF 0
#define EV$K_SRM_DEF 512
#define EV$K_IN_USE 0
#define EV$K_UNUSED 1024
#define EV$K_MALLOC 0
#define EV$K_NOMALLOC 2048
#define EV$K_UNMODIFIED 0
#define EV$K_MODIFIED 4096
#define EV$K_ARC 8192
#define EV$M_TYPE 0x1
#define EV$M_CLASS 0x2
#define EV$M_VOLATILE 0x4
#define EV$M_READ 0x8
#define EV$M_WRITE 0x10
#define EV$M_DELETE 0x20
#define EV$M_USER 0x40
#define EV$M_VALIDATE 0x80
#define EV$M_DEFAULT 0x100
#define EV$M_SRM 0x200
#define EV$M_UNUSED 0x400
#define EV$M_NOMALLOC 0x800
#define EV$M_MODIFIED 0x1000
#define EV$M_ARC 0x2000
struct EV_OPTIONS {
    unsigned EV$V_TYPE : 1;
    unsigned EV$V_CLASS : 1;
    unsigned EV$V_VOLATILE : 1;
    unsigned EV$V_READ : 1;
    unsigned EV$V_WRITE : 1;
    unsigned EV$V_DELETE : 1;
    unsigned EV$V_USER : 1;
    unsigned EV$V_VALIDATE : 1;
    unsigned EV$V_DEFAULT : 1;
    unsigned EV$V_SRM : 1;
    unsigned EV$V_UNUSED : 1;
    unsigned EV$V_NOMALLOC : 1;
    unsigned EV$V_MODIFIED : 1;
    unsigned EV$V_ARC : 1;
    unsigned EV$V_FILL_0 : 2;
    } ;
/*                                                                          */
/* Structure of a predefined environment variable table entry               */
/*                                                                          */
struct env_table {
    char *ev_name;                      /* pointer to parameter name        */
    char *ev_value;                     /* pointer to parameter value       */
    int ev_options;                     /* environment variable attributes  */
    void *validate;                     /* code associated with value       */
    int (*ev_wr_action)();              /* address of action routine        */
    int (*ev_rd_action)();              /* address of action routine        */
    int (*ev_init)();                   /* address of init routine          */
    } ;
/*                                                                          */
/* Structure of a predefined environment variable validation table          */
/*                                                                          */
/* ajbfix:  we shouldn't rely on hard coded abbreviations, let the program  */
/* figure it out!!!!                                                        */
struct set_param_table {
    char *param;                        /* pointer to parmeter value name   */
    int code;                           /* code associated with value       */
    } ;
/*                                                                          */
/* Structure of an environment variable entry                               */
/*                                                                          */
struct EVNODE {
    struct EVNODE *flink;               /* linked list flink                */
    struct EVNODE *blink;               /* linked list blink                */
    char name [32];                     /* variable name                    */
    int attributes;                     /* EV attributes field              */
    union  {                            /* union of validate entries        */
        struct set_param_table *table;  /* table of valid entries           */
        int (*routine)();               /* routine to validate entries      */
        } validate;
    int (*wr_action)();                 /* EV action routine                */
    int (*rd_action)();                 /* EV action routine                */
    int misc;                           /* miscellaneous field              */
    int size;                           /* length of value field            */
    union  {                            /* union of values                  */
        char string [256];
        int integer;
        } value;
    } ;
#define EV$K_LENGTH 256
#define EV$L_SIZE 60
#define EV$L_ATTRIBUTES 40
#define EV$R_VALUE 64
struct evar_table {
    char name [32];                     /* variable name                    */
    int *var;                           /* pointer to input param value     */
    } ;
 
