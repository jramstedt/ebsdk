/* file:	esc_nvram_driver.c   - the eerom driver
 *
 * Copyright (C) 1992, 1993 by
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
 */

/*
 *++
 *  FACILITY:
 *
 *      Sable Exerciser
 *
 *  MODULE DESCRIPTION:
 *
 *	Driver for ECS (Intel 82374EB) and SIO (Intel 823781B) eerom
 *
 *  AUTHORS:
 *
 *	Judith Gold
 *
 *  CREATION DATE:
 *  
 *	13-Jul-1993
 *
 *
 *  MODIFICATION HISTORY:
 *
 *	mr	16-Feb-1995	Add spinlocks to read_eerom(), write_eerom()
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	28-Sep-1994	Conditionalize for EB64+
 *				The page register on EB64+ is latched so
 *				it is basically a write-only register.
 *
 *	dtr	27_jan-1994	inport/outport change to esia_X
 *
 *	jeg	22-Oct-1993	Modify eerom_write to write only when data is
 *				changed.  Also, writes are now verified.
 *
 *	pel	25-Aug-1993	Modify to work w/ SIO chip.  Use new NVRAM base,
 *				page constants instead of esc_def.h constants.
 *				Also added eerom_clear to write zero's to
 *				NVRAM. eerom_clear is unconditionally excluded.
 *--
 */

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include "cp$src:alpha_arc.h"
#include "cp$src:alpha_arc_eisa.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:nvram_def.h"
#include	"cp$src:pb_def.h"
#include	"cp$src:mem_def.h"
#include "cp$inc:prototypes.h"
#include	"cp$inc:kernel_entry.h"
 
#if TURBO
extern struct window_head config;
#define ISP_DEBUG 0
#endif

#define MAX_WRITE 1

#if SABLE || RAWHIDE
#define MAX_WRITE 16
#endif

#if WILDFIRE || WFSTDIO
#define MAX_WRITE 1
#endif

extern int null_procedure ();

extern struct LOCK spl_atomic;

#if WILDFIRE || WFSTDIO
extern struct pb stdio_pb;
#define esc_nvram_pb &stdio_pb
#else
#define esc_nvram_pb 0
#endif

int eerom_read ();
int eerom_write ();
int eerom_open ();
int eerom_close ();

struct DDB eerom_ddb = {
#if WFSTDIO
	"wfeerom",		/* how this routine wants to be called	*/
#else
	"eerom",		/* how this routine wants to be called	*/
#endif
	eerom_read,		/* read routine				*/
	eerom_write,		/* write routine			*/
	eerom_open,		/* open routine				*/
	eerom_close,		/* close routine			*/
	null_procedure,		/* name expansion routine		*/
	null_procedure,		/* delete routine			*/
	null_procedure,		/* create routine			*/
	null_procedure,		/* setmode				*/
	null_procedure,		/* validation routine			*/
	0,			/* class specific use			*/
	1,			/* allows information			*/
	0,			/* must be stacked			*/
	0,			/* is a flash update driver		*/
	0,			/* is a block device			*/
	0,			/* not seekable				*/
	0,			/* is an ethernet device		*/
	0,			/* is a filesystem driver		*/
};

#if AVANTI

#define  BankSet8  0x80000B00
#define  BankValid 1

int irl (unsigned int p)
{

    __int64 	jensen_address = 0x10000000;

    jensen_address <<= 4;
    jensen_address += (__int64)p;

    return(*((int *)jensen_address));

}

void iwl (unsigned int p, unsigned int d)
{

    __int64 	jensen_address = 0x10000000;

    jensen_address <<= 4;
    jensen_address += (__int64)p;

    *((int *)jensen_address) = d;
    mb();
}
#endif

int read_eerom (int page, int offset, struct pb *pb) {
#if AVANTI
volatile int NvramOffset,discard,data;

	/* Avanti need to enable bank set 8	*/
	iwl (BankSet8, (irl (BankSet8) | BankValid));

	/* lets make sure That it gets there    */
	discard = irl (BankSet8);

	/* Load address port	*/
	NvramOffset = (page*256) + offset;
	iwl (0 , ( (0x100000|NvramOffset)<<8) ); 

	discard = irl (0);

	/* Read Data Port	*/
	data = irl ( 0x100000 );

	/* Avanti need to disable bank set 8	*/
	iwl (BankSet8, (irl (BankSet8) & (~BankValid)));
	return (data & 0xff );
#else
#if WILDFIRE || WFSTDIO
	unsigned char readdata;

	readdata = inmemb (pb, 0x1c0000 + page*256 + offset);
#else
	unsigned char readdata;
	unsigned char reg;
	spinlock(&spl_atomic);

	reg = inportb (pb, (uint64)(NVRAM_PAGE_REG));
	reg &= 0xe0;	/* mask out page bits */
	page |= reg;    /* but keep reserved bits */
	outportb (pb, (uint64)(NVRAM_PAGE_REG),page);

        readdata = inportb (pb, (uint64)(NVRAM_BASE + offset));

	spinunlock(&spl_atomic);
#endif

	return readdata;
#endif
}

