/*
* file:        lfu.c
*
* Copyright (C) 1990, 1991 by
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
* Abstract:    This program was originally only supported by a the laser 
*              console.  LFU is now a utility which is used to update
*              firmware on devices on different common console platforms.
*
*
* Author:      Denise Bolduc
*
* Modifications:
*
*       wcc        7-Dec-1993  major rewrite for common console use.
*
*       Mike Renda 25-Nov-1997  For noninteractive update - force
*				only checking to update SRM and 
*				ARC/AlphaBIOS. 
*
*       Mike Renda 15-May-1997	Add a means to update ARC/SRM console
*				without user intervention. This is a 
*				Digital Unix group and an ADP request.
*				A non-interactive update is done if 
*				boot_osflags contains a "LFU" prefix.
*				The console removes the prefix then updates 
*				console firmware only if the firmware image
*				are not corrupted.
*
*       Mike Renda 30-Oct-1997	Extend the "non-interactive update" method
*				to invoke a custom script after console 
*				firmware is updated and before resetting
*				the system. The caveat is that there is 
*				no guarantee that the invoked script 
*				exececuted without error. This will be used by
*				ADP on their custom DUNix CD where the script
*				called SETUP will reside in a directory called
*				CUSTOM.
*
*/


#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:parser.h"
#include "cp$src:mem_def.h"
#include "cp$src:common.h"
#include "cp$inc:prototypes.h"
#include "cp$src:msg_def.h"
#include "cp$src:ctype.h"
#include "cp$src:parse_def.h"
#include "cp$src:ub_def.h"
#include "cp$src:ev_def.h"

#undef MAX_ARGS
#define MAX_ARGS 80
#define MAX_NAMES 100


#define QPATH           0
#define QMAX            1
#define QSTRING "%spath"

#define FW_SIZE (512 * 1024)

enum commands {
    TBD, MOD_ATTR, VERIFY, UPDATE, LIST_SPT, 
    DISPLAY, HELP, EXIT, README, INVALID, FWUPD_VIA_OS
};

#define FAT_FILETYPE   1
#define ODS2_FILETYPE  2
#define BOOT_FILETYPE  4

#define MAX_COMMANDS INVALID

#define INT int
#define UINT unsigned int
#define CHAR char
#define UCHAR unsigned char
#define LONG long
#define ULONG unsigned long
#define SHORT short
#define USHORT unsigned short
#define PUCHAR unsigned char *

#define magic_header 'HF01'

#define _32K 0x8000
#define _16K 0x4000
#define _4K 0x1000

/* window size--must be a power of two, and at least 32K */

#define WSIZE		0x8000

#define MAX_BITS	13		/* used in unShrink() */
#define HSIZE		(1 << MAX_BITS)	/* size of global work area */
#define BMAX		16		/* maximum bit length of any code */
#define N_MAX		288		/* maximum num of codes in any set */
#define LBITS		9		/* bits in base lit/len lookup table */
#define DBITS		6		/* bits in base dist lookup table */

#define HF_BUF_SIZE	((_32K*4)+(N_MAX*4)+(288*4)+((286+30)*4)+(10*1024))
#define DISPATCH(x) ((x))

#define slide area->Slide

typedef union _work {
    struct {
	SHORT Prefix_of[HSIZE + 2];	/* (8194 * sizeof(short)) */
	UCHAR Suffix_of[HSIZE + 2];	/* also s-f length_nodes (smaller) */
	UCHAR Stack[HSIZE + 2];		/* also s-f distance_nodes (smaller) */
    } shrink;
    UCHAR Slide[WSIZE];
} work;

typedef struct huft {
    UCHAR e;				/* number of extra bits or operation */
    UCHAR b;				/* num of bits in this code/subcode */
    UCHAR pad1;
    UCHAR pad2;
    union {
	USHORT n;			/* lit, len base, or dist base */
	struct huft *t;			/* pointer to next level of table */
    } v;
};

/* Failure codes */

#define msg_success	0		/* success */
#define msg_failure	1		/* failure */
#define msg_iec		2		/* incorrect end code */
#define msg_ics		3		/* incorrect checksum */
#define msg_imh		4		/* incorrect magic header */
#define msg_tmb		5		/* too many bits */


/* global functions */
unsigned __int64 lfu_get_rev(char *fw_rev);
void lfu_get_options_firmware(char *options_fw_name);
int lfu(int pargc, char *pargv[]);

/* external functions */
extern void null_procedure();
extern void lfu_system_reset(int);
extern void lfu_display_config(void);
extern void lfu_system_init(int);
extern strcmp_ub(struct ub *ub1, struct ub *ub2);

/* external variables */
extern struct ZONE *memzone;
extern int rd_ddb;
extern struct SEMAPHORE ubs_lock;
extern struct ub **ubs;
extern unsigned int num_ubs;

/* local functions */
static void display_table(int argc, char *argv[]);
static void list_support(void);
static int selection(char argv[]);
static void parse_command_input(int *argc, char *argv[]);
static void lfu_print_device_name(char *name);
static void lfu_strstrip(char s1[]);
static int lfu_name_cb(char *name, int p0, int p1);
static int lfu_listfw_cb(char *name, int p0, int p1);
static void lfu_verify(char *name, char *alt_fname);
static void lfu_update(char *name, char *alt_fname);
static int lfu_get_firmware(char **base, struct FILE *fp);
static void update_flag(char *name, int state);
static int check_update_flag(void);
static void report_update_flag(void);
static void go_close_firmware(struct FILE *fp);
struct FILE *go_open_firmware(char *filename);
static void lfu_get_options_firmware_helper(struct FILE *fin, char *file_out, 
  int fwsize);
static int lfu_get_firmware_helper(struct FILE *fin, char **base, int fwsize);
static lfu_call_shell_cmd(char *command_string);
static int lfu_check_fw_device(char *device);
static void inflate_initialize();
static void flush();
static u_char *i_malloc();
static void ReadByte();
static int hf$_decompress(u_int from_adr, u_int buf_adr, u_int load_adr, 
 char *fetch_rtn);
static u_char *hf$_mem_fetch();

/* Flash names and count */
static char lfu_flash_name[MAX_NAMES] [MAX_NAME];
static int lfu_flash_cnt;


/* lfu messages */
static char *msg_lfu_banner =
  {"\n\n                ***** Loadable Firmware Update Utility *****\n"};
static char *msg_lfu_no_support = {"not supported under LFU.\n"};
static char *msg_lfu_prompt = {"UPD> "};
static char *msg_lfu_prompt_y_n = {"[Y/(N)]"};
static char *msg_lfu_prompt_continue_y_n = {"Continue [Y/(N)]"};
static char *msg_lfu_verify = {" Verifying %s... "};
static char *msg_lfu_verify_bad = {" FAILED."};
static char *msg_lfu_verify_good = {" PASSED."};
static char *msg_lfu_update = {" Updating to %s... "};
static char *msg_lfu_update_older_rev =
  {" update rev %s is less than current rev %s.\n"};
static char *msg_lfu_update_failed = {" FAILED."};
static char *msg_lfu_no_firm = {" no firmware file given\n"};
static char *msg_lfu_firm_opn_err =
  {" open failure firmware filename '%s' \n"};
static char *msg_lfu_firm_bad = {" firmware filename '%s' is bad"};
static char *msg_lfu_update_warn1 =
  {"WARNING: updates may take several minutes to complete for each device.\n"};
static char *msg_lfu_update_warn2 =
  {"\n                          DO NOT ABORT!\n"};
static char *msg_lfu_list_header =
  {"\nDevice         Current Revision    Filename            Update Revision\n"};
static char *msg_lfu_exit_error =
  {"\nErrors occured during update with the following devices:"};
static char *msg_lfu_exit_error_cont =
  {"\n\nDo you want to continue to exit?\n"};

static char lfu_prompt1[] = {"\
If you have a floppy containing option firmware,\n\
Please insert it now and hit <return> when ready. \
"};

static char lfu_prompt2[] = {"\n\
Option firmware files were not found on CD or floppy.\n\
If you want to load the options firmware,\n\
please enter the device on which the files are located(ewa0),\n\
or just hit <return> to proceed with a standard console update: \
"};

static char lfu_prompt3[] = {"\
Please insert next floppy containing the options firmware,\n\
Hit <return> when ready. Or type DONE to abort. \
"};

static char lfu_prompt4[] = {"\
Please enter the name of the options firmware files list, or\n\
Hit <return> to use the default filename (%s) \
"};


/* support modify attributecommand */
int lfu_support_modify_command = 0;
int (*lfu_modify_command)();

/* support readme command */
int lfu_support_readme_command = 0;
int (*lfu_readme_command)();

/* If any errors occur during the updates then this flag will be filled 
/* with names. */
static struct update_flag_array {
    char *name;
    int state;
} update_flag_array[MAX_NAMES];
					
static  int noninteractive;		/* flag to indicate we are doing an OS
					   update - non interactive update. The
				           value of this flag is passed to
					   lfu_system_int() to startup specific 
					   firmware drivers.*/
struct DLL_HEADER {
    u_int type;				/* type 'HF01' */
    u_int name[8];			/* file name */
    u_int len;				/* code length in bytes */
    u_int otime;			/* original file time */
    u_int cksum;			/* original file checksum */
    u_int code_len;
};


static char temp_str[128];
static char *temp_str_ptr;

static int olength;			/* decompressed length */
static int otime;			/* original .exe file time */
static int clength;			/* compressed length */
static int loadadr;			/* load address */
static int checksum;			/* decompressed checksum */

static u_char (*fetch_byte)();		/* byte read and inc ptr routine */

static u_char *hf_buf_adr;		/* working buffer pointer */

static u_char *mem_in_ptr;
static u_char *mem_out_ptr;

static work *area;
static char *MemoryHeap;
static char *initial_MemoryHeap;
static UINT *vblp;
static UINT *llistp;
static UINT *llp;

static UINT bb;				/* bit buffer */
static UINT bk;				/* bits in bit buffer */
static UINT wp;				/* current position in slide */
static UINT bytebuf;

/* Tables for deflate */

static USHORT mask_bits[] = {0x0000, 0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f,
  0x007f, 0x00ff, 0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff,
  0xffff};

/* Order of the bit length code lengths */

static UCHAR border[] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13,
  2, 14, 1, 15};

/* Copy lengths for literal codes 257..285 */

static USHORT cplens[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35,
  43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0};

/* note: see note #13 above about the 258 in this list. */

/* Extra bits for literal codes 257..285 */
/* 99==invalid */

static USHORT cplext[] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
  4, 4, 4, 4, 5, 5, 5, 5, 0, 99, 99};

/* Copy offsets for distance codes 0..29 */

static USHORT cpdist[] = {1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
  257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385,
  24577};

/* Extra bits for distance codes */

static USHORT cpdext[] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
  9, 9, 10, 10, 11, 11, 12, 12, 13, 13};



/*+
 * =========================================================================
 * = lfu - Update or Verify firmware. 					   =
 * =========================================================================
 *
 *  OVERVIEW:
 *
 *      Allows the user to update and verify the firmware on a varity of 
 *	adapters.
 * 
 *
 *      Update may-97 - the lfu can now update srm and arc consoles without 
 *	user interaction through the use of a new console ev "fwupd_via_os"
 *      to allow for the Digital Unix Install program to update firmware
 *      before installing the OS.
 *
 * COMMAND FMT: lfu 3 S 0 lfu
 *
 * COMMAND FORM:
 *
 *	lfu ( [<cmd>] )
 *
 * COMMAND TAG: lfu 0 RXB lfu
 *
 * COMMAND ARGUMENT(S):
 *
 *	<cmd> - optional LFU command.
 *
 * COMMAND OPTION(S):
 *
 *	None.
 *
 * COMMAND EXAMPLES(S):
 *~
 *	>>>lfu
 *~
 * FORM OF CALL:
 *  
 *	lfu(pargc, pargv)
 *  
 * RETURN CODES:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 * 	int pargc 	- Number of arguments on the command line.
 *	char *pargv[] 	- Array of pointers to argument strings.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int lfu(int pargc, char *pargv[])
{
    char **argv;
    int argc;
    int status;
    struct FILE	*fin;
    struct INODE *ip;
    struct QSTRUCT qual[QMAX];
    char *flags;
    char *script_name = "SETUP";

    struct EVNODE evn, *evp;	/* for OS-driven LFU firmware updates*/

    int select;
    int lfu_one_liner;		/* flag indicates full lfu command */
    int i;
    char alt_fname[132];


    noninteractive = 0;		/* default */
    select = TBD;		/* default */

    /* If for some ungodly reason someone passed typed 'lfu update *' without
     * the '*' in "" the * will get expanded to all the inode names and we
     * weren't designed to do that!!! */
    if (pargc > 10) {
	printf(msg_extra_params);
	return (msg_failure);
    }

    /* If we came into this with arguments, then  flag the fact that we have an
     * lfu 'one-liner'  command. I.E. 'lfu update *' */

       if (pargc > 1) 
          lfu_one_liner = 1;
       else 
          lfu_one_liner = 0;

