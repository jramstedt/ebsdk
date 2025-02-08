/*****************************************************************************

       Copyright � 1993, 1994 Digital Equipment Corporation,
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
__attribute__((unused)) static const char *rcsid = "$Id: dp264mck.c,v 1.2 1999/01/21 19:04:38 gries Exp $";
#endif

/*
 * $Log: dp264mck.c,v $
 * Revision 1.2  1999/01/21 19:04:38  gries
 * First Release to cvs gries
 *
 * Revision 1.4  1999/01/19  16:47:18  gries
 * change for ecc eroors
 *
 * Revision 1.3  1998/08/03  17:17:00  gries
 * Put more better in Machine Check
 *
 * Revision 1.2  1997/12/15  20:52:49  pbell
 * Updated for dp264.
 *
 */

#include "lib.h"
#include "mon.h"
#include "mcheck.h"
#include "callback.h"
extern int corrected_error_count;
extern int byte_number_error_count[8];
extern int bank_number_error_count[4];
extern int dimm_number_error_count[16];
#ifndef MAKEDEPEND
#include "paldata.h"		/* For logout area */
#include "palcsrv.h"
#endif
#define pause_on_error
syndrome_75_to_byte[8] = {
	2,1,4,7,6,5,0,3};

bit_to_syndrome[72] ={
	0xce,
	0xcb,
	0xd3,
	0xd5,
	0xd6,
	0xd9,
	0xda,
	0xdc,
	0x23,
	0x25,
	0x26,
	0x29,
	0x2a,
	0x2c,
	0x31,
	0x34,
	0x0e,
	0x0b,
	0x13,
	0x15,
	0x16,
	0x19,
	0x1a,
	0x1c,
	0xe3,
	0xe5,
	0xe6,
	0xe9,
	0xea,
	0xec,
	0xf1,
	0xf4,
	0x4f,
	0x4a,
	0x52,
	0x54,
	0x57,
	0x58,
	0x5b,
	0x5d,
	0xa2,
	0xa4,
	0xa7,
	0xa8,
	0xab,
	0xad,
	0xb0,
	0xb5,
	0x8f,
	0x8a,
	0x92,
	0x94,
	0x97,
	0x98,
	0x9b,
	0x9d,
	0x62,
	0x64,
	0x67,
	0x68,
	0x6b,
	0x6d,
	0x70,
	0x75,
	0x01,
	0x02,
	0x04,
	0x08,
	0x10,
	0x20,
	0x40,
	0x80};
/* Decoder for memory errors */
/* Tables:
Syndrome Hi or Lo determines low or high 127 bits
- sets a Low or High Flag
Decoded syndrome to bit in error, bit 3 determines even or odd byte
- sets the Odd or Even Flag

Decoder for DIMM Slot Number

*/
long array_address_register[4];
void get_array_address_register(void)
{
   array_address_register[0]= *(long *)(0x801a0000100ul);
   array_address_register[1]= *(long *)(0x801a0000140ul);
   array_address_register[2]= *(long *)(0x801a0000180ul);
   array_address_register[3]= *(long *)(0x801a00001c0ul);
}

void print_dimm_number(long C_ADDR,int byte_number)
{
  int dimm,i,j,bank=-1,index[4]= {0,1,2,3};
    get_array_address_register();
    for (i=0;i<4;i++)
      array_address_register[i]&= ~(0xfffffful);
    for (i=1;i<4;i++)
    if (array_address_register[index[i]]<array_address_register[index[0]])
    {
      j= index[i];
      index[i]= index[0];
      index[0]= j;
    }
    for (i=2;i<4;i++)
    if (array_address_register[index[i]]<array_address_register[index[1]])
    {
      j= index[i];
      index[i]= index[1];
      index[1]= j;
    }
    if (array_address_register[index[3]]<array_address_register[index[2]])
    {
      j= index[3];
      index[3]= index[2];
      index[2]= j;
    }
    dimm= ((byte_number & 1) << 1) + ((C_ADDR >> 4) & 1);
    for (i=0;i<4;i++)
    {
      if ((C_ADDR >= array_address_register[index[i]])
       & (array_address_register[index[i+1]]>C_ADDR))
      bank= index[i];
    }
    printf("C_ADDR %x\nBank %d Dimm %d byte number %d in error\n",
	C_ADDR,bank,dimm,byte_number);
    byte_number_error_count[byte_number]++;
    bank_number_error_count[bank]++;
    dimm_number_error_count[(bank<<2) + dimm]++;
}


