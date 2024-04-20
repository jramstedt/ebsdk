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
 *      Color Frame Buffer Test Routine 
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

static unsigned int copybuffer[8] = {
        0x11111111,0x22222222,0x33333333,0x44444444,
        0x55555555,0x66666666,0x77777777,0x88888888};

static   struct pshift_type {
         unsigned int  pshift;                                  /* Shift by this amount */
         unsigned int  expect[8];                               /* Expected Dest Buff   */
         char         *text;                                    /* text                 */                                     
        } pshift_table [] =
        {
        0x00000000,0x11111111,0x22222222,0x33333333,0x44444444,
                   0x55555555,0x66666666,0x77777777,0x88888888 ,   "s + 0",

        0x00000001,0x11111100,0x22222211,0x33333322,0x44444433,
                   0x55555544,0x66666655,0x77777766,0x88888877 ,   "s + 1 ",

        0x00000002,0x11110000,0x22221111,0x33332222,0x44443333,
                   0x55554444,0x66665555,0x77776666,0x88887777 ,   "s + 2 ", 

        0x00000003,0x11000000,0x22111111,0x33222222,0x44333333,
                   0x55444444,0x66555555,0x77666666,0x88777777 ,   "s + 3 ",

        0x00000004,0x00000000,0x11111111,0x22222222,0x33333333,
                   0x44444444,0x55555555,0x66666666,0x77777777,    "s + 4 ",

        0x00000005,0x00000000,0x11111100,0x22222211,0x33333322,
                   0x44444433,0x55555544,0x66666655,0x77777766,    "s + 5 ",

        0x00000006,0x00000000,0x11110000,0x22221111,0x33332222,
                   0x44443333,0x55554444,0x66665555,0x77776666,    "s + 6 ",

        0x00000007,0x00000000,0x11000000,0x22111111,0x33222222,
                   0x44333333,0x55444444,0x66555555,0x77666666,    "s + 7 ",
        };

static   struct neg_pshift_type {
         unsigned int  pshift;                                  /* Shift by this amount */
         unsigned int  expect[8];                               /* Expected Dest Buff   */
         char         *text;                                    /* text                 */                                     
        } neg_pshift_table [] =
        {
        0x0000000F,0x00000000,0x00000000,0x44333333,0x55444444,
                   0x66555555,0x77666666,0x88777777,0x00888888,    "s - 1 ",

        0x0000000E,0x00000000,0x00000000,0x44443333,0x55554444,
                   0x66665555,0x77776666,0x88887777,0x00008888,    "s - 2",

        0x0000000D,0x00000000,0x00000000,0x44444433,0x55555544,
                   0x66666655,0x77777766,0x88888877,0x00000088,    "s - 3",

        0x0000000C,0x00000000,0x00000000,0x44444444,0x55555555,
                   0x66666666,0x77777777,0x88888888,0x00000000,    "s - 4",

        0x0000000B,0x00000000,0x00000000,0x55444444,0x66555555,
                   0x77666666,0x88777777,0x00888888,0x00000000,    "s - 5",

        0x0000000A,0x00000000,0x00000000,0x55554444,0x66665555,
                   0x77776666,0x88887777,0x00008888,0x00000000,    "s - 6",

        0x00000009,0x00000000,0x00000000,0x55555544,0x66666655,
                   0x77777766,0x88888877,0x00000088,0x00000000,    "s - 7",
        };