/* Check if this is a request for a non-interactive fw update 
   by examing the value of console ev: console boot_osflags for a
   text-value if lfu or LFU
*/
       evp = &evn;
       if( ev_read( "boot_osflags", &evp, 0) == msg_success )  {
          flags = evp->value.string;
          if ( ((flags[0] == 'L' && flags[1] == 'F' && flags[2] == 'U')) ||
               ((flags[0] == 'l' && flags[1] == 'f' && flags[2] == 'u')) ) {
	     noninteractive = 1;
             flags = &flags[3];
             if( ev_write("boot_osflags", flags, EV$K_STRING) == msg_success )
                printf("\nResetting boot_osflags\n");
             else
                printf("Resetting boot_osflags ev NOT successful\n");
          }
       }

    /* change the mode on any "ni_db_" file to not execute. */
    /*lfu_call_shell_cmd("if (ls ni_db* > nl) then chmod -x ni_db*; fi");*/

    if ( !lfu_one_liner ) 
       lfu_system_init(noninteractive);

    /* print utility banner */
    if ( !lfu_one_liner) 
       printf(msg_lfu_banner);

    /* initalize our update flag array */
    if (!lfu_one_liner) {
	for (i = 0; i < MAX_NAMES; i++) {
	    update_flag_array[i].name = 0;
	    update_flag_array[i].state = msg_success;
	}
    }
    /* display options for user input  */
    if (!lfu_one_liner && !noninteractive) 
	display_table(0, 0);

     do {
	argv = malloc(MAX_ARGS * sizeof(char *));


	if (lfu_one_liner) {
	    for (i = 1; i < pargc; i++) {
		argv[i - 1] = malloc(strlen(pargv[i]) + 1);
		strcpy(argv[i - 1], pargv[i]);
	    }
	    argc = pargc - 1;
	} 
        else { 
	/* for command to be like a one liner; "lfu update" */
	 if (noninteractive) {
	    argv[0] = mk_dynamic("update");
	    argc = 1;
	 } 
         else
	    parse_command_input(&argc, argv);
	}

	if (argc < 1) 
	    select = TBD;
        else 
	    select = selection(argv[0]);

	/* if we fail to scan this command line ignore the remaining part. */
	if ((status = qscan(&argc, argv, "-", QSTRING, qual)) != msg_success) {
	    printf(status);
	    select = TBD;;
	}

	/* Save alternate filename. */
	if (qual[QPATH].present) 
	    strcpy(alt_fname, qual[QPATH].value.string);
        else 
	    alt_fname[0] = '\0';

	/* modify attribute command qualify */
	if (select == MOD_ATTR) {
	    if (argc > 2) {
		printf(msg_extra_params);
		select = TBD;
	    } else {
		 if (argc < 2) {
		    argv[1] = mk_dynamic("*");
		    argc = 2;
		 }
	      }
	}

	/* update and verify command qualify */
	if (select == UPDATE || select == VERIFY) {
	    if (argc > 2) {
		printf(msg_extra_params);
		select = TBD;
	    } else {
		if (argc < 2) {
		   argv[1] = mk_dynamic("*");
		   argc = 2;
		}

		/* good arg count try expanding it. */
		lfu_flash_cnt = 0;
		fexpand(argv[1], 0, lfu_name_cb, 0, 0);

		/* If the name didn't expand anything */
		/* try with "_flash" on it.           */
		if (lfu_flash_cnt == 0) {
		    sprintf(temp_str, "%s_flash", argv[1]);
		    fexpand(temp_str, 0, lfu_name_cb, 0, 0);
		}

		/* if none of the names were correct then inform user. */
		if (lfu_flash_cnt == 0 && !noninteractive ) {
		    printf("\n%-15s", argv[1]);
		    printf(msg_lfu_no_support);
		    select = TBD;
		}
	    }
	}

	switch (select) {
	    case VERIFY: 
		for (i = 0; i < lfu_flash_cnt; i++) {
		    lfu_verify(lfu_flash_name[i], alt_fname);
		}
		break;

	    case UPDATE: 
		   if (!noninteractive) {
		    printf("\nConfirm update on:");
		    for (i = 0; i < lfu_flash_cnt; i++) 
		      lfu_print_device_name(lfu_flash_name[i]);
		    printf("\n");
		    read_with_prompt(msg_lfu_prompt_y_n, 20, temp_str, 0, 0, 1);
		   }
		    if (temp_str[0] == 'y' || temp_str[0] == 'Y'|| noninteractive) {
                      printf(msg_lfu_update_warn1);
		      printf(msg_lfu_update_warn2);

		/* For noninteractive update force only updating srm and arc/alphabios.
		*  In a future release (for ADP/DUnix) we will want the ability to
		*  update console and options firmware noninteractively
		*/			
                    if (noninteractive)
  			lfu_flash_cnt = 2;

		    /* perform the updates... */
		    for (i = 0; i < lfu_flash_cnt; i++) 
			lfu_update(lfu_flash_name[i], alt_fname);
		break;

	    case HELP: 			/* help or ? */
		display_table(argc, argv);
		break;

	    case LIST_SPT: 		/* list */
		list_support();
		break;

	    case DISPLAY: 
		lfu_display_config();
		break;

	    case README: 
		(*lfu_readme_command)();
		break;

	    case EXIT: 
		if (check_update_flag() == msg_failure) {
		    printf(msg_lfu_exit_error);
		    report_update_flag();
		    printf(msg_lfu_exit_error_cont);
		    read_with_prompt(msg_lfu_prompt_continue_y_n, 20, temp_str,
		      0, 0, 1);
		    if (temp_str[0] != 'y' && temp_str[0] != 'Y') {
			select = TBD;
		    }
		}
		break;

	    case MOD_ATTR: 
		if ((*lfu_modify_command)(argv[1]) == msg_failure) {
		    printf("\n modify attribute command failed for:\n     %s\n",
		      argv[1]);
		}
		break;

	    case TBD: 
		break;

	    default: 			/* invalid function (user selection) */
		{
		/* Check to see if this is a script command. If it is then
		 * execute it else report it was not a good command */
		if (fin = fopen (argv[0], "rs")) {
		    ip = fin->ip;
		    if ((ip->attr & ATTR$M_EXECUTE) && 
		      !(ip->attr & ATTR$M_BINARY)) {
			select = TBD;
		     }
		    fclose(fin);
		}
		if (select == TBD) {
		    /* If this command is a script then send the script
		     * the entire command line. */
		    temp_str[0] = 0;
		    for (i = 0; i < argc; i++) {
			strcat(temp_str, argv[i]);
			strcat(temp_str, " ");
		    }
		    lfu_call_shell_cmd(temp_str);
		} else
		    printf("Invalid Selection\n");
		}
	} /*end of switch statment */

    	/* empty the input array */
	for (i = 0; i < argc; i++) {
	   if (argv[i])
		free(argv[i]);
	}
	free(argv);
	argc = 0;
        }
    } while( (select != EXIT) &&  (!lfu_one_liner) && 
             (!killpending()) &&  (!noninteractive) );

    /* end of while loop */

    /* call our death */

/* In noninteractive mode and prior to resetting the system,
   check the Firmware CD to invoke a SETUP script from the
   CUSTOM directory area (if it exists).
*/
    if (noninteractive) {
       lfu_invoke_script(script_name);
    }

    if ( !lfu_one_liner || noninteractive )
	lfu_system_reset(noninteractive);

/* In noninteractive mode, the OS request the console invoke a boot after
   the system is initialized. The OS will ensure that the ev's -
   BOOT_RESET is ON and AUTO_ACTION is set to BOOT or set to RESTART.
*/
    if (noninteractive) {
       temp_str_ptr = temp_str;
       temp_str_ptr = "echo prepare to boot system";
       lfu_call_shell_cmd(temp_str);
       krn$_sleep(5000);
       lfu_call_shell_cmd("testing");
/*       temp_str_ptr; = "BOOT";     */
/*       lfu_call_shell_cmd("Boot"); */
    }

    return (msg_success);
}					/* end of main */

/*+
 * =========================================================================
 * = lfu_invoke_script                                           	   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *	The purpose of this routine is to invoke a custom script from
 *   	the CUSTOM directory on the Alpha Firmware CD. A custom script
 * 	is invoked after console firmware has been updated.
 *
 * 	This routine is an extension to a request, from Digital Unix & ADP, 
 *	to execute an unattended console firmware update. The sequence of 
 *	events to perform an unattended console firmware update are: 
 *	1) update console firmware (SRM & ARC/AlphaBIOS only)
 *	2) invoke a custom script 
 *      3) reset the system
 *
 * CAVEAT:
 *
 *	There is NO guarantee a custom-script has executed successfully.
 *
 * FORM OF CALL:
 *  
 *	lfu_invoke_script()
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      char *script_name 
 *
 * SIDE EFFECTS:
 *
 *      None. 
 *
 * =========================================================================
 *--
 */

void lfu_invoke_script(char *script_name)
{
    struct FILE *fw_dev, *fw_list;
    struct ub *ub;

    int i, j;
    char device[32];		/* CD-ROM Device Name*/
    char directory[32];		/* custom directory name on CD-ROM*/
    char list[32];		/* custom script name*/
    char format[32];		/* CD-ROM format*/
    char file_string[80];	/* holds a string to be passed to shell_cmd()*/

    device[0] = 0;
    directory[0] = 0;
    printf("\n");

/* Get the device name of the CD-Drive. */

    for (i = 0; i < 15; i++) {
       krn$_wait(&ubs_lock);	
       shellsort(ubs, num_ubs, strcmp_ub);
       for (j = 0; j < num_ubs; j++) {
          ub = ubs[i];
	  if (((ub->inode->name[1] == 'k') || (ub->inode->name[1] == 'q')) && 
	    (ub->inq_len > 1)) {
             if ((ub->inq[0] & 0x1f) == 5) {
                strcpy(device, ub->inode->name);
                break;
             }
          }
       }
       krn$_post(&ubs_lock);
    }
/* Verify CD-ROM has a custom directory and script */

    if (device[0]) {
       strcpy(directory, "[custom]");
       sprintf(list, "%s", script_name);

       printf("Checking %s for a script file. . .\n", device);
       krn$_sleep(5000);

       strcpy(format, "iso9660");  /* Only ISO9660 formatted CD is used */

       sprintf(file_string,  "%s:%s%s/%s"  , format, directory, list, device);

       /* load script into memory */

       if (fw_dev = fopen(file_string, "sr")) {
          lfu_get_options_firmware_helper(fw_dev, script_name, 2048);
          fclose(fw_dev);	

          /* run script */

          if (fw_list = fopen(script_name, "rs")) {
             fclose(fw_list);
	     setattr(script_name, ATTR$M_READ | ATTR$M_WRITE | ATTR$M_EXECUTE);
             lfu_call_shell_cmd(script_name);
          }
       }
       else {
          printf("Script Not found\n");
          krn$_sleep(5000);
       }
    }
}

