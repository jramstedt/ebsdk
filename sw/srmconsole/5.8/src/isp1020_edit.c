/*
 * Copyright (C) 1994 by
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
 * Abstract:	Edit ISP1020 NVRAM
 *
 * Author:	Stephen F. Shirron
 *
 * Modifications:
 *
 *	sfs	25-Oct-1994	Initial entry.
 *
 */

#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:prototypes.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:parse_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:pb_def.h"
#include "cp$src:isp1020_def.h"
#if STARTSHUT
#include "cp$src:mem_def.h"
#include "cp$inc:platform_io.h"
#define MAX_ADAPTER 100
extern struct window_head config;
#endif

extern struct SEMAPHORE pbs_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern struct nvram isp1020_default_nvram;

struct nvram isp1020_60mhz_default_nvram = {NVRAM_DEF_60};

/*+
 * ============================================================================
 * = isp1020_edit - Edit ISP1020 NVRAM parameters.                            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	The isp1020_edit command permits the modification of ISP1020 NVRAM
 *	parameters from the console.  Individual bits can be set or cleared,
 *	or bytes can be set to a specific value.  Entering isp1020_edit
 *	without an option or target device will list these parameters for
 *	all ISP1020 devices in the system.
 *  
 *   COMMAND FMT: isp1020_edit 2 Z 0 isp1020_edit
 *
 *   COMMAND FORM:
 *  
 *	isp1020_edit ( [-sd] [-offset] [-id] [-allids] [-value]
 *		       [-bit] [-set] -[clear] [<isp1020_controller_name>] )
 *
 *   COMMAND TAG: isp1020_edit 0 RXBZ isp1020_edit
 *
 *   COMMAND ARGUMENT(S):
 *
 * 	<isp1020_controller_name> - ISP1020 controller name.
 *		Only the parameters for this controller will be modified.
 *
 *   COMMAND OPTION(S):
 *
 *	-sd	- Set defaults
 *	-offset	- Selects the byte offset to be modified
 *	-id	- Selects a SCSI ID's parameters to be modified
 *	-allids	- Selects all SCSI ID's parameters to be modified
 *	-value	- Gives the new byte value(s), valid only with -offset
 *	-bit	- Selects the bit to be modified, valid only with -offset
 *		- -value and -bit are mutually exclusive
 *	-set	- The bit should be set, valid only with -bit
 *	-clear	- The bit should be clear, valid only with -bit
 *		- -set and -clear are mutually exclusive
 *
 *   COMMAND EXAMPLE(S):
 *~
 *	>>># Find all ISP1020 devices (firmware version 1.15)
 *	>>>sh dev pk
 *	pka0.7.0.1.0               PKA0                  SCSI Bus ID 7
 *	pkb0.7.0.1001.0            PKB0                  SCSI Bus ID 7
 *	pkc0.7.0.1002.0            PKC0                  SCSI Bus ID 7  1.15
 *	pkd0.7.0.1003.0            PKD0                  SCSI Bus ID 7  1.15
 *	pke0.7.0.7.0               PKE0                  SCSI Bus ID 7  1.15
 *	pkf0.7.0.4.1               PKF0                  SCSI Bus ID 7  G.2
 *	>>># List all current NVRAM parameters
 *	>>>isp1020_edit
 *	pkc0.7.0.1002.0
 *	old NVRAM
 *	00000000   49 53 50 20 02 7a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 dd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000030   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 6f
 *	pkd0.7.0.1003.0
 *	NVRAM is invalid, using defaults
 *	old NVRAM
 *	00000000   49 53 50 20 02 7a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 fd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000030   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 00
 *	pke0.7.0.7.0
 *	old NVRAM
 *	00000000   49 53 50 20 02 7a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 fd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 cd 10
 *	00000030   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 7f
 *	>>># Change the host SCSI ID from 7 to 6
 *	>>>isp1020_edit pkc -offset 5 -value 6a
 *	pkc0.7.0.1002.0
 *	old NVRAM
 *	00000000   49 53 50 20 02 7a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 dd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000030   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 6f
 *	new NVRAM
 *	00000000   49 53 50 20 02 6a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 dd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000030   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 7f
 *	>>># Modify four bytes of SCSI ID 3's parameters
 *	>>>isp1020_edit pkc -id 3 -offset 0 -value 11223344
 *	pkc0.7.0.1002.0
 *	old NVRAM
 *	00000000   49 53 50 20 02 6a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 dd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000030   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 7f
 *	new NVRAM
 *	00000000   49 53 50 20 02 6a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 dd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 11 22
 *	00000030   33 44 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 17
 *	>>># Set "stop queue on check" for all SCSI IDs
 *	>>>isp1020_edit pkc -allids -offset 0 -bit 1 -set
 *	pkc0.7.0.1002.0
 *	old NVRAM
 *	00000000   49 53 50 20 02 6a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 dd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 11 22
 *	00000030   33 44 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 17
 *	new NVRAM
 *	00000000   49 53 50 20 02 6a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 df 10 19 1c
 *	00000020   00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00 13 22
 *	00000030   33 44 00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00
 *	00000040   ff 10 19 1c 00 00 ff 10 19 1c 00 00 ff 10 19 1c
 *	00000050   00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00 ff 10
 *	00000060   19 1c 00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00
 *	00000070   ff 10 19 1c 00 00 ff 10 19 1c 00 00 00 00 00 f7
 *	>>># Set all defaults, then set target control flags for SCSI ID 0
 *	>>>isp1020_edit pkc -sd -id 0 -offset 0 -value dd
 *	pkc0.7.0.1002.0
 *	old NVRAM
 *	00000000   49 53 50 20 02 6a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 df 10 19 1c
 *	00000020   00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00 13 22
 *	00000030   33 44 00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00
 *	00000040   ff 10 19 1c 00 00 ff 10 19 1c 00 00 ff 10 19 1c
 *	00000050   00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00 ff 10
 *	00000060   19 1c 00 00 ff 10 19 1c 00 00 ff 10 19 1c 00 00
 *	00000070   ff 10 19 1c 00 00 ff 10 19 1c 00 00 00 00 00 f7
 *	new NVRAM
 *	00000000   49 53 50 20 02 7a 03 08 05 f6 08 00 fa 00 00 01
 *	00000010   00 00 00 00 00 00 00 00 00 00 00 00 dd 10 19 1c
 *	00000020   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000030   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000040   fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c
 *	00000050   00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00 fd 10
 *	00000060   19 1c 00 00 fd 10 19 1c 00 00 fd 10 19 1c 00 00
 *	00000070   fd 10 19 1c 00 00 fd 10 19 1c 00 00 00 00 00 6f
 *	>>>
 *~
 * FORM OF CALL:
 *  
 *	isp1020_edit (int argc, char *argv [])
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

#define QSD 0
#define QOFFSET 1
#define QID 2
#define QALLIDS 3
#define QVALUE 4
#define QBIT 5
#define QSET 6
#define QCLEAR 7

int isp1020_edit(int argc, char *argv[])
{
    unsigned char result[128];
    int i;
    int j;
    int k;
    int l;
    int m;
    int status;
    int strcmp_pb();
    struct QSTRUCT qual[8];
    struct pb *pb;
#if STARTSHUT
    int adapter_cnt = 0;
    struct device *p[MAX_ADAPTER];
    struct device *dev;
    int loadadr = 0;
    char device_name[MAX_NAME];
#endif

    status = qscan(&argc, argv, "-",
      "sd %xoffset %did allids %svalue %xbit set clear", qual);
    if (status != msg_success) {
	err_printf(status);
	return (status);
    }
    i = qual[QVALUE].present;
    j = qual[QBIT].present;
    k = i + j;
    l = qual[QSET].present + qual[QCLEAR].present;
    m = i + l;

    if ((k > 1) || (l > 1) || (m > 1) || (k && !qual[QOFFSET].present)) {
	err_printf(msg_qual_conflict);
	return (msg_qual_conflict);
    }

    if (qual[QVALUE].present) {
	status = common_convert(qual[QVALUE].value.string, 16, result, 128);
	if (status != msg_success)
	    return (status);
	j = (strlen(qual[QVALUE].value.string) + 1) / 2;
    } else
	j = 0;

    if (qual[QOFFSET].present) {
	i = qual[QOFFSET].value.integer;
	if (qual[QID].present || qual[QALLIDS].present) {
	    if ((i < 0x00) || (i > (sizeof(struct nvram_id) - 1))) {
		err_printf("offset (with id) must be between 00 and %02x\n",
		  sizeof(struct nvram_id) - 1);
		return (msg_failure);
	    }
	    if ((i + j) > sizeof(struct nvram_id)) {
		err_printf("value too large\n");
		return (msg_failure);
	    }
	} else {
	    if ((i < 0x00) || (i > (sizeof(struct nvram) - 1))) {
		err_printf("offset must be between 00 and %02x\n",
		  sizeof(struct nvram) - 1);
		return (msg_failure);
	    }
	    if ((i + j) > sizeof(struct nvram)) {
		err_printf("value too large\n");
		return (msg_failure);
	    }
	}
    }

    if (qual[QID].present) {
	i = qual[QID].value.integer;
	if ((i < 0) || (i > 15)) {
	    err_printf("id must be between 0 and 15\n");
	    return (msg_failure);
	}
    }

    if (qual[QBIT].present) {
	i = qual[QBIT].value.integer;
	if ((i < 0) || (i > 7)) {
	    err_printf("bit must be between 0 and 7\n");
	    return (msg_failure);
	}
    }

#if !STARTSHUT
    if (argc < 2) {
	argc = 2;
	argv[1] = "";
    }
    krn$_wait(&pbs_lock);
    shellsort(pbs, num_pbs, strcmp_pb);
    for (i = 1; i < argc; i++) {
	for (j = 0; j < num_pbs; j++) {
	    pb = pbs[j];
	    if ((strcmp(pb->device, "isp1020") == 0) &&
	      substr_nocase(argv[i], pb->name)) {
		isp1020_edit_sub(pb, qual, 0);
	    }
	}
    }
    krn$_post(&pbs_lock);
    return (msg_success);
#else
    adapter_cnt =
      find_all_dev(&config, ISP_1020, MAX_ADAPTER, p, FAD$K_ALLDEV);

    if (adapter_cnt) {

	for (i = 0; i < adapter_cnt; i++) {
	    if (dev = p[i]) {
		sprintf(&device_name[0], "%s%d", dev->tbl->mnemonic,
		  dev->unit);
		if (argc > 1) {
		    if (!(substr_nocase(argv[1], &device_name[0])))
			continue;
		}
		if (dev->tbl->setmode) {
		    dev->tbl->setmode(DDB$K_START, dev);
		    pb = dev->devdep.io_device.devspecific;
		    isp1020_edit_sub(pb, qual, device_name);
		    dev->tbl->setmode(DDB$K_STOP, dev);
		}
	    }
	}
	status = msg_success;
    } else {
	status = msg_success;
    }
    return (msg_success);
#endif
}

void isp1020_edit_sub(struct pb *pb, struct QSTRUCT qual[], char *device_name)
{
    unsigned char result[128];
    unsigned char *pc;
    unsigned char *value;
    int i;
    int j;
    int k;
    int dirty;
    int t;
    int offset;
    struct nvram *nvram;
    struct nvram_id *nvram_id;

    int clock_speed = isp1020_get_clock(pb);

#if !STARTSHUT
    printf("%s  %s %d\n", pb->name, pb->device, (clock_speed) ? 60 : 40);
#else
    printf("%s\n", device_name);
#endif

    nvram = malloc_noown(sizeof(*nvram));
    t = isp1020_get_nvram(pb, nvram);
    if (t != msg_success)
	printf("NVRAM is invalid, using defaults\n");
    printf("old NVRAM");
    isp1020_show_nvram(pb, nvram);
    dirty = 0;
    if (qual[QSD].present) {
	if (clock_speed & 0x20)
	    memcpy(nvram, &isp1020_60mhz_default_nvram, sizeof(*nvram));
	else
	    memcpy(nvram, &isp1020_default_nvram, sizeof(*nvram));

	dirty = 1;
    }
    if (qual[QOFFSET].present) {
	if (qual[QALLIDS].present) {
	    k = 16;
	    pc = &nvram->id[0];
	} else if (qual[QID].present) {
	    k = 1;
	    pc = &nvram->id[qual[QID].value.integer];
	} else {
	    k = 1;
	    pc = nvram;
	}
	for (j = 0; j < k; j++) {
	    offset = qual[QOFFSET].value.integer;
	    if (qual[QVALUE].present) {
		value = qual[QVALUE].value.string;
		common_convert(value, 16, result, 128);
		t = (strlen(value) + 1) / 2;
		for (i = 0; i < t; i++)
		    pc[offset + t - i - 1] = result[i];
		dirty = 1;
	    }
	    if (qual[QBIT].present) {
		i = 1 << qual[QBIT].value.integer;
		if (qual[QSET].present)
		    pc[offset] |= i;
		if (qual[QCLEAR].present)
		    pc[offset] &= ~i;
		dirty = 1;
	    }
	    pc += sizeof(struct nvram_id);
	}
    }
    if (dirty) {
	t = isp1020_put_nvram(pb, nvram);
	if (t != msg_success)
	    err_printf("NVRAM could not be updated\n");
	printf("new NVRAM");
	isp1020_show_nvram(pb, nvram);
    }
}

void isp1020_show_nvram(struct pb *pb, struct nvram *nvram)
{
    unsigned char *pc;
    int i;

    pc = nvram;
    for (i = 0; i < sizeof(*nvram) / 2; i++) {
	if ((i & 7) == 0)
	    printf("\n%08x  ", i * 2);
	printf(" %02x", *pc++);
	printf(" %02x", *pc++);
    }
    printf("\n");
}
