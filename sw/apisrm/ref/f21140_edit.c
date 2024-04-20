/*
 * Copyright (C) 1996 by
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
 * Abstract:	Edit 21140A EEROM
 *
 * Author:	James F. Reilley
 *
 * Modifications:
 *
 *	jfr	10-Jun-1996	Initial entry.
 *
 */



#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"		/*Kernel definitions*/
#include "cp$src:pb_def.h"		/*Port block definitions*/
#include "cp$src:ctype.h"
/*#include "cp$src:tu.h"			/*Tulip register definitions*/
#include "cp$src:common.h"		/*Common definitions*/

/* define these here so we don't need to include tu.h, which causes some 
definition conflicts */
typedef struct _TU_RING  {
   u_long       tu_flags;       /* rcv/xmt status flags */
   u_long       tu_info;        /* rcv/xmt buffer size and info*/
   u_long       tu_bfaddr1;     /* buffer address 1*/
   u_long       tu_bfaddr2;     /* buffer address 1*/
} TU_RING,*PTU_RING;
#define TU_CSR9_ADDR            0x0048  /* Diag address                     */

#include "cp$inc:prototypes.h"
#include "cp$src:mop_counters64.h"	/*MOP counters*/
#include "cp$src:probe_io_def.h"
#include "cp$src:tupb.h"		/*Tulip port block definitions*/

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern shellsort(int *, int , int (*) ());
extern struct pb **pbs;
extern unsigned int num_pbs;

/* Function prototypes */
int srom21140_edit(int,char *[]);
int srom21140_show(struct TUPB *, int argc, char *argv[] );
void uwire_srom(struct TUPB *, unsigned char *);
void read_21140srom(struct TUPB *, unsigned char *);
void uwire_srom_wr(struct TUPB *, unsigned int,unsigned int);
void uwire_srom_ewen (struct TUPB *);
void srom_display(union srom21140_srom  *);
unsigned short calc_crc(union srom21140_srom *);

/*MACROS */


#define RD_TULIP_CSR_LONG(p,r)     (p->pb.type==TYPE_EISA?inportl(p,p->pb.csr+(r)*p->rm):inmeml(p,p->pb.csr+(r)*p->rm))
#define WRT_TULIP_CSR_LONG(p,r,v)  (p->pb.type==TYPE_EISA?outportl(p,p->pb.csr+(r)*p->rm,v):outmeml(p,p->pb.csr+(r)*p->rm,v))

/* 21140 SROM format structure */
union srom21140_srom {
	struct{
		unsigned char subsys_vend_id[2];
		unsigned char subsys_id[2];
		unsigned char cardbus_ptr_low[2];
		unsigned char cardbus_ptr_high[2];
		unsigned char id_res1[8];
		unsigned char id_res2[1];
		unsigned char id_block_crc[1];
		unsigned char srom_for_ver[1];
		unsigned char cont_cnt[1];
		unsigned char na[6];
		unsigned char adapter_cnt[1];
		unsigned char adapter_0_leaf_offset[2];
		unsigned char name[8];
		unsigned char stat[2];
		unsigned char serial_no[10];
		unsigned char res[16];
		unsigned char conection_type[2];
		unsigned char gen_p_cntl[1];
		unsigned char media_cnt[1];
		unsigned char media_1[4];
		unsigned char media_2[4];
		unsigned char media_3[4];
		unsigned char media_4[4];
		unsigned char media_5[4];
		unsigned char the_rest[37];
		unsigned char crc[2];
		} srom21140_bf;		/* 128 bytes */
	unsigned char array[128];	/* byte array */
	};

unsigned char default_21140srom[128] = {
00,00,
00,00,
00,00,
00,00,
00,00,00,00,00,00,00,00,
00,00,				    /* ID_res2, ID_BLOCK_CRC */
01,01,				    /* SROM format version, controller cnt */
00,00,00,00,00,00,		    /* NA */
00,0x41,
00,'M','o','n','e','t',' ',' ',' ',' ',    /* adapter name P2SEPLUS */
0x02,00,                             /* status AGB info */
00,00,00,00,00,00,00,00,00,00,           /* serial # ASCII, AGB info */
00,00,00,00,00,00,00,00,00,00,
00,00,00,00,00,00,                     /* ABG info */
0x08,0x0f,                             /* sel connection type  autosense */
                                    /* General purpose control */

0x05,00,                               /* media count ,Media #1 */
0x08,0x9e,                               /* GP Port Data */
0x00,0x03,				     /* Command ,media code */
0x09,0xed,
0x00,0x05,
                              /* Media #3 ??? */
0x01,0xed,
0x00,0x04,
                               /* Media #4 ??? */
0x00,0x9e,
0x00,0x01,			/* Media #5 10Base2 (BNC) */
0x10,0x9e,
0x00,00,
0x00,00,
00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,
00,00,00,00,00,00,00,00,
00,00,
};

 /* strtol  - Converts ascii string of numerical digits to a long */