/*+
 * ============================================================================
 * = parse_command_input - lfu parse command line input function              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will prompt the user for input and then take 
 *	that input and fill in argc and argv with it. It is similar to
 *	the standard console command parser with the exception of not 
 *	doing anything with the input other than lowercasing it.
 *  
 * FORM OF CALL:
 *  
 *	parse_command_input(argc, argv)
 *  
 * RETURN CODES:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      int *argc       - address of argc
 *      char *argv []   - address of array of pointers to strings
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static void parse_command_input(int *argc, char *argv[])
{
    char user_input[84];
    char temp_buf[84];
    int i, j, z, in;

    for (i = 0; i != 84; i++)		/* clear buffers */
	temp_buf[i] = user_input[i] = 0;

    /* Read in user input and temporarily store in the user input buffer. */
    printf("\n");
    read_with_prompt(msg_lfu_prompt, 80 - strlen(msg_lfu_prompt), user_input,
      0, 0, 1);


    /* Condense out multiple spaces, tabs, and commas to a single space */
    /* between input. Also trash off any preceeding spaces, tabs...     */
    for (z = i = 0; user_input[i] != '\n' && user_input[i] != '\0'; i++)
					/* continue until finding <CR> */
      {
	if (isspace(user_input[i]) || user_input[i] == ',') {
	    if ((z > 0) && (!isspace(temp_buf[z - 1])))
		temp_buf[z++] = ' ';
	} else {
	    temp_buf[z++] = tolower(user_input[i]);
	}
    }

    /* replace <CR> with null character */
    if ((z > 0) && (isspace(temp_buf[z - 1]))) {
	temp_buf[z - 1] = '\0';
	z--;
    } else
	temp_buf[z] = '\0';
    in = z;
    i = 0;

    for (z = 0; z < in;) {

	argv[i] = malloc(in + 1);

	/* Place data from temporary buffer into the argument variables. */
	for (j = 0; temp_buf[z] != ' ' && temp_buf[z] != '\0'; z++, j++)
	    *(argv[i] + j) = temp_buf[z];
	z++;
	*(argv[i] + j + 1) = '\0';
	i++;
    }
    *argc = i;

}

/*+
 * ============================================================================
 * = display_table - display command table function                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will print to the user the list of allowable commands
 *	that the user can enter.
 *	It will also display more help if the user type help "command"
 *  
 * FORM OF CALL:
 *  
 *	display_table(argc, argv) 
 *  
 * RETURN CODES:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      int argc        - argument count
 *      char *argv []   - address of array of pointers to strings
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static void display_table(int argc, char *argv[])
{
    char sel;
    char *str1;
    char *str2;
    char *str3;

    /* report the function table */
    if (argc < 2) {
	printf("------------------------------------------------------------------------------\n");
	printf(" Function    Description\n");
	printf("------------------------------------------------------------------------------\n");
	printf("\n");
	printf(" Display     Displays the system's configuration table.\n");
	printf(" Exit        Done exit LFU (reset).\n");
	printf(" List        Lists the device, revision, firmware name, and update revision.\n");
	if (lfu_support_modify_command)
	    printf(" Modify      Modifies port parameters and device attributes.\n");
	if (lfu_support_readme_command)
	    printf(" Readme      Lists important release information.\n");
	printf(" Update      Replaces current firmware with loadable data image.\n");
	printf(" Verify      Compares loadable and hardware images.\n");
	printf(" ? or Help   Scrolls this function table.\n");
	printf("------------------------------------------------------------------------------\n");
	printf("\n");
    }

    /* If user just typed help them feed him into more */
    if (argc == 1) {
	printf(" Type Help <command> for additional information\n\n");
    }
    if (argc > 1) {
	sel = selection(argv[1]);
	switch (sel) {

	    case VERIFY: 
	    case UPDATE: 

		if (sel == VERIFY) {
		    str1 = "Verify";
		    str2 = "VERIFY";
		    str3 = "verify";
		} else {
		    str1 = "Update";
		    str2 = "UPDATE";
		    str3 = "update";
		}

		printf(" %s a particular device with LFU's firmware.\n", str1);
		printf("   The command format is: %s [<device>] [-PATH <filename>]\n", str2);
		printf("   For example:\n");
		printf("           %s *\n", str3);
		printf("       or  %s\n\n", str3);
		printf("   Will %s all LFU supported devices found in this system\n\n", str3);
		printf("           %s io \n\n", str3);
		printf("   Will %s the device named IO\n", str3);
		printf("   Use the LIST command to see the supported LFU devices\n\n");
		printf("   You can optionally %s a device with different firmware than\n", str3);
		printf("   defaulted to by LFU, by using the -PATH switch.\n");
		printf("   For example:\n");
		printf("           %s io -path mopdl:new_firm/eza0\n\n", str3);
		printf("   Will %s the device named IO with firmware NEW_FIRM from the\n", str3);
		printf("   network.\n\n");
		break;

	    case HELP: 			/* help or ? */
		printf(" Type HELP <command> for additional information\n");
		printf(" or type help to see the list of commands\n");
		printf("   For example:\n");
		printf("           HELP UPDATE\n");
		printf("   Will give you help on the update command\n");
		break;

	    case LIST_SPT: 		/* list */
		printf(" Lists the device, revision, firmware name and update revsion.\n");
		printf("   This helps to understand what devices LFU can update or verify and\n");
		printf("   firmware that would be used to update or verify with.\n");
		break;

	    case DISPLAY: 
		printf(" Displays the system's configuration table.\n");
		printf("   This is helpful to locate the specific device to\n");
		printf("   update or verify.\n");
		break;

	    case EXIT: 
		printf(" Done with updating so leave. If any devices failed\n");
		printf(" update then it will comfirm your intention to EXIT.\n");
		break;

	    case MOD_ATTR: 
		printf(" Modifies a particular device's attributes or port parameters.\n");
		printf("   The command format is: MODIFY [<device>]\n");
		printf("   For example:\n");
		printf("           modify *\n");
		printf("       or  modify\n\n");
		printf("   Will modify all LFU supported devices found in this system\n\n");
		printf("           modify io \n\n");
		printf("   Will modify the device named IO\n");
		printf("   Use the LIST command to see the supported LFU devices\n\n");
		break;

	    case README: 
		printf(" Lists important release information.\n");
		break;

	    default: 			/* invalid function (user selection) */
		printf("  No help on this topic.\n");
		break;
	}
    }
}

/*+
 * ============================================================================
 * = selection - detern function user requested                               =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will search the string given to it to determine the
 *	command selected. 
 *  
 * FORM OF CALL:
 *  
 *	selection(argv[])
 *  
 * RETURN CODES:
 *
 *      int  - the numerical value of the selection requested.
 *       
 * ARGUMENTS:
 *
 *      char argv[] 	- string containg the user command.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int selection(char argv[])
{
    char loop;
    int x, y;

    /* function array.  User can supply any of the following valid functions.
     */
    char *func[MAX_COMMANDS];

    func[TBD] = "";
    func[MOD_ATTR] = "modify";
    func[VERIFY] = "verify";
    func[UPDATE] = "update";
    func[LIST_SPT] = "list";
    func[DISPLAY] = "display";
    func[README] = "readme";
    func[HELP] = "help";
    func[EXIT] = "exit";

    loop = 1;

    /* compare each character of selection buffer with function array. */
    for (x = TBD; argv[0] && x < MAX_COMMANDS && loop; x++) {
	if (argv[0] == *func[x]) {

	    /* if characters match increment column y and compare next. */
	    for (y = 0; argv[y] == *(func[x] + y) && *(func[x] + y); y++) {
		if (argv[y + 1] == '\0') {

		    /* valid function */
		    loop = 0;
		    x--;		/* X will get incremented before
					 * leaving */
		    break;
		}
	    }
	}
    }

    /* user request help */
    if (argv[0] == '?')
	x = HELP;

    /* check on modify attribute command and skip it if not supported. */
    if (x == MOD_ATTR && !lfu_support_modify_command)
	x = INVALID;

    /* check on readme command and skip it if not supported. */
    if (x == README && !lfu_support_readme_command)
	x = INVALID;

    return (x);
}

/*+
 * ============================================================================
 * = list_support - display supported devices function                        =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will print to the user the list of all the devices that
 *	an update driver has defined. It also will show the revision of the
 *	device the recommended firmware filename and where or not it is 
 *	found.
 *  
 * FORM OF CALL:
 *  
 *	list_support()
 *  
 * RETURN CODES:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static void list_support(void) 
{
    struct DDB *ddb;
    struct FILE *flash_fp;
    struct FILE *file_fp;
    char fw[30];
    char fname[80];
    int (*revision)();			/* revision routine  */
    int i, j, k;
    char str[20];
    int status;
    int *arg;
    char *firm_base;
    char *firm_size;

    /* expand names. */
    arg = mk_dynamic("*");
    lfu_flash_cnt = 0;
    fexpand(arg, 0, lfu_name_cb, 0, 0);

    printf(msg_lfu_list_header);

    for (k = 0; k < lfu_flash_cnt; k++) {
	revision = 0;
	flash_fp = fopen(lfu_flash_name[k], "r");
	if (flash_fp) {
	    ddb = flash_fp->ip->dva;

	    /* Print the name of the adapter. */
	    lfu_print_device_name(lfu_flash_name[k]);

	    /* Go get the revision information. */
	    if (ddb->misc) {

		/* default to no info gotten from inquire */
		fname[0] = fw[0] = '\0';

		revision = ddb->misc;
		status = (*revision)(fw, fname, flash_fp);
		if (status == msg_success) {

		    /* Print the firmware rev. Remember to line up the  dots
		     * for the number. */
		    lfu_strstrip(fw);
		    if (i = strchr(fw, '.')) {
			j = (char *) i - fw;
		    } else {
			j = strlen(fw);
		    }
		    for (i = 5; i > j; i--)
			printf(" ");
		    i = 20 - ((j < 5) ? 5 - j : 0);
		    sprintf(str, "%%-%d.%ds", i, i);
		    printf(str, fw);

		    /* report filename and if it there... */
		    printf("%-20s", fname);
		} else
		    printf("\n");
	    }				/* end if (ddb->misc) */
	}				/* end if (flash_fp) */
	fclose(flash_fp);

	/* Report if the fname suggested by the driver is there. Also report if
	 * the image is valid by reporting its fw rev or if not valid report
	 * that too. */
	if (revision && status == msg_success) {
	    file_fp = go_open_firmware(fname);
	    if (file_fp) {
		firm_size = lfu_get_firmware(&firm_base, file_fp);
		go_close_firmware(file_fp);
		flash_fp = fopen(lfu_flash_name[k], "r");
		ddb = flash_fp->ip->dva;
		status = (*ddb->validation)(2, str, fw, firm_base, firm_size,
		  flash_fp);
		fclose(flash_fp);
		if (status == msg_failure) {
		    printf("Invalid image\n");
		} else {

		    /* must be valid image report fw rev (lined up). */
		    lfu_strstrip(fw);
		    if (i = strchr(fw, '.')) {
			j = (char *) i - fw;
		    } else {
			j = strlen(fw);
		    }
		    for (i = 5; i > j; i--)
			printf(" ");
		    printf("%s\n", fw);
		}
		dyn$_free(firm_base, DYN$K_SYNC | DYN$K_NOOWN);
	    } else {
		printf("Missing file\n");
	    }
	}
    }
    free(arg);

    /* now go after the firmware files only. */
    arg = mk_dynamic("ZZZZZZFW_*");
    fexpand(arg, 0, lfu_listfw_cb, 0, 0);
    free(arg);

}					/* end of list_support */

/*+
 * ============================================================================
 * = int lfu_verify - routine to perform the verify                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This is the routine to actually perform the  verify on the device.
 *  
 * FORM OF CALL:
 *  
 *	lfu_verify (name, alt_fname)
 *  
 * RETURN CODES:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      char *name      - the name of the device you want to verify.
 *	char *alt_fname - Alternate firmware filename if any.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static void lfu_verify(char *name, char *alt_fname)
{
    struct FILE *flash_fp;
    struct FILE *file_fp;
    struct INODE *ip;
    char fw[30];
    char fname[80];
    char new_name[30];
    char firm_fw[30];
    int (*revision)();			/* revision routine  */
    int i, j;
    char *firm_base;
    char *firm_size;
    struct DDB *ddb;
    int status = msg_failure;

    flash_fp = fopen(name, "r");
    if (flash_fp) {
	ip = flash_fp->ip;

	ddb = ip->dva;

	/* Print the name of the adapter. */
	lfu_print_device_name(name);

	/* default to no info gotten from inquire */
	fname[0] = fw[0] = '\0';

	/* Go get the revision information. */
	if (ddb->misc) {

	    revision = ddb->misc;
	    if ((status = (*revision)(fw, fname, flash_fp)) == msg_success) {
		lfu_strstrip(fw);
	    }
	}

	/* If we have an alternate name subsitute fname with it. */
	if (strlen(alt_fname) > 0)
	    sprintf(fname, "%s", alt_fname);

	/* Report missing name. This could only happen from driver code bugs */
	if (strlen(fname) == 0) {
	    err_printf(msg_lfu_no_firm);
	    fclose(flash_fp);
	    return;
	}

	fclose(flash_fp);

	/* Get the firmware data. */
	if (status == msg_success) {
	    file_fp = go_open_firmware(fname);

	    /* Report open errors */
	    if (file_fp == 0) {
		err_printf(msg_lfu_firm_opn_err, fname);
		return;
	    }

	    firm_size = lfu_get_firmware(&firm_base, file_fp);
	    go_close_firmware(file_fp);

	    /* Re-open the device to be verified. We do this after the getting 
	     * of firmware to furture prevent contention with getting the
	     * firmware and verifying using the same device. */
	    flash_fp = fopen(name, "r");
	    if (flash_fp) {

		ddb = ip->dva;

		printf(msg_lfu_verify, fw);
		if ((*ddb->validation)(1, new_name, firm_fw, firm_base,
		  firm_size, flash_fp) == msg_success) {
		    printf(msg_lfu_verify_good);
		} else {
		    printf(msg_lfu_verify_bad);
		}
	    }
	    fclose(flash_fp);
	    dyn$_free(firm_base, DYN$K_SYNC | DYN$K_NOOWN);
	}
    }
    printf("\n");
    return;
}

