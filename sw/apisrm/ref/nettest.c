/* file:	nettest.c
 *                                                                       
 * Copyright (C) 1992, 1994 by
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
 *      This is a network diagnostic.   It will test the ez ports in
 *	internal loopback, external loopback or normal network mode.
 *
 *  AUTHORS:
 *                  
 *      Harold Buckingham               
 *                     
 *  CREATION DATE:
 *                                                          
 *      19-Mar-1992    
 *
 *  MODIFICATION HISTORY:                                    
 *                                     
 *      hcb	29-Sep-1994	Fix resetting of the mode environment
 *				variable for ew(TULIP).
 *				Make ew default for Sable. Make work on 
 *				up to 8 devices.
 *
 *      hcb	21-May-1993	Remove sup_dev. Allow to run on all devices.
 *
 *      hcb	18-Aug-1992	Fix halt on error code
 *
 *      jad	24-Jun-1992	Change the wait time
 *                                     
 *      hcb	18-May-1992	check for msg_killed status from moplp_wait
 *
 *      hcb	13-May-1992	Only allow one nettest per port
 *
 *      hcb	08-May-1992	Add nc(no change) to mode option
 *
 *      hcb	29-Apr-1992	Change default to external loopback
 *
 *      hcb	23-Apr-1992	Add -mode(mode), -f(loop file), -s(sleep)
 *				qualifiers.
 *
 *      hcb	15-Apr-1992	Add code to do useful work
 *
 *      hcb	19-Mar-1992	initial entry                    
 *                  
 *--                   
 */                                                

#include	"cp$src:platform.h"
#include 	"cp$src:prdef.h"                                   
#include	"cp$src:kernel_def.h"          
#include	"cp$src:parse_def.h"    
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include 	"cp$src:ctype.h"                                           
#include      	"cp$src:stddef.h"
#include      	"cp$src:common.h"                 
#include "cp$inc:prototypes.h"
#include 	"cp$src:diag_def.h"                    
#include      	"cp$src:diag_groups.h"
#include 	"cp$src:msg_def.h"                               
#include 	"cp$src:ni_env.h"	                           
#include 	"cp$src:ni_dl_def.h"	  
#include 	"cp$src:mop_def.h"                               
#include 	"cp$src:mb_def.h"                              
#include 	"cp$src:mop_counters.h"                               
#include 	"cp$src:ev_def.h"
#include	"cp$inc:kernel_entry.h"

#define DEBUG1 0                                
#define DEBUG 0
                                                                 
#define	QM      NUM_QUALS    	/* Mode to set the TGEC */
#define	QF      NUM_QUALS + 1	/* File which contains the list of nodes */
#define	QW      NUM_QUALS + 2	/* Wait, time to delay between test passes */
#define	QTO     NUM_QUALS + 3	/* Time allowed before timeout */
#define	QSV     NUM_QUALS + 4	/* MOP version to use */
#define MORE_QUALS 5
#define NETTEST_DEF_TIMEOUT 2
#define NETTEST_MAX_PORTS 8
                                                   
extern struct SEMAPHORE spl_kernel;
extern int diag$r_evs;

void		nettest_cleanup(void);                         

static struct DIAG_DIS_TABLE dispatch_table[] = { {NULL, NULL} };           
typedef struct externals_struct { struct FILE	*pf[NETTEST_MAX_PORTS];
 			      	  long int	mode[NETTEST_MAX_PORTS];
			      	  int		change_mode;
				} externals;
                                                 
