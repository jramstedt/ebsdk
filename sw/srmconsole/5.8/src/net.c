/* file:	net.c
 *
 * Copyright (C) 1990, 1991, 1995 by
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
 *      Cobra Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *	Net command.
 *
 *  AUTHORS:
 *                         
 *	John DeNisco
 *
 *  CREATION DATE:
 *  
 *      13-March-1991
 *
 *  MODIFICATION HISTORY:
 *
 *	tna     11-Nov-1998     Conditionalized for YUKONA
 *
 *	jcc	25-Jan-1995	Conditionalize for Cortex
 *
 *	dwn	14-Sept-1995	Added -rsa -env 
 *
 *	swt	28-Jun-1995	Add Noritake platform.
 *
 *	rbb	14-Mar-1995	Conditionalize for EB164
 *
 *	er	27-Dec-1994	Conditionalize for EB66
 *
 *	er	27-Oct-1994	Conditionalize for SD164
 *
 *	er	08-Jul-1994	Conditionalize for EB64+
 *
 *	jmp	11-29-93	Add avanti support
 *
 *	pel	25-Jun-1993	Add net -stop functionality to stop driver &
 *				change -ri qualifier to -start
 *
 *	joes 	15-Apr-1993	Conditionalize for medulla
 *
 *	pel	15-Jan-1993	Conditionalize for morgan
 *
 *	hcb	08-jan-1993	#if OPTIONAL out -rb code
 *
 *	phk	18-Nov-1991	Modify -id, -l0, -rb
 *                              Add -da, -csr
 *
 *	jad	12-Nov-1991	Add some code for cobra init
 *
 *	pel	04-Nov-1991	chg ez_init interface to compile
 *
 *	jds	16-Aug-1991	Changed for platform.
 *
 *	jad	14-March-1991	Massive cleanup
 *
 *	hcb	13-March-1991	Remove build_loop_list
 *
 *	jad	13-March-1991	Initial entry.
 *  
 *--
 */

/*Include files*/
#include "cp$src:platform.h"
#include "cp$src:prdef.h"		/*Processor definitions*/
#include "cp$src:stddef.h"
#include "cp$src:kernel_def.h"		/*Common definitions*/
#include "cp$src:common.h"		/*Kernel definitions*/
#include "cp$src:msg_def.h"		/*Message definitions*/
#include "cp$src:parse_def.h"		/*Parser definitions*/
#include "cp$src:net_def.h"		/*Net command definitions*/
#include "cp$src:ni_env.h"		/*NI environment vars*/
#include "cp$src:ni_dl_def.h"		/*NI datalink definitions*/
#include "cp$src:mop_def.h"		/*MOP definitions*/
#include "cp$src:mop_counters.h"	/*MOP counters*/
#include "cp$src:mb_def.h"		/*MOP block defs*/
#include "cp$src:dynamic_def.h"		/*Memory allocation defs*/
#include "cp$src:ev_def.h"		/*Environment vars*/
#include "cp$inc:kernel_entry.h"
#include "cp$inc:prototypes.h"

/*Debug stuff*/
void jad(int argc, char *argv[])
{
}
;

/*Local constants*/

#define MOPRC 0

#define TRUE  1
#define FALSE 0

/*External routine definitions*/
extern int fclose(struct FILE *);
extern struct FILE *fopen(char *, char *);

extern int null_procedure();

#if !MODULAR
extern void ndl_get_nisa(struct FILE *fp, unsigned char *sa);
extern int moplp_wait(struct MB *mp, struct NI_ENV *ev, int secs);
extern void mop_control_t(struct NI_GBL *np);
extern int mop_loop_requester(struct FILE *fp, int version);
extern void mop_read_env(struct NI_ENV *ev, char *name);
extern void mop_send_loop_fwd(struct FILE *fp, unsigned char *dest,
  unsigned short size, unsigned char *fwd, int ver,
  unsigned short rec_number, int patt);
extern void mop_send_rem_boot(struct FILE *fp, unsigned char *dest,
  unsigned *password);
extern void print_enet_address(char *name, unsigned char *enet);
#endif

extern void ez_dump_nicsrs(struct NI_GBL *np);