/*+
 * ============================================================================
 * = int lfu_update - routine to perform the update                           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This is the routine to actually perform the  update on the device.
 *  
 * FORM OF CALL:
 *  
 *	lfu_update (name, alt_fname)
 *  
 * RETURN CODES:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      char *name      - the name of the device you want to update.
 *	char *alt_fname - Alternate firmware filename if any.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static void lfu_update(char *name, char *alt_fname)
{
    struct FILE *flash_fp;
    struct FILE *file_fp;
    struct INODE *ip;
    char fw[30];
    char fname[80];
    char new_name[80];
    char firm_fw[30];
    int (*revision)();			/* revision routine  */
    int i, j;
    char *firm_base;
    char *firm_size;
    unsigned int oldattr;
    struct DDB *ddb;
    int status = msg_failure;

    oldattr = setattr(name, ATTR$M_READ | ATTR$M_WRITE);

    /* default to no info gotten from anything */
    fname[0] = fw[0] = firm_fw[0] = new_name[0] = '\0';

    flash_fp = fopen(name, "r");
    if (flash_fp) {
	ip = flash_fp->ip;

	ddb = ip->dva;

	/* Print the name of the adapter. */
	lfu_print_device_name(name);

	/* Go get the revision information. */
	if (ddb->misc) {

	    revision = ddb->misc;
	    if ((status = (*revision)(fw, fname, flash_fp)) == msg_success) {
		lfu_strstrip(fw);
	    }
	}

	/* If we have an alternate name subsitute fname with it. */
	if (strlen(alt_fname) > 0)
	    sprintf(fname, "%s", alt_fname);

	/* Report missing name. This could only happen from driver code bugs */
	if (strlen(fname) == 0) {
	    err_printf(msg_lfu_no_firm);
	    fclose(flash_fp);
	    update_flag(name, msg_failure);
	    return;
	}

	fclose(flash_fp);



	/* Get the firmware data. */
	if (status == msg_success) {
	    file_fp = go_open_firmware(fname);

	    /* Report open errors */
	    if (file_fp == 0) {
		err_printf(msg_lfu_firm_opn_err, fname);
		update_flag(name, msg_failure);
		return;
	    }

	    firm_size = lfu_get_firmware(&firm_base, file_fp);
	    go_close_firmware(file_fp);

	    /* Re-open the device to be updated. We do this after the getting
	     * of firmware to furture prevent contention with getting the
	     * firmware and updating using the same device. */
	    flash_fp = fopen(name, "r+");
	    if (flash_fp) {
		ip = flash_fp->ip;

		ddb = ip->dva;

		/* validate image */
		status = (*ddb->validation)(0, new_name, firm_fw, firm_base,
		  firm_size, flash_fp);
		lfu_strstrip(firm_fw);

		/* bad firmware image */
		if (status == msg_failure) {
		    printf(msg_lfu_firm_bad, fname);
		    update_flag(name, msg_failure);
		}

/*
 * For Wilfire the micro firmware is very unstable this time to allow for an
 *  update of "X" rev firmware.
*/
#if WFUPDATE
temp_str[0] = 'y';
if (substr_nocase(name, "micro") != 0) {
if (substr_nocase("x", firm_fw) != 0) {
printf("\n firmware of rev %s is not very stable are you sure you want to perform this update.\n", firm_fw);
read_with_prompt(msg_lfu_prompt_continue_y_n, 20, temp_str,  0, 0, 1);
}
}
if (temp_str[0] == 'y' || temp_str[0] == 'Y') {
#endif

		/* good but old rev firmware */
		if (status == 2 && !noninteractive) {
		    printf(msg_lfu_update_older_rev, firm_fw, fw);
		    read_with_prompt(msg_lfu_prompt_continue_y_n, 20, temp_str,
		      0, 0, 1);
		    if (temp_str[0] == 'y' || temp_str[0] == 'Y') {
			status = msg_success;

			/* Print the name of the adapter. */
			lfu_print_device_name(name);
		    }
		}
		/* If we are doing this special update only overide older 
		 * revs */
		if (noninteractive && status == 2) {
		   status = msg_success;
		}

		/* Good or old rev overide */
		if (status == msg_success) {

		    /* If the validation process has replaced the file for
		     * update then  we must reopen it and get the information.
		     */
		    if (new_name[0] != '\0') {
			file_fp = go_open_firmware(new_name);
			dyn$_free(firm_base, DYN$K_SYNC | DYN$K_NOOWN);
			firm_size = lfu_get_firmware(&firm_base, file_fp);
			go_close_firmware(file_fp);
		    }

		    printf(msg_lfu_update, firm_fw);

		    update_flag(name, msg_success);

		    /* Lets write the image now that our ducks are lined up */
		    if (fwrite(firm_base, 1, firm_size, flash_fp) ==
		      firm_size) {

			/* close and reopen the file so that the validate
			 * routine fp->offset is set correctly. */
			fclose(flash_fp);
			flash_fp = fopen(name, "r+");
			ip = flash_fp->ip;
			ddb = ip->dva;

			/* now that the update write is over verify it */
			if (ddb->misc) {

			    revision = ddb->misc;
			    if ((*revision)(fw, fname, flash_fp) ==
			      msg_success) {
				lfu_strstrip(fw);
			    }
			}

			/* Verify it after write */
			printf(msg_lfu_verify, fw);
			if ((*ddb->validation)(1, new_name, firm_fw, firm_base,
			  firm_size, flash_fp) == msg_success) {
			    printf(msg_lfu_verify_good);
			} else {
			    update_flag(name, msg_failure);
			    printf(msg_lfu_verify_bad);
			}
		    } else {
			update_flag(name, msg_failure);
			printf(msg_lfu_update_failed);
		    }
		}

/*
 * For Wilfire the micro firmware is very unstable this time to allow for an
 *  update of "X" rev firmware.
*/
#if WFUPDATE
}
#endif

	    }
	    fclose(flash_fp);
	    setattr(name, oldattr);
	    dyn$_free(firm_base, DYN$K_SYNC | DYN$K_NOOWN);
	}
    } else
	update_flag(name, msg_failure);

    printf("\n");
    return;
}

/*+
 * ============================================================================
 * = lfu_strstrip - strip spaces					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	This functions strips off beginning and trailing spaces/tabs.
 *  
 * FORM OF CALL:
 *  
 *      lfu_strstrip(s)
 *  
 * RETURN CODES:
 *
 *	None.
 *       
 * ARGUMENTS:
 *
 *      char s[]   - address of string
 *
 * SIDE EFFECTS:
 *
 *	String s is now stripped. 
 *
-*/
static void lfu_strstrip(char s[])
{
    int i, j;

    i = j = 0;
    while (s[i]) {
	if (isgraph(s[i]))
	    s[j++] = s[i];
	i++;
    }
    s[j] = s[i];
}

/*+
 * ============================================================================
 * = lfu_print_device_name - print the flash Inode name                       =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This function prints the device name with the word "_flash" stripped
 *      off of it if there. LFU flash drivers add this word on to the Inode 
 *      so that the basic device can be uniquely identified.
 *  
 * FORM OF CALL:
 *  
 *	lfu_print_device_name(name)
 *  
 * RETURN CODES:
 *
 *	None.
 *       
 * ARGUMENTS:
 *
 *      char s[]	- address of string
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
static void lfu_print_device_name(char *name)
{
    int i, j, k;
    char report_name[132];


    /* if the name has the word "_flash" on the end then copy the name up to
     * it. */
    k = strlen(name);
    if (j = strstr(name, "_flash")) {
	k = (char *) j - name;
    }

    for (i = j = 0; i < k; j++, i++)
	report_name[j] = name[i];
    report_name[j] = '\0';

    printf("\n%-15.14s", report_name);

}

/*+
 * ============================================================================
 * = int lfu_name_cb - fexpand callback flash supported names                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This is the callback routine that fexpand uses to pass back the 
 *      individual names.
 *  
 * FORM OF CALL:
 *  
 *	lfu_name_cb (name, p0, p1)
 *  
 * RETURN CODES:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 *	int p0		- extra parameter set to us by fexpand. Ignored.
 *	int p1		- extra parameter set to us by fexpand. Ignored.
 *
 * SIDE EFFECTS:
 *
 *	lfu_flash_name will be updated and its
 *	count will be incremented.
 *
-*/
static int lfu_name_cb(char *name, int p0, int p1)
{
    struct INODE *ip;

    /* find the name in the inode list and save it if a flash type. */
    if (allocinode(name, 0, &ip) == 1) {
	if (ip->dva) {
	    if (ip->dva->flash) {

		/* found a flash name so save it. */
		strcpy(lfu_flash_name[lfu_flash_cnt], name);
		lfu_flash_cnt++;
	    }
	}
    }
    ilist_unlock();
    return (msg_success);
}

/*+
 * ============================================================================
 * = int lfu_listfw_cb - fexpand callback flash fw only names                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This is the callback routine that fexpand uses to pass back the 
 *      individual names of the expanded list. In the case of this callback
 *	all the names are going to come to us with the name "ZZZZZFW_" on the
 *	front of them.
 *  
 * FORM OF CALL:
 *  
 *	lfu_listfw_cb (name, p0, p1)
 *  
 * RETURN CODES:
 *
 *      msg_success
 *       
 * ARGUMENTS:
 *
 *	int p0		- extra parameter set to us by fexpand. Ignored.
 *	int p1		- extra parameter set to us by fexpand. Ignored.
 *
 * SIDE EFFECTS:
 *
 *	Each name will be printed out.
-*/
static int lfu_listfw_cb(char *name, int p0, int p1)
{
    struct DDB *ddb;
    struct FILE *flash_fp;
    struct FILE *file_fp;
    char fw[30];
    char fname[80];
    int i, j, k;
    char str[20];
    int status;
    char *firm_base;
    char *firm_size;

    /* strip off the "ZZZZZZFW_" off the file name" */
    j = 0;
    for (i = 9; i < strlen(name); i++) 
	fname[j++] = name[i];
    fname[j] = 0;
 
    file_fp = go_open_firmware(fname);
    if (file_fp) {

	/* Only print the firmware filename if we find one. */
	printf("\n%35s%-20s", "", fname);

	firm_size = lfu_get_firmware(&firm_base, file_fp);
	go_close_firmware(file_fp);
	flash_fp = fopen(name, "r");
	ddb = flash_fp->ip->dva;
	status = (*ddb->validation)(2, str, fw, firm_base, firm_size, 
	  flash_fp);
	fclose(flash_fp);
	if (status == msg_failure) {
	    printf("Invalid image\n");
	} else {

	    /* must be valid image report fw rev (lined up). */
	    lfu_strstrip(fw);
	    if (i = strchr(fw, '.')) {
		j = (char *) i - fw;
	    } else {
		j = strlen(fw);
	    }
	    for (i = 5; i > j; i--)
		printf(" ");
	    printf("%s\n", fw);
	}
	dyn$_free(firm_base, DYN$K_SYNC | DYN$K_NOOWN);
    }
    return (msg_success);
}

