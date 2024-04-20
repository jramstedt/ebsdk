/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:26:48 by OpenVMS SDL EV1-33     */
/* Source:   6-OCT-1994 09:34:50 AFW_USER$:[CONSOLE.V58.COMMON.SRC]ANSI_DEF.SDL; */
/********************************************************************************************************************************/
/* file:	ansi_def.sdl                                                */
/*                                                                          */
/* Copyright (C) 1987, 1988 by                                              */
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
/* Facility:	EVAX/Cobra firmware                                         */
/*                                                                          */
/* Abstract:	This file describes data structures used by the ansi        */
/*		parser.                                                     */
/*                                                                          */
/* Author:	AJ Beaverson                                                */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	jrk	06-Oct-1994	Align structures.                           */
/*                                                                          */
/*	ajb	03-Aug-1990	Remove facility codes from structure elements. */
/*                                                                          */
/*      ajb	11-Apr-90	Adapted from KA670 code.                    */
/*                                                                          */
/*** MODULE CP_CONST ***/
#define NUL 0
#define SOH 1
#define STX 2
#define ETX 3
#define EOT 4
#define ENQ 5
#define ACK 6
#define BEL 7
#define BS 8
#define HT 9
#define LF 10
#define VT 11
#define FF 12
#define CR 13
#define SO 14
#define SI 15
#define DLE 16
#define XON 17
#define DC2 18
#define XOFF 19
#define DC4 20
#define NAK 21
#define SYN 22
#define ETB 23
#define CAN 24
#define EM 25
#define SUB 26
#define ESC 27
#define FS 28
#define GS 29
#define RS 30
#define US 31
#define SPACE 32
#define DEL 127
#define IND 132
#define NEL 133
#define SSA 134
#define ESA 135
#define HTS 136
#define HTJ 137
#define VTS 138
#define PLD 139
#define PLU 140
#define RI 141
#define SS2 142
#define SS3 143
#define DCS 144
#define PU1 145
#define PU2 146
#define STS 147
#define CCH 148
#define MW 149
#define SPA 150
#define EPA 151
#define CSI 155
#define ST 156
#define OSC 157
#define PM 158
#define APC 159
#define CTRL_A 1
#define CTRL_B 2
#define CTRL_C 3
#define CTRL_D 4
#define CTRL_E 5
#define CTRL_F 6
#define CTRL_G 7
#define CTRL_H 8
#define CTRL_I 9
#define CTRL_J 10
#define CTRL_K 11
#define CTRL_L 12
#define CTRL_M 13
#define CTRL_N 14
#define CTRL_O 15
#define CTRL_P 16
#define CTRL_Q 17
#define CTRL_R 18
#define CTRL_S 19
#define CTRL_T 20
#define CTRL_U 21
#define CTRL_V 22
#define CTRL_W 23
#define CTRL_X 24
#define CTRL_Y 25
#define CTRL_Z 26
 