/*Routine definitions*/
void net(int argc, char *argv[]);
void net_cm(struct FILE *fp, struct NI_DL *dl, char *mode);
void net_i(char *port_name);
void net_se(char *port_name);
void net_l(char *port_name, char *load_file, char *load_address);

/*+
 * ============================================================================
 * = net - Access a network port and execute MOP maintenance functions.       =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Using the specified port, perform some maintenance operations.
 *	If no port is specified, the default port is used.
 *
 *	The net command performs basic MOP operations, such as, loopback,
 *	request IDs, and remote file loads.  The net command also provides
 *	the means to observe the status of a network port.  Specifically,
 *	the 'net -s' will display the current status of a port including
 *	the contents of the MOP counters.  This is useful for monitoring
 *	port activities and trying to isolate network failures.
 *
 *	To get a port's Ethernet station address, use 'net -sa'.
 *  
 *   cmd fmt:     routine abbrev attributes stack command
 *
 *   COMMAND FMT: net 3 HZ 0 net
 *  
 *   COMMAND FORM:
 *  
 *      net ( [-s] [-sa] [-rsa] [-env] [-ri] [-ic] [-id] [-l0] [-l1] [-rb] 
 *	      [-csr] [-els] [-kls] [-start] [-stop] [-cm <mode_string>] 
 *	      [-da <node_address>] [-l <file_name>] [-lw <wait_in_secs>]
 *	      [-sv <mop_version>] <port_name> ) 
 *  
 *   COMMAND TAG: net 0 RXBZ net
 *  
 *   COMMAND ARGUMENT(S):
 *
 *	<port_name> - Specifies the Ethernet port on which to operate.
 *
 *   COMMAND OPTION(S):
 *
 *	-s    - Display port status information including MOP counters.
 *
 *	-sa   - Display the port's Ethernet station address.
 *
 *	-rsa  - Display TMS380 station address in "bit reversed" format
 *
 *	-env  - Display file2dev() device name conversion
 *
 *	-ri   - Reinitialize the port drivers.
 *
 *	-start - Start the port drivers.
 *
 *	-stop - Stop the port drivers.
 *
 *	-ic   - Initialize the MOP counters.
 *
 *	-id   - Send a MOP Request ID to the specified node.
 *		Uses -da to specify the destination address.
 *
 *	-l0   - Send an Ethernet loopback to the specified node.
 *		Uses -da to specify the destination address.
 *
 *	-l1   - Do a MOP loopback requester.
 *
 *	-rb   - Send a MOP V4 boot message to remote boot a node.
 *		Uses -da to specify the destination address.
 *
 *	-csr  - Displays the values of the Ethernet port CSRs.
 *
 *	-els  - Enable the extended DVT loop service.
 *
 *	-kls  - Kill the extended DVT loop service.
 *
 *	-cm <mode_string> - Change the mode of the port device.
 *		The mode string may be any one of the following abbreviations.
 *#o
 *			      o	nm =  Normal mode
 *			      o	in =  Internal Loopback
 *			      o	ex =  External Loopback
 *			      o	nf =  Normal Filter
 *			      o	pr =  Promiscious
 *			      o	mc =  Multicast
 *			      o	ip =  Internal Loopback and Promiscious
 *			      o	fc =  Force collisions
 *			      o	nofc = Don't Force collisions
 *			      o	df = Default 
 *			      o	tp =  Twisted pair
 *			      o	fd_tp =  full duplex, Twisted pair
 *			      o	aui = AUI
 *#
 *	-da <node_address> - Specifies the destination address of a node
 *		to be used with -l0, -id, or -rb, options.
 *
 *	-l <file_name> - Attempt a MOP load of the file name.
 *
 *	-lw <wait_in_secs> - Wait the number of seconds specified for the 
 *		loop messages from the -l1 option to return.  If the 
 *		messages don't return in the time period, an error message is
 *		generated.
 *
 *	-sv <mop_version> - Set the prefered MOP version number for operations.
 *		Legitimate values are 3 or 4.
 *
 *   COMMAND EXAMPLE(S):
 *
 *	Display the local Ethernet ports' station address.
 *~
 *      >>>net -sa
 *      -eza0:  08-00-2b-1d-02-91
 *      >>>net -sa ezb0
 *      -ezb0:  08-00-2b-1d-02-92
 *~
 *	Display the eza0 port status, including the MOP counters.
 *~
 *      >>>net -s
 *      
 *	DEVICE SPECIFIC:
 * 	 TI: 203 RI: 42237 RU: 4 ME: 0 TW: 0 RW: 0 BO: 0
 * 	 HF: 0 UF: 0 TN: 0 LE: 0 TO: 0 RWT: 39967 RHF: 39969  TC: 54
 *
 *	PORT INFO:
 *	tx full: 0 tx index in: 10 tx index out: 10
 *	rx index in: 11
 *
 *	MOP BLOCK:
 * 	 Network list size: 0
 *
 *	MOP COUNTERS:
 *	Time since zeroed (Secs): 2815
 *
 *	TX:
 * 	 Bytes: 116588 Frames: 204
 *  	 Deferred: 2 One collision: 52 Multi collisions: 14
 *	TX Failures:
 *	 Excessive collisions: 0 Carrier check: 0 Short circuit: 0
 *	 Open circuit: 0 Long frame: 0 Remote defer: 0
 *	 Collision detect: 0
 *
 *	RX:
 *	 Bytes: 116564 Frames: 194
 *	 Multicast bytes: 13850637 Multicast frames: 42343
 *	RX Failures:
 *	 Block check: 0 Framing error: 0 Long frame: 0
 *	 Unknown destination: 42343 Data overrun: 0 No system buffer: 22
 *	 No user buffers: 0
 *	>>>
 *~
 *   COMMAND REFERENCES:
 *
 *	netexer, nettest
 *
 * FORM OF CALL:
 *
 *	net (argc,argv); 
 *
 * RETURNS:
 *
 *	int Status - msg_success or FALIURE.
 *
 * ARGUMENTS:
 *
 *	int argc - Number of arguments specified.
 *	char *argv[] - List of arguments.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void net(int argc, char *argv[])
{
    struct FILE *pf;			/*Port file pointer*/
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct NI_DL *dl;			/*Pointer to the datalink*/
    struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/
    struct MB *mp;			/*Pointer to the MOP block*/
    struct NI_ENV *ev;			/*Pointer to the environment
					 * variables*/
    struct QSTRUCT qual[NET$K_ARG_MAX];	/*Qualifiers returned from qscan*/
    char port[MAX_NAME];		/*Port file name*/
    int i,j,k;				/*Index into arguments*/
    int load_address;			/*Set the load address*/
    char s[200];			/*String holder*/
    unsigned char src[6];		/*Station address*/
    unsigned char dest[6];		/*Destination address*/
    unsigned char da = FALSE;
    unsigned pwd[3];			/*Remote boot password*/
    int version = MOP$K_VERSION_3;	/*Mop version number*/
    struct EVNODE *evp;			/*Pointer to the evnode*/
    unsigned char tmp[32], temp;
    unsigned status;