void write_eerom (int page, int offset, unsigned char *data, int length, struct pb *pb) {

#if AVANTI
volatile int NvramOffset,discard,temp_data;

	/* Avanti need to enable bank set 8	*/
	iwl (BankSet8, (irl (BankSet8) | BankValid));

	/* lets make sure That it gets there    */
	discard = irl (BankSet8);

	/* Load address port			*/
	/* Setup for Write			*/
	/* 1.0000.0000 = 0x8000.0000+data	*/

	/* Compute offset			*/
	
	NvramOffset = (page*256) + offset;
	iwl (0 , (0x80000000|((0x100000|NvramOffset)<<8)) ); 

	discard = irl (0);

	/* Write Data Port	*/
	iwl ( 0x100000 , *data );

	discard = irl ( 0x100000 );

	/* Avanti need to enable bank set 8	*/
	iwl (BankSet8, (irl (BankSet8) & (~BankValid)));

#else
#if WILDFIRE || WFSTDIO
	int i;

	for (i=0; i<length; i++)
	    outmemb (pb, 0x1c0000 + page*256 + offset + i, data[i]);
#else
	unsigned char reg;
	int i;

	spinlock(&spl_atomic);

	reg = inportb (pb, (uint64)(NVRAM_PAGE_REG));
	reg &= 0xe0;	/* mask out page bits */
	page |= reg;    /* but keep reserved bits */
	outportb (pb, (uint64)(NVRAM_PAGE_REG),page);

	for (i=0; i<length; i++)
	    outportb (pb, (uint64)(NVRAM_BASE + offset + i),data[i]);

	spinunlock(&spl_atomic);
#endif
#endif
}


int esc_nvram_init ();
int eerom_open (struct FILE *fp, char *info, char *next, char *mode);
int eerom_close (struct FILE *fp);
int eerom_read (struct FILE *fp, int size, int number, unsigned char *buf);
int eerom_write (struct FILE *fp, int size, int number, unsigned char *buf);


/*
 * Initialize the driver and the inode 
 */
int esc_nvram_init () {
	struct INODE 	*ip;
	struct pb *pb;
#if TURBO
	struct device *eisa;

        pb = dyn$_malloc (sizeof (struct pb), DYN$K_SYNC | DYN$K_NOOWN);
        eisa = find_dev ("eisa0", &config);
        pb->hose = eisa->hose;
        pb->slot = eisa->slot;
        pb->bus = eisa->bus;
        pb->function = eisa->function;
#else
	pb = esc_nvram_pb;
#endif

	allocinode (eerom_ddb.name, 1, &ip);
	ip->dva = & eerom_ddb;
	ip->attr = ATTR$M_WRITE | ATTR$M_READ;
	ip->len[0] = EEROM_LENGTH;
	ip->misc = pb;
	INODE_UNLOCK (ip);

	return msg_success;
}


int eerom_open (struct FILE *fp, char *info, char *next, char *mode) {

	/*
	 * Treat the information field as a byte offset
	 */
	fp->offset = &fp->local_offset;
	*fp->offset = xtoi (info);

#if TURBO
	if (platform() == ISP_MODEL)
	    isp_eerom_open(3);		/* open eisa eerom */
#endif
	return msg_success;
}

int eerom_close (struct FILE *fp) {

#if TURBO
    if (platform() == ISP_MODEL)
	    isp_eerom_open(4);		/* close eisa eerom */
#endif
    return msg_success;
}

int eerom_read (struct FILE *fp, int size, int number, unsigned char *buf) {
	int i;
	int length;
	struct INODE *ip;
	struct pb *pb;
	unsigned char page;
	unsigned char offset;

	ip = fp->ip;
	pb = ip->misc;
	length = min (size * number, *ip->len - *fp->offset);

#if TURBO
	if (platform() == ISP_MODEL)
	    isp_eerom_read(length, *fp->offset, buf);	/* read eisa eerom */
	else
#endif
	for (i=0; i<length; i++) {

	    page = *fp->offset >> 8;	    
	    offset = *fp->offset & 0xff;

	    *buf++ = read_eerom (page, offset, pb);
	    *fp->offset += 1;
	}

	return length;
}

