/*
 *        Copyright (c) Digital Equipment Corporation, 1990
 *        All Rights Reserved.  Unpublished-rights reserved
 *        under the copyright laws of the United States.
 *
 *        The software contained on this media is proprietary to"
 *        and embodies confidential technology of Digital.  Possession,
 *        use, duplication or dissemination of the software and media
 *        is authorized only pursuant to a valid written license from
 *        Digital.
 *
 *        RESTRICTED RIGHTS LEGEND  Use, duplication, or disclosure"
 *        by the U.S. Government is subject to restrictions as set 
 *        forth in subparagraph (c)(1)(ii) of DFARS 252.227-7013, or 
 *        in FAR 52.227-19, as applicable.
 */

/*
 *++
 *  FACILITY:
 *
 *      Color Frame Buffer Boolean Static Test Table
 *
 *  ABSTRACT:
 *
 *      Test Description:
  *
 *  AUTHORS:
 *
 *      James Peacock
 *
 *
 *  CREATION DATE:      02-28-91
 *
 *  MODIFICATION HISTORY:
 *
 *--
*/

/**
** INCLUDES
**/

static   struct expect_type {
        int      code;                  /* This is the boolean function code            */
        unsigned int initial;           /* Write this initial value to video ram  dst   */
        unsigned int src;               /* Write this src data to video RAM             */
        unsigned int expect;            /* And this is what I expect for the result     */
        char     *text;                 /* And this is text details if want to print it */
        } expect_table [] = 
        {
        0x0,0xaaaaaaaa,0x55555555,0x00000000, "0:   0                       ",
        0x1,0x22222222,0x88888888,0x00000000, "1:   src AND dst             ",
        0x2,0x55555555,0xffffffff,0xaaaaaaaa, "2:   src AND NOT dst         ",
        0x3,0x88888888,0x55555555,0x55555555, "3:   src                     ",
        0x4,0x55555555,0xaaaaaaaa,0x55555555, "4:   NOT (src) AND dst       ",
        0x5,0x44444444,0x33333333,0x44444444, "5:   dst                     ",
        0x6,0x55555555,0x55555555,0x00000000, "6:   src XOR dst             ",
        0x7,0x22222222,0x44444444,0x66666666, "7:   src OR dst              ",
        0x8,0x55555555,0xaaaaaaaa,0x00000000, "8:   NOT(src) AND NOT(dst)   ",
        0x9,0x55555555,0x55555555,0xffffffff, "9:   NOT(src) XOR (dst)      ",
        0xa,0x33333333,0x00000000,0xcccccccc, "0xa: NOT dst                 ",
        0xb,0x44444444,0x44444444,0xffffffff, "0xb: OR (NOT dst)            ",
        0xc,0x55555555,0x88888888,0x77777777, "0xc: NOT src                 ",
        0xd,0x11111111,0x22222222,0xdddddddd, "0xd: (NOT src) OR dst        ",
        0xe,0x55555555,0xaaaaaaaa,0xffffffff, "0xe: (NOT src) or (NOT dst)  ",
        0xf,0x33333333,0x00000000,0xffffffff, "0xf: 0xffffffff              ",
        };

