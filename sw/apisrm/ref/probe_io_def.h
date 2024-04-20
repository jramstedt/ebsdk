/*
 * Copyright (C) 1993 by
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
 * Abstract:	I/O Probing Definitions
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	11-Nov-1993	Initial entry.
 */

#define I2O_CLASS 0xE
#define BASE_CLASS( x ) ((( x >> 8 ) & 0xFF ))
 
#define TYPE_UNKNOWN 0
#define TYPE_LBUS 1
#define TYPE_FBUS 2
#define TYPE_XMI 3
#define TYPE_EISA 4
#define TYPE_ISA 5
#define TYPE_PCI 6
#define TYPE_VME 7
#define TYPE_TLSB 8

struct io_device {
    int type;
    unsigned int id;
    unsigned int id_ext;
    char *device;
    char *name;
    char *protocol;
    int *controller;
#if DRIVERSHUT
    char *overlay;
    int (*setmode) ();
    int (*config) ();
    int (*establish) ();
#endif
    } ;

struct io_bus {
    int type;
    int (*probe)();
    } ;

#define SIZE_GRAN_BASE_IO_16_BIT 0
#define SIZE_GRAN_BASE_IO 1
#define SIZE_GRAN_BASE_SPARSE 2
#define SIZE_GRAN_BASE_DENSE 3
#define SIZE_GRAN_BASE_NUM 4

#define PCI_FLAGS_EISA_BRIDGE 1
#define PCI_FLAGS_ISA_BRIDGE 2
#define PCI_FLAGS_PCI_BRIDGE 4
#define PCI_FLAGS_IO_16_BIT 8
#define PCI_FLAGS_VGA 16
#define PCI_FLAGS_ISA 32
#define PCI_FLAGS_IMPOSSIBLE 64
#define PCI_FLAGS_NOT_ASSIGNED 128
#define PCI_FLAGS_NOT_ASSIGNED_IO 256
#define PCI_FLAGS_NOT_ASSIGNED_MEM 512

struct pci_size_info {
    struct pb *pb;
    int flags;
    int index;
    unsigned int data;
    unsigned int size[SIZE_GRAN_BASE_NUM];
    unsigned int gran[SIZE_GRAN_BASE_NUM];
    unsigned int base[SIZE_GRAN_BASE_NUM];
    struct pci_size_info *parent_psi;
    struct pci_size_info *child_psi;
    struct pci_size_info *peer_psi;
    } ;

struct pci_bridge_misc {
    int flags;
    unsigned int base_int_pin;
    unsigned int pri_bus;
    unsigned int sec_bus;
    unsigned int sub_bus;
    unsigned int base[SIZE_GRAN_BASE_NUM];
    unsigned int size[SIZE_GRAN_BASE_NUM];
    } ;

struct pci_device_misc {
    int flags;
    unsigned int base_slot;
    unsigned int base[7];
    unsigned int size[7];
    struct pci_bridge_misc *pbm;
    } ;

struct io_device_name {
    unsigned int id;
    unsigned int id_ext;
    char *name;
    };