/*+                                                                  
 * ============================================================================
 * = nettest - Test the network ports using MOP loopback.                     =
 * ============================================================================
 *                                                
 * OVERVIEW:                                            
 *                                                                           
 *      This is a network test.   It can test the ez ports in internal 
 *	loopback, external loopback or live network loopback mode.
 *                                                                           
 *      Nettest contains the basic options to allow the user to run MOP
 *	loopback tests.  It is assumed that nettest will be included in a
 *	script for most applications.  Many environment variables can be
 *	set to customize nettest.  These may be set from the console before
 *	nettest is started.  Listed below are the environment variables,
 *	a brief description, and their default values.
 *
 *	Note: Each variable name is proceded by eza0_ or ezb0_ to specify
 *	the desired port.                        
 *
 *      Experienced users my also desire to change other network driver
 *	characteristics by modifying the port mode.  Refer to the -mode option.
 *                                                        
 *   cmd fmt:     routine abbrev attributes stack command
 *
 *   COMMAND FMT: nettest 6 HZ 0 nettest
 *                                               
 *   COMMAND FORM:                                             
 *                                                  
 *	nettest ( [-f <file>] [-mode <port_mode>] [-p <pass_count>]
 *                [-sv <mop_version>] [-to <loop_time>] [-w <wait_time>] 
 *		  [<port>] )
 *                                                                           
 *   COMMAND TAG: nettest 0 RXBZ nettest
 *                                               
 *   COMMAND ARGUMENT(S):     
 *                                                                 
 *	<port> - Specifies the Ethernet port on which to run the test.
 *                                             
 *   COMMAND OPTION(S):                                                
 *                                                             
 *     -f <file> - Specifies the file containing the list of network station
 *		addresses to loop messages to.                         
 *		The default file name is lp_nodes_eza0 for port eza0.
 *		The default file name is lp_nodes_ezb0 for port ezb0.
 *		The files by default have their own station address.		
 *                                                 
 *     -mode <port_mode> - Specifies the mode to set the port adapter (TGEC).
 *		The default is 'ex' (external loopback).
 *		Allowed values are:
 *#o
 *		o df : default, use environment variable values
 *		o ex : external loopback
 *		o in : internal loopback
 *	       	o nm : normal mode         
 *		o nf : normal filter
 *		o pr : promiscious
 *		o mc : multicast                    
 *		o ip : internal loopback and promiscious
 *    		o fc : force collisions
 *		o nofc : do not force collisions
 *		o nc : do not change mode
 *#
 *                                                             
 *     -p <pass_count> - Specifies the number of times to run the test.
 *		If 0, then run forever.  The default is 1.
 *		Note: This is the number of passes for the diagnostic.
 *		Each pass will send the number of loop messages as set by the
 *		environment variable, eza*_loop_count.  
 *                                                     
 *     -sv <mop_version> - Specifies which MOP version protocol to use.
 *		If 3, then MOP V3 (DECNET Phase IV) packet format is used.
 *		If 4, then MOP V4 (DECNET Phase V IEEE 802.3) format is used.
 *                     
 *     -to <loop_time> - Specifies the time in seconds allowed for the loop
 *		messages to be returned.  The default is 2 seconds.	
 *		                                 
 *     -w <wait_time> - Specifies the time in seconds to wait between passes
 *		of the test.  The default is 0 (no delay).	
 *		The network device can be very CPU intensive.
 *		This option will allow other processes to run.		
 *		                                 
 *   ENVIRONMENT VARIABLE(S):
 *
 *	eza*_loop_count - Specifies the number (hex) of loop requests to send.
 *		The default is 0x3E8 loop packets.
 *	eza*_loop_inc - Specifies the number (hex) of bytes the message size
 *		is increased on successive messages.  The default is 0xA bytes.
 *	eza*_loop_patt - Specifies the data pattern (hex) for the loop messages.
 *		The following are legitimate values.
 *#o
 *		o 0 : all zeroes
 *		o 1 : all ones
 *		o 2 : all fives
 *		o 3 : all 0xAs
 *		o 4 : incrementing data
 *		o 5 : decrementing data
 *		o ffffffff : all patterns
 *#
 *	loop_size - Specifies the size (hex) of the loop message.
 *		The default packet size is 0x2E.
 *                                               
 *   COMMAND EXAMPLE(S):                                 
 *~                                                               
 *	>>>nettest		   - internal loopback test on port eza0
 *	>>>nettest ez*		   - internal loopback test on ports eza0/ezb0
 *	>>>nettest -mode ex 	   - external loopback test on port eza0
 *	>>>nettest -mode ex -w 10  - external loopback test on port eza0 wait 10
 *				     seconds between tests	
 *	>>>nettest -f foo -mode nm - normal mode loopback test on port eza0 
 *				     using the list of nodes contained in the 
 *				     file foo
 *~                                             
 *   COMMAND REFERENCES:
 *
 *	net, netexer
 *
 * FORM OF CALL:                                              
 *                                       
 *	nettest( argc, *argv[] )                            
 *                                                                        
 * RETURN CODES:                                  
 *                                                             
 *	msg_success          - Success                       
 *	msg_failure          - Failure           
 *                                                     
 * ARGUMENTS:                                                                
 *                                                                
 *     	int argc - number of command line arguments passed by the shell 
 *  	char *argv [] - array of pointers to arguments             
 *                                              
 * SIDE EFFECTS:                                              
 *                                                                
 *	None                                                                
 *                                                             
-*/                                                                
                                                             