/*First make sure we have enough arguments*/

    if (argc < 2) {
	err_printf(msg_insuff_params);
	return;
    }

/*Look at our qualifiers*/
    status =
      qscan(&argc, argv, "-",
"i sa s l1 %dsv %scm ic id %sl l0 %xla ri %srb els kls %sda csr %dlw start stop rsa env",
      qual);
    if (status != msg_success) {
	err_printf(status);
	return;
    }

/*Try and open the port*/
/*Check and see if we should use the default*/
/*Initiliaze the index*/
    if (argc < 2) {
#if TURBO	/* must have parameters */
	err_printf(msg_missing_device);
        return;
#endif
#if MEDULLA || RAWHIDE || CORTEX || YUKONA
	strcpy(port, "ewa0");		/* Tulip */
#else
	strcpy(port, "eza0");
#endif
    } else {

	/*Copy the port then eat the argument*/
	strncpy(port, argv[1], MAX_NAME);
	argc--;
    }

/*Execute the commands these command should be first*/

/*The -i qualifier*/
    if (qual[NET$K_I].present)
	net_i(port);

/*The -la qualifier*/
/*Set the load address*/
    if (qual[NET$K_LA].present)
	load_address = qual[NET$K_LA].value.integer;
    else
	load_address = MOP$K_MEM_OFFSET;

