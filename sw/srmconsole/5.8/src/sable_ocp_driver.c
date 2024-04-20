/* file:	sable_ocp_driver.c
 *
 * Copyright (C) 1993, 1994 by
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
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 *
 *
 * Abstract:	Sable OCP driver. This module is used to access
 *		the OCP on Sable.
 *
 * Author:	hcb - Harold Buckingham
 *
 * Modifications:
 *
 *	mc	27-Apr-1994	Added support for low-cost OCP.
 *
 *	mc	19-Nov-1993	Fixed up write routine.
 *
 *	hcb	xx-Sep-1993	Initial entry
 *
 */


#define DEBUG 0
#define DEBUG1 0

#include "cp$src:platform.h"
#include    "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include    "cp$src:kernel_def.h"
#include    "cp$src:msg_def.h"
#include    "cp$src:sable_ocp.h"

/* Function declarations: */

int sable_ocp_read (struct FILE *fp, int size, int number, unsigned char *buf);
int sable_ocp_write (struct FILE *fp, int size, int number, unsigned char *buf);
int sable_ocp_send_data (struct FILE *fp);
int sable_ocp_send_command (struct FILE *fp);

int null_procedure ();
struct DDB sable_ocp_ddb = {
	"sable_ocp",		/* how this routine wants to be called	*/
	sable_ocp_read,		/* read routine				*/
	sable_ocp_write,	/* write routine			*/
	null_procedure,		/* open routine				*/
	null_procedure,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/*					*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};
unsigned char lcd_display;


/*+
 * ============================================================================
 * = sable_ocp_init - Initialization routine for the sable ocp driver. 	      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routines will initialize the sable_ocp driver.
 *	It's primary function is to initialize the driver data base.
 *
 * FORM OF CALL:
 *
 *	sable_ocp_init (); 
 *      
 * RETURNS:
 *   
 *	int Status - msg_success
 *
 * ARGUMENTS:
 *
 *      
 *      None
 *
 * SIDE EFFECTS:               
 *      
 *      None
 *   
-*/                            

int sable_ocp_init () {
/*                                                      */
/*  Fill in inode for sable_ocp				*/
/*  *ip is the pointer to the INODE for this driver	*/
/*                                                      */
unsigned char read_buf[1], write_buf[1];
char ocp_string[16];
struct FILE *fp0;
struct INODE *ip;
    allocinode (sable_ocp_ddb.name, 1, &ip);
    ip->dva = & sable_ocp_ddb;
    ip->attr = ATTR$M_READ | ATTR$M_WRITE;
    ip->loc = 0;
    INODE_UNLOCK (ip);

#if !PC264
    fp0 = fopen("iic_ocp0","sr+");  
    if (fp0 == 0) {           
      pprintf("Device Open Error: IIC_OCP0\n");
      return(msg_failure);    
    }                         
#else  /* PC264 */                     
    {                         
       int i;                 
       struct FILE *fp1;      
       fp0 = fopen("iic_ocp0","sr+");  
       if (fp0 == 0) {           
          return(msg_success);    
       }                         
       fp1 = fopen("iic_ocp1","sr+");  
       if (fp1 == 0) {              
         pprintf("Device Open Error: IIC_OCP1\n");
         return(msg_failure);       
       }                            
       for (i=0; i < 6; i++)        
       {                            
         static char init_buf[] = {0x38,0x38,0x38,0x0e,0x02,0x01};
         fseek(fp1, OCP_EXPNDR_OFFSET, SEEK_SET);
         fwrite(&init_buf[i],1,1,fp1);  
         sable_ocp_send_command(fp0);  
       }  
       fclose(fp1);                          
    }  
#endif    / * PC264 */

    write_buf[0] = 0xff;        
    fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
    fwrite(write_buf, 1, 1, fp0);
    fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
    fread(read_buf, 1, 1, fp0);
#if DEBUG1
pprintf("read buffer = %x\n", read_buf[0]);
#endif
    if (read_buf[0] & OCP_TYPE)
      lcd_display = 0;
    else
      lcd_display = 1;
#if RAWHIDE
      lcd_display = 1;
#endif
    fclose(fp0);		
#if DEBUG1
pprintf("read buffer = %x\n", read_buf[0]);
pprintf("sable_ocp_init: lcd_display = %x\n", lcd_display);
#endif
    sprintf(ocp_string, "%s", "Console  Started");
    sable_ocp_write(fp0, strlen(ocp_string), 1, ocp_string);
    return msg_success;
}


/*+
 * ============================================================================
 * = sable_ocp_read - Read of sable_ocp port.				      =
 * ============================================================================
 *
 * OVERVIEW:         
 *  
 *      This routine is used to read the sable_ocp port. It's primary function
 *	is to hide all the translations needed to access the port.
 *  
 * FORM OF CALL:
 *  
 *	sable_ocp_read (fp,size,number,c);
 *  
 * RETURNS:
 *
 *	number of bytes read
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fp - Pointer to a file.
 *	int size - Used to test for buffer size.
 *	int number - Used to test for buffer size.
 *	unsigned char *c - For MOP load the base of where this is to be loaded.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int sable_ocp_read (struct FILE *fp,int size,int number,unsigned char *c)
{
#if DEBUG
    pprintf ("sable_ocp_read\n");
#endif
return 0;
}


/*+
 * ============================================================================
 * = sable_ocp_write - Sable_ocp port write routine.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This routine writes to the sable_ocp port. Its primary function is to hide
 *	the translations needed to write to the port.
 *
 * FORM OF CALL:
 *
 *	sable_ocp_write (fp, item_size, number, buf); 
 *                    
 * RETURNS:
 *
 *	int Status - msg_success or FAILURE.
 *
 * ARGUMENTS:
 *
 *      struct FILE *fp - Pointer to the file to be written.
 *	int item_size	- size of item
 *	int number	- number of items
 *	char *buf - Buffer to be written. 
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int sable_ocp_write (struct FILE *fp, int size, int number, unsigned char *buf)
{
    struct FILE *fp0;
    struct FILE *fp1;
    unsigned char cntrl_bits, addr_bits, led_bank;
    unsigned char cntrl_buf[1], data_buf[1], ocp_string[16];
    int j, status, i, len;

    fp0 = fopen("iic_ocp0","sr+");  
    if (fp0 == 0) {
      /* pprintf("Device Open Error: IIC_OCP0\n"); */
      return(msg_failure);
    }
    fp1 = fopen("iic_ocp1","sr+");  
    if (fp1 == 0) {
      pprintf("Device Open Error: IIC_OCP1\n");
      return(msg_failure);
    }

    len = size * number;
    if (len > 16)
	len = 16;
    memset(ocp_string, ' ', 16);
    memcpy(ocp_string, buf, len);

    addr_bits = 0xff;
    cntrl_bits = 0x58;