/*------------------------------------------------------------------------------
  Initialization Code.                                            
------------------------------------------------------------------------------*/
                                            
int nettest (int argc, char *argv []) {               
    unsigned int     	status,passcount,i,j,sleep,num_args;
    int			version;
    struct PCB	  	*pcb;                                           
    diag_common		*diag;
    struct IOB		*iob_ptr[NETTEST_MAX_PORTS];    	     
    struct QSTRUCT     	qual[NUM_QUALS + MORE_QUALS];           
    char 		port[32];	     		/*Port file name*/
    externals	     	*eptr;                               
    struct NI_GBL 	*np;	    	/*Pointer to NI global database*/	
    struct NI_DL 	*dl;		/*Pointer to the datalink*/	
    struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	
    struct MB 		*mp;		/*Pointer to the MOP block*/
    struct NI_ENV 	*ev;		/*Pointer to the environment variables*/
    UINT		rx_bytes[NETTEST_MAX_PORTS];
    UINT	 	tx_bytes[NETTEST_MAX_PORTS];
    int			timeout=NETTEST_DEF_TIMEOUT;
    char var[EV_NAME_LEN];		/*General holder for a name*/
    char name[32];			/*General holder for a name*/
                                                   
    pcb = getpcb();                                          
    pcb->pcb$a_dis_table = dispatch_table;               
    pcb->pcb$a_sup_dev   = NULL; 	/* allow all devices */
                                                              
    /* Call diag_init to set up the diagnostic environment. */
    status = diag_init(&argc,argv,qual, "%smode %sf %dw %dto %dsv",0);

    /* Common diag structure */
    diag = (diag_common *)pcb->pcb$a_diag_common;

    if (status != msg_success)                         
    	return (status);                                           
                                                                        
    diag->diag$a_ext_error = NULL;      /* extended error reporting not used */

    /*  Load rundown with cleanup routine after returning successfully from
    diag_init -- this way won't get IOB errors when diag_rundown tries
    to remove them */                                              
    pcb->pcb$a_rundown = (void *)nettest_cleanup;            
                                                   
    /* Allocate memory for the external variables.   	*/
    eptr = (externals *) dyn$_malloc(sizeof(externals),DYN$K_SYNC); 
    pcb->pcb$a_globals = (int *) eptr;
                             
    /* Get the user inputed parameters */                     
    passcount = (qual [QP].present) ? qual [QP].value.integer : 1;
    sleep = (qual [QW].present) ? (qual [QW].value.integer) * 1000 : 0;
    version = MOP$K_VERSION_4;                         
    if(qual[QSV].present)   
	if(qual[QSV].value.integer < 4)
	    version = MOP$K_VERSION_3;                      
	else                                           
    	    version = MOP$K_VERSION_4;
                                               
    /* Set the device name */                      
    /* Check and see if we should use the default*/    
    eptr->pf[0] = eptr->pf[1] = 0;                       
    if (argc == 1)                 
    {	                     
#if TURBO	/* must have parameters */
	err_printf(msg_missing_device);
        return msg_failure;
#endif
#if MEDULLA || SABLE || RAWHIDE
    	eptr->pf[0] = fopen("ewa0","r+");                     
#else
    	eptr->pf[0] = fopen("eza0","r+");                     
#endif
 	num_args = 1;                                  
    }                                    
    else                   
	num_args = argc-1;
                         
    for (i=0; i<num_args; i++)                              
    {
        if (argc >= 2)            
       	    eptr->pf[i] = fopen(argv[i+1],"r+");         
      	if(!eptr->pf[i])  
	    return(msg_bad_open);              
                
      	np = (struct NI_GBL*)eptr->pf[i]->ip->misc;
    	dl = (struct NI_DL*)np->dlp;
    	mc = (struct MOP$_V4_COUNTERS*)np->mcp;               
    	mp = (struct MB*)np->mbp;                      
    	ev = (struct NI_ENV*)np->enp;    

	/* Check the moplp flag to see if another nettest is */
	/* running on this port */
	spinlock (&spl_kernel);
 	if (mp->lp.moplp_flag != 0)
	{
	    spinunlock (&spl_kernel);
    	    err_printf(msg_moplp_err_run, eptr->pf[i]->ip->name);
	    return msg_failure;	
        }
 	else
    	    mp->lp.moplp_flag = pcb->pcb$l_pid;
	spinunlock (&spl_kernel);
 
        /* find the iob */               
        iob_ptr[i] = &eptr->pf[i]->iob;                  
                                         
	/* init the rx and tx bytes */
	rx_bytes[i] = tx_bytes[i] = 0;
                                                       
    	if (qual [QF].present)                        
       	    strcpy(mp->loop_file,qual[QF].value.string);

	/*Check for a timeout value*/
    	if (qual [QTO].present)                        
       	    timeout = qual[QTO].value.integer;
                                                
        /* save the current value of the mode */
        eptr->mode[i] = ev->mode;           
	eptr->change_mode = TRUE;
    	if (qual [QM].present)           
        {
  	    if (strncmp_nocase(qual[QM].value.string, "nc", 2) == 0)   
	        eptr->change_mode = FALSE;
	    if (eptr->change_mode)
    	        net_cm(eptr->pf[i],dl,qual[QM].value.string);   
	}                       
	else              
	{
	    if (eptr->change_mode)
	        net_cm(eptr->pf[i],dl,"ex");   
	}	

	/* If this is a TULIP set the mode to -1 */
	sprintf(name,"%2.2s",eptr->pf[i]->ip->name);

	if(!strcmp_nocase(name ,"ew"))
		eptr->mode[i] = -1;
    }	                                                      
            
    /*  Call the network loopback test */
    i = 0;                                                            
    while ((i < passcount) || (passcount == 0))          
    {                                                       
    	for (j=0; j<num_args; j++)
       	{            
            strncpy(pcb->pcb$b_dev_name, eptr->pf[j]->ip->name, MAX_NAME);   
      	    diag_print_test_trace();     

    	    status = net_loop_test(j, version, &rx_bytes[j],
							&tx_bytes[j],timeout);

    	    if ((status != msg_success) && (status != msg_killed))
	    {                  
		if (status == msg_bad_open)
		    return msg_failure;

	        iob_ptr[j]->soft_errors++;  

	        if (diag$r_evs & m_halt_soft)
       	            return status;	
            }             
		        
    	    /* add the bytes transmitted and recieved to the iob */
    	    *(UINT *)&iob_ptr[j]->bytes_read = *(UINT *)&rx_bytes[j];
    	    *(UINT *)&iob_ptr[j]->bytes_written = *(UINT *)&tx_bytes[j];
                     
    	    diag->diag$l_pass_count++;                 
    	    diag_print_end_of_pass();              
    	    if (killpending())                             
                return msg_killed;	    
	    i++;             
	    if (sleep)
	        krn$_sleep(sleep);              

        }    /* end for */

    }    /* end while */

    return msg_success;	                                             
}                                                               
                                                  