long strtol (const char *s, char **endp, int base)
{
  long i;

  if (base == 0)base = 10;

  common_convert(s,base,(unsigned char *)&i,sizeof(i));
  return i;

}



/*+
 * ============================================================================
 * = srom21140_edit - Edit 21140A EEROM parameters.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The srom21140_edit command permits the modification of 21140A eerom
 *	parameters from the console.  Individual bits can be set or cleared,
 *	or bytes can be set to a specific value.  Entering srom21140_edit
 *	without an option or target device will list these parameters for
 *	all ISP1020 devices in the system.
 *  
 *   COMMAND FMT: srom21140_edit 2 Z 0 srom21140_edit
 *
 *   COMMAND FORM:
 *  
 *	srom21140_edit ( {w/z/d/na} {device name} 
 *			 [w {word offset in decimal} {word of data in hex}]
 *			 [z { zero out srom, no args}]
 *			 [d {default values}]
 *                       [n {network address, no delimiters}]
 *  
 *	examples:
 *		    srom ewa0			# read ewa0 srom
 *  
 *		    srom na 08002b001122 ewa0	# write ewa0 network address
 *
 *		    srom w 63 9854		# write ewa0 checksum
 *  
 *  
 *  
 *                       )
 *
 *   COMMAND TAG: srom21140_edit 0 RXBZ srom21140_edit
 *
 *   COMMAND ARGUMENT(S):
 *
 *
 *   COMMAND OPTION(S):
 *
 *
 *   COMMAND EXAMPLE(S):
 *~
 *~
 * FORM OF CALL:
 *  
 *  
 * RETURN CODES:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	argc	- number of words present on command line
 *	argv	- array of pointers to words on command line
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/


int srom21140_edit( int argc, char *argv[] )
    {
    int i,j,qual=0;
    struct pb *pb;
    struct tupb *tupb;
    int strcmp_pb( );

    krn$_wait( &pbs_lock );
    shellsort( pbs, num_pbs, strcmp_pb );


	for( j = 0; j < num_pbs; j++ )
	    {
	    pb = pbs[j];
	    if( ( strcmp( pb->device, "ew" ) == 0 ))
	    srom21140_show(pb, argc, argv);               /*atoi (argv [1]));*/
	    }
    krn$_post( &pbs_lock );


    return(1);
    }


