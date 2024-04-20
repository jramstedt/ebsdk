/*                 
 * CHANGES:
 *
 *  MODIFICATION HISTORY:
 *
 * 17-Oct-1996 jlw  Backed out previous - not required
 *
 * 05-Sep-1996 jlw  Increased max_entries from 12 to 15. Also added new
 *                  qualifer -cend to clear expanded end of table
 *
 * 11-mar-1996  al  Made changes to allow for Burns.
 *                  
 * ??-???-1995  al  Added msg_found_conflict
 */

#ifndef ISACFG_DEF
#define ISACFG_DEF 1

#if MONET
#define NUM_EMBEDDED_ENTRIES 9 
#else
#define NUM_EMBEDDED_ENTRIES 6      
#endif

#if CORTEX || YUKONA     
#define NUM_EMBEDDED_ENTRIES 7     
#endif

#define ISACFG_MAJOR_VERSION  1
#define ISACFG_MINOR_VERSION  0
#define NUM_ISA_SLOTS ISA_MAX_SLOT   /*includes the on board slot 0*/

#define ENTRY_NOT_USED 0
#define MAX_ENTRIES 12
#define NVR_TABLE_BASE_PAGE 0xc  /* the 3K boundary */
#define NVR_NUM_PAGES 32
#define NVR_PAGE_SIZE 256

struct TABLE_HEADER{
    char identifier[8];
    char major_version[2];
    char minor_version[2];
    unsigned long platform_specific_num_isa_slots;
    unsigned long num_entries;
    unsigned long first_entry_offset;
    unsigned long table_checksum;
    unsigned long header_checksum;
}; /*end of struct table_header*/

struct TABLE_ENTRY {
    unsigned long entry_type;
    unsigned long reserved0;

    unsigned long isa_slot;     /* 0 = embedded, 1-3 option slots for mustang*/
    unsigned long reserved1;

    unsigned long dev_num;
    unsigned long reserved2;

    unsigned long total_devices;   
    unsigned long reserved3;

    uint64 io_base_addr0;
    uint64 io_base_addr1;
    uint64 io_base_addr2;
    uint64 io_base_addr3;
    uint64 io_base_addr4;
    uint64 io_base_addr5;

    uint64 mem0_base_addr;
    uint64 mem0_length;
    uint64 mem1_base_addr;
    uint64 mem1_length;
    uint64 mem2_base_addr;
    uint64 mem2_length;

    uint64 rom_base_addr;
    uint64 rom_length;

    unsigned long device_enable;
    unsigned long reserved4;

    unsigned long dma0;
    unsigned long dma1;
    unsigned long dma2;
    unsigned long dma3;

    unsigned long irq0_assignment;
    unsigned long irq1_assignment;
    unsigned long irq2_assignment;
    unsigned long irq3_assignment;

    char handle[16];
}; /*end of struct table_entry*/



struct table_struct {
        struct TABLE_HEADER header;
        struct TABLE_ENTRY  device[MAX_ENTRIES];
        };


union isa_config_table {
     struct table_struct table;
     unsigned char string[sizeof(struct table_struct)];
}; /*end of union isa_config_table*/



#define QSTRING "%xslot %xdev %xetyp %xtotdev %xiobase0 %xiobase1 %xiobase2 %xiobase3 %xiobase4 %xiobase5 %xmembase0 %xmemlen0 %xmembase1 %xmemlen1 %xmembase2 %xmemlen2 %xrombase %xromlen %xenadev %ddmachan0 %ddmachan1 %ddmachan2 %ddmachan3 %dirq0 %dirq1 %dirq2 %dirq3 %shandle mk rm mod all init"

#define QSLOT           0
#define QDEV            1
#define QETYP           2
#define QTOTDEV         3
#define QIOBASE0        4
#define QIOBASE1        5
#define QIOBASE2        6
#define QIOBASE3        7
#define QIOBASE4        8
#define QIOBASE5        9
#define QMEMBASE0       10
#define QMEMLEN0        11
#define QMEMBASE1       12
#define QMEMLEN1        13
#define QMEMBASE2       14
#define QMEMLEN2        15
#define QROMBASE        16
#define QROMLEN         17
#define QENADEV         18
#define QDMACHAN0       19
#define QDMACHAN1       20
#define QDMACHAN2       21
#define QDMACHAN3       22
#define QIRQ0           23
#define QIRQ1           24 
#define QIRQ2           25
#define QIRQ3           26
#define QHANDLE         27
#define QQUALMAX        28   /*# of field qualifiers up to here*/
#define QMK             28   /*add an entry */
#define QRM             29   /*delete an entry*/
#define QMOD            30
#define QALL            31
#define QINIT           32
#define QMAX            33

#define msg_success             0
#define msg_isa_noslot          1
#define msg_isa_exists          2
#define msg_isa_notfound        3
#define msg_isa_noetyp          4
#define msg_permdev_nomod       5
#define msg_permdev_nodel       6
#define msg_isa_tblful          7
#define msg_continue            8
#define msg_found_conflict      9
#define msg_isa_noname         10

#endif /*isacfg_def*/