int net_loop_test(int node_num, int version, UINT *rx_bytes, UINT *tx_bytes,
								int timeout)
{                                                        
    struct NI_GBL    	*np;	    	/*Pointer to NI global database*/	
    struct NI_DL 	*dl;	    	/*Pointer to the datalink*/	
    struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	
    struct MB 	     	*mp;	   	/*Pointer to the MOP block*/
    struct NI_ENV    	*ev;		/*Pointer to the environment variables*/
    int 		status;
    struct PCB		*pcb;
    externals	     	*eptr;            
                                         
#if DEBUG      
    pprintf("start of net_loop_test\n");  	                             
#endif                                       

    /* get the pcb */                              
    pcb = getpcb();                    
    eptr = (externals *)pcb->pcb$a_globals;
                              
    /*Get some pointers*/                  
    np = (struct NI_GBL*)eptr->pf[node_num]->ip->misc;
    dl = (struct NI_DL*)np->dlp;
    mc = (struct MOP$_V4_COUNTERS*)np->mcp;
    mp = (struct MB*)np->mbp;              
    ev = (struct NI_ENV*)np->enp;                                        
                
    /* send the loop requests; and wait for the loop requests to be returned */
    status = mop_loop_requester(eptr->pf[node_num],version);        

    /* Status from above is either msg_failure or msg_success...if msg_failure
       they are 'bad/hard' failures (like couldn't init loop requester or 
       fopen failed) and diag needs to be stopped (return msg_bad_open
       status so they aren't treated as 'soft' failures) */
    if (status)
	return(msg_bad_open);

    status = moplp_wait(mp,ev,timeout);                    
 
    /* add the bytes tranmitted and recieved */
    *tx_bytes += mp->lp.tx_bytes; 
    *rx_bytes += mp->lp.rx_bytes; 

    return status;              
}                                     
                                                                  