int srom21140_show( struct TUPB *pb, int argc, char *argv[] )
 {
  union srom21140_srom srom_dat, *srom_ptr = &srom_dat;
  int i,add[3];
  char temp[5]={0,0,0,0,0};

    printf("\n\n");
    for (i=0;  i<argc;  i++){printf("%s  ",argv[i]);}
    printf("\n");

    printf("pb %x\n",pb->pci_type);
 if((pb->pci_type & 0xfffff) == 0x91011)
  {
  printf("pb %x\ndevice   - %s\nname - %s\nalias - %s\ninfo -%s\nstring %s\n",
	pb,
	pb->pb.device,
	pb->pb.name,
	pb->pb.alias,
	pb->pb.info,
	pb->pb.string);

  printf("pb->pb.csr  - %x\n",pb->pb.csr);
  printf("pb->pci_type %x\n\n",pb->pci_type);
  }

  if((pb->pci_type & 0xfffff) == 0x91011) /* check for 21140A 0r 43 device */
  {

  /* only look at a particular device is serveral are found */
  if((strncmp_nocase(argv[1], pb->pb.alias,4) ==0)  || argc == 1)
   {
   read_21140srom(pb,srom_ptr->array);
   srom_display(srom_ptr);
   printf("calculated CRC = %x\n",calc_crc(srom_ptr));
   }

   if(*argv[1] == 'z')
   if(argc == 3)
   if(strncmp_nocase(argv[2], pb->pb.alias,4)==0)
   {
   uwire_srom_ewen(pb);         /* set program mode */
   for(i=0;i<64;i++)
   uwire_srom_wr(pb,i,strtol(argv[3],NULL,16));/* wr add with word */
   read_21140srom(pb,srom_ptr->array);
   srom_display(srom_ptr);
   }

   if(*argv[1] == 'w')
   if(argc == 5)
   if(strncmp_nocase(argv[4], pb->pb.alias,4)==0)
   { 
   uwire_srom_ewen(pb);		/* set program mode */
   uwire_srom_wr(pb,atoi(argv[2]),strtol(argv[3],NULL,16));/* wr add with word of data */
   read_21140srom(pb,srom_ptr->array);
   srom_display(srom_ptr);
   } 

   if(*argv[1] == 'd')
   if(argc == 3)
   if(strncmp_nocase(argv[2], pb->pb.alias,4)==0)
   {
   uwire_srom_ewen(pb);         /* set program mode */
   for(i=0;i<64;i++)
   uwire_srom_wr(pb,i,(int)(default_21140srom[i*2]<<8 | default_21140srom[(i*2)+1]));	/* add, data*/
   read_21140srom(pb,srom_ptr->array);
   srom_display(srom_ptr);
   }

  if(*argv[1] == 'n')
  if(argc == 4)
  if(strncmp_nocase(argv[3], pb->pb.alias,4)==0)
   {
   printf("Present Network address = ");
   for(i=0;i<6;i++)
   printf("%s%02x%c",(i==0)?"NA ":'',srom_ptr->srom21140_bf.na[i],(i==5)?'\n':'-');

   uwire_srom_ewen(pb);         /* set program mode */

    for(i=0;i<3;i++){
    temp[0] = argv[2][(i*4)];
    temp[1] = argv[2][(i*4)+1];
    temp[2] = argv[2][(i*4)+2];
    temp[3] = argv[2][(i*4)+3];
    add[i]=strtol(temp,NULL,16);
    }
    uwire_srom_wr(pb,10, add[0]); 
    uwire_srom_wr(pb,11, add[1]); 
    uwire_srom_wr(pb,12, add[2]); 

   read_21140srom(pb,srom_ptr->array);
   srom_display(srom_ptr);
   }
 


  }else /* not a 21141 or 21143 type device */
  printf(", but it not my type!\n");

return(1);
}