/*+
 * ============================================================================
 * = lfu_get_firmware - get the firmware function                             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This function will get the entier contents of the file pointed to
 *      by fp. If the file has more data than initially planned then the
 *      base buffer size is expanded to accommodate it.
 *  
 * FORM OF CALL:
 *  
 *      lfu_get_firmware(&base, fp)
 *  
 * RETURN CODES:
 *
 *      the number of bytes gotten from file.
 *       
 * ARGUMENTS:
 *
 *	char **base	- pointer to a char pointer. It is returned to caller 
 *      struct FILE *fp	- file pointer of file to get data.
 *
 * SIDE EFFECTS:
 *
 *	None.
 *
-*/
static int lfu_get_firmware(char **base, struct FILE *fp)
{
    char *firm_base;
    unsigned long bytesleft;
    int newlen;

    /* lets preallocate some data space. */
    bytesleft = fp->ip->len[0] + 0x10000;

    newlen = lfu_get_firmware_helper(fp, &firm_base, bytesleft);

    /* All done so set our return values. */
    *base = firm_base;
    return newlen;
}

/*+
 * ============================================================================
 * = update_flag - update the update flag array 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will update the update flag array to indicate if an 
 *	update failed or if retried it succeeded.
 *  
 * FORM OF CALL:
 *  
 *	update_flag(name, state)
 *  
 * RETURN CODES:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      char *name      - the name of the device you want to update the flag on.
 *	int state	- the state of the update msg_success or msg_failure.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static void update_flag(char *name, int state)
{
    int i;
    int matched;


    matched = 0;

    /* Search the list if we matched update the state for that name. */
    for (i = 0; i < MAX_NAMES; i++) {
	if (update_flag_array[i].name) {
	    if (substr_nocase(name, update_flag_array[i].name) == 1) {
		matched = 1;
		update_flag_array[i].state = state;
		break;
	    }
	} else
	    break;
    }

    /* If we didn't match then update the list of names with the proper state
     */
    if ((!matched) && i < MAX_NAMES) {
	update_flag_array[i].name = malloc(strlen(name) + 1);
	strcpy(update_flag_array[i].name, name);
	update_flag_array[i].state = state;
    }

}					/* end of update_flag */

/*+
 * ============================================================================
 * = check_update_flag - check the update flag array 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will look through the update flag array to see if any 
 *	update failed and if retried it didn't succeed.
 *  
 * FORM OF CALL:
 *  
 *	check_update_flag()
 *  
 * RETURN CODES:
 *
 *      msg_success or msg_failure
 *       
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static int check_update_flag(void) 
{
    int i;
    int state;


    state = msg_success;

    /* Search the list for failed updates. */
    for (i = 0; i < MAX_NAMES; i++) {
	if (update_flag_array[i].name) {
	    if (update_flag_array[i].state == msg_failure) {
		state = msg_failure;
		break;
	    }
	} else
	    break;
    }

    return (state);
}					/* end of check_update_flag */

/*+
 * ============================================================================
 * = report_update_flag - report the update flag array 			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This routine will report the names of the devices that failed in 
 *	the update flag array.
 *  
 * FORM OF CALL:
 *  
 *	report_update_flag()
 *  
 * RETURN CODES:
 *
 *      None
 *       
 * ARGUMENTS:
 *
 *      None
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
static void report_update_flag(void) 
{
    int i;

    /* Search the list for failed updates. */
    for (i = 0; i < MAX_NAMES; i++) {
	if (update_flag_array[i].name) {
	    if (update_flag_array[i].state == msg_failure) {
		lfu_print_device_name(update_flag_array[i].name);
	    }
	} else
	    break;
    }

}					/* end of report_update_flag */

/*+
 * =========================================================================
 * = go_close_firmware - close firmware file function                      =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *      This is a shell routine used by LFU to close the firmware file.
 *      We call the function fclose with stdout,stderr set to
 *	"nl". We do it this way to avoid any error messages from the close.
 *
 * FORM OF CALL:
 *  
 *	go_close_firmware(fp) 
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      struct FILE  *fp - filepointer of device to close.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */
static void go_close_firmware(struct FILE *fp)
{
    struct PCB *pcb = getpcb();
    struct FILE *saved_outfp;
    struct FILE *saved_errfp;
    char *outname;
    char *errname;

   /*
    * Change stdout,stderr to null device so that the firmware driver
    * doesn't print during LFU updates
    */
    outname = pcb->pcb$a_stdout_name;
    errname = pcb->pcb$a_stderr_name;
    pcb->pcb$a_stdout_name = pcb->pcb$a_stderr_name = "nl";
    saved_outfp = pcb->pcb$a_stdout;
    saved_errfp = pcb->pcb$a_stderr;
    pcb->pcb$a_stdout = fopen(pcb->pcb$a_stdout_name, "w");
    pcb->pcb$a_stderr = fopen(pcb->pcb$a_stderr_name, "w");

    /*  Close the file. */
    fclose(fp);

    /* Change back stdout,stderr */
    fclose(pcb->pcb$a_stdout);
    fclose(pcb->pcb$a_stderr);
    pcb->pcb$a_stdout = saved_outfp;
    pcb->pcb$a_stderr = saved_errfp;
    pcb->pcb$a_stdout_name = outname;
    pcb->pcb$a_stderr_name = errname;
}

/*+
 * =========================================================================
 * = go_open_firmware - open firmware file function 	                   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *      This is a shell routine used by LFU to open the firmware file.
 *      We call the function fopen with stdout,stderr set to
 *	"nl". We do it this way to avoid any error messages from the open.
 *
 * FORM OF CALL:
 *  
 *      go_open_firmware(filename)
 *
 * RETURN CODES:
 *
 *      File pointer of open or null on error.
 *       
 * ARGUMENTS:
 *
 *      char *filename - name of file to open.
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */
struct FILE *go_open_firmware(char *filename)
{
    struct FILE *fp;
    struct PCB *pcb = getpcb();
    struct FILE *saved_outfp;
    struct FILE *saved_errfp;
    char *outname;
    char *errname;

   /*
    * Change stdout,stderr to null device so that the firmware driver
    * doesn't print during LFU updates
    */
    outname = pcb->pcb$a_stdout_name;
    errname = pcb->pcb$a_stderr_name;
    pcb->pcb$a_stdout_name = pcb->pcb$a_stderr_name = "nl";
    saved_outfp = pcb->pcb$a_stdout;
    saved_errfp = pcb->pcb$a_stderr;
    pcb->pcb$a_stdout = fopen(pcb->pcb$a_stdout_name, "w");
    pcb->pcb$a_stderr = fopen(pcb->pcb$a_stderr_name, "w");

    /* Open it. */
    fp = fopen(filename, "r");

    /* Change back stdout,stderr */
    fclose(pcb->pcb$a_stdout);
    fclose(pcb->pcb$a_stderr);
    pcb->pcb$a_stdout = saved_outfp;
    pcb->pcb$a_stderr = saved_errfp;
    pcb->pcb$a_stdout_name = outname;
    pcb->pcb$a_stderr_name = errname;

    return fp;
}

/*+
 * ============================================================================
 * = lfu_get_rev - interperet the revision givin it.			      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      This function will give you a numbered value for comparing
 *      two different revisions of firmware
 *  
 * FORM OF CALL:
 *  
 *      lfu_get_rev(fw_rev)
 *  
 * RETURN CODES:
 *
 *      0 	- not in xn.n-nnn format.
 *	mmm nnn v sssss
 *		mmm   - major version number
 *		nnn   - minor version number
 *		v     - version letter; 0 - other, 1 - x, 2 - t, 3 - v  
 *		sssss - sequence number
 *       
 * ARGUMENTS:
 *
 *	char *fw_rev 	- string containing the fw_rev.
 *
 * SIDE EFFECTS:
 *
 *	none
-*/
unsigned __int64 lfu_get_rev(char *fw_rev)
{


    int i, j, k;
    char fw[3][32];
    int fwrev_length;
    unsigned __int64 result, major_rev;

    for (j = 0; j < 3; j++)
	for (i = 0; i < 32; i++)
	    fw[j][i] = 0;

    fwrev_length = strlen(fw_rev);

    result = 0;
    major_rev = 0;

    if (strchr(fw_rev, '.') && strchr(fw_rev, '-')) {
	for (j = 0, k = 0, i = 0; i < fwrev_length; i++) {
	    if ((fw_rev[i] == '.') || (fw_rev[i] == '-')) {
		j++;
		k = 0;
	    }
	    if isdigit(fw_rev[i]) {
		fw[j][k] = fw_rev[i];
		k++;
	    }
	}
	major_rev = (unsigned __int64)(atoi(fw[0]));
	result += (major_rev * (unsigned __int64) 1000000000);
	result += (unsigned __int64)(atoi(fw[1]) * 1000000);
	result += (unsigned __int64)(atoi(fw[2]));
	if ( tolower( fw_rev[0] ) == 'v' )
	    result += ( unsigned __int64 )( 300000 );
	if ( tolower( fw_rev[0] ) == 't' )
	    result += ( unsigned __int64 )( 200000 );
	if ( tolower( fw_rev[0] ) == 'x' )
	    result += ( unsigned __int64 )( 100000 );
    }

    return (result);
}

/*+
 * =========================================================================
 * = lfu_get_options_firmware - copy the separate option firmware	   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *	This routine copies all the option firmware off the device from
 *	which the LFU is booted. The first thing it copies is the 
 *	options file (.TXT or .SYS for mop) and from that file it gets
 * 	the names of the firmware files to copy.
 *
 *
 * FORM OF CALL:
 *  
 *	lfu_get_options_firmware ()
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      char *options_fw_name	- The text name of the options fw file
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */

