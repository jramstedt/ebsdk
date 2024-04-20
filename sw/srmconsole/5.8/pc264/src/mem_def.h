/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:06 by OpenVMS SDL EV1-31     */
/* Source:  20-APR-1999 15:27:17 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]MEM_DEF.SDL; */
/********************************************************************************************************************************/
/* file:	mem_def.sdl                                                 */
/*                                                                          */
/* Copyright (C) 1990, 1991, 1992 by                                        */
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
/* Abstract:	Device data structure definitions for Alpha firmware.       */
/*                                                                          */
/* Author:	David Mayo                                                  */
/*                                                                          */
/* Modifications:                                                           */
/*                                                                          */
/*	dwr	23-Sep-1993	Add new cpu struct to devdep union          */
/*                                                                          */
/*	kcq	24-May-1993	Added Python driver support.                */
/*                                                                          */
/*	dtm	 2-Feb-1992	Add pointer to INODE in device structure.   */
/*                                                                          */
/*	wcc	29-Oct-1992	Added comment to io window count. BUG 571.  */
/*				                                            */
/*	djm	26-Apr-1992	Added FAD constants used in find_all_dev    */
/*				options.                                    */
/*                                                                          */
/*	jrk	18-Mar-1992	add controller to io_device, add            */
/*				establish to device_list.                   */
/*                                                                          */
/*	jrk	12-Mar-1992	correct "device" structure, it had filler   */
/*				of 456 ints at end.                         */
/*                                                                          */
/*	jds	12-Feb-1992	Added setmode/config to device_list.        */
/*				Added state/devspecific to device.devdep.io_device. */
/*                                                                          */
/*	djm	 1-May-1991	Changed the "device" structure.             */
/*                                                                          */
/*	dtm	11-Dec-1990	System configuration data structures added  */
/*                                                                          */
/*	dtm	30-Oct-1990	Initial entry.                              */
/*                                                                          */
/*** MODULE $memdef ***/
/*                                                                          */
/* Options used by find_all_dev (FAD$)                                      */
/* The default options are for DEVICES and everything whether it            */
/* passed selftest or not.                                                  */
/*                                                                          */
#define FAD$K_ALLDEV 0
#define FAD$M_WANT_WINDOWS 1
#define FAD$M_PASSED_ST 2
#define FAD$M_FAILED_ST 4
#define FAD$M_CLASS 8
#define PCI_MAX_SPACE 3
#define PCI_MAX_SPACE_RANGE 3
/*                                                                          */
/* Structure of a list of device types                                      */
/*                                                                          */
struct device_list {
    int type;                           /* Bus type                         */
    int dtype;                          /* Device Type register             */
    int dtype_ext;                      /* Device Type extention register   */
    char *name;                         /* name of device                   */
    char *mnemonic;                     /* generic mnemonic for device      */
    char *overlay;                      /* overlay name pointer (if any)    */
    char *protocol;                     /* Device Protocol                  */
    int class;                          /* device type class                */
    int frutype;                        /* fru type class                   */
    int max_controller;                 /* Number of controllers            */
    int *count;                         /* number of devices found          */
    int (*assign)();                    /* assign routine pointer (if any)  */
    int (*setmode)();                   /* setmode routine pointer (if any) */
    int (*config)();                    /* config routine pointer (if any)  */
    int (*establish)();                 /* establish routine pointer (if any) */
    } ;
/*                                                                          */
/* List of device types                                                     */
/*                                                                          */
#define DEVTYPE$K_EMPTY 0
#define DEVTYPE$K_IO_DEVICE 1
#define DEVTYPE$K_IO_NETWORK 2
#define DEVTYPE$K_IO_DISK 3
#define DEVTYPE$K_IO_SPECIAL 4
#define DEVTYPE$K_MEMORY 5
#define DEVTYPE$K_CPU_EV3 6
#define DEVTYPE$K_CPU_EV4 7
#define DEVTYPE$K_CPU_VAX 8
#define DEVTYPE$K_IO_WINDOW 9
/*                                                                          */
/* List of device classes                                                   */
/*                                                                          */
#define DEVCLASS$K_EMPTY 0
#define DEVCLASS$K_DISK 1
#define DEVCLASS$K_TAPE 2
#define DEVCLASS$K_DUP 3
/*                                                                          */
/* Structure of a hw configuration window header                            */
/*                                                                          */
struct window_head {
    char name [52];                     /* name of configuration window     */
    int count;                          /* number of window descriptors,    */
/* starting with 0 (14 slot xmi count would be 15; xmi has no slot 0).      */
/*                                                                          */
    int size;                           /* size of window descriptor        */
    struct device (*(*descr));          /* pointer window descriptors       */
    } ;