int eerom_write (struct FILE *fp, int size, int number, unsigned char *buf) {
	int i,j,k,l;
	int length;
	struct INODE *ip;
	struct pb *pb;
	unsigned char page;
	unsigned char offset;
	unsigned char temp;

	ip = fp->ip;
	pb = ip->misc;
	length = min (size * number, *ip->len - *fp->offset);

#if TURBO
	if (platform() == ISP_MODEL)
	    isp_eerom_write(length, *fp->offset, buf);	/* write eisa eerom */
	else
#endif
	for (i=0; i<length; i += l) {

	    page = *fp->offset >> 8;	    
	    offset = *fp->offset & 0xff;

	    l = min (MAX_WRITE - (offset & (MAX_WRITE-1)), length - i);

	    k = 0;
	    for (j=0; j<l; j++) {
		temp = read_eerom (page, offset+j, pb);
		if (temp != buf[j])
		    k++;
	    }

	    /* only write if data needs to change */
	    if (k) {
		write_eerom (page, offset, buf, l, pb);
		/* it may take time for the write */
		for(j=0; j<1000; j++) {
		    temp = read_eerom (page, offset+l-1, pb);
		    if(temp == buf[l-1])
			break;
		    krn$_micro_delay (10);
		}
            }

	    *fp->offset += l;
	    buf += l;
	}

	return length;
}

#if TURBO
void isp_eerom_open(int code)
{
    union {
	int busadr[2];
	int64 *busadr64;
    } ba;

	ba.busadr[0] = 0xdffffff0;
	ba.busadr[1] = 0xfff;
	*((int *)ba.busadr64) = code;		/* write open/close code to eerom */
}

int isp_eerom_read(int len, int *fp_offset, char *dst)
{
    int i;
    int offset;
    int dest = dst;
    union {
	int busadr[2];
	int64 *busadr64;
    } ba;
    union {
	int data[2];
	int64 data64;
    } dat;

    offset = *fp_offset;
    ba.busadr[0] = 0xd0200000 + offset;
    ba.busadr[1] = 0xfff;

#if ISP_DEBUG
    pprintf("Reading eisa rom, %d bytes from %x to %x\n", len, offset, dst);
#endif

    dat.data[0] = *ba.busadr64;
    ba.busadr[0] += 4;
    dat.data[1] = *ba.busadr64;
    ba.busadr[0] -= 4;
#if ISP_DEBUG
    pprintf("First data = %08x %08x\n", dat.data[0], dat.data[1]);
#endif

    for (i = 0; i < len/8; i++) {
	dat.data[0] = *ba.busadr64;
	ba.busadr[0] += 4;
	dat.data[1] = *ba.busadr64;
	ba.busadr[0] += 4;
	*((int64 *)dest) = dat.data64;
	dest += 8;
   }

    *fp_offset += len;
#if ISP_DEBUG
    pprintf("eisa rom read complete\n");
#endif
    return 0;
}

int isp_eerom_write(int len, int *fp_offset, char *src)
{
    int i;
    int offset;
    u_int source = src;
    union {
	int busadr[2];
	int64 *busadr64;
    } ba;
    union {
	u_int data[2];
	u_int64 data64;
    } dat;

    offset = *fp_offset;
    ba.busadr[0] = 0xd0200000 + offset;
    ba.busadr[1] = 0xfff;

#if ISP_DEBUG
    pprintf("Writing eisa rom, %d bytes from %x to %x\n", len, src, offset);
#endif

    dat.data64 = *((int64 *)source);
#if ISP_DEBUG
    pprintf("First data = %08x %08x\n", dat.data[0], dat.data[1]);
#endif

    for (i = 0; i < len/8; i++) {

	dat.data64 = *((u_int64 *)source);	/* get 64 bits from source */

	ba.busadr[0] = offset + 0xd0000000;
	ba.busadr[1] = 0xfff;
	*((int *)ba.busadr64) = dat.data[0];		/* write 32 bits to eerom */

	ba.busadr[0] = offset + 0xd0000000 + 4;
	ba.busadr[1] = 0xfff;
	*((int *)ba.busadr64) = dat.data[1];		/* write 32 bits to eerom */

	offset += 8;
	source += 8;
   }
    *fp_offset += len;
#if ISP_DEBUG
    pprintf("isa rom write complete\n");
#endif
    return 0;
}
#endif