void lfu_get_options_firmware(char *options_fw_name)
{
    struct SEMAPHORE done;
    struct FILE *fw_dev;
    struct FILE *fw_list;
    struct FILE *fw_in;
    struct ub *ub;

    int i;
    int j;
    int k;
    int repeat_fw_name;
    int fw_type;		/* what type of firmware do this device have */
    int found_floppy;
    char ready[32];
    char read_device[32];	/* if we have to ask the user for the device*/
    char device[32];		/* the device that holds the options firmware*/
    char directory[32];		/* directory in which the firmware resides */
    char buf[32];		/* if we need the /BUF: qualifier for cat */
    char list[32];		/* AS2X00FW.TXT or AS2X00FW.SYS */
    char inlist[32];		/* firmware list if input by user */
    char format[32];		/* ODS2, FAT, or MOPDL */
    char filin[32];		/* name of the firmware file on the media */
    char filout[32];		/* name that firware is copied to */
    char extension[32];		/* may be .IMG or .SYS depending on media */
    char file_string[80];	/* holds a string to be passed to shell_cmd()*/
    char set_command[80];	/* holds a string to be passed to shell_cmd()*/

    device[0] = 0;
    buf[0] = 0;
    directory[0] = 0;

    printf("\n");

/* Get the CD device name.  */

    for (j = 0; j < 15; j++) {

	krn$_wait(&ubs_lock);

	shellsort(ubs, num_ubs, strcmp_ub);

	for (i = 0; i < num_ubs; i++) {
	    ub = ubs[i];
	    if (((ub->inode->name[1] == 'k') || (ub->inode->name[1] == 'q')) && 
	      (ub->inq_len > 1)) {

/* if the device code is for CD; get the device name */

		if ((ub->inq[0] & 0x1f) == 5) {
		    strcpy(device, ub->inode->name);
		    break;
		}
	    }
	}
	krn$_post(&ubs_lock);

/* if we found a cd then break out else sleep abit before trying next device. */

	if (device[0])
	    break;
	else
	    krn$_sleep(1000);
    }

/* If we found a CD, see if the files we want are on it. */

    if (device[0]) {

	strcpy(directory, "[options]");
	sprintf(list, "%s.txt", options_fw_name);
	strcpy(extension, ".SYS");

	printf("Checking %s for the option firmware files. . .\n", device);

	for (i = 0; i < 2; i++) {

	    if (i == 0)			/* Check ISO9660 formatted CD */
		strcpy(format, "iso9660");

	    if (i == 1)			/* Check ODS2 formatted CD */
		strcpy(format, "ods2");


	    sprintf(file_string, "%s:%s%s/%s", format, directory, list, device);

	    if (fw_dev = fopen(file_string, "sr")) {
		lfu_get_options_firmware_helper(fw_dev, options_fw_name, 2048);
		fclose(fw_dev);	
		if (fw_list = fopen(options_fw_name, "rs"))
		    goto COPY_FW;
	    }
	}
    }

/* No CD or the files we want are not on the CD. Check the floppy. */

    printf("Checking dva0 for the option firmware files. . .\n");

    strcpy(device, "dva0");
    sprintf(list, "%s.txt", options_fw_name);
    strcpy(extension, ".SYS");

    if (fw_type = lfu_check_fw_device("dva0")) { 

	found_floppy = 1;

	/* An ODS2 formatted floppy is valid only if it is bootable. */
	if (fw_type == (ODS2_FILETYPE | BOOT_FILETYPE)) {
	    strcpy(format, "ods2");
	    strcpy(directory, "[options]");
	} else {

	    /* A fat formatted floppy. */
	    strcpy(format, "fat");
	    strcpy(directory, "");

	    /* Only look for fwupdate.exe if it is a non bootable floppy */
	    if (!(fw_type & BOOT_FILETYPE)) {
		sprintf(file_string, "%s:fwupdate.exe/%s", format, device);

	 	if (fw_dev = fopen(file_string, "sr"))
		    fclose(fw_dev);	
		else 
		    found_floppy = 0;
	    }
	}


	if (found_floppy) {
	    read_with_prompt(lfu_prompt1, 4, ready, 0, 0, 1);

	    sprintf(file_string, "%s:%s%s/%s", format, directory, list, device);

	    if (fw_dev = fopen(file_string, "sr")) {
		lfu_get_options_firmware_helper(fw_dev, options_fw_name, 2048);
		fclose(fw_dev);	

		if (fw_type & ODS2_FILETYPE)
		    strcpy(buf, "/buf:2000");

		if (fw_list = fopen(options_fw_name, "rs"))
		    goto COPY_FW;

	    } else {

		/*
		 * return if the second floppy was not inserted, but
		 * record the update device in case the user wants to
		 * use any of the scripts that depend on update_dev
		*/
		sprintf( set_command, "set update_dev %s 2> nl", device);
		lfu_call_shell_cmd(set_command);
		sprintf( set_command, "set update_format %s 2> nl", format);
		lfu_call_shell_cmd(set_command);
		printf("Option firmware not copied.\n");
		return;
	    }
	}
    }

/* Files are not on floppy either. Ask for device name. */

    read_with_prompt(lfu_prompt2, 32, device, 0, 0, 1);

    device[strlen(device) - 1] = 0;	/* get rid of <cr><lf> */

/* get rid of colon if it's there */

    if (device[strlen(device) - 1] == ':')
	device[strlen(device) - 1] = 0;

/* If user just typed return then were all done. */

    if (device[0] == 0)
	return;

    /* get rid of leading/trailing whitespace, etc. */
    lfu_strstrip(device);

    /* Define the list file and other default stuff if the device specified
     * was a disk. */
    if (toupper(device[0]) == 'D') {
	sprintf(list, "%s.txt", options_fw_name);
	strcpy(extension, ".SYS");
    }

    /* Define the list file and other default stuff if the device specified
     * was the NET. */
    if ((toupper(device[0]) == 'E') || (toupper(device[0]) == 'F')) {
	strcpy(format, "mopdl");
	strcpy(directory, "");
	sprintf(list, "%s", options_fw_name);
	for (i = 0; list[i]; i++)
	    list[i] = toupper(list[i]);
	strcpy(extension, "");
    }

    printf(lfu_prompt4, list);
    read_with_prompt(" : ", 14, inlist, 0, 0, 1);

    inlist[strlen(inlist)-1] = 0;		/* get rid of <cr><lf> */
    lfu_strstrip(inlist);			/* strip off white space */
    if (inlist[0] != 0)
        strcpy(list, inlist);

    if (toupper(device[0]) == 'D') {

	/* check FLOPPYs differently than CDs */
	if (toupper(device[1]) == 'V') {

	    for (i = 0; i < 2; i++) {

		if (i == 0) {			/* Check fat */
		    strcpy(format, "fat");
		    strcpy(directory, "");
		}
		if (i == 1) {			/* Check ODS2 */
		    strcpy(format, "ods2");
		    strcpy(directory, "[options]");
		    strcpy(buf, "/buf:2000");
		}

		sprintf(file_string, "%s:%s%s/%s", format, directory, list, 
		  device);

		if (fw_dev = fopen(file_string, "sr")) {
		    fclose(fw_dev);	
		    break;
		}
	    }

	/* Handle non FLOPPY types. */
	} else {

	    strcpy(directory, "[options]");

	    for (i = 0; i < 2; i++) {

		if (i == 0)			/* Check ISO9660 formatted CD */
		    strcpy(format, "iso9660");

		if (i == 1)			/* Check ODS2 formatted CD */
		    strcpy(format, "ods2");

		sprintf(file_string, "%s:%s%s/%s", format, directory, list, 
		  device);

		if (fw_dev = fopen(file_string, "sr")) {
		    fclose(fw_dev);	
		    break;
		}
	    }

	}
    }



/* must be uppercase in case it's an infoserver boot */   
    if ((toupper(device[0]) == 'E') || (toupper(device[0]) == 'F')) {
	for (i = 0; list[i]; i++)
	    list[i] = toupper(list[i]);
    }

    sprintf(file_string, "%s:%s%s/%s", format, directory, list, device);
    if (fw_dev = fopen(file_string, "rs")) {
	printf("Copying %s from %s. . .\n", list, device);
	lfu_get_options_firmware_helper(fw_dev, options_fw_name, 2048);
	fclose(fw_dev);	
	fw_list = fopen(options_fw_name, "rs");
    } else {
	/*
	 * Return if the we couldn't find the firmware, but
	 * record the update device in case the user wants to
	 * use any of the scripts that depend on update_dev
	*/
	sprintf( set_command, "set update_dev %s 2> nl", device);
	lfu_call_shell_cmd(set_command);
	sprintf( set_command, "set update_format %s 2> nl", format);
	lfu_call_shell_cmd(set_command);
	printf("\nCouldn't read %s from %s\n", list, device);
	return;
    }

COPY_FW: 

/* Run through options_fw_name file and copy the option firmware. */

    sprintf( set_command, "set update_dev %s 2> nl", device);
    lfu_call_shell_cmd(set_command);
    sprintf( set_command, "set update_format %s 2> nl", format);
    lfu_call_shell_cmd(set_command);

    repeat_fw_name = 0;

    while (1) {
	if (!repeat_fw_name) {

	    if (fgets(filin, 32, fw_list) == NULL)
		break;

	    if (filin[0] == '!')	/* file terminator */
		break;

	    if (fgets(filout, 32, fw_list) == NULL)
		break;

	    lfu_strstrip(filin);
	    lfu_strstrip(filout);
	}

	sprintf(file_string, "%s:%s%s%s%s/%s", format, directory, filin,
	  extension, buf, device);

/* open the fw input file. */

	if (fw_in = fopen(file_string, "rs")) {
	    printf("Copying %s from %s. . .\n", filin, device);

	    lfu_get_options_firmware_helper(fw_in, filout, FW_SIZE);

	    fclose(fw_in);
	    repeat_fw_name = 0;

	} else {
/* 
 * Try opening the fw input file without the default .SYS extension. 
 * This allows for alternative file extensions, such as .ROM or .IMG,
 * to be specified directly in the fw_list file.
 */
	    sprintf( file_string, "%s:%s%s%s/%s", format, directory, filin,
		     buf, device );

	    if (fw_in = fopen(file_string, "rs")) {
		printf("Copying %s from %s. . .\n", filin, device);

		lfu_get_options_firmware_helper(fw_in, filout, FW_SIZE);

		fclose(fw_in);
		repeat_fw_name = 0;
	    } 
	    else {

		if (strcmp_nocase(device, "dva0") == 0) {

		    read_with_prompt(lfu_prompt3, 6, ready, 0, 0, 1);

		    if (strncmp_nocase(ready, "done", 4) == 0) {
			fclose(fw_list);
			return;
		    }
		    repeat_fw_name = 1;
		} else
		    break;
	    }
	}
    }					/* end while */
    fclose(fw_list);
}

/*+
 * =========================================================================
 * = lfu_get_options_firmware_helper - copy firmware helper		   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *	This routine is a helper routine to the get_options_firmware routine
 *	copy the option firmware into memory and assign to the filesystem.
 *	This just like cat except that it uses memzone and not defzone.
 *
 * FORM OF CALL:
 *  
 *	lfu_get_options_firmware_helper (fin, file_out, fwsize)
 *
 * RETURN CODES:
 *
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fin	- file pointer to int the input file.
 *	char *file_out		- output filename
 *	int fwsize		- firmware file read size
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */

static void lfu_get_options_firmware_helper(struct FILE *fin,
  char *file_out, int fwsize)
{
    unsigned char *out_buf;
    unsigned int out_offset;
    struct INODE *ip;

    /* Let the helper do the work... */
    out_offset = lfu_get_firmware_helper(fin, &out_buf, fwsize);


/* create the file */
    if (out_offset) {
	allocinode(file_out, 1, &ip);
	ip->dva = &rd_ddb;
	strcpy(ip->name, file_out);
	ip->attr = ATTR$M_READ;
	ip->len[0] = out_offset;
	ip->loc = out_buf;
	ip->alloc[0] = ip->len[0];
	INODE_UNLOCK(ip);
    }

    return;
}


/*+
 * =========================================================================
 * = lfu_get_firmware_helper - read firmware helper			   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *	This routine is a helper routine to the get_options_firmware_helper
 *      and get_firmware routines. 
 *
 * FORM OF CALL:
 *  
 *	lfu_get_firmware_helper (fin, base, fwsize)
 *
 * RETURN CODES:
 *
 *      length of firmware gotten.
 *       
 * ARGUMENTS:
 *
 *	struct FILE *fin	- file pointer to the input file.
 *	char **base		- pointer to base
 *	int fwsize		- firmware file read size
 *
 * SIDE EFFECTS:
 *
 *      None.
 *
 *--
 */

static int lfu_get_firmware_helper(struct FILE *fin, char **base, int fwsize)
{
    unsigned int bytes_read;
    unsigned int offset;
    unsigned int *bufptr;
    unsigned char *buf;
    unsigned char *out_buf;
    unsigned int blocksize, out_blocksize, out_offset;
    int requested_expansion, status;
    unsigned int expansion;
    struct INODE *ip;
    struct DLL_HEADER *dlp;
    u_char *adr, *buf_adr, *decomp_adr;
    unsigned int file_length = 0;

/* Allocate space for the read buffer */

    blocksize = fwsize;
    buf = (char *) dyn$_malloc(blocksize,
      DYN$K_NOWAIT | DYN$K_SYNC | DYN$K_ZONE, 0, 0, memzone);

/* Allocate space for the output buffer */

    out_blocksize = DEF_ALLOC;
    out_buf = (char *) dyn$_malloc(out_blocksize,
      DYN$K_NOWAIT | DYN$K_SYNC | DYN$K_ZONE | DYN$K_NOOWN, 0, 0, memzone);

    if (!buf || !out_buf) {
	err_printf("lfu_get_firmware_helper: can't allocate enough memory\n");
	return(0);
    }

/* start out at the beginning. */

    out_offset = 0;

    while (!killpending()) {

	bytes_read = fread(buf, blocksize, 1, fin);
	if (bytes_read == 0)
	    break;

	requested_expansion = (out_offset + bytes_read) - out_blocksize;

	if (requested_expansion > 0) {
	    expansion = max(requested_expansion, DEF_EXPAND);
	    out_blocksize += expansion;
	    out_buf = (char *) dyn$_realloc(out_buf, out_blocksize,
	      DYN$K_SYNC | DYN$K_ZONE | DYN$K_NOOWN, 0, 0, memzone);
	}

/* Now presume we have enough room.  Stuff the buffer and bump the
 * length if necessary. */

	memcpy(out_buf + out_offset, buf, bytes_read);
	out_offset += bytes_read;
    }

/* check for a MOP header */

    bufptr = out_buf + 0x58;
    if (*bufptr == 0x504f4d03) {		/* MOP. */
	bufptr = out_buf + 0xc;
	if (*bufptr == 0x35303230) {		/* 0205 */
	    bufptr = out_buf + 0x14;
	    if (*bufptr == 0xffffffff) {	/* -1 */
		/* we do this copying of the data around so that we don't
		 * loose our malloced file pointer. */
		out_offset -= 512;
		bufptr = dyn$_malloc(out_offset,
		  DYN$K_NOWAIT | DYN$K_SYNC | DYN$K_ZONE, 0, 0, memzone);
		memcpy(bufptr, out_buf+512, out_offset);
		free(out_buf);
		out_buf = bufptr;
	    }
	}
    }

/* Is this a compressed file ? */

    bufptr = out_buf;
    if (*bufptr == 'HF01') {
	dlp = out_buf;
	out_offset = dlp->len;
	adr = dyn$_malloc(dlp->len, DYN$K_NOWAIT | DYN$K_SYNC | DYN$K_ZONE |
	  DYN$K_NOOWN, 0, 0, memzone);
	out_buf = adr;
	buf_adr = dyn$_malloc(HF_BUF_SIZE, DYN$K_NOWAIT | DYN$K_SYNC | 
	  DYN$K_ZONE | DYN$K_NOOWN, 0, 0, memzone);
	status = hf$_decompress(dlp, buf_adr, adr, hf$_mem_fetch);
	free(dlp);
	free(buf_adr);
	if (status != 0) {			/* failed */
	    free(adr);
	    return(0);				/* don't say we got anything */
	}
    }

    *base = out_buf;
    file_length = out_offset;

/*
 * all done with the local buffer.
 * We don't free out_buf because it's data is pointed to by the rd_driver.
 */
    free(buf);
    return(file_length);
}