void srom_display(union srom21140_srom *srom_ptr)
{
   int i;
   printf("word cnt\n");
   for(i=1;i<129;i+=2)
   {
   if(((i-1)%16)==0)printf("%03d:",(i-1)/2);
   printf("%02x%02x%c",srom_ptr->array[i-1],srom_ptr->array[i]	/* data bytes */
		  ,((i+1)%16)?'.':'\n');	/* new line */
   }

/*   printf("Network ID = ");
   for(i=0;i<6;i++)
   printf("%02x%c",srom_ptr->srom21140_bf.na[i],(i<5)?'-':'\n');
*/

    printf("sub-vend_id     %02x%02x\n",
	srom_ptr->srom21140_bf.subsys_vend_id[0],
        srom_ptr->srom21140_bf.subsys_vend_id[1]);
    printf("subsys_id       %02x%02x\n",
        srom_ptr->srom21140_bf.subsys_id[0],
        srom_ptr->srom21140_bf.subsys_id[1]);
    printf("cardbus_ptr_low %02x%02x\n",
        srom_ptr->srom21140_bf.cardbus_ptr_low[0],
        srom_ptr->srom21140_bf.cardbus_ptr_low[1]);
    printf("cardbus_ptr_hi  %02x%02x\n",
        srom_ptr->srom21140_bf.cardbus_ptr_high[0],
        srom_ptr->srom21140_bf.cardbus_ptr_high[1]);
    printf("id_res1         %02x%02x%02x%02x%02x%02x%02x%02x\n",
        srom_ptr->srom21140_bf.id_res1[0],
        srom_ptr->srom21140_bf.id_res1[1],
        srom_ptr->srom21140_bf.id_res1[2],
        srom_ptr->srom21140_bf.id_res1[3],
        srom_ptr->srom21140_bf.id_res1[4],
        srom_ptr->srom21140_bf.id_res1[5],
        srom_ptr->srom21140_bf.id_res1[6],
        srom_ptr->srom21140_bf.id_res1[7]);
    printf("id_res2         %02x\n",
        srom_ptr->srom21140_bf.id_res2[0]);
    printf("id_block_crc    %02x\n",
        srom_ptr->srom21140_bf.id_block_crc[0]);

    printf("srom_for_ver    %x\n",srom_ptr->srom21140_bf.srom_for_ver[0]);
    printf("cont_cnt        %x\n",srom_ptr->srom21140_bf.cont_cnt[0]);
    printf("HW address      %02x-%02x-%02x-%02x-%02x-%02x\n",
	srom_ptr->srom21140_bf.na[0],
	srom_ptr->srom21140_bf.na[1],
	srom_ptr->srom21140_bf.na[2],
	srom_ptr->srom21140_bf.na[3],
	srom_ptr->srom21140_bf.na[4],
	srom_ptr->srom21140_bf.na[5]);
    printf("adapter_cnt     %02x\n",srom_ptr->srom21140_bf.adapter_cnt[0]);
    printf("leaf_offset     %02x%02x\n",
	srom_ptr->srom21140_bf.adapter_0_leaf_offset[0],
	srom_ptr->srom21140_bf.adapter_0_leaf_offset[1]);
    printf("Name            %c%c%c%c%c%c%c%c\n",
	srom_ptr->srom21140_bf.name[0],
	srom_ptr->srom21140_bf.name[1],
	srom_ptr->srom21140_bf.name[2],
	srom_ptr->srom21140_bf.name[3],
	srom_ptr->srom21140_bf.name[4],
	srom_ptr->srom21140_bf.name[5],
	srom_ptr->srom21140_bf.name[6],
	srom_ptr->srom21140_bf.name[7]);
    printf("stat            %02x%02x\n",srom_ptr->srom21140_bf.stat[0],srom_ptr->srom21140_bf.stat[1]);
    printf("serial_no       ");
    for(i=0;i<10;i++)(printf("%02x",srom_ptr->srom21140_bf.serial_no[i]));
    printf("\ncon-type        %02x%02x\n",
	srom_ptr->srom21140_bf.conection_type[0],
	srom_ptr->srom21140_bf.conection_type[1]);
    printf("gen_p_cntl      %02x\n",srom_ptr->srom21140_bf.gen_p_cntl[0]);
    printf("media_cnt       %02x\n",srom_ptr->srom21140_bf.media_cnt[0]);
    printf("media_1         %02x.%02x.%02x%02x\n",
	srom_ptr->srom21140_bf.media_1[0],
	srom_ptr->srom21140_bf.media_1[1],
	srom_ptr->srom21140_bf.media_1[2],
	srom_ptr->srom21140_bf.media_1[3]);
    printf("media_2         %02x.%02x.%02x%02x\n",
	srom_ptr->srom21140_bf.media_2[0],
	srom_ptr->srom21140_bf.media_2[1],
	srom_ptr->srom21140_bf.media_2[2],
	srom_ptr->srom21140_bf.media_2[3]);
    printf("media_3         %02x.%02x.%02x%02x\n",
	srom_ptr->srom21140_bf.media_3[0],
	srom_ptr->srom21140_bf.media_3[1],
	srom_ptr->srom21140_bf.media_3[2],
	srom_ptr->srom21140_bf.media_3[3]);
    printf("media_4         %02x.%02x.%02x%02x\n",
	srom_ptr->srom21140_bf.media_4[0],
	srom_ptr->srom21140_bf.media_4[1],
	srom_ptr->srom21140_bf.media_4[2],
	srom_ptr->srom21140_bf.media_4[3]);
    printf("media_5         %02x.%02x.%02x%02x\n",
	srom_ptr->srom21140_bf.media_5[0],
	srom_ptr->srom21140_bf.media_5[1],
	srom_ptr->srom21140_bf.media_5[2],
	srom_ptr->srom21140_bf.media_5[3]);

    printf("crc read        %02x%02x\n",
	srom_ptr->srom21140_bf.crc[0],
	srom_ptr->srom21140_bf.crc[1]);
/*
   for(i=0;i<16;i++)
   printf("ID BLOCK  <0:16>     :%02x\n",srom_ptr->array[i]);
   printf("SROM frmt <17>       :%02x\n",srom_ptr->array[17]);
   printf("Controler Cnt<18>    : <0:16>\n%02x\n",srom_ptr->array[18]);
   for(i=20;i<26;i++)
   printf("NA <20:26>           :%02x\n",srom_ptr->array[i]);

   printf("Controller %d <i:i+3>:%02x\n",srom_ptr->array[i]);
*/

}