void nettest_cleanup ( ) {       
    struct PCB		*pcb;
    externals  	      	*eptr;
    struct NI_GBL 	*np;	    	/*Pointer to NI global database*/	
    struct NI_DL 	*dl;		/*Pointer to the datalink*/	
    struct MOP$_V4_COUNTERS *mc;	/*Pointer to the MOP counters*/	
    struct MB 		*mp;		/*Pointer to the MOP block*/
    struct NI_ENV 	*ev; 		/*Pointer to the environment variables*/
    char name[20];			/*Process name,device name*/
    char var[EV_NAME_LEN];		/*General holder for a name*/
    int	i;	                    
                                    
    /* get the pcb */
    pcb = getpcb();             
    eptr = (externals *)pcb->pcb$a_globals;
                   
    for (i=0; i<NETTEST_MAX_PORTS; i++)
    {	              
    	if (eptr->pf[i])
    	{   	            
    	    np = (struct NI_GBL*)eptr->pf[i]->ip->misc;
    	    dl = (struct NI_DL*)np->dlp;
    	    mc = (struct MOP$_V4_COUNTERS*)np->mcp;
    	    mp = (struct MB*)np->mbp;
    	    ev = (struct NI_ENV*)np->enp;

            strncpy(pcb->pcb$b_dev_name, eptr->pf[i]->ip->name, MAX_NAME);   
        
            /* set the mode back to the original value */
  	    if (eptr->change_mode)
	    {
		/* Most have a default, some use normal. */
		if (eptr->mode[i] != -1) {
		    sprintf(var,"%4.4s_mode",eptr->pf[i]->ip->name);
		    ev_write(var,eptr->mode[i],EV$K_INTEGER);
		    net_cm(eptr->pf[i],dl,"df");   
		}
		else
		    net_cm(eptr->pf[i],dl,"nm");   
	    }	
#if 0       
	    (dl->cb->cm)(eptr->pf[i],NDL$K_DEFAULT);
#endif
	    /* Restore the loop file name*/
	    sprintf(name,"lp_nodes_%4.4s",eptr->pf[i]->ip->name);
	    strcpy(mp->loop_file,name);

    	    diag_rundown();

	    /* Check the moplp flag to see if this is the cleanup *\
	    /* process for the currently completing nettest */
	    spinlock (&spl_kernel);            
 	    if (mp->lp.moplp_flag == pcb->pcb$l_pid)
 	        mp->lp.moplp_flag = 0;
	    spinunlock (&spl_kernel);

    	    /* close the file */
    	    fclose(eptr->pf[i]);
    	}
    }
}
                      