#if DEBUG
pprintf("number = %d\n", number);
pprintf("size = %d\n", size);
pprintf("string = %.16s\n", buf);
#endif

/*								*/
/* The OCP write routine will do the following:	       		*/
/* 	-- open the OCP control device (iic_ocp0) 		*/
/* 	-- open the OCP data device (iic_ocp1)			*/
/*	-- erase the display by writing 0xff to iic_ocp0	*/
/*	-- copy the string to be displayed into a buffer and	*/
/*	   write it to the OCP one byte at a time.		*/
/*								*/
/* Note:  Once the eigth character is reached address bit 3 is  */
/* 	  shifted two places to the left and used to select the */
/* 	  the next bank of LEDs.				*/
/*								*/
#if DEBUG1
pprintf("sable_ocp_write: lcd_display = %x\n", lcd_display);
#endif         
    if (!lcd_display) {
      for (j=0; j<16; j++) {
         if ((ocp_string[j] > 0x1f) && (ocp_string[j] < 0x7b) && (j<16)) {
            addr_bits = addr_bits + 1;
            led_bank = (((addr_bits & OCP_MODE_SELECT) << 2) ^ OCP_BANK_SELECT); 
            cntrl_buf[0] = ((cntrl_bits | (addr_bits & OCP_ADDRESS_MASK)) ^ led_bank);
            fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
            status = fwrite(cntrl_buf,1,1,fp0);  
            data_buf[0] = ocp_string[j];
#if DEBUG
pprintf("data %d = %x\n", j, ocp_string[j]);
#endif
            fseek(fp1, OCP_EXPNDR_OFFSET, SEEK_SET);
            status = fwrite(data_buf,1,1,fp1);  
            cntrl_buf[0] = (cntrl_buf[0] ^ OCP_WRITE_ENABLE);
            fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
            status = fwrite(cntrl_buf,1,1,fp0);  
            cntrl_buf[0] = (cntrl_buf[0] | OCP_WRITE_ENABLE);
            fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
            status = fwrite(cntrl_buf,1,1,fp0);  
         }
      }
    }
    else {
      cntrl_buf[0] = 0x80;		/* Select display position			*/
      fseek(fp1, OCP_EXPNDR_OFFSET, SEEK_SET);
      fwrite(cntrl_buf,1,1,fp1);  
      sable_ocp_send_command(fp0);
      
      for (j=0;j<16;j++){
        if (j == 8) {
          cntrl_buf[0] = 0xc0;		/* Select display position			*/
          fseek(fp1, OCP_EXPNDR_OFFSET, SEEK_SET);
          fwrite(cntrl_buf,1,1,fp1);  
          sable_ocp_send_command(fp0);  
        }  
        data_buf[0] = ocp_string[j];
        fseek(fp1, OCP_EXPNDR_OFFSET, SEEK_SET);
        fwrite(data_buf,1,1,fp1);  
	sable_ocp_send_data(fp0);
      }
    }
    fclose(fp0);		
    fclose(fp1);	
    return 16;
}

void sable_ocp_send_command(struct FILE *fp0){
unsigned char command_temp[1];
  command_temp[0] = 0x60;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(command_temp,1,1,fp0);  
  command_temp[0] = 0x40;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(command_temp,1,1,fp0);  
  command_temp[0] = 0x60;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(command_temp,1,1,fp0);  
  command_temp[0] = 0x71;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(command_temp,1,1,fp0);  
}

void sable_ocp_send_data(struct FILE *fp0){
unsigned char data_temp[1];
  data_temp[0] = 0x61;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(data_temp,1,1,fp0);  
  data_temp[0] = 0x41;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(data_temp,1,1,fp0);  
  data_temp[0] = 0x61;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(data_temp,1,1,fp0);  
  data_temp[0] = 0x71;
  fseek(fp0, OCP_EXPNDR_OFFSET, SEEK_SET);
  fwrite(data_temp,1,1,fp0);  
}
