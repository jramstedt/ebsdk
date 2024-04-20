/* file:        srom_def.h
 *
 * Copyright (C) 1990 by
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
 *      Alpha Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      The srom_def.h module contains the data Structures used by the 
 *      srom Console routines
 *
 *  AUTHORS:
 *
 *      David Baird
 *
 *  CREATION DATE:
 *  
 *      17-Jun-1991
 *
 *  MODIFICATION HISTORY:
 *
 *      db      24-Oct-1991     Updated to enhance the Serial ROM based
 *                              Mini-Console
 *
 *      db      05-Aug-1991     Updated to support the Serial ROM-based
 *                              Mini-Console
 *
 *      ini     17-Jun-1991     David Baird
 *      
 *--
 */


#ifndef srom_h

#define srom_h 0        /* Prevent multiple compiles of this module */

#define SR_DATA_SIZE 400
#define SR_DUMP_SIZE 40
#define SROM_READ_BUF_SIZE 0x200000

/*  Define SROM Command Numbers  */

#define SROM_DEPOSIT_CMD 0x0
#define SROM_DUMP_CMD    0x1
#define SROM_EXAMINE_CMD 0x2
#define SROM_FLOOD_CMD   0x3
#define SROM_LOAD_CMD    0x4
#define SROM_GO_CMD      0x5
#define SROM_INIT_CMD    0x6
#define SROM_READ_CMD    0x7
#define SROM_WRITE_CMD   0x8
#define SROM_EXIT_CMD    0x9
#define SROM_TEST_CMD    0xA
#define SROM_FLOAD_CMD   0xB

/* Define Data Width's */
#define SR_TYPE_L 4
#define SR_TYPE_Q 8
#define SR_TYPE_O 16
#define SR_TYPE_H 32

/* Define Data Types  */
#define SR_TYPE_M 0
#define SR_TYPE_R 1
#define SR_TYPE_F 2
#define SR_TYPE_I 3
#define SR_TYPE_C 4

/* define Data Address Spaces */
#define SR_S_ADDR	0x03
#define SR_SG_ADDR	0x83
#define SR_C_ADDR	0x02
#define SR_CG_ADDR	0x82
#define SR_A_ADDR	0x01
#define SR_AG_ADDR	0x81

/* Sable defines */
#define SR_CPU0_OFFSET 0x80000000
#define SR_CPU1_OFFSET 0x81000000
#define SR_CPU2_OFFSET 0x82000000
#define SR_CPU3_OFFSET 0x83000000
#define SR_I0_OFFSET 0x8E000000
#define SR_I1_OFFSET 0x8F000000
#define SR_MEM0_OFFSET 0x88000000
#define SR_MEM1_OFFSET 0x89000000
#define SR_MEM2_OFFSET 0x8A000000
#define SR_MEM3_OFFSET 0x8B000000

/* Cobra/Fang defines */
#define SR_C1_OFFSET 0x0
#define SR_C2_OFFSET 0x08000000
#define SR_IO_OFFSET 0x10000000
#define SR_M0_OFFSET 0x40000000
#define SR_M1_OFFSET 0x50000000
#define SR_M2_OFFSET 0x60000000
#define SR_M3_OFFSET 0x70000000

struct SR_PACKET_HEAD {
        int              length;
        unsigned char    cmd;
        unsigned char    data_type;
        unsigned char    data_width;
        long int         count;
        long int         address_h;
        long int         address;
};

struct SR_PACKET {
        struct SR_PACKET_HEAD header_pkt;
        long int   data_pkt[SR_DATA_SIZE];
};

union SR_UNION {
        struct          SR_PACKET detail;
        unsigned char   bytes[sizeof(struct SR_PACKET)];
};

#define SROM_EXAMINE 2
#define SROM_DUMP    1
#define SROM_ERROR   0x0F
#define SROM_COUNT   0x0E
#define SROM_UART    0x1E
#define SROM_VER     0xE4
#define SROM_PRIMARY 0xE6

struct SD_PACKET_HEAD {
        unsigned char   length;
        unsigned char   cmd;
        unsigned char   data_type;
        unsigned char   data_width;
};

struct SD_PACKET {
        struct SD_PACKET_HEAD header_pkt;
        long int  data_pkt[SR_DATA_SIZE+2];
};

union SD_UNION {
        struct          SD_PACKET detail;
        unsigned char   bytes[sizeof(struct SD_PACKET)];
};

#endif