/*** MODULE $ANSI ***/
/*+                                                                         */
/* ===============================================                          */
/* = ANSI - state for an ansi parser             =                          */
/* ===============================================                          */
/*                                                                          */
/* STRUCTURE OVERVIEW:                                                      */
/*                                                                          */
/*      This data structure contains all the state needed                   */
/*	for the reentrant ansi parser in the alpha console.                 */
/*                                                                          */
/* STRUCTURE MEMBERS:                                                       */
/*:                                                                         */
/*       ---------------------------------------------------------------------- */
/*       Structure Members    | Data Type  | Description                    */
/*       ---------------------------------------------------------------------- */
/*                  il_overfl |       bit  | overflow bit that is set if too */
/*                            |            | many intermediates are encountered */
/*                            |            |                                */
/*                  pv_overfl |       bit  | set if we overflow the parameter */
/*                            |            | space                          */
/*                            |            |                                */
/*                  buf_overfl|       bit  | character history buffer overflow */
/*                            |            |                                */
/*                  error     |       bit  | generic syntax error           */
/*                            |            |                                */
/*                  states    | byte array | state history.  This keeps track of */
/*                            |            | the last n states of the parser. */
/*                            |            |                                */
/*                       c    |      byte  | Current input character. Set to ESC */
/*                            |            | or CSI when valid ESC or CSI is */
/*                            |            | present                        */
/*                            |            |                                */
/*                    pchar   |     byte   | Non digit or ; encountered     */
/*                            |            |                                */
/*                    pvc     |     byte   | Number of parameters found     */
/*                            |            |                                */
/*                    pval    | word array | Vector of parameters           */
/*                            |            |                                */
/*                    ilc     |     byte   | Number of intermediates found  */
/*                            |            |                                */
/*                    ilist   | byte array | Vector of found intermediates  */
/*                            |            |                                */
/*                    buf     | byte array | last n characters read in      */
/*                            |            |                                */
/*                    bufix   | longword   | index into above buffer        */
/*:      ---------------------------------------------------------------------- */
/*                                                                          */
/*--                                                                        */
#define m_il_overfl 0x1
#define m_pv_overfl 0x2
#define m_error 0x4
#define m_buf_overfl 0x8
struct ANSI {
    unsigned il_overfl : 1;             /* intermediate list overflow       */
    unsigned pv_overfl : 1;             /* parameter list overflow          */
    unsigned error : 1;
    unsigned buf_overfl : 1;            /* input buffer overflow            */
    unsigned reserved : 4;
    char states [4];                    /* previous 4 states                */
    char rvalue;                        /* last value returned              */
    unsigned char c;                    /* last character processed         */
    unsigned char pchar;
    char pvc;                           /* number of parameters             */
    char ilc;                           /* number of intermediates          */
    char ilist [3];                     /* intermediate list                */
    char resvd [3];
    char buf [32];                      /* last n characters processed      */
    int pval [8];                       /* parameter values                 */
    int bufix;
    } ;
/*                                                                          */
/* Tokens recognized by the get_keystroke routine.                          */
/*                                                                          */
#define KEY_C0 32768                    /* all C0 codes                     */
#define KEY_GL 32769                    /* all GL codes                     */
#define KEY_DEL 32770                   /* delete, 7/15                     */
#define KEY_C1 32771
#define KEY_GR 32772
#define KEY_ESC 32773
#define KEY_CSI 32774
#define KEY_0 32775
#define KEY_1 32776
#define KEY_2 32777
#define KEY_3 32778
#define KEY_4 32779
#define KEY_5 32780
#define KEY_6 32781
#define KEY_7 32782
#define KEY_8 32783
#define KEY_9 32784
#define KEY_DASH 32785
#define KEY_COMMA 32786
#define KEY_PERIOD 32787
#define KEY_ENTER 32788
#define KEY_PF1 32789
#define KEY_PF2 32790
#define KEY_PF3 32791
#define KEY_PF4 32792
#define KEY_F6 32793
#define KEY_F7 32794
#define KEY_F8 32795
#define KEY_F9 32796
#define KEY_F10 32797
#define KEY_F11 32798
#define KEY_F12 32799
#define KEY_F13 32800
#define KEY_F14 32801
#define KEY_HELP 32802
#define KEY_DO 32803
#define KEY_F17 32804
#define KEY_F18 32805
#define KEY_F19 32806
#define KEY_F20 32807
#define KEY_FIND 32808
#define KEY_INSERT 32809
#define KEY_REMOVE 32810
#define KEY_SELECT 32811
#define KEY_PREV 32812
#define KEY_NEXT 32813
#define KEY_CUP 32814
#define KEY_CUD 32815
#define KEY_CUL 32816
#define KEY_CUR 32817
#define KEY_MAX 32818
#define ANSI_ACCEPT 0                   /* current state is an accepting state */
#define ANSI_NEEDMORE 1                 /* need more input                  */
 
