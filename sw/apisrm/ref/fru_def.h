/*
 * Copyright (C) 1992 by
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
 * Abstract:	FRU Table Definitions
 *
 * Author:	Paul LaRochelle
 *
 * Modifications:
 *
 *	pel	16-Nov-1992	initial entry
 */
 
#define FRR_IO		0
#define FRR_CPU1	1
#define FRR_CPU0	2
#define FRR_MEM0  	3
#define FRR_MEM1	4
#define FRR_MEM2	5
#define FRR_MEM3	6
#define FRR_FBUS1	7
#define FRR_FBUS2	8
#define FRR_FBUS3	9
#define FRR_FBUS4	10
#define FRR_FBUS5	11
#define FRR_FBUS6	12
#define FRR_PSC  	13
#define FRR_BUSA   	14
#define FRR_BUSB   	15
#define FRR_BUSC   	16
#define FRR_BUSD   	17
#define FRR_BUSE   	18
#define FRR_BUS_UNIT	19
#define FRR_CPU2        20
#define FRR_CPU3        21 
#define FRR_END    	'\xFF'

	/* Frame ID numbers */
#define FRAME_MIN		0
#define FRAME_IO		1
#define FRAME_CPU  		2
#define FRAME_MEM 		3
#define FRAME_FBUS  		4
#define FRAME_PSC		5 
#define FRAME_BUS   		6 
#define FRAME_SCSI1 		7 
#define FRAME_SCSI2 		8 
#define FRAME_DSSI  		9 
#define FRAME_END   		'\xFF'

	/* number of bytes in each frame */
#define MIN_FRAME_SZ 		8
#define CPU_MEM_IO_FRAME_SZ 	32
#define PSC_FRAME_SZ 		24
#define BUS_FRAME_SZ 		32
#define BUS_UNIT_MIN_SZ		32

#define SELF_TEST_PASS	'P'
#define SELF_TEST_FAIL	'F'
#define SELF_TEST_NUL	'\0'

    union FRU_FRAME_LWD0 {
	unsigned int		lwd;
	struct {
	    unsigned char	qwd_count;
	    unsigned char	self_test;
	    unsigned char	frr_bit;
	    unsigned char	frame_id;
	} bytes;
    } ;

    union FRU_FRAME_LWD1 {
	unsigned int	lwd;
	struct {
	    unsigned char	chip_rev;
	    unsigned char	fill1;
	    unsigned char	bus_id;
	    unsigned char	bus;
	} bytes;
    } ;

    struct FRU_FRAME_HDR1 {
	union FRU_FRAME_LWD0	lwd0;
	char			serial_num[12];
	char			partnum_rev[12];  /* partnum, sw, hw rev */
	unsigned int		mem_csr3;
    } ;

    struct FRU_FRAME_BUS     {	/*	Storage Bus Frame Definition */
	union FRU_FRAME_LWD0	lwd0;
	union FRU_FRAME_LWD1	lwd1;
	char			dev_name[12];
	unsigned char		n_scntl0;	    /* NCR chip registers */
	unsigned char		n_scntl1;	    
	unsigned char		n_ctest0;	    
	unsigned char		n_ctest7;	    
	unsigned char		n_dmode; 	    
	unsigned char		n_dwt;   	    
	unsigned char		n_dcntl; 	    
    } ;


    struct FRU_FRAME_BUS_UNIT	{	/*	Storage Bus Unit Frame Def */
	union FRU_FRAME_LWD0	lwd0;
	union FRU_FRAME_LWD1	lwd1;
	char			dev_name[20];
	char			inq[MAX_INQ_SZ];
    } ;