struct base_address_node {
    struct base_address_node *flink;    /* flink                            */
    struct base_address_node *blink;    /* blink                            */
    struct device *dev;                 /* pointer to device                */
    int reg;                            /* base address reg number          */
    unsigned int size;                  /* size of space                    */
    unsigned int base_address;          /* base address                     */
    int info;                           /* base address information bits    */
    struct base_address_node *ba;       /* pointer to list of base address node for device */
    } ;
struct base_address_range {
    int range_num;                      /* range number		            */
    unsigned int base [3];              /* base                             */
    unsigned int limit [3];             /* limit                            */
    } ;
struct base_address_head {
    struct base_address_node baq [3];
    int prim_bus;                       /* primary bus number               */
    int sec_bus;                        /* secondary bus number             */
    int sub_bus;                        /* subordinate bus number           */
    struct base_address_range range [3]; /* range                           */
    } ;
struct dmem {
    int type;                           /* type                             */
    unsigned int size [2];              /* size                             */
    unsigned int base [2];              /* base address                     */
    } ;
struct daughter_memory {
    struct dmem dm [8];
    int info;
    } ;
struct part_info_ {
    char serial_num [16];
    char part_num [16];
    } ;
struct part_info {
    struct part_info_ info [2];
    } ;
/*                                                                          */
/* Structure of a device descriptor                                         */
/*                                                                          */
struct device {
    int flags;                          /* flags for this module            */
    struct device_list *tbl;            /* table of attributes              */
    int dtype;                          /* raw contents of dev:dtype        */
    int dtype_ext;                      /* contents of dtype extention register */
    int class;                          /* device class                     */
    int rev;                            /* raw contents of dev:rev          */
    struct INODE *ip;                   /* pointer to device inode          */
    int unit;                           /* unit# of the adapter             */
    int hose;                           /* hose                             */
    int slot;                           /* slot                             */
    int gid;                            /* global id                        */
    int channel;                        /* channel                          */
    int node;                           /* node                             */
    int bus;                            /* bus                              */
    int function;                       /* function                         */
    struct base_address_node *ba;       /* pointer to list of base addresses */
    struct device *bridge;              /* pointer back to bridge           */
    struct part_info *part;             /* pointer to part information      */
    union  {
        struct  {
            int proccnt;                /* # of processors                  */
            int csize;                  /* Cache size                       */
            } cpu;
        struct  {
            int size;                   /* memory capacity                  */
            int strings;                /* number of memory strings installed */
            int base;                   /* beginning memory address         */
            int intlv;                  /* number of ways interleaved       */
            int position;               /* position in interleave set       */
            int set;                    /* interleave set number            */
            struct daughter_memory *dp; /* pointer to daughter cards 		 */
            } mem;
        struct  {
            int vector [4];
            struct window_head *window; /* pointer to next config structure */
            struct base_address_head *bah; /* pointer to base_address header */
            } io_window;
        struct  {
            int vector [4];
            int state;                  /* device state                     */
            int *devspecific;           /* Pointer to device-specific information */
            int *devspecific2;          /* Pointer to device-specific information */
            int controller;             /* controller base character        */
            int int_pin;                /* interrupt pin                    */
            } io_device;
        } devdep;
    } ;
/*                                                                          */
/* Structure of a memory interleave tree node                               */
/*                                                                          */
struct mtree {
    char label;                         /* single character label for node  */
    struct mtree *left;                 /* pointer to left child            */
    struct mtree *right;                /* pointer to right child           */
    struct mtree *parent;               /* pointer to parent                */
    } ;
 