/*The -l qualifier*/
    if (qual[NET$K_L].present) {

	/*Do a load*/
	net_l(port, qual[NET$K_L].value.string, load_address);
    }

/*If we have some more arguments open the port*/
    pf = 0;
    for (i = (NET$K_I + 1); i < NET$K_ARG_MAX; i++)

	/*Check for a qualifier present*/
	if (qual[i].present) {
	    pf = fopen(port, "sr+");
	    if (pf) {

		/*Get some pointers*/
		np = (struct NI_GBL *) pf->ip->misc;
		dl = (struct NI_DL *) np->dlp;
		mc = (struct MOP$_V4_COUNTERS *) np->mcp;
		mp = (struct MB *) np->mbp;
		ev = (struct NI_ENV *) np->enp;
	    } else
		return;
	    break;
	}

/*Handle the rest of the qualifiers*/
/*The -stop qualifier*/
    if (qual[NET$K_STOP].present) {

	/*Halt the port driver*/
	(*pf->ip->dva->setmode)(DDB$K_STOP);
    }

/*The -start qualifier*/
    if (qual[NET$K_START].present) {

	/*Start the port*/
	(*pf->ip->dva->setmode)(DDB$K_START);
    }

/*The -ri qualifier*/
    if (qual[NET$K_RI].present) {

	/*Reinit the port*/
	(dl->cb->ri)(pf);
    }

    /* The -sa qualifier
    */
    if (qual[NET$K_SA].present) 
    	{
	/* Get and display the station address
    	*/
	ndl_get_nisa(pf, src);
	sprintf(s, "-%s: ", port);
	print_enet_address(s, src);
        }

    /* The -env qualifier.  Prints environment variable device name 
    */
    if (qual[NET$K_ENV].present) 
    	{
        char dname[EV_VALUE_LEN+1];
    	file2dev( pf->ip->name, dname, 1 );
        pprintf ("%s\n", dname);
    	}

    /* The -rsa qualifier.  TMS380 "Bit Reversed" station address format
    */
    if (qual[NET$K_RSA].present) 
    	{
	ndl_get_nisa(pf, src);

    	/* convert to "bit reversed" format
    	*/
        for (i=0; i<6; i++)
            {
            temp   = src[i];
            src[i] = 0;

            for (j=0,k=7; j<8; j++,k--)
                src[i] |= (( (temp >> j) & 0x1) << k);
            }

	sprintf(s, "-%s: ", port);
	print_enet_address(s, src);
        }

/*The -ic qualifier*/
    if (qual[NET$K_IC].present)

	/*Init the mop counters*/
	init_mop_counters(mc);

/*The -s qualifier*/
    if (qual[NET$K_S].present) {

	/*Display some port status*/
	(dl->cb->st)(np);
#if MEDULLA||TURBO||SABLE||RAWHIDE||AVANTI||MIKASA||K2||MTU||CORTEX||YUKONA||TAKARA||APC_PLATFORM||REGATTA||FALCON

	  /*Display some MOP status*/
	  mop_control_t(np);
#endif
    }

/*The -sv qualifier*/
    if (qual[NET$K_SV].present)

	/*Set the version*/
	if (qual[NET$K_SV].value.integer < 4)
	    version = MOP$K_VERSION_3;
	else
	    version = MOP$K_VERSION_4;

/*The -cm qualifier*/
    if (qual[NET$K_CM].present)
	net_cm(pf, dl, qual[NET$K_CM].value.string);

/*The -l1 qualifier*/
    if (qual[NET$K_L1].present)

	/*Start the requester*/
	mop_loop_requester(pf, version);

/*The -da qualifier*/
    if (qual[NET$K_DA].present)

	/*convert the destination address*/
	if ((convert_enet_address(dest, qual[NET$K_DA].value.string)) ==
	  msg_success)
	    da = TRUE;

/*The -id qualifier*/
    if (qual[NET$K_ID].present & da)

	/*Send a reqid*/
	mop_send_req_id(pf, dest, version);

