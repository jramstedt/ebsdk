/*
 * Copyright (C) 1998 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Adaptec AIC-78xx Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	27-Mar-1998	Initial entry.
 */

/*$Header:   Y:/source/chimscsi/src/himscsi/hwmscsi/SEQ_S64.HV_   1.23.14.2   08 Dec 1997 15:43:36   LIU3659  $*/
#if (OSD_BUS_ADDRESS_SIZE == 32)

SCSI_UEXACT8 Seq_s64[] = {
0x00,0x65,0x0B,0x10,0x00,0x65,0xB7,0x10,0x00,0x65,0xD0,0x10,0x00,0x65,0xF1,0x10,
0x00,0x65,0xD5,0x11,0x5A,0x6A,0x00,0x00,0xF7,0x11,0x11,0x02,0x02,0x6F,0x38,0x0C,
0x00,0x65,0x0C,0x10,0x02,0x6F,0x36,0x0C,0x00,0x65,0x0C,0x10,0x80,0x39,0x39,0x00,
0x08,0xEB,0x0E,0x1A,0x02,0xEB,0x48,0x1A,0x8A,0xEB,0x16,0x1C,0x80,0x39,0x12,0x1A,
0x02,0x38,0x6F,0x0C,0xFF,0x6A,0x6A,0x03,0x40,0x0B,0xB0,0x1A,0x20,0x0B,0x6E,0x1A,
0x98,0xEE,0x64,0x02,0x88,0x64,0x18,0x18,0x02,0x36,0x6F,0x0C,0xFF,0x6A,0x6A,0x03,
0x54,0xEE,0x64,0x02,0x54,0x64,0x41,0x1C,0xFF,0x5E,0x56,0x18,0x08,0xEB,0x0C,0x1A,
0x18,0xEE,0x0C,0x1A,0x01,0x3B,0x64,0x0C,0x00,0x3A,0x3B,0x18,0xFF,0x70,0x0C,0x1C,
0x01,0x90,0x72,0x0C,0x04,0x56,0xE0,0x0C,0xFF,0x4C,0x25,0x1A,0x04,0x4E,0xE0,0x0C,
0x01,0x4D,0x4D,0x06,0x01,0x6A,0xEE,0x00,0x01,0x70,0x90,0x0C,0x01,0x90,0xEC,0x0C,
0x06,0xED,0xED,0x06,0x01,0x4D,0xEC,0x0C,0x01,0xC6,0x90,0x0C,0x01,0x4C,0x4C,0x06,
0x01,0x78,0x64,0x0C,0x00,0x4C,0x2F,0x18,0x00,0x6A,0x4C,0x02,0xFF,0x4C,0x32,0x1E,
0xFF,0x90,0x32,0x1C,0x40,0xED,0x27,0x18,0x01,0x90,0x70,0x0C,0x01,0xED,0xE8,0x0C,
0x09,0x6A,0xEE,0x00,0x01,0x72,0x90,0x0C,0x00,0x65,0x09,0x16,0x00,0x6A,0xEE,0x02,
0x04,0xE0,0x56,0x0C,0x02,0x6A,0x91,0x00,0x00,0x65,0x0C,0x10,0x04,0x3C,0xE0,0x0C,
0x30,0x6A,0xE8,0x00,0x01,0x44,0xF1,0x0C,0x1D,0x6A,0xEE,0x00,0x01,0x3B,0x3B,0x06,
0x00,0x65,0x0C,0x10,0x00,0x6A,0xEE,0x02,0x01,0x90,0x72,0x0C,0x01,0xF1,0x90,0x0C,
0x04,0xC8,0x3C,0x0C,0x01,0xC6,0x44,0x0C,0x5E,0x6A,0xE1,0x17,0x00,0x65,0x57,0x10,
0x08,0xEE,0x4C,0x1E,0x80,0xEE,0x0C,0x1E,0x10,0xEE,0x16,0x1E,0xF7,0xEE,0xEE,0x02,
0x08,0xEE,0x4C,0x1A,0x04,0xA6,0xE0,0x0C,0x80,0x6A,0xE8,0x00,0x0B,0x6A,0xEB,0x00,
0x00,0x65,0x09,0x16,0x07,0xA6,0x64,0x02,0x00,0xA5,0xA5,0x00,0x01,0xA5,0xEA,0x0C,
0x00,0x6A,0xEB,0x02,0x00,0x65,0x0F,0x10,0x01,0x90,0x72,0x0C,0x40,0x00,0x6C,0x1A,
0x01,0x5E,0x90,0x0C,0x01,0x90,0x34,0x0C,0x01,0xC3,0x68,0x1A,0x08,0xAC,0x5E,0x1E,
0x80,0x39,0x6C,0x1E,0x00,0x65,0x9E,0x10,0x08,0xC3,0x63,0x1E,0x80,0x39,0x6C,0x1E,
0x01,0x90,0x35,0x0C,0x00,0x65,0xEB,0x17,0x00,0x65,0x3B,0x11,0x20,0xAC,0xA2,0x1A,
0x00,0x65,0xDA,0x17,0xFF,0xDF,0xA0,0x1C,0x01,0xDF,0x90,0x0C,0x01,0x34,0x90,0x0C,
0xFF,0xC6,0x6C,0x1C,0x01,0x90,0x33,0x0C,0x01,0xC6,0x90,0x0C,0x00,0x65,0x59,0x10,
0x01,0x72,0x90,0x0C,0x00,0x65,0x1B,0x10,0xFF,0x6A,0x35,0x00,0x08,0x6A,0x0C,0x00,
0x08,0x11,0x11,0x00,0x20,0x0B,0x06,0x1E,0x20,0x6A,0x0B,0x00,0x00,0x65,0xC4,0x17,
0xE0,0x64,0x78,0x1C,0xA0,0x64,0xCE,0x19,0x21,0x6A,0x91,0x00,0x00,0x65,0x73,0x10,
0x80,0x12,0x9D,0x1E,0x0F,0x12,0x64,0x02,0x00,0x19,0x32,0x00,0x00,0x32,0xDD,0x17,
0x3F,0x12,0x32,0x02,0xFF,0xDF,0x87,0x1C,0x01,0xDF,0x90,0x0C,0x20,0xAC,0x87,0x1A,
0x01,0x90,0x35,0x0C,0x08,0xC3,0x83,0x1E,0xF0,0x6A,0x03,0x00,0x4C,0x19,0x72,0x0A,
0x00,0x72,0x84,0x17,0x8F,0x04,0x87,0x18,0x01,0xDE,0x04,0x0C,0x00,0x65,0xC3,0x17,
0xE0,0x64,0x8E,0x1C,0xA0,0x64,0x8F,0x18,0xFF,0x35,0x8F,0x1C,0x08,0xC3,0x8F,0x1E,
0x81,0x6A,0x91,0x00,0x00,0x65,0x44,0x11,0x20,0x12,0x94,0x1C,0xFF,0x35,0x9C,0x1C,
0x08,0xAC,0x92,0x1E,0xFF,0xAF,0xCE,0x1F,0xFF,0xC6,0xD0,0x1E,0x01,0xC6,0xF4,0x1C,
0x00,0x65,0xC3,0x17,0xE0,0x64,0xCE,0x19,0x01,0x12,0x90,0x0C,0x20,0xAC,0x9C,0x1E,
0x01,0x32,0x64,0x0C,0x00,0xC7,0x80,0x1C,0x4C,0x19,0x64,0x0A,0x00,0xAD,0x80,0x1C,
0x81,0x6A,0x91,0x00,0x91,0x6A,0x91,0x00,0x20,0xAC,0xA2,0x1A,0x00,0x65,0xDA,0x17,
0x01,0x34,0xDF,0x0C,0x01,0x34,0x90,0x0C,0x44,0xAD,0x64,0x0A,0x0F,0x05,0x05,0x02,
0x00,0x05,0x05,0x00,0x80,0x39,0xAA,0x1A,0x5A,0x6A,0x00,0x00,0x00,0x65,0xEB,0x17,
0x01,0x72,0x90,0x0C,0x00,0x65,0x0C,0x10,0x00,0x6A,0x93,0x02,0x18,0x01,0x01,0x00,
0x20,0x6A,0x0C,0x00,0x5A,0x6A,0x00,0x00,0x00,0x65,0xEB,0x17,0x40,0x0B,0x0B,0x1E,
0x01,0x34,0x90,0x0C,0x01,0x90,0x35,0x0C,0x08,0x6A,0x0C,0x00,0x12,0x6A,0x00,0x00,
0x08,0x11,0x11,0x00,0x40,0x0B,0x05,0x1E,0x40,0x6A,0x0B,0x00,0x00,0x65,0xC4,0x17,
0xA0,0x64,0xD8,0x18,0x3F,0xC7,0x32,0x02,0x40,0xAC,0xBE,0x1E,0x30,0x6A,0x7F,0x17,
0x00,0x6E,0xBE,0x1A,0x40,0x32,0x32,0x00,0x80,0x32,0x33,0x00,0x01,0x33,0x32,0x0C,
0x20,0xAC,0xC9,0x1E,0x01,0x32,0x06,0x0C,0x00,0x65,0xC4,0x17,0xA0,0x64,0xD8,0x18,
0x23,0xAC,0x33,0x02,0x01,0x33,0x06,0x0C,0x00,0x65,0xC4,0x17,0xA0,0x64,0xD8,0x18,
0x01,0x90,0x32,0x0C,0x08,0xAC,0xCB,0x1E,0xFF,0xAF,0x40,0x1F,0x08,0xC3,0x41,0x1B,
0x00,0xAD,0x85,0x17,0x8F,0x04,0xCF,0x1C,0x40,0x6A,0x0C,0x00,0x01,0x32,0x06,0x0C,
0x00,0x65,0xC4,0x17,0x80,0x64,0xDC,0x1C,0xE0,0x64,0xD6,0x1C,0xC0,0x64,0x2E,0x1D,
0xA0,0x64,0xCE,0x19,0x01,0x6A,0x91,0x00,0x00,0x6A,0x60,0x17,0x00,0x65,0xD0,0x10,
0x40,0x6A,0x0C,0x00,0xC0,0x64,0x2E,0x1D,0x80,0x64,0xCE,0x19,0x20,0xAC,0xCE,0x1B,
0x40,0x6A,0x0C,0x00,0x00,0xAD,0x84,0x17,0x8F,0x04,0xD5,0x1C,0x1F,0xAE,0x8C,0x02,
0x02,0x6A,0x8D,0x0C,0x03,0x8C,0x08,0x0C,0x10,0x8C,0xE9,0x1E,0x04,0xA0,0x88,0x0C,
0x3D,0x6A,0x93,0x00,0x00,0x65,0x07,0x16,0x04,0x0B,0xF4,0x1A,0x10,0x0C,0xEF,0x1A,
0x00,0x65,0x14,0x10,0x00,0x6A,0x88,0x02,0x05,0x6A,0x93,0x00,0x10,0xA0,0x99,0x0C,
0x34,0x6A,0x93,0x00,0x04,0x0B,0xF4,0x1A,0x10,0x0C,0xEC,0x1E,0x04,0x0B,0xF4,0x1A,
0x00,0x6A,0x93,0x02,0x00,0x65,0xC4,0x17,0xC0,0x64,0x2E,0x1D,0x11,0x6A,0x91,0x00,
0xC7,0x93,0x93,0x02,0x40,0xAE,0x2B,0x1B,0x01,0x04,0xDE,0x0C,0x07,0xB8,0x88,0x0C,
0x1F,0xAE,0xFB,0x1E,0x00,0x6A,0xAB,0x02,0x00,0x65,0x9A,0x11,0x03,0x8C,0x08,0x0C,
0x39,0x6A,0x32,0x00,0xFF,0xAB,0xFF,0x1A,0x80,0xAE,0x00,0x1B,0x40,0x32,0x32,0x00,
0x01,0x0C,0x00,0x1F,0xA0,0x03,0x23,0x1B,0xE0,0x03,0x03,0x02,0x40,0x03,0x05,0x1B,
0x04,0x32,0x32,0x00,0x01,0x32,0x93,0x0C,0xE0,0xAE,0xAE,0x02,0x06,0xAE,0xA4,0x0C,
0x00,0x65,0xC1,0x17,0x00,0x65,0x07,0x16,0x10,0x0C,0x13,0x1B,0x01,0x0B,0x14,0x1E,
0x01,0x08,0x0F,0x1B,0xFF,0xAB,0x0F,0x1B,0x80,0xA4,0x29,0x1B,0x01,0x0C,0x0F,0x1F,
0x10,0x0C,0x15,0x1B,0x00,0x65,0x8B,0x17,0x00,0x65,0x09,0x11,0x04,0x93,0x15,0x1B,
0x01,0x94,0x14,0x1F,0x10,0x94,0x15,0x1B,0xC7,0x93,0x93,0x02,0x08,0x93,0x17,0x1B,
0x01,0x0B,0x1F,0x1F,0x01,0x08,0x1F,0x1B,0xFF,0xAB,0x1F,0x1B,0x80,0xA4,0x29,0x1B,
0x23,0x12,0x1F,0x1D,0x00,0x6A,0x32,0x02,0x00,0x65,0x8D,0x17,0x03,0x08,0xA0,0x0C,
0x08,0xEB,0x22,0x1F,0x80,0xEB,0x20,0x1F,0x00,0x6A,0xEB,0x02,0x04,0x0C,0x25,0x1F,
0x00,0x65,0xC9,0x17,0xE0,0x03,0x64,0x02,0xE0,0x03,0x03,0x02,0x00,0x65,0xCA,0x17,
0x00,0x65,0xF4,0x10,0x40,0xA4,0xA4,0x00,0xC7,0x93,0x93,0x02,0x00,0x65,0xC9,0x17,
0x31,0x6A,0x91,0x00,0x00,0x65,0x2B,0x11,0x1F,0xAE,0x5B,0x1B,0x40,0xA4,0x5C,0x1F,
0x01,0x06,0x33,0x0C,0x00,0x65,0xC4,0x17,0xE0,0x64,0xCE,0x19,0xFF,0x12,0x35,0x1F,
0x41,0x6A,0x91,0x00,0xFF,0x33,0x3F,0x1B,0x00,0x65,0xCF,0x17,0x20,0xAC,0x3B,0x1B,
0x00,0x65,0xDA,0x17,0xFF,0x6A,0xDF,0x00,0x01,0x35,0x90,0x0C,0xF0,0xC3,0x4E,0x1B,
0x70,0x6A,0xE1,0x17,0x80,0x39,0x39,0x00,0x00,0x65,0x12,0x10,0x51,0x6A,0x91,0x00,
0x81,0x6A,0x91,0x00,0x01,0x32,0x06,0x0C,0x00,0x65,0xC4,0x17,0xA0,0x64,0xCE,0x19,
0x40,0x6A,0x0C,0x00,0xF7,0x01,0x01,0x02,0x08,0xAC,0x48,0x1F,0xFF,0xAF,0x4B,0x1F,
0x06,0x6A,0xB0,0x00,0x20,0xAC,0x4B,0x1F,0x0D,0x6A,0xB0,0x00,0x01,0xB0,0x06,0x0C,
0x08,0x01,0x01,0x00,0x00,0x65,0x36,0x11,0x01,0xC5,0x90,0x0C,0xFE,0xC3,0xC3,0x02,
0x01,0x35,0x90,0x0C,0x01,0xC4,0x90,0x0C,0xE0,0xC3,0x51,0x1F,0x10,0xC3,0xC3,0x06,
0x00,0x65,0x0B,0x14,0x01,0xC3,0x3C,0x1F,0xF0,0xC3,0xC3,0x06,0xFE,0xC3,0xC3,0x02,
0x02,0xC3,0x0B,0x1E,0x01,0xC4,0x90,0x0C,0x00,0x65,0x57,0x11,0x40,0xAE,0x30,0x1B,
0x31,0x6A,0x91,0x00,0x00,0x65,0x30,0x11,0x01,0x6A,0x60,0x17,0x00,0x65,0xF4,0x10,
0x01,0x65,0xC6,0x0C,0x02,0x01,0x01,0x00,0x02,0x12,0x68,0x1D,0x04,0x12,0x6F,0x1D,
0x01,0x12,0x74,0x1D,0x03,0x12,0x7C,0x1D,0x08,0x12,0x7B,0x1D,0x41,0x6A,0x91,0x01,
0x1F,0xAE,0x6C,0x1B,0x06,0xA4,0xAE,0x0C,0x04,0x14,0xB8,0x0C,0x03,0x08,0xBC,0x0C,
0x00,0x65,0xC3,0x17,0xE0,0x64,0x63,0x1D,0xFF,0x6A,0x6A,0x03,0x40,0xAC,0x67,0x1F,
0x30,0x6A,0x7F,0x17,0x00,0x6E,0x67,0x1B,0x00,0x65,0xCF,0x17,0x00,0x65,0x0B,0x10,
0x00,0x65,0xC3,0x17,0xE0,0x64,0xD4,0x19,0x01,0x12,0x33,0x0C,0x00,0x65,0xC3,0x17,
0xE0,0x64,0xD4,0x19,0x71,0x6A,0x91,0x00,0xFF,0x6A,0x6A,0x08,0x01,0x06,0x6A,0x0D,
0x00,0x65,0xC3,0x17,0x80,0x64,0xDC,0x1C,0xFF,0x6A,0x6A,0x03,0x08,0xAD,0x81,0x1F,
0x01,0x65,0x65,0x06,0x44,0xAD,0x64,0x0A,0x01,0x64,0x6E,0x0C,0x01,0x6C,0x64,0x0D,
0x1A,0x01,0x01,0x00,0x20,0x65,0x65,0x06,0x01,0x6C,0x04,0x0C,0x76,0x6A,0x7F,0x17,
0x00,0x6E,0x8A,0x1F,0x20,0x01,0x01,0x00,0xFF,0x6A,0x6A,0x03,0xC7,0x93,0x93,0x02,
0x38,0x93,0x8C,0x1B,0x02,0xEB,0x90,0x1F,0x00,0x65,0x07,0x16,0x02,0xEB,0x0C,0x1A,
0xFF,0xAB,0x99,0x1F,0x04,0x88,0x88,0x0C,0x01,0xAB,0x8C,0x0C,0x02,0x6A,0x8D,0x0C,
0x00,0x6A,0xAB,0x02,0x03,0x8C,0x08,0x0C,0x80,0xA4,0x98,0x1F,0xBF,0x32,0x93,0x03,
0x01,0x32,0x93,0x0D,0x07,0xE9,0x88,0x0C,0xFF,0x79,0xAD,0x1F,0x01,0x8C,0x64,0x0C,
0x00,0x88,0x33,0x06,0x01,0x33,0x64,0x0C,0x00,0x79,0xAB,0x02,0xFF,0xAB,0xAD,0x1F,
0xFF,0x8E,0xA7,0x1B,0xFF,0x8D,0xA7,0x1B,0xFF,0x79,0x64,0x04,0x00,0x8C,0x33,0x06,
0x00,0x65,0xA7,0x13,0x00,0x6A,0xAB,0x02,0x00,0x65,0xAD,0x11,0x01,0xAB,0x64,0x0C,
0xFF,0x64,0x64,0x04,0x01,0x64,0x64,0x06,0x00,0x8C,0x8C,0x08,0xFF,0x8D,0x8D,0x08,
0xFF,0x8E,0x8E,0x08,0x1F,0xAE,0xFB,0x1A,0x03,0x8C,0x08,0x0C,0xFF,0xE9,0xB3,0x1F,
0x80,0xA4,0xA4,0x00,0xFF,0xAB,0x98,0x1B,0xBF,0x32,0x93,0x03,0x01,0x32,0x93,0x0C,
0x01,0xEA,0x64,0x0C,0x00,0x6A,0xEA,0x06,0x01,0xEA,0xA5,0x0C,0x07,0xA6,0x64,0x02,
0x80,0x64,0x64,0x00,0x00,0xEA,0xD4,0x19,0x00,0x6A,0xA5,0x02,0x80,0xA6,0xA6,0x06,
0x00,0x6A,0x64,0x02,0x00,0xA7,0xA7,0x08,0x00,0xA8,0xA8,0x08,0x00,0xA9,0xA9,0x08,
0x02,0x6A,0xEB,0x01,0x80,0xA4,0xC0,0x1F,0xFF,0x6A,0x6A,0x03,0x01,0x06,0x6A,0x0C,
0x01,0x0C,0xC4,0x1F,0x04,0x0C,0xC4,0x1B,0xE0,0x03,0x03,0x02,0xE0,0x03,0x64,0x03,
0x21,0x6A,0x91,0x00,0x00,0x65,0xC4,0x17,0xE0,0x64,0x5E,0x1D,0xC0,0x64,0x2E,0x1D,
0xA0,0x64,0xC8,0x1D,0xA0,0x64,0xD4,0x1F,0x61,0x6A,0x91,0x00,0xDF,0x01,0x01,0x02,
0xF7,0x11,0x11,0x02,0x01,0x06,0x6A,0x0C,0x09,0x0C,0xD2,0x1F,0x08,0x0C,0xCE,0x1F,
0xFF,0x6A,0x6A,0x03,0xFF,0xA0,0xA0,0x06,0xFF,0xA0,0xD5,0x1B,0xFF,0xA1,0xA1,0x06,
0xFF,0xA1,0xD5,0x19,0x01,0x6A,0x91,0x00,0x44,0xAD,0x65,0x0A,0x0F,0xC7,0x64,0x02,
0x00,0x65,0x65,0x00,0xFF,0x65,0x65,0x02,0x04,0x65,0x90,0x0A,0x00,0x6A,0x64,0x02,
0x00,0x90,0x90,0x07,0x01,0x90,0x64,0x0C,0xFF,0x6A,0xC6,0x00,0xFF,0x6C,0xE7,0x19,
0xFF,0x65,0x66,0x06,0x01,0x64,0x6D,0x0C,0x01,0x64,0x6D,0x0D,0x01,0x65,0x66,0x0C,
0x01,0x6C,0x90,0x0C,0x01,0x64,0xC6,0x0C,0x01,0x64,0x6D,0x0D,0x01,0x90,0x64,0x0C,
0x00,0x5E,0xEE,0x19,0x01,0xC6,0x5E,0x0D,0x01,0xC6,0x32,0x0C,0x01,0x33,0x90,0x0C,
0x01,0x32,0xC6,0x0C,0x01,0x64,0x90,0x0C,0x00,0x5F,0xD4,0x19,0x01,0x33,0x5F,0x0D,
};
#else
SCSI_UEXACT8 Seq_s64[] = {
0x00,0x65,0x0B,0x10,0x00,0x65,0xB8,0x10,0x00,0x65,0xD1,0x10,0x00,0x65,0xF5,0x10,
0x00,0x65,0xE2,0x11,0x5A,0x6A,0x00,0x00,0xF7,0x11,0x11,0x02,0x02,0x6F,0x38,0x0C,
0x00,0x65,0x0C,0x10,0x02,0x6F,0x36,0x0C,0x00,0x65,0x0C,0x10,0x80,0x39,0x39,0x00,
0x08,0xEB,0x0E,0x1A,0x02,0xEB,0x48,0x1A,0x8A,0xEB,0x16,0x1C,0x80,0x39,0x12,0x1A,
0x02,0x38,0x6F,0x0C,0xFF,0x6A,0x6A,0x03,0x40,0x0B,0xB1,0x1A,0x20,0x0B,0x6F,0x1A,
0x98,0xEE,0x64,0x02,0x88,0x64,0x18,0x18,0x02,0x36,0x6F,0x0C,0xFF,0x6A,0x6A,0x03,
0x54,0xEE,0x64,0x02,0x54,0x64,0x41,0x1C,0xFF,0x5E,0x57,0x18,0x08,0xEB,0x0C,0x1A,
0x18,0xEE,0x0C,0x1A,0x01,0x3B,0x64,0x0C,0x00,0x3A,0x3B,0x18,0xFF,0x70,0x0C,0x1C,
0x01,0x90,0x72,0x0C,0x08,0x56,0xE0,0x0C,0xFF,0x4C,0x25,0x1A,0x08,0x4E,0xE0,0x0C,
0x01,0x4D,0x4D,0x06,0x01,0x6A,0xEE,0x00,0x01,0x70,0x90,0x0C,0x01,0x90,0xEC,0x0C,
0x06,0xED,0xED,0x06,0x01,0x4D,0xEC,0x0C,0x01,0xC6,0x90,0x0C,0x01,0x4C,0x4C,0x06,
0x01,0x78,0x64,0x0C,0x00,0x4C,0x2F,0x18,0x00,0x6A,0x4C,0x02,0xFF,0x4C,0x32,0x1E,
0xFF,0x90,0x32,0x1C,0x40,0xED,0x27,0x18,0x01,0x90,0x70,0x0C,0x01,0xED,0xE8,0x0C,
0x09,0x6A,0xEE,0x00,0x01,0x72,0x90,0x0C,0x00,0x65,0x09,0x16,0x00,0x6A,0xEE,0x02,
0x08,0xE0,0x56,0x0C,0x02,0x6A,0x91,0x00,0x00,0x65,0x0C,0x10,0x08,0x3C,0xE0,0x0C,
0x30,0x6A,0xE8,0x00,0x01,0x44,0xF1,0x0C,0x1D,0x6A,0xEE,0x00,0x01,0x3B,0x3B,0x06,
0x00,0x65,0x0C,0x10,0x00,0x6A,0xEE,0x02,0x01,0x90,0x72,0x0C,0x01,0xF1,0x90,0x0C,
0x08,0xC8,0x3C,0x0C,0x01,0xC6,0x44,0x0C,0x5E,0x6A,0xEE,0x17,0x00,0x65,0x58,0x10,
0x08,0xEE,0x4C,0x1E,0x80,0xEE,0x0C,0x1E,0x10,0xEE,0x16,0x1E,0xF7,0xEE,0xEE,0x02,
0x08,0xEE,0x4C,0x1A,0x04,0xB4,0xE4,0x0C,0x04,0xA6,0xE0,0x0C,0x80,0x6A,0xE8,0x00,
0x0B,0x6A,0xEB,0x00,0x00,0x65,0x09,0x16,0x07,0xA6,0x64,0x02,0x00,0xA5,0xA5,0x00,
0x01,0xA5,0xEA,0x0C,0x00,0x6A,0xEB,0x02,0x00,0x65,0x0F,0x10,0x01,0x90,0x72,0x0C,
0x40,0x00,0x6D,0x1A,0x01,0x5E,0x90,0x0C,0x01,0x90,0x34,0x0C,0x01,0xC3,0x69,0x1A,
0x08,0xAC,0x5F,0x1E,0x80,0x39,0x6D,0x1E,0x00,0x65,0x9F,0x10,0x08,0xC3,0x64,0x1E,
0x80,0x39,0x6D,0x1E,0x01,0x90,0x35,0x0C,0x00,0x65,0xF8,0x17,0x00,0x65,0x42,0x11,
0x20,0xAC,0xA3,0x1A,0x00,0x65,0xE7,0x17,0xFF,0xDF,0xA1,0x1C,0x01,0xDF,0x90,0x0C,
0x01,0x34,0x90,0x0C,0xFF,0xC6,0x6D,0x1C,0x01,0x90,0x33,0x0C,0x01,0xC6,0x90,0x0C,
0x00,0x65,0x5A,0x10,0x01,0x72,0x90,0x0C,0x00,0x65,0x1B,0x10,0xFF,0x6A,0x35,0x00,
0x08,0x6A,0x0C,0x00,0x08,0x11,0x11,0x00,0x20,0x0B,0x06,0x1E,0x20,0x6A,0x0B,0x00,
0x00,0x65,0xD1,0x17,0xE0,0x64,0x79,0x1C,0xA0,0x64,0xDB,0x19,0x21,0x6A,0x91,0x00,
0x00,0x65,0x74,0x10,0x80,0x12,0x9E,0x1E,0x0F,0x12,0x64,0x02,0x00,0x19,0x32,0x00,
0x00,0x32,0xEA,0x17,0x3F,0x12,0x32,0x02,0xFF,0xDF,0x88,0x1C,0x01,0xDF,0x90,0x0C,
0x20,0xAC,0x88,0x1A,0x01,0x90,0x35,0x0C,0x08,0xC3,0x84,0x1E,0xF0,0x6A,0x03,0x00,
0x4C,0x19,0x72,0x0A,0x00,0x72,0x8E,0x17,0x8F,0x04,0x88,0x18,0x01,0xDE,0x04,0x0C,
0x00,0x65,0xD0,0x17,0xE0,0x64,0x8F,0x1C,0xA0,0x64,0x90,0x18,0xFF,0x35,0x90,0x1C,
0x08,0xC3,0x90,0x1E,0x81,0x6A,0x91,0x00,0x00,0x65,0x4B,0x11,0x20,0x12,0x95,0x1C,
0xFF,0x35,0x9D,0x1C,0x08,0xAC,0x93,0x1E,0xFF,0xAF,0xDB,0x1F,0xFF,0xC6,0xD1,0x1E,
0x01,0xC6,0xF8,0x1C,0x00,0x65,0xD0,0x17,0xE0,0x64,0xDB,0x19,0x01,0x12,0x90,0x0C,
0x20,0xAC,0x9D,0x1E,0x01,0x32,0x64,0x0C,0x00,0xC7,0x81,0x1C,0x4C,0x19,0x64,0x0A,
0x00,0xAD,0x81,0x1C,0x81,0x6A,0x91,0x00,0x91,0x6A,0x91,0x00,0x20,0xAC,0xA3,0x1A,
0x00,0x65,0xE7,0x17,0x01,0x34,0xDF,0x0C,0x01,0x34,0x90,0x0C,0x44,0xAD,0x64,0x0A,
0x0F,0x05,0x05,0x02,0x00,0x05,0x05,0x00,0x80,0x39,0xAB,0x1A,0x5A,0x6A,0x00,0x00,
0x00,0x65,0xF8,0x17,0x01,0x72,0x90,0x0C,0x00,0x65,0x0C,0x10,0x00,0x6A,0x93,0x02,
0x18,0x01,0x01,0x00,0x20,0x6A,0x0C,0x00,0x5A,0x6A,0x00,0x00,0x00,0x65,0xF8,0x17,
0x40,0x0B,0x0B,0x1E,0x01,0x34,0x90,0x0C,0x01,0x90,0x35,0x0C,0x08,0x6A,0x0C,0x00,
0x12,0x6A,0x00,0x00,0x08,0x11,0x11,0x00,0x40,0x0B,0x05,0x1E,0x40,0x6A,0x0B,0x00,
0x00,0x65,0xD1,0x17,0xA0,0x64,0xD9,0x18,0x3F,0xC7,0x32,0x02,0x40,0xAC,0xBF,0x1E,
0x30,0x6A,0x89,0x17,0x00,0x6E,0xBF,0x1A,0x40,0x32,0x32,0x00,0x80,0x32,0x33,0x00,
0x01,0x33,0x32,0x0C,0x20,0xAC,0xCA,0x1E,0x01,0x32,0x06,0x0C,0x00,0x65,0xD1,0x17,
0xA0,0x64,0xD9,0x18,0x23,0xAC,0x33,0x02,0x01,0x33,0x06,0x0C,0x00,0x65,0xD1,0x17,
0xA0,0x64,0xD9,0x18,0x01,0x90,0x32,0x0C,0x08,0xAC,0xCC,0x1E,0xFF,0xAF,0x47,0x1F,
0x08,0xC3,0x48,0x1B,0x00,0xAD,0x8F,0x17,0x8F,0x04,0xD0,0x1C,0x40,0x6A,0x0C,0x00,
0x01,0x32,0x06,0x0C,0x00,0x65,0xD1,0x17,0x80,0x64,0xDD,0x1C,0xE0,0x64,0xD7,0x1C,
0xC0,0x64,0x35,0x1D,0xA0,0x64,0xDB,0x19,0x01,0x6A,0x91,0x00,0x00,0x6A,0x67,0x17,
0x00,0x65,0xD1,0x10,0x40,0x6A,0x0C,0x00,0xC0,0x64,0x35,0x1D,0x80,0x64,0xDB,0x19,
0x20,0xAC,0xDB,0x1B,0x40,0x6A,0x0C,0x00,0x00,0xAD,0x8E,0x17,0x8F,0x04,0xD6,0x1C,
0x1F,0xAE,0x8C,0x02,0x02,0x6A,0x8D,0x0C,0x03,0x8C,0x08,0x0C,0x10,0x8C,0xED,0x1E,
0x01,0x85,0x85,0x00,0x04,0xA4,0x88,0x0C,0xFE,0x85,0x85,0x02,0x04,0xA0,0x88,0x0C,
0x3D,0x6A,0x93,0x00,0x00,0x65,0x07,0x16,0x04,0x0B,0xF8,0x1A,0x10,0x0C,0xF3,0x1A,
0x00,0x65,0x14,0x10,0x00,0x6A,0x88,0x02,0x05,0x6A,0x93,0x00,0x10,0xA0,0x99,0x0C,
0x34,0x6A,0x93,0x00,0x04,0x0B,0xF8,0x1A,0x10,0x0C,0xF0,0x1E,0x04,0x0B,0xF8,0x1A,
0x00,0x6A,0x93,0x02,0x00,0x65,0xD1,0x17,0xC0,0x64,0x35,0x1D,0x11,0x6A,0x91,0x00,
0xC7,0x93,0x93,0x02,0x40,0xAE,0x32,0x1B,0x01,0x04,0xDE,0x0C,0x01,0x85,0x85,0x00,
0x07,0xBC,0x88,0x0C,0xFE,0x85,0x85,0x02,0x04,0xB8,0x88,0x0C,0x1F,0xAE,0x02,0x1F,
0x00,0x6A,0xAB,0x02,0x00,0x65,0xA8,0x11,0x03,0x8C,0x08,0x0C,0x39,0x6A,0x32,0x00,
0xFF,0xAB,0x06,0x1B,0x80,0xAE,0x07,0x1B,0x40,0x32,0x32,0x00,0x01,0x0C,0x07,0x1F,
0xA0,0x03,0x2A,0x1B,0xE0,0x03,0x03,0x02,0x40,0x03,0x0C,0x1B,0x04,0x32,0x32,0x00,
0x01,0x32,0x93,0x0C,0xE0,0xAE,0xAE,0x02,0x06,0xAE,0xA4,0x0C,0x00,0x65,0xCE,0x17,
0x00,0x65,0x07,0x16,0x10,0x0C,0x1A,0x1B,0x01,0x0B,0x14,0x1E,0x01,0x08,0x16,0x1B,
0xFF,0xAB,0x16,0x1B,0x80,0xA4,0x30,0x1B,0x01,0x0C,0x16,0x1F,0x10,0x0C,0x1C,0x1B,
0x00,0x65,0x95,0x17,0x00,0x65,0x10,0x11,0x04,0x93,0x1C,0x1B,0x01,0x94,0x1B,0x1F,
0x10,0x94,0x1C,0x1B,0xC7,0x93,0x93,0x02,0x08,0x93,0x1E,0x1B,0x01,0x0B,0x26,0x1F,
0x01,0x08,0x26,0x1B,0xFF,0xAB,0x26,0x1B,0x80,0xA4,0x30,0x1B,0x23,0x12,0x26,0x1D,
0x00,0x6A,0x32,0x02,0x00,0x65,0x97,0x17,0x03,0x08,0xA0,0x0C,0x08,0xEB,0x29,0x1F,
0x80,0xEB,0x27,0x1F,0x00,0x6A,0xEB,0x02,0x04,0x0C,0x2C,0x1F,0x00,0x65,0xD6,0x17,
0xE0,0x03,0x64,0x02,0xE0,0x03,0x03,0x02,0x00,0x65,0xD7,0x17,0x00,0x65,0xF8,0x10,
0x40,0xA4,0xA4,0x00,0xC7,0x93,0x93,0x02,0x00,0x65,0xD6,0x17,0x31,0x6A,0x91,0x00,
0x00,0x65,0x32,0x11,0x1F,0xAE,0x62,0x1B,0x40,0xA4,0x63,0x1F,0x01,0x06,0x33,0x0C,
0x00,0x65,0xD1,0x17,0xE0,0x64,0xDB,0x19,0xFF,0x12,0x3C,0x1F,0x41,0x6A,0x91,0x00,
0xFF,0x33,0x46,0x1B,0x00,0x65,0xDC,0x17,0x20,0xAC,0x42,0x1B,0x00,0x65,0xE7,0x17,
0xFF,0x6A,0xDF,0x00,0x01,0x35,0x90,0x0C,0xF0,0xC3,0x55,0x1B,0x70,0x6A,0xEE,0x17,
0x80,0x39,0x39,0x00,0x00,0x65,0x12,0x10,0x51,0x6A,0x91,0x00,0x81,0x6A,0x91,0x00,
0x01,0x32,0x06,0x0C,0x00,0x65,0xD1,0x17,0xA0,0x64,0xDB,0x19,0x40,0x6A,0x0C,0x00,
0xF7,0x01,0x01,0x02,0x08,0xAC,0x4F,0x1F,0xFF,0xAF,0x52,0x1F,0x06,0x6A,0xB0,0x00,
0x20,0xAC,0x52,0x1F,0x0D,0x6A,0xB0,0x00,0x01,0xB0,0x06,0x0C,0x08,0x01,0x01,0x00,
0x00,0x65,0x3D,0x11,0x01,0xC5,0x90,0x0C,0xFE,0xC3,0xC3,0x02,0x01,0x35,0x90,0x0C,
0x01,0xC4,0x90,0x0C,0xE0,0xC3,0x58,0x1F,0x10,0xC3,0xC3,0x06,0x00,0x65,0x0B,0x14,
0x01,0xC3,0x43,0x1F,0xF0,0xC3,0xC3,0x06,0xFE,0xC3,0xC3,0x02,0x02,0xC3,0x0B,0x1E,
0x01,0xC4,0x90,0x0C,0x00,0x65,0x5E,0x11,0x40,0xAE,0x37,0x1B,0x31,0x6A,0x91,0x00,
0x00,0x65,0x37,0x11,0x01,0x6A,0x67,0x17,0x00,0x65,0xF8,0x10,0x01,0x65,0xC6,0x0C,
0x02,0x01,0x01,0x00,0x02,0x12,0x6F,0x1D,0x04,0x12,0x79,0x1D,0x01,0x12,0x7E,0x1D,
0x03,0x12,0x86,0x1D,0x08,0x12,0x85,0x1D,0x41,0x6A,0x91,0x01,0x1F,0xAE,0x76,0x1B,
0x06,0xA4,0xAE,0x0C,0x04,0x14,0xB8,0x0C,0x01,0x85,0x85,0x00,0x04,0x88,0xBC,0x0C,
0xFE,0x85,0x85,0x02,0x03,0x08,0xC0,0x0C,0x00,0x65,0xD0,0x17,0xE0,0x64,0x6A,0x1D,
0xFF,0x6A,0x6A,0x03,0x40,0xAC,0x6E,0x1F,0x30,0x6A,0x89,0x17,0x00,0x6E,0x6E,0x1B,
0x00,0x65,0xDC,0x17,0x00,0x65,0x0B,0x10,0x00,0x65,0xD0,0x17,0xE0,0x64,0xE1,0x19,
0x01,0x12,0x33,0x0C,0x00,0x65,0xD0,0x17,0xE0,0x64,0xE1,0x19,0x71,0x6A,0x91,0x00,
0xFF,0x6A,0x6A,0x08,0x01,0x06,0x6A,0x0D,0x00,0x65,0xD0,0x17,0x80,0x64,0xDD,0x1C,
0xFF,0x6A,0x6A,0x03,0x08,0xAD,0x8B,0x1F,0x01,0x65,0x65,0x06,0x44,0xAD,0x64,0x0A,
0x01,0x64,0x6E,0x0C,0x01,0x6C,0x64,0x0D,0x1A,0x01,0x01,0x00,0x20,0x65,0x65,0x06,
0x01,0x6C,0x04,0x0C,0x76,0x6A,0x89,0x17,0x00,0x6E,0x94,0x1F,0x20,0x01,0x01,0x00,
0xFF,0x6A,0x6A,0x03,0xC7,0x93,0x93,0x02,0x38,0x93,0x96,0x1B,0x02,0xEB,0x9A,0x1F,
0x00,0x65,0x07,0x16,0x02,0xEB,0x0C,0x1A,0xFF,0xAB,0xA3,0x1F,0x04,0x88,0x88,0x0C,
0x01,0xAB,0x8C,0x0C,0x02,0x6A,0x8D,0x0C,0x00,0x6A,0xAB,0x02,0x03,0x8C,0x08,0x0C,
0x80,0xA4,0xA2,0x1F,0xBF,0x32,0x93,0x03,0x01,0x32,0x93,0x0D,0x04,0xE9,0x88,0x0C,
0x01,0x85,0x85,0x00,0x07,0xE9,0x88,0x0C,0xFE,0x85,0x85,0x02,0x01,0x88,0x88,0x0C,
0xFF,0x79,0xBB,0x1F,0x01,0x8C,0x64,0x0C,0x00,0x88,0x33,0x06,0x01,0x33,0x64,0x0C,
0x00,0x79,0xAB,0x02,0xFF,0xAB,0xBB,0x1F,0xFF,0x8E,0xB5,0x1B,0xFF,0x8D,0xB5,0x1B,
0xFF,0x79,0x64,0x04,0x00,0x8C,0x33,0x06,0x00,0x65,0xB5,0x13,0x00,0x6A,0xAB,0x02,
0x00,0x65,0xBB,0x11,0x01,0xAB,0x64,0x0C,0xFF,0x64,0x64,0x04,0x01,0x64,0x64,0x06,
0x00,0x8C,0x8C,0x08,0xFF,0x8D,0x8D,0x08,0xFF,0x8E,0x8E,0x08,0x1F,0xAE,0x02,0x1B,
0x03,0x8C,0x08,0x0C,0xFF,0xE9,0xC1,0x1F,0x80,0xA4,0xA4,0x00,0xFF,0xAB,0xA2,0x1B,
0xBF,0x32,0x93,0x03,0x01,0x32,0x93,0x0C,0x04,0xEA,0xEA,0x06,0x01,0xEA,0xA5,0x0C,
0x07,0xA6,0x64,0x02,0x80,0x64,0x64,0x00,0x00,0xEA,0xE1,0x19,0x00,0x6A,0xA5,0x02,
0x80,0xA6,0xA6,0x06,0x00,0x6A,0x64,0x02,0x00,0xA7,0xA7,0x08,0x00,0xA8,0xA8,0x08,
0x00,0xA9,0xA9,0x08,0x02,0x6A,0xEB,0x01,0x80,0xA4,0xCD,0x1F,0xFF,0x6A,0x6A,0x03,
0x01,0x06,0x6A,0x0C,0x01,0x0C,0xD1,0x1F,0x04,0x0C,0xD1,0x1B,0xE0,0x03,0x03,0x02,
0xE0,0x03,0x64,0x03,0x21,0x6A,0x91,0x00,0x00,0x65,0xD1,0x17,0xE0,0x64,0x65,0x1D,
0xC0,0x64,0x35,0x1D,0xA0,0x64,0xD5,0x1D,0xA0,0x64,0xE1,0x1F,0x61,0x6A,0x91,0x00,
0xDF,0x01,0x01,0x02,0xF7,0x11,0x11,0x02,0x01,0x06,0x6A,0x0C,0x09,0x0C,0xDF,0x1F,
0x08,0x0C,0xDB,0x1F,0xFF,0x6A,0x6A,0x03,0xFF,0xA0,0xA0,0x06,0xFF,0xA0,0xE2,0x1B,
0xFF,0xA1,0xA1,0x06,0xFF,0xA1,0xE2,0x19,0x01,0x6A,0x91,0x00,0x44,0xAD,0x65,0x0A,
0x0F,0xC7,0x64,0x02,0x00,0x65,0x65,0x00,0xFF,0x65,0x65,0x02,0x04,0x65,0x90,0x0A,
0x00,0x6A,0x64,0x02,0x00,0x90,0x90,0x07,0x01,0x90,0x64,0x0C,0xFF,0x6A,0xC6,0x00,
0xFF,0x6C,0xF4,0x19,0xFF,0x65,0x66,0x06,0x01,0x64,0x6D,0x0C,0x01,0x64,0x6D,0x0D,
0x01,0x65,0x66,0x0C,0x01,0x6C,0x90,0x0C,0x01,0x64,0xC6,0x0C,0x01,0x64,0x6D,0x0D,
0x01,0x90,0x64,0x0C,0x00,0x5E,0xFB,0x19,0x01,0xC6,0x5E,0x0D,0x01,0xC6,0x32,0x0C,
0x01,0x33,0x90,0x0C,0x01,0x32,0xC6,0x0C,0x01,0x64,0x90,0x0C,0x00,0x5F,0xE1,0x19,
0x01,0x33,0x5F,0x0D,
};
#endif