/*+
 * =========================================================================
 * = lfu_call_shell_cmd - set up the semaphore and call the shell command  =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *  
 *	lfu_call_shell_cmd(command_string)
 *
 * RETURN CODES:
 *
 *      None.
 *       
 * ARGUMENTS:
 *
 *      char *command_string - the address of any legal shell command string.
 *
 * SIDE EFFECTS:
 *
 *
 *--
 */

static lfu_call_shell_cmd(char *command_string)
{
    int shell_cmd_ret;
    struct SEMAPHORE done;

    krn$_seminit(&done, 0, "command done");

    shell_cmd(command_string, &done);

    shell_cmd_ret = krn$_wait(&done);
    krn$_semrelease(&done);

    return shell_cmd_ret;
}

/*+
 * =========================================================================
 * = hf$_decompress - local version our famous decompression function.     =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *  This is a localize copy of the same decompression routins found in all
 *  module consoles. We need this to allow us to support decompressing 
 *  compressed fw files.
 *
 * ARGUMENTS:
 *
 * from_adr	= flash rom address for desired segment.
 * buf_adr	= buffer address for decompression
 * load_adr	= address for decompressed code
 * fetch_rtn	= routine to fetch bytes
 *
 * SIDE EFFECTS:
 *
 *
 *--
 */
static int hf$_decompress(u_int from_adr, u_int buf_adr, u_int load_adr, 
 char *fetch_rtn)
{
    int status;

    mem_in_ptr = from_adr;		/* get Flash Rom offset */
    hf_buf_adr = buf_adr;		/* get buffer address */
    loadadr = load_adr;			/* get load address */
    fetch_byte = fetch_rtn;		/* external routine */

    status = get_magic_header();
    if (status != msg_success)
	return status;			/* incorrect magic header */

    mem_out_ptr = loadadr;

    inflate_initialize();

    status = inflate();
    if (status != 0)
	return msg_iec;			/* incorrect end code */

    olength = mem_out_ptr - loadadr;

    status = compute_checksum(loadadr, (olength + 3) / 4);
    if (status != checksum)
	return msg_ics;			/* incorrect checksum */

    return msg_success;			/* success */
}

/*
 * hf$_mem_fetch - read a byte of data from memory
 *	mem_in_ptr = offset from DLB block
 *	mem_in_ptr = incremented to point to next byte
 */

static u_char *hf$_mem_fetch()
{
    char *mem_adr;
    u_char ch;

    mem_adr = mem_in_ptr;
    ch = *mem_adr;
    mem_in_ptr++;
    return ch;
}

/*
 * Read the compressed file header. 
 *
 * The HF01 compressed file header is as follows:
 *	  0/ HF01		identifier
 *	  4/ file spec		filename, 32 chars
 *        8/  "			trucates filename
 *       12/  "
 *       16/  "
 *       20/  "
 *       24/  "
 *       28/  "
 *       32/  "
 *	 36/ olength		length of original uncompressed file.
 *	 40/ otime		creation time of original uncompressed file.
 *	 44/ checksum		checksum of original uncompressed file.
 *       48/ clength		length of compressed file.
 *       52/ beginning of compressed data
 */

static int get_magic_header()
{
    int i;

/* check for magic header */

    i = get_header_info();		/* get magic header */
    if (i != magic_header)
	return msg_imh;

/* get filename, throw it away */

    for (i = 0; i < (32 / 4); i++) {
	get_header_info();
    }

    olength = get_header_info();	/* get original length */

    otime = get_header_info();		/* get original creation time */

    checksum = get_header_info();	/* get checksum */

    clength = get_header_info();	/* get compressed length */

    return msg_success;
}

static int get_header_info()
{
    register int i;
    register u_int info;
    register u_char ch;

    info = 0;
    for (i = 0; i < 4; i++) {
	ch = (*DISPATCH(*fetch_byte))();
	info |= ch << (8 * i);
    }
    return info;
}

static void NEEDBITS(int n)
{
    while (bk < (n)) {
	ReadByte();
	bb |= bytebuf << bk;
	bk += 8;
    }
}

static void DUMPBITS(int n)
{
    bb >>= (n);
    bk -= (n);
}

/* read a UCHAR; return 8 if UCHAR available, abort if not */

static void ReadByte()
{
    if (clength-- <= 0) {
	return;
    }

    bytebuf = (*DISPATCH(*fetch_byte))();
}

static void inflate_initialize()
{
    area = hf_buf_adr;

/* values in order of bit length */

    vblp = (int) area + (_32K * 4);

/* length list for huft_build */

    llistp = (int) vblp + (N_MAX * 4);

/* literal/length and distance code lengths */

    llp = (int) llistp + (288 * 4);

    initial_MemoryHeap = (int) llp + ((286 + 30) * 4);
    MemoryHeap = initial_MemoryHeap;
}

/*
 Given a list of code lengths and a maximum table size, make a set of
 tables to decode that set of codes.  Return zero on success, one if
 the given code set is incomplete (the tables are still built in this
 case), two if the input is invalid (all zero length codes or an
 oversubscribed set of lengths), and three if not enough memory.

    UINT *b;	code lengths in bits (all assumed <= BMAX)
    UINT n;	number of codes (assumed <= N_MAX)
    UINT s;	number of simple-valued codes (0..s-1)
    USHORT *d;	list of base values for non-simple codes
    USHORT *e;	list of extra bits for non-simple codes
    struct huft **t;	result: starting table
    int *m;	maximum lookup bits, returns actual
 */

static int huft_build(b, n, s, d, e, t, m)
    UINT *b;
    UINT n;
    UINT s;
    USHORT *d;
    USHORT *e;
    struct huft **t;
    int *m;
{
    UINT a;				/* counter for codes of length k */
    UINT c[BMAX + 1];			/* bit length count table */
    UINT f;				/* i repeats in table every f */
    int g;				/* maximum code length */
    int h;				/* table level */
    UINT i;				/* counter, current code */
    UINT j;				/* counter */
    int k;				/* number of bits in current code */
    int l;				/* bits per table (returned in m) */
    UINT *p;				/* pointer into c[], b[], or v[] */
    struct huft *q;			/* points to current table */
    struct huft r;			/* table entry for struct assignment */
    struct huft *u[BMAX];		/* table stack */
    int w;				/* bits before this table == (l * h) */
    UINT x[BMAX + 1];			/* bit offsets, then code stack */
    UINT *xp;				/* pointer into x */
    int y;				/* number of dummy codes added */
    UINT z;				/* number of entries in cur table */
    PUCHAR dest;

/* Generate counts for each bit length */

    i = sizeof(c);
    dest = c;
    while (i > 0) {
	*dest++ = 0;
	i--;
    }

    p = b;
    i = n;

    do {
	c[*p++]++;			/* assume all entries <= BMAX */
    } while (--i);

    if (c[0] == n)			/* null input--all zero length codes */
      {
	*t = (struct huft *) 0;
	*m = 0;
	return 0;
    }

/* Find minimum and maximum length, bound *m by those */

    l = *m;
    for (j = 1; j <= BMAX; j++)
	if (c[j])
	    break;

    k = j;				/* minimum code length */
    if ((UINT) l < j)
	l = j;
    for (i = BMAX; i; i--)
	if (c[i])
	    break;

    g = i;				/* maximum code length */
    if ((UINT) l > i)
	l = i;
    *m = l;

/* Adjust last length count to fill out codes, if needed */

    for (y = 1 << j; j < i; j++, y <<= 1)
	if ((y -= c[j]) < 0)
	    return 2;			/* bad input: more codes than bits */
    if ((y -= c[i]) < 0)
	return 2;
    c[i] += y;

/* Generate starting offsets into the value table for each length */

    x[1] = j = 0;
    p = c + 1;
    xp = x + 2;
    while (--i) {			/* note that i == g from above */
	*xp++ = (j += *p++);
    }

/* Make a table of values in order of bit lengths */

    p = b;
    i = 0;
    do {
	if ((j = *p++) != 0)
	    (vblp)[x[j]++] = i;
    } while (++i < n);

/* Generate the Huffman codes and for each, make the table entries */

    x[0] = i = 0;			/* first Huffman code is zero */
    p = vblp;				/* grab values in bit order */
    h = -1;				/* no tables yet--level -1 */
    w = -l;				/* bits decoded == (l * h) */
    u[0] = (struct huft *) 0;		/* just to keep compilers happy */
    q = (struct huft *) 0;		/* ditto */
    z = 0;				/* ditto */

/* go through the bit lengths (k already is bits in shortest code) */

    for (; k <= g; k++) {
	a = c[k];
	while (a--) {

/*
 here i is the Huffman code of length k bits for value *p
 make tables up to required level
 */
	    while (k > w + l) {
		h++;
		w += l;			/* previous table always l bits */

/* compute minimum size table less than or equal to l bits */

		z = (z = g - w) > (UINT) l ? l : z;
					/* upper limit on table size */
		if ((f = 1 << (j = k - w)) > a + 1)
					/* try a k-w bit table */
		  {			/* too few codes for k-w bit table */
		    f -= a + 1;		/* deduct codes from patterns left */
		    xp = c + k;

		    while (++j < z)	/* try smaller tables up to z bits */
		      {
			if ((f <<= 1) <= *++xp)
			    break;	/* enough codes to use up j bits */
			f -= *xp;	/* else deduct codes from patterns */
		    }

		}
		z = 1 << j;		/* table entries for j-bit table */

/* allocate and link in new table */

		if ((q = (struct huft *) i_malloc((z + 1) *
		  sizeof(struct huft))) == (struct huft *) 0) {
		    return 3;		/* not enough memory */
		}
		*t = q + 1;		/* link to list */
		*(t = &(q->v.t)) = (struct huft *) 0;
		u[h] = ++q;		/* table starts after link */

/* connect to last table, if there is one */

		if (h) {
		    x[h] = i;		/* save pattern for backing up */
		    r.b = (UCHAR) l;	/* bits to dump before this table */
		    r.e = (UCHAR)(16 + j);
					/* bits in this table */
		    r.v.t = q;		/* pointer to this table */
		    j = i >> (w - l);	/* (get around Turbo C bug) */
		    u[h - 1][j] = r;	/* connect to last table */
		}
	    }

/* set up table entry in r */

	    r.b = (UCHAR)(k - w);
	    if (p >= vblp + n)
		r.e = 99;		/* out of values--invalid code */
	    else if (*p < s) {
		r.e = (UCHAR)(*p < 256 ? 16 : 15);
					/* 256 is end-of-block code */
		r.v.n = *p++;		/* simple code is just the value */
	    } else {
		r.e = (UCHAR) e[*p - s];
					/* non-simple--look up in lists */
		r.v.n = d[*p++ - s];
	    }

/* fill code-like entries with r */

	    f = 1 << (k - w);
	    for (j = i >> w; j < z; j += f)
		q[j] = r;

/* backwards increment the k-bit code i */

	    for (j = 1 << (k - 1); i & j; j >>= 1)
		i ^= j;
	    i ^= j;

/* backup over finished tables */

	    while ((i & ((1 << w) - 1)) != x[h]) {
		h--;			/* don't need to update q */
		w -= l;
	    }
	}
    }

/* Return (1) if we were given an incomplete table */

    return y != 0 && g != 1;
}