int get_byte_number_from_ecc(int low)
{
int i,j,k,byte_number= -1;

  j= 1;
    for(i= 0;i<8;i++)
    {
      if (low==j) byte_number= i;
      j+= j;
    }
    if (byte_number<0)
    {
      byte_number= syndrome_75_to_byte[low>> 5];
    }
    return byte_number;
}


void DumpLogout(LogoutFrame_t *Frame)
{
  int c=-1;

#ifdef pause_on_error
  if (corrected_error_count<10)
  {
  Beep(100,1000);
  printf("DumpLogout\n");
  printf("MCHK_Code \t%d\n",Frame->MCHK_Code);
  printf("MCHK_Frame_Rev \t%d\n",Frame->MCHK_Frame_Rev);
  printf("I_STAT \t%016lx\n",Frame->I_STAT);
  printf("DC_STAT \t%016lx\n",Frame->DC_STAT);
  printf("C_ADDR \t%016lx\n",Frame->C_ADDR);
  printf("DC1_SYNDROME \t%016lx\n",Frame->DC1_SYNDROME);
  printf("DC0_SYNDROME \t%016lx\n",Frame->DC0_SYNDROME);
  printf("C_STAT \t%016lx\n",Frame->C_STAT);
  printf("C_STS \t%016lx\n",Frame->C_STS);
  printf("RESERVED0 \t%016lx\n",Frame->RESERVED0);
  printf("EXC_ADDR \t%016lx\n",Frame->EXC_ADDR);
  printf("IER_CM \t%016lx\n",Frame->IER_CM);
  printf("ISUM \t%016lx\n",Frame->ISUM);
  printf("MM_STAT \t%016lx\n",Frame->MM_STAT);
  printf("PAL_BASE \t%016lx\n",Frame->PAL_BASE);
  printf("I_CTL \t%016lx\n",Frame->I_CTL);
  printf("PCTX \t%016lx\n",Frame->PCTX);
  while (c<0)    
   c = (*UserGetChar)();
  }
#endif
}

void ParseLogout(LogoutFrame_t *Frame)
{
  int c=-1;
#ifdef pause_on_error
  if (corrected_error_count<10)
  {
  Beep(100,1000);
  printf("ParseLogout\n");
  printf("MCHK_Code \t%d\n",Frame->MCHK_Code);
  printf("MCHK_Frame_Rev \t%d\n",Frame->MCHK_Frame_Rev);
  printf("I_STAT \t%016lx\n",Frame->I_STAT);
  printf("DC_STAT \t%016lx\n",Frame->DC_STAT);
  printf("C_ADDR \t%016lx\n",Frame->C_ADDR);
  printf("DC1_SYNDROME \t%016lx\n",Frame->DC1_SYNDROME);
  printf("DC0_SYNDROME \t%016lx\n",Frame->DC0_SYNDROME);
  printf("C_STAT \t%016lx\n",Frame->C_STAT);
  printf("C_STS \t%016lx\n",Frame->C_STS);
  printf("RESERVED0 \t%016lx\n",Frame->RESERVED0);
  printf("EXC_ADDR \t%016lx\n",Frame->EXC_ADDR);
  printf("IER_CM \t%016lx\n",Frame->IER_CM);
  printf("ISUM \t%016lx\n",Frame->ISUM);
  printf("MM_STAT \t%016lx\n",Frame->MM_STAT);
  printf("PAL_BASE \t%016lx\n",Frame->PAL_BASE);
  printf("I_CTL \t%016lx\n",Frame->I_CTL);
  printf("PCTX \t%016lx\n",Frame->PCTX);

  while (c<0)    
   c = (*UserGetChar)();
  }
#endif
  outLed(0xD0);

}