/*The -l0 qualifier*/
    if (qual[NET$K_L0].present && da) {

	/*Read the environment variable*/
	evp = (void *) malloc_noown(sizeof(struct EVNODE));
	sprintf(tmp, "%4.4s_loop_size", port);

	/*Send a loop forward*/
	if (ev_read(tmp, &evp, 0) == msg_success)
	    mop_send_loop_fwd(pf, dest, evp->value.integer, mp->sa, version, 0,
	      MOP$K_INCREMENTING);
	else
	    mop_send_loop_fwd(pf, dest, 60, mp->sa, version, 0,
	      MOP$K_INCREMENTING);

	/*Free the environment variable*/
	free(evp);
    }

#if EXTRA && MOPRC
/*The -rb qualifier*/
    if (qual[NET$K_RB].present && da) {

	/*Send the remote boot*/

	/*extract the password information*/

	if (strlen(qual[NET$K_RB].value.string) == 20 &&
	  qual[NET$K_RB].value.string[2] == '-' &&
	  qual[NET$K_RB].value.string[11] == '-') {
	    memcpy(tmp, &qual[NET$K_RB].value.string[0], 2);
	    tmp[2] = '\0';
	    pwd[2] = xtoi(tmp);
	    memcpy(tmp, &qual[NET$K_RB].value.string[3], 8);
	    tmp[8] = '\0';
	    pwd[1] = xtoi(tmp);
	    memcpy(tmp, &qual[NET$K_RB].value.string[12], 8);
	    tmp[8] = '\0';
	    pwd[0] = xtoi(tmp);

	    mop_send_rem_boot(pf, dest, pwd);
	} else
	    err_printf(msg_rb_inc_pwd);
    }
#endif
#if EXTRA  || TURBO || RAWHIDE ||TAKARA || APC_PLATFORM
/*The -csr qualifier*/
    if (qual[NET$K_CSR].present)

	/*Display the csrs*/
	(dl->cb->dc)(np);
#endif

/*The -els qualifier*/
    if (qual[NET$K_ELS].present)

	/*Reset the kill server flag*/
	mp->lp.kill_server = FALSE;

/*The -kls qualifier*/
    if (qual[NET$K_KLS].present)

	/*Reset the kill server flag*/
	mp->lp.kill_server = TRUE;

/*The -lw qualifier*/
    if (qual[NET$K_LW].present)

	/*Wait for a loop count*/
	moplp_wait(mp, ev, qual[NET$K_LW].value.integer);

/*Close the file*/
    if (pf)
	fclose(pf);
}

/*+
 * ============================================================================
 * = net_i - Implimentation of the -i qualifier for the net command.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This is the implementation of the -i qualifier for the net command.
 *	This command will initialize a given port. Done mainly with
 *	conditional compilation.
 *
 * FORM OF CALL:
 *
 *	net_i (port_name); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	char *port_name - Name of the port to be initialized.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void net_i(char *port_name)
{
#if 0
/***********************************************************/
/*This is a debug option should be compiled out for release*/
/***********************************************************/
    ez_init();
#endif
#if 0
    struct FILE *pf, *mp;		/*Port file pointer*/
    char *pkt, c[10];
    int len, size;


/*Open the files*/

    mp = fopen("msg", "r+");
    pf = fopen(port_name, "r+");
    pkt = ndl_allocate_pkt(pf->ip, 0);

/*read the size*/
    fread(c, 1, 5, mp);
    c[5] = 0;
    size = atoi(c);

/*Read the message*/
    len = 0;
    while (fread(c, 1, 3, mp)) {

	/*If its not null copy the bytes*/
	if (c[0] && c[1] && c[2]) {
	    c[2] = 0;
	    pkt[len++] = xtoi(c);
	}
    }

    pprintf("Message Transmitted\n");
    pprint_pkt(pkt, size);
    fwrite(pkt, 1, size, pf);
#endif
}