/*
 * flush bytes to output
 *
 *    w = number of bytes to flush
 */

static void flush(w)
    UINT w;
{
    UCHAR *p;

    p = slide;

    while (w > 0) {
	*mem_out_ptr++ = *p++;
	w--;
    }
}

/*
 * inflate (decompress) the codes in a deflated (compressed) block.
 * Return an error code or zero if it all goes ok.
 *
 *	struct huft *tl, *td;	literal/length and distance decoder tables
 *	int bl, bd;		number of bits decoded by tl[] and td[]
 */

static int inflate_codes(tl, td, bl, bd)
    struct huft *tl, *td;
    int bl, bd;
{
    UINT e;				/* table entry flag/number of extra    
					 *           bits */
    UINT n, d;				/* length and index for copy */
    struct huft *t;			/* pointer to table entry */
    UINT ml, md;			/* masks for bl and bd bits */

/* inflate the coded data */

    ml = mask_bits[bl];			/* precompute masks for speed */
    md = mask_bits[bd];

    while (1)				/* do until end of block */
      {
	NEEDBITS((UINT) bl);
	if ((e = (t = tl + ((UINT) bb & ml))->e) > 16)
	    do {
		if (e == 99)
		    return 1;
		DUMPBITS(t->b);
		e -= 16;
		NEEDBITS(e);
	    } while ((e = (t = t->v.t + ((UINT) bb & mask_bits[e]))->e) > 16);

	DUMPBITS(t->b);

	if (e == 16)			/* then it's a literal */
	  {
	    slide[wp++] = (UCHAR) t->v.n;
	    if (wp == WSIZE) {
		flush(wp);
		wp = 0;
	    }
	} else				/* it's an EOB or a length */
	  {

/* exit if end of block */

	    if (e == 15)
		break;

/* get length of block to copy */

	    NEEDBITS(e);
	    n = t->v.n + ((UINT) bb & mask_bits[e]);
	    DUMPBITS(e);

/* decode distance of block to copy */

	    NEEDBITS((UINT) bd);

	    if ((e = (t = td + ((UINT) bb & md))->e) > 16)
		do {
		    if (e == 99)
			return 1;
		    DUMPBITS(t->b);
		    e -= 16;
		    NEEDBITS(e);
		} while ((e = (t = t->v.t + ((UINT) bb & mask_bits[e]))->e) >
		  16);

	    DUMPBITS(t->b);
	    NEEDBITS(e);
	    d = wp - t->v.n - ((UINT) bb & mask_bits[e]);
	    DUMPBITS(e);

/* do the copy */

	    do {
		n -= (e =
		  (e = WSIZE - ((d &= WSIZE - 1) > wp ? d : wp)) > n ? n : e);

		do {
		    slide[wp++] = slide[d++];
		} while (--e);

		if (wp == WSIZE) {
		    flush(wp);
		    wp = 0;
		}
	    } while (n);
	}
    }
    return 0;				/* done */
}

/* "decompress" an inflated type 0 (stored) block. */

static int inflate_stored()
{
    UINT n;				/* number of bytes in block */

/* go to byte boundary */

    n = bk & 7;
    DUMPBITS(n);

/* get the length and its complement */

    NEEDBITS(16);
    n = ((UINT) bb & 0xffff);
    DUMPBITS(16);
    NEEDBITS(16);
    if (n != (UINT)((~bb) & 0xffff))
	return 1;			/* error in compressed data */
    DUMPBITS(16);

/* read and output the compressed data */

    while (n--) {
	NEEDBITS(8);
	slide[wp++] = (UCHAR) bb;
	if (wp == WSIZE) {
	    flush(wp);
	    wp = 0;
	}
	DUMPBITS(8);
    }
    return 0;
}

/*
 * decompress an inflated type 1 (fixed Huffman codes) block.  We should
 * either replace this with a custom decoder, or at least precompute the
 * Huffman tables.
 */

static int inflate_fixed()
{
    int i;				/* temporary variable */
    struct huft *tl;			/* literal/length code table */
    struct huft *td;			/* distance code table */
    int bl;				/* lookup bits for tl */
    int bd;				/* lookup bits for td */

/* set up literal table */

    for (i = 0; i < 144; i++)
	(llistp)[i] = 8;
    for (; i < 256; i++)
	(llistp)[i] = 9;
    for (; i < 280; i++)
	(llistp)[i] = 7;
    for (; i < 288; i++)		/* make a complete, but wrong code set
					 */
	(llistp)[i] = 8;
    bl = 7;
    if ((i = huft_build(llistp, 288, 257, cplens, cplext, &tl, &bl)) != 0)
	return i;

/* set up distance table */

    for (i = 0; i < 30; i++)		/* make an incomplete code set */
	(llistp)[i] = 5;
    bd = 5;
    if ((i = huft_build(llistp, 30, 0, cpdist, cpdext, &td, &bd)) > 1) {
	return i;
    }

/* decompress until an end-of-block code */

    if (inflate_codes(tl, td, bl, bd))
	return 1;			/* failed */

    return 0;
}

/* decompress an inflated type 2 (dynamic Huffman codes) block. */

static int inflate_dynamic()
{
    int i;				/* temporary variables */
    UINT j;
    UINT l;				/* last length */
    UINT m;				/* mask for bit lengths table */
    UINT n;				/* number of lengths to get */
    struct huft *tl;			/* literal/length code table */
    struct huft *td;			/* distance code table */
    int bl;				/* lookup bits for tl */
    int bd;				/* lookup bits for td */
    UINT nb;				/* number of bit length codes */
    UINT nl;				/* number of literal/length codes */
    UINT nd;				/* number of distance codes */

/* read in table lengths */

    NEEDBITS(5);
    nl = 257 + ((UINT) bb & 0x1f);	/* number of literal/length codes */
    DUMPBITS(5);
    NEEDBITS(5);
    nd = 1 + ((UINT) bb & 0x1f);	/* number of distance codes */
    DUMPBITS(5);
    NEEDBITS(4);
    nb = 4 + ((UINT) bb & 0xf);		/* number of bit length codes */
    DUMPBITS(4);
    if (nl > 286 || nd > 30)
	return 1;			/* bad lengths */

/* read in bit-length-code lengths */

    for (j = 0; j < nb; j++) {
	NEEDBITS(3);
	(llp)[border[j]] = (UINT) bb & 7;
	DUMPBITS(3);
    }
    for (; j < 19; j++)
	(llp)[border[j]] = 0;

/* build decoding table for trees--single level, 7 bit lookup */

    bl = 7;
    if ((i = huft_build(llp, 19, 19, 0, 0, &tl, &bl)) != 0) {
	return i;			/* incomplete code set */
    }

/* read in literal and distance code lengths */

    n = nl + nd;
    m = mask_bits[bl];
    i = l = 0;

    while ((UINT) i < n) {
	NEEDBITS((UINT) bl);
	j = (td = tl + ((UINT) bb & m))->b;
	DUMPBITS(j);
	j = td->v.n;
	if (j < 16)			/* length of code in bits (0..15) */
	    (llp)[i++] = l = j;		/* save last length in l */
	else if (j == 16)		/* repeat last length 3 to 6 times */
	  {
	    NEEDBITS(2);
	    j = 3 + ((UINT) bb & 3);
	    DUMPBITS(2);
	    if ((UINT) i + j > n)
		return 1;
	    while (j--)
		(llp)[i++] = l;
	} else if (j == 17)		/* 3 to 10 zero length codes */
	  {
	    NEEDBITS(3);
	    j = 3 + ((UINT) bb & 7);
	    DUMPBITS(3);
	    if ((UINT) i + j > n)
		return 1;
	    while (j--)
		(llp)[i++] = 0;
	    l = 0;
	} else				/* j == 18: 11 to 138 zero length codes
					 */
	  {
	    NEEDBITS(7);
	    j = 11 + ((UINT) bb & 0x7f);
	    DUMPBITS(7);
	    if ((UINT) i + j > n)
		return 1;
	    while (j--)
		(llp)[i++] = 0;
	    l = 0;
	}
    }

/* build the decoding tables for literal/length and distance codes */

    bl = LBITS;
    if ((i = huft_build(llp, nl, 257, cplens, cplext, &tl, &bl)) != 0) {
	return i;			/* incomplete code set */
    }
    bd = DBITS;
    if ((i = huft_build(llp + nl, nd, 0, cpdist, cpdext, &td, &bd)) != 0) {
	return i;			/* incomplete code set */
    }

/* decompress until an end-of-block code */

    if (inflate_codes(tl, td, bl, bd))
	return 1;			/* failed */

    return 0;
}

/* decompress an inflated block */

static int inflate_block(int *last_block)
{
    UINT block_type;			/* block type */

/* reset huft storage */

    MemoryHeap = initial_MemoryHeap;

/* read in last block bit */

    NEEDBITS(1);
    *last_block = (int) bb & 1;
    DUMPBITS(1);

/* read in block type */

    NEEDBITS(2);
    block_type = (UINT) bb & 3;
    DUMPBITS(2);

/* inflate that block type */

    if (block_type == 2)
	return inflate_dynamic();
    if (block_type == 0)
	return inflate_stored();
    if (block_type == 1)
	return inflate_fixed();

/* bad block type */

    return 2;
}

/* decompress an inflated entry */

static int inflate()
{
    int last_block;			/* last block flag */
    int result;				/* result code */

/* initialize window, bit buffer */

    wp = 0;
    bk = 0;
    bb = 0;

/* decompress until the last block */

    do {
	if ((result = inflate_block(&last_block)) != 0) {
	    return result;
	}
    } while (!last_block);

/* flush out slide */

    flush(wp);

    return 0;
}

/* ------------------------------- */

static u_char *i_malloc(int size)
{
    PUCHAR base;
    UINT newBase;

    base = (PUCHAR) MemoryHeap;

/* Bump up past the allocation */

    newBase = (UINT)(base + size);

/* round up to next quadword */

    if (newBase & 7 != 0) {
	newBase = (newBase & 7) + 8;
    }

    MemoryHeap = (void *) (newBase);

    return base;
}

/*+
 * =========================================================================
 * = lfu_check_fw_device - check the fw device for fat or ods2		   =
 * =========================================================================
 *
 * OVERVIEW:
 *
 *
 * FORM OF CALL:
 *  
 *	lfu_check_fw_device(device)
 *
 * RETURN CODES:
 *
 *	0 - no disk
 *	FAT_FILETYPE
 *	ODS2_FILETYPE
 *	(FAT_FILETYPE | BOOT_FILETYPE)
 *	(ODS2_FILETYPE | BOOT_FILETYPE)
 *       
 * ARGUMENTS:
 *
 *      char *device - the fw device name to check
 *
 * SIDE EFFECTS:
 *
 *
 *--
 */
static int lfu_check_fw_device(char *device)
{
    unsigned char buffer[512];
    struct FILE *fp;
    int file_type;
    int i;

    file_type = 0;
    fp = fopen( device, "sr" );

    /* No device available.. */
    if( !fp ) {
	return( 0 );
    }

    /* Can't read device. */
    if( fread( buffer, 512, 1, fp ) != 512 ) {
	return( 0 );
    }

    /* If this has a boot block then check for how the boot block was
     * made. */
    if( boot_check_boot_block( buffer ) == msg_success ) {
	file_type = FAT_FILETYPE | BOOT_FILETYPE;
	for (i = 0; i < 50; i++) {
	    if (buffer[i]) {
		file_type = ODS2_FILETYPE | BOOT_FILETYPE;
		break;
	    }
	}
    } else {
	/* This is what is found on fat formatted floppies if inited from
	 * Windows 3.1 or Windows NT. */
	if ((*(int *)&buffer[0] == 0x4D903CEB) && 
	  (*(int *)&buffer[4] == 0x534F4453)) {
	    file_type = FAT_FILETYPE;
	} else {
	    /* This is what is found on fat formatted floppies if inited from
	     * Windows 95. */
	    if ((buffer[0] == 0xEB) && (*(int *)&buffer[508] == 0xAA550000)) {
		file_type = FAT_FILETYPE;
	    } else {
		file_type = ODS2_FILETYPE;
	    }
	}
    }
    fclose (fp);
    return (file_type);
}