void read_21140srom(struct TUPB *tupb, unsigned char *ni)
{
    int i, j, k,l,d,w;
    unsigned long datain, dinbit, raddr, addrb, chksum;

    chksum = 0;
    k = 0;
    l=0;

printf("read_srom\n");

	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	krn$_sleep (1);

        WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	krn$_sleep (1);

/* output the command */

     for (i=0,j=0;j<64;j++) /* j = word count */
         {
	  WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	  krn$_sleep (1);
	  WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	  krn$_sleep (1);
	  WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	  krn$_sleep (1);
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	  krn$_sleep (1);
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
          krn$_sleep (1);
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
          krn$_sleep (1);
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
          krn$_sleep (1);
          WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
          krn$_sleep (1);


/* output the word address */
          for (k=0; k<6; k++)
            {
             d= ((j>> (5-k)) & 1) << 2;
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801 | d);
	     krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803 | d);
	     krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801 | d);
	     krn$_sleep (1);
            }             
  
 /* read word of data */
          for (w=0,k=0; k<16; k++)
           {
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	     krn$_sleep (1);
             dinbit=RD_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR) & 0x08;
             w|= ((dinbit >>3) & 1) << (15-k);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	     krn$_sleep (1);
           }


           ni[i++] = w&0xff;
           ni[i++] = w>>8;


	   WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	   krn$_sleep (1);
         } /* end for i */

}


void uwire_srom_ewen(struct TUPB *tupb)
{
 int i;
/*printf("ewen \n");
*/	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	krn$_sleep (1);


        WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);

/* output the command */

	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);


	   for(i=0;i<6;i++) 
	   {
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
	   }

	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	krn$_sleep (1);

}

/* this is a write one word routine
   args
   1 - pointer to TUPB
   2 - word address (2 bytes)
   3 - word of data to be written
*/
void uwire_srom_wr(struct TUPB *tupb, unsigned int address, unsigned int data)
{
    unsigned int i, j, k=0,l=0,add;
    unsigned char dx;
    int loop;

printf("write srom word add %d with %04x\n",address,data);
    /*transpose the bytes */
    i = (data>>8)&0xff;
    j = (data<<8)&0xff00;
    data = i+j;

	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	krn$_sleep (1);


        WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	   krn$_sleep (1);
	WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
/* output the command */

	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02807);
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02805);
	   krn$_sleep (1);
  

	/* shift in the word address 6 bits , high order bits first */
          for (k=0; k<6; k++)
            {
             add= ((address>> (5-k)) & 1) << 2;
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,(0x02801 | add));
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,(0x02803 | add));
	   krn$_sleep (1);
	     WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,(0x02801 | add));
	   krn$_sleep (1);
            }             
 

            for(k=0;k<16;k++)
	     {
	      dx = ((data>>(15-k))&1)<<2;  /* get into bit position 2 */
 	      WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801|dx);
              krn$_sleep (1);
              /*krn$_sleep (1);*/
	      WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02803|dx);
              krn$_sleep (1);
              /*krn$_sleep (1);*/
	      WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801|dx);
              krn$_sleep (1);
              /*krn$_sleep (1);*/
              }


	   /* turn EEPROM off */
	   WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);
	   krn$_sleep (1);

	   krn$_sleep (1);

	   /* turn EEPROM back on */
	   WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02801);
	   krn$_sleep (1);

	   k=0;
	   do{
	     krn$_sleep (1);
	     }while((k++<16)&& ((RD_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR)&8)!=1));


	   WRT_TULIP_CSR_LONG(tupb,TU_CSR9_ADDR,0x02800);

}


/* crc stuff from appendix A */

#define DATA_LEN 126

unsigned short calc_crc(union srom21140_srom *SromData)
{
#define POLY 0x04C11DB6 /* 0x04C11DB6L */

 unsigned long crc=0xFFFFFFFF;
 unsigned long FlippedCRC = 0;

 unsigned char CurrentByte;
 unsigned int Index;
 unsigned char Bit;
 unsigned int Msb;
 int i;

 for(Index=0;Index<DATA_LEN;Index++)
 {
  CurrentByte=SromData->array[Index];
  for(Bit=0;Bit<8;Bit++)
  {
   Msb=(crc>>31)&1;
   crc<<=1;
   if(Msb^(CurrentByte&1))
   {
    crc^=POLY;
    crc|=0x00000001;
   }
   CurrentByte>>=1;
  }
 }

 for(i=0;i<32;i++)
 {
  FlippedCRC<<=1;
  Bit=crc&1;
  crc>>=1;
  FlippedCRC+=Bit;
 }


 crc=FlippedCRC^0xFFFFFFFF;
 i  = (crc<<8)&0xff00;
 i += (crc>>8)&0x00ff;

 return(i&0xFFFF);
}

/*
00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
00.00.01.01.00.00.f8.00.38.b5.00.41.00.44.45.35
30.30.2d.58.41.11.02.00.00.00.00.00.00.00.00.00
00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
00.00.08.0f.04.00.08.9e.00.03.09.ed.00.05.01.ed
00.04.00.9e.00.00.00.00.00.00.00.00.00.00.00.00
00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00
00.00.00.00.00.00.00.00.00.00.00.00.00.00.3f.66

*/                                                                              