/*+
 * ============================================================================
 * = net_cm - Implimentation of the -cm qualifier for the net command.        =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This is the implementation of the -cm qualifier for the net command.
 *	This command will change the mode of a given port.
 *
 * FORM OF CALL:
 *
 *	net_cm (pf,dl,mode); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	struct FILE *pf - Pointer to the port file.
 *	struct NI_DL *dl - Pointer to the datalink structure.
 *	char *mode - Pointer to the mode string.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void net_cm(struct FILE *pf, struct NI_DL *dl, char *mode)
{
char c;
    trace("net_cm\n", 0);

    c = mode[0];

/*Change mode to normal operating mode*/
    if (c == 'n')
	(dl->cb->cm)(pf, NDL$K_NORMAL_OM);
/*Change mode to internal loopback*/
    if (c == 'i')
	(dl->cb->cm)(pf, NDL$K_INT);
/*Change mode to external loopback*/
    if (c == 'e')
	(dl->cb->cm)(pf, NDL$K_EXT);

#if SABLE||MEDULLA||ALCOR||AVANTI||MIKASA||K2||RAWHIDE||MTU||CORTEX || YUKONA||TAKARA||APC_PLATFORM
/*Change mode to all multicast*/
    if (c == 't')
	(dl->cb->cm)(pf, NDL$K_TP);
/*Change mode to internal and promiscious*/
    if (c == 'a')
	(dl->cb->cm)(pf, NDL$K_AUI);
    if (c == 'b')
	(dl->cb->cm)(pf, NDL$K_BNC);
    if (c == 'f')
	(dl->cb->cm)(pf, NDL$K_TP_FD);
#endif
}

/*+
 * ============================================================================
 * = net_l - Implimentation of the -l qualifier for the net command.          =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	This is the implementation of the -l qualifier for the net command.
 *	This command will perform a network load on a given string.
 *
 * FORM OF CALL:
 *
 *	net_l (file_specifier); 
 *
 * RETURNS:
 *
 *	None.
 *
 * ARGUMENTS:
 *
 *	char *port_name - Port file to be used for the load.
 *	char *load_file - Name of the file to be loaded.
 *	int *load_address - Load address.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

void net_l(char *port_name, char *load_file, char *load_address)
{
    struct DL_FILE_BLOCK *dl;		/*Pointer to the dump load file block*/
    struct NI_GBL *np;			/*Pointer to NI global database*/
    struct MB *mp;			/*Pointer to the MOP block*/
    struct FILE *fp;
    char file_specifier[100];
    int len, dpl;

    trace("net_l\n", 0);

/*Make the file*/
    sprintf(file_specifier, "mopdl:%s/%s", load_file, port_name);

/*Open the file*/
    printf(msg_net_001, file_specifier);
    printf(msg_net_002, load_address);
    fp = fopen(file_specifier, "r");
    if (fp) {

	/*Load the file*/
	dpl = 0;
	while ((len = fread(load_address + dpl, 2048, 1, fp)) != 0)
	    dpl += len;

	/*If we didn't load maybe the user wants to try again*/
	if (fp->status != msg_success)
	    err_printf(msg_net_003);
	else {
	    dl = (struct DL_FILE_BLOCK *) fp->misc;
	    np = (struct NI_GBL *) dl->fp->ip->misc;
	    mp = (struct MB *) np->mbp;
	    printf(msg_net_004);
	    printf(msg_net_005, mp->dl.offset + load_address);
	    printf(msg_net_006, mp->dl.image_size);
	    printf(msg_net_007, mp->dl.tgt_sys_name);
	    printf(msg_net_008, mp->dl.tgt_sys_addr[0], mp->dl.tgt_sys_addr[1],
	      mp->dl.tgt_sys_addr[2], mp->dl.tgt_sys_addr[3],
	      mp->dl.tgt_sys_addr[4], mp->dl.tgt_sys_addr[5]);
	    printf(msg_net_009, mp->dl.host_sys_name);
	    printf(msg_net_010, mp->dl.host_sys_addr[0],
	      mp->dl.host_sys_addr[1], mp->dl.host_sys_addr[2],
	      mp->dl.host_sys_addr[3], mp->dl.host_sys_addr[4],
	      mp->dl.host_sys_addr[5]);
	}

	/*Close the file*/
	fclose(fp);
    }
}