void DumpShortLogout(LogoutFrame_t *Frame)
{
  int c=-1;
#ifdef pause_on_error
  if (corrected_error_count<10)
  {
  Beep(100,1000);
  printf("DumpShortLogout\n");
  printf("MCHK_Code \t%d\n",Frame->MCHK_Code);
  printf("MCHK_Frame_Rev \t%d\n",Frame->MCHK_Frame_Rev);
  printf("I_STAT \t%016lx\n",Frame->I_STAT);
  printf("DC_STAT \t%016lx\n",Frame->DC_STAT);
  printf("C_ADDR \t%016lx\n",Frame->C_ADDR);
  printf("DC1_SYNDROME \t%016lx\n",Frame->DC1_SYNDROME);
  printf("DC0_SYNDROME \t%016lx\n",Frame->DC0_SYNDROME);
  printf("C_STAT \t%016lx\n",Frame->C_STAT);
  printf("C_STS \t%016lx\n",Frame->C_STS);
  printf("RESERVED0 \t%016lx\n",Frame->RESERVED0);
  printf("EXC_ADDR \t%016lx\n",Frame->EXC_ADDR);
  printf("IER_CM \t%016lx\n",Frame->IER_CM);
  printf("ISUM \t%016lx\n",Frame->ISUM);
  printf("MM_STAT \t%016lx\n",Frame->MM_STAT);
  printf("PAL_BASE \t%016lx\n",Frame->PAL_BASE);
  printf("I_CTL \t%016lx\n",Frame->I_CTL);
  printf("PCTX \t%016lx\n",Frame->PCTX);

  Beep(100,1000);
  while (c<0)    
   c = (*UserGetChar)();
  }
#endif
  if (Frame->C_STAT)
  {
  printf("DumpShortLogout\n");
  printf("MCHK_Code \t%d\n",Frame->MCHK_Code);
  printf("MCHK_Frame_Rev \t%d\n",Frame->MCHK_Frame_Rev);
  printf("I_STAT \t%016lx\n",Frame->I_STAT);
  printf("DC_STAT \t%016lx\n",Frame->DC_STAT);
  printf("C_ADDR \t%016lx\n",Frame->C_ADDR);
  printf("DC1_SYNDROME \t%016lx\n",Frame->DC1_SYNDROME);
  printf("DC0_SYNDROME \t%016lx\n",Frame->DC0_SYNDROME);
  printf("C_STAT \t%016lx\n",Frame->C_STAT);
  printf("C_STS \t%016lx\n",Frame->C_STS);
  printf("RESERVED0 \t%016lx\n",Frame->RESERVED0);
  printf("EXC_ADDR \t%016lx\n",Frame->EXC_ADDR);
  printf("IER_CM \t%016lx\n",Frame->IER_CM);
  printf("ISUM \t%016lx\n",Frame->ISUM);
  printf("MM_STAT \t%016lx\n",Frame->MM_STAT);
  printf("PAL_BASE \t%016lx\n",Frame->PAL_BASE);
  printf("I_CTL \t%016lx\n",Frame->I_CTL);
  printf("PCTX \t%016lx\n",Frame->PCTX);
   if (Frame->DC0_SYNDROME)
     print_dimm_number(Frame->C_ADDR,get_byte_number_from_ecc(Frame->DC0_SYNDROME));
   if (Frame->DC1_SYNDROME)
     print_dimm_number(Frame->C_ADDR,get_byte_number_from_ecc(Frame->DC1_SYNDROME));
  }

  outLed(0xD1);

}

void SetMcheckSystem(State_t state)
{
  int c=-1;
  outLed(0xD2);
  outLed(0xD3);

}
