/*
 * Copyright (C) 1997 by
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
 * Abstract:	WWID manager tool
 *
 * Author:	Dennis Mazur
 *
 * Modifications:
 *
 *	dm	28-May-1996	Initial entry.
 */

#include "cp$src:platform.h"
#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"
#include "cp$src:ev_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:scsi_def.h"
#include "cp$src:common.h"
#include "cp$src:stddef.h"
#include "cp$src:pb_def.h"
#include "cp$src:sb_def.h"
#include "cp$src:ub_def.h"
#include "cp$inc:platform_io.h"
#include "cp$src:print_var.h"
#include "cp$src:ctype.h"
#include "cp$src:wwid.h"
#include "cp$src:mem_def.h"
#include "cp$src:kgpsa_emx.h"
#include "cp$src:fc.h"
#include "cp$src:kgpsa_def.h"
#include "cp$src:kgpsa_pb_def.h"
#include "cp$src:parse_def.h"
#include "cp$inc:kernel_entry.h"
#include "cp$src:regexp_def.h"

extern struct SEMAPHORE pbs_lock;
extern struct SEMAPHORE scsi_ub_lock;
extern struct pb **pbs;
extern unsigned int num_pbs;
extern unsigned int num_pbs_all;
extern struct SEMAPHORE sbs_lock;
extern struct sb **sbs;
extern unsigned int num_sbs;
extern unsigned int num_sbs_all;
extern struct SEMAPHORE ubs_lock;
extern struct ub **ubs;
extern unsigned int num_ubs;
extern unsigned int num_ubs_all;
extern struct SEMAPHORE wwids_lock;
extern struct registered_wwid **wwids;
extern unsigned int num_wwids;
extern unsigned int num_wwids_all;
extern unsigned int wwid_ev_mask;
#if 0
extern struct registered_portname **portnames;
extern unsigned int num_portnames;
extern unsigned int portname_ev_mask;
#endif

#if KGPSA_DEBUG
extern int kgpsa_pbs[];
extern int num_kgpsa;
#endif


extern volatile int kgpsa_debug_flag;
extern void kgpsa_dump_kgpsa_sb(struct kgpsa_sb *sb, int do_header, int doit);
extern void kgpsa_mbx(struct kgpsa_pb *pb, MAILBOX *mbx, int mbxtype, int arg);
extern int kgpsa_connection_helper( struct kgpsa_pb *pb, int verbose );
extern int wwidmgr_mode;
extern int scsi_send();

extern struct REGEXP *regexp_compile(char *expression, int foldcase);
extern regexp_match(struct REGEXP *re, char *s);
extern regexp_extract(struct REGEXP *regexp, char *mask, char *s,
  char *symbol);

#if ( STARTSHUT || DRIVERSHUT )
extern struct window_head config;
extern int advanced_mode_flag;
extern int diagnostic_mode_flag;
#endif

extern struct ZONE *memzone;

#if KGPSA_DEBUG
extern void dprintf();
extern void d2printf();
extern void d4printf();
extern void d8printf();
#else
#define dprintf(fmt, args)
#define d2printf(fmt, args)
#define d4printf(fmt, args)
#define d8printf(fmt, args)
#endif



struct kgpsa_sb **fcsbs;
unsigned int num_fcsbs = 0;
unsigned int num_fcsbs_all = 0;
int *kgpsa_pbs2;
int num_kgpsa2;

#define WWID_EV 1
#define N_EV    2
#define DEFAULT_COLLISION 1
#define ALL_ITEMS 9999

#define DONT_WAIT        0            /* Dont wait for command completion */
#define WAIT             1            /* Wait for command completion.     */
#define kgpsa_init_link( x, y, wait )    kgpsa_mbx( x, y, MBX_INIT_LINK, wait )

/* missing from the red-line dynamic.c in V53 Rawhide redline */
#define local_malloc_noown(xx1)   \
    dyn$_malloc(xx1, DYN$K_SYNC|DYN$K_FLOOD|DYN$K_NOOWN, 0, 0, 0)

#define memzone_malloc( x ) dyn$_malloc(x, DYN$K_SYNC | DYN$K_NOOWN | DYN$K_ZONE, 0, 0, memzone)

char *Yes_No[] = {"Yes", "No "};

#define NVR_TOPO_UNAVAIL 0
char *Topo[] = {"UNAVAIL","1","FABRIC","3","LOOP"};

enum { FCSCAN_MIN=1,
       FCSCAN_FULL,
       FCSCAN_CONNECTED,
       FCSCAN_NOT_CONNECTED
} FCSCAN_MODES;




void dump_wwids(int full, char *filter,
                int itempresent, int itemnum,
                int udidpresent, int udidnum)
{
    int i, j;
    char wwidtxt[100], *pWWID;
    char evname[10];

    struct kgpsa_sb *kgsb;
    struct registered_wwid *wwid;
    struct REGEXP *r = 0;
    char *filterptr;
    int filter_found;
    char c, temp[40], *t = temp;

#define NEXP 2

    if (filter)
      {

	/* massage filter */

	filterptr = filter;
	while (c = *filterptr++)
	  {
	    if (c == '*')
	      {
		*t++ = '.';
		*t++ = '*';
	    }
	      else
	      {
		*t++ = c;
	    }
	}
	*t = 0;
	dprintf("new filter: %s\n", p_args1(temp));

	for (i = 1; i < NEXP; i++) {
	    r = regexp_compile(temp, 1);
	}

	if (r == 0) {
	    printf("-regexp_compile failure\n");
	    return;
	}

    }

    krn$_wait(&wwids_lock);
    dprintf("wwids:%x  num_wwids:%x num_wwids_all:%x \n\n",
      p_args3(wwids, num_wwids, num_wwids_all));
    for (i = 0; i < num_wwids; i++) {

	/*
	 * Use the item or udid if it matches the wwids[].  But note
	 * that there is no error check on the user putting both
	 * -item and -udid on the command line.  THe user should not do that,
	 * and I only really expect itempresent or udidpresent but I do not
	 * feel it is worth any flash bytes to check for that.
	 */

	if( itempresent && (itemnum != i) ) continue;

	wwid = wwids[i];

        if ( udidpresent && (udidnum != wwid->udid) ) continue;

 	pWWID = wwid2wwev(wwidtxt, 0, 0, &wwid->wwid);

	if (r)
	    filter_found = regexp_match(r, pWWID);

	if (!filter_found)
	    dprintf("regexp_match failure \n", p_args0);

	/*
	 * If a filter matches this wwid or if no -filter is
	 * specified, then display this wwid.
	 */
	if (((r != 0) && filter_found) || (filter == 0))
	  {
	    if (pWWID) {
		if (wwid->evnum != -1) {
		    sprintf(&evname, "wwid%d", wwid->evnum);
		} else {
		    strcpy(&evname, "none");
		}
		printf("[%d] UDID:%d %s (ev:%s)\n", i, wwid->udid, pWWID, evname);
		if (full) {
		    int found = 0;
	 	    printf(" - current_unit:%d current_col:%2d default_unit:%5d   \n",
		            wwid->unit, wwid->collision, wwid_2_unit(&wwid->wwid) );

		    printf("   %20s %20s %5s %8s %6s\n", "via adapter  ",
                                    "via fc_nport    ", "Con", "DID ", "Lun");
		    for (j = 0; j < MULTIPATH_MAX; j++) {
			if (wwid->sbs[j]) {
			    dprintf("    sb[%d]:%x  lun[%d]:%x  ip:%x \n",
			      p_args4(j, wwid->sbs[j], j, wwid->lun[j]));
			    found = 1;
			    kgsb = wwid->sbs[j];
			    printf(" - %20s %20s %5s", kgsb->pb->name,
				               kgsb->portname,
			              Yes_No[(wwid->online[j] & 0x20) != 0]);
			    if (!kgsb->valid)
				printf(" NO LONGER VALID\n");
                            else
			        printf(" %8x %6d\n", kgsb->did, wwid->lun[j]);


			}		/* if */
		    }			/* for */
		    if (!found)
			printf(" - Device not found.\n");
		    printf("\n");
		}			/* if full */
	    } else {
		dprintf("wwids[%d] (%x) empty \n", p_args2( i, wwids[i] ) );
	    }
	}

    }					/* filter */

    if (filter)
	regexp_release(r);

    krn$_post(&wwids_lock);
}

void insert_fcsb(struct kgpsa_sb *sb)
{
    int k;
    int found = FALSE;

    for (k = 0; k < num_fcsbs; k++) {
	if (kgpsa_cmp_name(&sb->serv_parm.portName,
	  &fcsbs[k]->serv_parm.portName) == 0) {
	    found = TRUE;
	    break;
	}
    }

    if (found) {
	dprintf("Duplicate portname %s \n", p_args1(sb->portname));
    } else {
	if (num_fcsbs == num_fcsbs_all)
	  {
	    num_fcsbs_all += 32;
	    fcsbs = dyn$_realloc(fcsbs,
	      sizeof( struct sb * ) * num_fcsbs_all,
	      DYN$K_SYNC | DYN$K_NOOWN);
	}
	fcsbs[num_fcsbs++] = sb;
    }
}

/*+
 * ============================================================================
 * = wwidmgr - Manage the wwid registration.                                  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *
 *      The device out on the Fibre Channel loop or fabric can only be
 *      identified uniquely by their World Wide Id (WWID) which are large
 *      values.  The 'target' ids are also large values.   The console has
 *      always created the device name from the unit id and the target id.
 *      But since these values are too large in the Fibre Channel environment,
 *      an aliasing must be defined by the user.   The wwidmgr assists the user
 *      in finding his/her device and in setting up the aliasing.
 *
 *      This aliasing is limited by the non-volatile storage space which the
 *      aliasing information is stored.   Limit aliasing also has the
 *      beneficial side effect of less heap consumption.   Because the console
 *      is only works with defined devices as registered by the wwidmgr, the
 *      devices that are not registered will not be allocated any data
 *      structures.
 *
 *      The wwidmgr runs in diagnostic mode and used all of memory.  This
 *      allows the wwidmgr to keep information about all devices, registered
 *      and non-registered.   The user must initialize the system after
 *      running the wwidmgr in order to free the heap and for the devices to
 *      be renamed to their defined alias value.
 *
 *
 *   COMMAND FMT: wwidmgr 0 D 8 wwidmgr
 *
 *   COMMAND FORM:
 *
 *	wwidmgr ( [-quickset {-item <#>], -udid <#> } ]
 *                [-set {wwid] | port} -item <#> [-unit <#>] [-col <#>] [-filter <'string>] ]
 *                [-show {wwid] | port} [-full] [-filter <'string'>] ]
 *                [-show {ev | reachability} ]
 *                [-clear {all | wwid<#> | N<#>} ]
 *
 *                Diagnostic mode command
 *              )
 *
 *   COMMAND TAG: wwidmgr 8*1024 RXBZP wwidmgr wwidmgr
 *
 *   COMMAND ARGUMENT(S):
 *
 *   COMMAND OPTION(S):
 *
 *	-show < W[WID] | P[ORT] | E[V] | R[EACHABILITY] > - display information
 *              about the WWID, N_ports, FC environment variables, or
 *              the reachability of devices.
 *
 *	-set <WWID | PORT> - set up a small integer alias for a WWID in the
 *              environment variables.  This option does not set up any port
 *              paths.
 *
 * 	-quickset - set up a small integer alias for a WWID in the environment
 *              variables.  This option also sets up the port path ev's that
 *              access the WWID.
 *
 *	-clear <wwid0 | wwid1 | wwid2 | wwid3 | N1 | N2 | N3 | N4 | all > -
 *              Clear the FC related environment variables, either one at a
 *              time or all at once.
 *
 *      -item <item#> - specifies a WWID or PORT menu item in -set, -quickset,
 *              and -show options.
 *
 *      -udid <udid#> - specifies a UDID in -set, -quickset, and -show options.
 *
 *      -unit <unit#> - the unit number to be associated with the WWID.  Used
 *              with the -set option.
 *
 *      -filter <regexp> - specifies a regular expression used to narrow the
 *              displays of -show.
 *
 *      -full - Used with -show to provide more detailed information.
 *
 *      -col - Used to specify a collision value.  Defaults to 1.
 *
 *   COMMAND EXAMPLE(S):
 *
 *      Show a detailed display the WWIDs that are known to the console
 *
 *~
 *	>>> wwidmgr -show wwid -full
 *~
 *	Show a detailed display of UDID 4
 *~
 *	>>> wwidmgr -show wwid -udid 4 -full
 *~
 *	Show only the ports that have 5000 in their wwid.
 *~
 *	>>> wwidmgr -show port -filter *5000*
 *~
 *
 *	Set up UDID 4 for recognition under the console.  It will be assigned
 *      a unit number of 4.
 *~
 *	>>> wwidmgr -quickset -udid 4
 *~
 *	Set up menu item 2 for recognition under the console. It will be
 *      assigned a unit number of 200.
 *~
 *	>>> wwidmgr -quickset -item 2 -unit 200.
 *~
 *	Set up menu item 2 with a unit number of 200, but do not automatically
 *      set up all the access paths.
 *~
 *	>>> wwidmgr -set wwid -item 2 -unit 200
 *~
 *      Set up port menu item 7 as N2 allowing console access via that path.
 *~
 *	>>> wwidmgr -set port -item 7 -node 2
 *~
 *
 *   COMMAND REFERENCES:
 *
 *	set, show
 *
 * FORM OF CALL:
 *
 *	wwidmgr( argc, *argv[] )
 *
 * RETURN CODES:
 *
 *	msg_success - Indicates normal completion.
 *
 * ARGUMENTS:
 *
 *	int argc - The number of command line arguments passed by the shell.
 *	char *argv [] - Array of pointers to arguments.
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/

#define QUNIT        0
#define QSET         1
#define QSHOW        2
#define QCLEAR       3
#define QCOL         4
#define QADAPTER     5
#define QFULL        6
#define QITEM        7
#define QNODENUM     8
#define QFILTER      9
#define QQUICKSET   10
#define QUDID       11
#define QDIAG       12
#define QTOPO       13
#define QMAX        14
#define QSTRING "%dunit %sset %sshow %sclear %dcol %sadapter full \
                 %ditem %dnodenum %sfilter quickset %dudid %sdiag \
                 %stopo"

int wwidmgr(int argc, char *argv[])
{
    int i, j, t, status;
    int unitno;
    int collision;
    int idx;
    int item;
    int udid;
    int evnum;
    unsigned __int64 swap_pn;
    char c;
    char evtype;
    char buf[120];
    char evname[10];
    struct QSTRUCT *qual;
    struct registered_wwid *pwwid, *w;



#if ( STARTSHUT || DRIVERSHUT )
    if (!diagnostic_mode_flag)
      {
	printf("Not in diagnostic mode.\n");
	return msg_failure;
    }
#else
    if (!memzone)
       {
        printf("wwidmgr available only prior to booting.  Reinit system and try again.\n");
        return msg_failure;
       }
#endif

    qual = malloc(sizeof(struct QSTRUCT) * QMAX);
    status = qscan(&argc, argv, "-", QSTRING, qual);
    if (status != msg_success) {
	err_printf(status);
	return (status);
    }

    /*
     * wwidmgr_mode is cleared zero at the start.  It is cleared out
     * by probe_io.c on some platforms and on other platforms it is
     * zero'ed by virtue of a load from flash.
     *
     * fcsbs is not zeroed by a load from flash on those platforms that do
     * that.  On the platforms that are not loaded from flash on init, there
     * could be stale data in this fcsbs global from a run before the init.
     * We key off of wwidmgr_mode to clear it.  It acts as a flag that
     * indicates this is the initial run of the wwidmgr, and used in
     * wwidmgr_scan.
     *
     */

    if ( wwidmgr_mode == 0 )
      {
      if( !qual[QDIAG].present )
         {
         wwidmgr_mode = 1;
         }
      fcsbs = 0;

      /* May 16, 2000 (djm)
       * The number of paths to a disk prior to this edit was 16.  This 
       * edit along with an edit in SCSI.C and WWID.H increase the connection
       * limit to 128 (current MULTIPATH_MAX).  The fields below are not
       * used prior to wwidmgr_mode.  So to avoid wasting the heap space
       * when not in wwidmgr_mode, we do not allocate the blocks for these 
       * fields until now.  It is only done at wwidmgr start up of course.
       * As new wwids are found, the code in SCSI.C will do the mallocs
       * for these fields.
       */
      for(i=0; i<MAX_WWID_ALIAS_EV; i++)
        {
        w = wwids[i];
        w->sbs = memzone_malloc( sizeof(struct sb *) * MULTIPATH_MAX );
        w->lun = memzone_malloc( sizeof(int) * MULTIPATH_MAX);
        w->online = memzone_malloc( sizeof(int) * MULTIPATH_MAX);
      }

      }


#if !( STARTSHUT || DRIVERSHUT )
    start_drivers();
#endif




    /*
     ********************************************************
     *                      DIAG                            *
     ********************************************************
     */

    if (qual[QDIAG].present)
      {

        struct ub *ub;
	wwidmgr_init( qual );
	fcscan(FCSCAN_CONNECTED);
        free( fcsbs );
	free( kgpsa_pbs2 );
	fcsbs = 0;
	return msg_success;
    }





    /*
     ********************************************************
     *                      ADAPTER                         *
     ********************************************************
     */

    if (qual[QADAPTER].present)
      {

	wwidmgr_init( qual );

 	wwidmgr_reachability();

	return msg_success;
    }




    /*
     ********************************************************
     *                      QUICK                           *
     ********************************************************
     */

    if (qual[QQUICKSET].present)
      {

	wwidmgr_init( qual );

	if (qual[QITEM].present)
	  {
	    int unit;

	    if (qual[QUNIT].present)
	      {
		unit = qual[QUNIT].value.integer;
	      }
	    else
	      {
		/* May 9, 2000 (djm)
		 * Reconstruct the default unit number.  It could have been overwritten by
                 * other manipulations during this session.  The old code that was at this
		 * line prior to May 9th was wrong.
                 */
	        unit =  wwid_2_unit(&wwids[qual[QITEM].value.integer]->wwid);
	      }
	    wwidmgr_set_item(qual[QITEM].value.integer, unit, DEFAULT_COLLISION);
 	    wwidmgr_autoport(qual[QITEM].value.integer);
	}

	if (qual[QUDID].present )
	  {
	    for (i = 0; i < num_wwids; i++)
	      {
		if (wwids[i]->udid == qual[QUDID].value.integer)
		  {
		    wwidmgr_set_item(i, wwids[i]->udid, DEFAULT_COLLISION );
	            wwidmgr_autoport(i);
		    break;
		}
	    }
	    if (i == num_wwids)
		printf("UDID %d not found\n", qual[QUDID].value.integer );
	}

	wwidmgr_reachability();

	return msg_success;
    }


    /*
     ********************************************************
     *                       SHOW                           *
     ********************************************************
     */

    if (qual[QSHOW].present) {
	int i, k, idx;
	struct registered_portname *p;

	wwidmgr_init( qual );

	dprintf("show %s\n", p_args1(qual[QSHOW].value.string) );
	switch (qual[QSHOW].value.string[0]|0x20) {
	    case 'r':
		fcscan(FCSCAN_MIN);
		wwidmgr_reachability();
		break;
	    case 'w':
		if( qual[QITEM].present || qual[QUDID].present )
		  fcscan(FCSCAN_MIN);
                else
		  fcscan(qual[QFULL].present ? FCSCAN_FULL : FCSCAN_MIN);
		dump_wwids(qual[QFULL].present, qual[QFILTER].value.string,
                     qual[QITEM].present, qual[QITEM].value.integer,
                     qual[QUDID].present, qual[QUDID].value.integer);
		break;
	    case 'e':
                {
		char *argv1[] = {"show","wwid%"};
		char *argv2[] = {"show","N%"};
#if (MODULAR)
		ovly_load("SHOW");
                show_routine(2,argv1);
                show_routine(2,argv2);
		ovly_remove("SHOW");
#else
                show(2,argv1);
                show(2,argv2);
#endif
                break;
                }
	    case 'p':
		fcscan(qual[QFULL].present ? FCSCAN_FULL : FCSCAN_MIN);
		{

		    struct REGEXP *r = 0;
		    int filter_found;
		    char *filterptr;
		    char c, temp[40], *t = temp;

#define NEXP 2

		    if (qual[QFILTER].value.string)
		      {

			/* massage filter */

			filterptr = qual[QFILTER].value.string;
			while (c = *filterptr++)
			  {
			    if (c == '*')
			      {
				*t++ = '.';
				*t++ = '*';
			    }
			      else
			      {
				*t++ = c;
			    }
			}
			*t = 0;
			dprintf("new filter: %s\n", p_args1(temp));

			for (i = 1; i < NEXP; i++) {
			    r = regexp_compile(temp, 1);
			}

			if (r == 0) {
			    printf("regexp_compile failure\n");
			    return msg_failure;
			}

		    }



		    for (i = 0; i < num_fcsbs; i++)
		      {
			if (r)
			    filter_found =
			      regexp_match(r, &fcsbs[i]->portname);

			if (!filter_found)
			    dprintf("regexp_match failure \n", p_args0);

			if (((r != 0) && filter_found) ||
			  (qual[QFILTER].value.string == 0))
			  {

			    printf("[%d] %s ", i, fcsbs[i]->portname);
			    for (k = 1; k <= 4; k++)
			      {
				unsigned __int64 swap_pn;
				struct EVNODE evn, *evp = &evn;
				char n[3] = {'N', '\0', '\0'};

				n[1] = '0' + k;
				if (ev_read(n, &evp, 0) != msg_success)
				    continue;

				fc_swap_data(&swap_pn, &fcsbs[i]->
				  serv_parm.portName, sizeof(NAME_TYPE));
				sprintf(&buf, "%16.16x", swap_pn);
				if (strcmp(&evp->value.string, &buf) == 0) {
				    printf("(%s)", n);
				}	/* if */

			    }		/* for k number of portname evs */

#if 0
I would like to flag n_ports that are no longer reachable.  But since I only
keep one sb of the many paths in the fcsbs list, the below code is not
accurate.  I could have availability one of several pbs.   And so I need to
track the reachability of all the pb to that n_port, or list along with the
n_port, the pb.  That might not be all that bad.  I could list off to the
right all the adapter names.  An empty list means not reachable.

			    if( !fcsbs[i]->valid )
				printf(" - NO LONGER VALID");
#endif

			    printf("\n");

			}

		    }

		    if (qual[QFILTER].value.string)
			regexp_release(r);

		}
		break;

            case 'a':
                {
                int i;
                struct kgpsa_pb *pb;
                struct kgpsa_sb *pbsb;
		struct kgpsa_nvram_region nvstack, *nvr=&nvstack;

		printf("%-7s %-25s %-20s %-10s %-10s\n",
                              "item", "adapter", "WWN", "Cur. Topo", "Next Topo");

	        for( i=0; i<num_kgpsa2; i++)
                  {
                    pb = kgpsa_pbs2[i];
                    pbsb = pb->pb.sb[0];
                    if(kgpsa_read_compaq_nvdata( pb, nvr ) != msg_success)
                       nvr->topology = NVR_TOPO_UNAVAIL;
                    printf("[%2d] %-20s %-30s %-10s %-10s \n",
                         i, pb->pb.name, pbsb->nodename,Topo[pb->topology],
                                                        Topo[nvr->topology] );
                  }
                if(num_kgpsa2 > 0)
                  {
                    printf("[%d] All of the above.\n", ALL_ITEMS);
		  }

                }
                break;


	    default:
		printf("Invalid qualifier\n");
	}
	return msg_success;
    }

    /*
     ********************************************************
     *                      CLEAR                           *
     ********************************************************
     */

    if (qual[QCLEAR].present) {
	evtype = qual[QCLEAR].value.string[0];
	if ((evtype == 'a') || (evtype == 'A')) {
	    for (i = 0; i <= 3; i++)
		wwidmgr_delete_wwev(WWID_EV, i);
	    for (i = 1; i <= 4; i++)
		wwidmgr_delete_wwev(N_EV, i);
	} else if (evtype == 'w') {
	    wwidmgr_delete_wwev(WWID_EV, qual[QCLEAR].value.string[4] - '0');
	} else if (evtype == 'N') {
	    wwidmgr_delete_wwev(N_EV, qual[QCLEAR].value.string[1] - '0');
	} else {
	    printf("Invalid parameter\n");
	}
	return msg_failure;
    }

    /*
     ********************************************************
     *                       SET                            *
     ********************************************************
     */

    if (qual[QSET].present)
      {
	evtype = qual[QSET].value.string[0]|0x20;

	if (qual[QITEM].present) {
	    item = qual[QITEM].value.integer;
	} else if (qual[QUDID].present) {
	    udid = qual[QUDID].value.integer;
	} else {
	    printf("-item or -udid needs to be specified\n");
	    return msg_failure;
	}

	if (evtype == 'w') {

	    if (qual[QUNIT].present)
		unitno = qual[QUNIT].value.integer;
	    else
		unitno = wwids[item]->unit;

	    if (qual[QCOL].present)
		collision = qual[QCOL].value.integer;
	    else
		collision = DEFAULT_COLLISION;   /* May 9, 2000 (djm);  old
                                                  * code was setting it from
                                                  * an invalid field.  We always
                                                  * want the default if not
                                                  * specified.
                                                  */

  	    if (qual[QITEM].present) {
	      item = qual[QITEM].value.integer;
  	      wwidmgr_set_item( item, unitno, collision );
	    } else if (qual[QUDID].present) {
	      for (i = 0; i < num_wwids; i++)
	        {
		  if (wwids[i]->udid == qual[QUDID].value.integer)
		    {
		      wwidmgr_set_item(i, wwids[i]->udid, DEFAULT_COLLISION );
		      break;
		    }
	       }
	      if (i == num_wwids)
		printf("UDID %d not found\n", qual[QUDID].value.integer );

	    }

	} else if (evtype == 'p') {
	    if (!qual[QNODENUM].present) {
		printf("Need an -node assignment of either 1,2,3, or 4\n");
		goto EXIT;
	    } else {
		evnum = qual[QNODENUM].value.integer;
	    }
/*
 * TODO: Bounds check evnum.
 */
	    sprintf(&evname, "N%d", evnum);
	    fc_swap_data(&swap_pn, &fcsbs[item]->serv_parm.portName,
	      sizeof(NAME_TYPE));
	    sprintf(&buf, "%16.16x", swap_pn);
	    dprintf("setting %s to %s (%s)\n", p_args3(evname, fcsbs[item]->portname, buf) );
	    ev_write(evname, buf, EV$K_STRING | EV$K_SYSTEM);
	} else if (evtype == 'a' ) {
	    int i,startitem,enditem;
	    int status;
            struct kgpsa_pb *pb;
	    struct kgpsa_nvram_region nvstack, *nvr = &nvstack;
	    MAILBOX mbx, *mb=&mbx;
  #define kgpsa_load_sm( x, y )          kgpsa_mbx( x, y, MBX_LOAD_SM,     0 )

	    wwidmgr_scan(qual);
            startitem = item;
     	    enditem = item;
	    if(item == ALL_ITEMS)
	       {
               startitem = 0;
     	       enditem = num_kgpsa2-1;
	       }
	    for(i=startitem; i<=enditem; i++)
              {
	       pb = kgpsa_pbs2[i];
               status = kgpsa_read_compaq_nvdata( pb, nvr );
	       if( (status == msg_failure)   ||
                   ((qual[QTOPO].value.string[0]|0x20) == 'r') ) /* reformat */
	         {
		  printf("Reformatting nvram\n");
		  kgpsa_load_sm( pb, mb );
		  kgpsa_init_compaq_nvdata( pb, nvr );
		 }
               if((qual[QTOPO].value.string[0]|0x20) == 'f')     /* fabric */
                 {
      	         nvr->topology = FLAGS_TOPOLOGY_MODE_PT_PT;
                 }
               else if((qual[QTOPO].value.string[0]|0x20) == 'l') /* loop */
                 {
                 nvr->topology = FLAGS_TOPOLOGY_MODE_LOOP;
                 }
               else if((qual[QTOPO].value.string[0]|0x20) == 'e') /* erase */
                 {
		  kgpsa_load_sm( pb, mb );
		  continue;
                 }
	       kgpsa_write_compaq_nvdata( pb, nvr );
              }
        } else {
            printf("unknown set\n");
        }
    }

EXIT:

    free(qual);
    return msg_success;
}

void wwidmgr_scan( struct QSTRUCT *qual )
{
#define MAX_ADAPTER 64
    int i, j;
    int first_time = 0;
    struct kgpsa_pb *pb;
    MAILBOX mb;
    void make_pb_list();

#if ( STARTSHUT || DRIVERSHUT )
    struct device *devlist[MAX_ADAPTER];
    int devcnt_kgpsa_x = 0;
    int *FC_supported_adapter_list[] = {
        PCI$K_DTYPE_DEC_KGPSA_A,
        PCI$K_DTYPE_DEC_KGPSA_B,
        PCI$K_DTYPE_DEC_KGPSA_C,
        0
    };
#endif


/*
 * On Turbo and Rawhide we can have a lot of adapters.  The user may not
 * want to start them all up for his/her work.   The -adapter qualifier
 * was invented to allow the user to select adapters.
 *
 * This code flow gets repeated every command.  It tries to start up and
 * build structures for all adapters if the user does not specify any adapter
 * the first time the command is issued.  If adapter is on the line, then
 * that particular adapter is started in a cumulative way.   That is if
 * the user does a "wwidmgr -adapter kgpsa0" then later a
 * "wwidmgr -adapter kgpsa1", all future commands will use both adapters.
 *
 * Monolithic consoles without startshut do not need the -adapter switch.
 *
 */


    if ( fcsbs == 0 )
      {
        first_time = 1;
      } else {
        free( fcsbs );
	free( kgpsa_pbs2 );
      }
    num_fcsbs = 0;
    num_fcsbs_all = 4;
    fcsbs = local_malloc_noown(sizeof( struct sb * ) * num_fcsbs_all);
    kgpsa_pbs2 = local_malloc_noown(sizeof( int* ) * 64);


#if ( STARTSHUT || DRIVERSHUT )
  if( qual[QADAPTER].present )
    {
      char *argv[] = {"config","kgpsaxx"};
      strcpy( argv[1], qual[QADAPTER].value.string );
      configure( 2, argv );
    }
  else
    {
      char *argv[] = {"config","kgpsa*"};
      if( first_time )
         configure( 2, argv );
    }

#endif


    num_kgpsa2 = 0;
    memset( kgpsa_pbs2, 0, sizeof( int* ) * 64 );

#if ( STARTSHUT || DRIVERSHUT )
    j = 0;
    while(FC_supported_adapter_list[j] != 0)
    {
        devcnt_kgpsa_x =
        find_all_dev(&config, FC_supported_adapter_list[j], MAX_ADAPTER, devlist,
        FAD$M_PASSED_ST);

        for (i = 0; i < devcnt_kgpsa_x; i++)
            if ((void *) devlist[i]->devdep.io_device.devspecific2 != 0) {
                kgpsa_pbs2[num_kgpsa2++] = devlist[i]->devdep.io_device.devspecific2;
                    dprintf("kgpsa_pbs2[%d] = %x\n", p_args2(i, devlist[i]->devdep.io_device.devspecific2) );
            }
        j++;
   }
#else

    if( qual[QADAPTER].present )
       err_printf(msg_not_on_this_platform,"-adapter");

    find_pb("kgpsa", 0, make_pb_list, first_time );
    for( i = 0; i < num_kgpsa2; i++ )
       {
       struct kgpsa_pb *pb;
       pb = kgpsa_pbs2[i];
       kgpsa_connection_helper( pb, NO_VERBOSE );
       }


#endif


}


void make_pb_list( struct kgpsa_pb *pb, int first_time)
{
  MAILBOX mb;
  kgpsa_pbs2[num_kgpsa2++] = pb;
  if(first_time)
    kgpsa_init_link(pb, &mb, DONT_WAIT );
}




void fcscan(int fcscan_mode)
{
    int i, j, k;
    int idx;
    int cmd_len;
    int t;
    int dat_in_len;
    int udid;
    char wwidtext[100], *pWWID;
    unsigned char cmd[6];
    unsigned char dat_in[1];
    struct kgpsa_pb *pb;
    struct kgpsa_sb *sb;
    struct INODE *inode;
    struct ub *ub;
    struct registered_wwid *w;
    struct registered_portname *p;
    char *valtxt[2] = {"NO LONGER VALID", ""};

#define DO_IT 1
    dprintf("num_kgpsa2: %x\n", p_args1(num_kgpsa2) );

    for (i = 0; i < num_kgpsa2; i++)
      {
	pb = kgpsa_pbs2[i];

        dprintf("working on pb: %x\n", p_args1(pb) );

        dprintf("pb->node_count: %x\n", p_args1(pb->node_count) );

	for (j = 0; j < pb->node_count; j++)
	  {
	    if (fcscan_mode==FCSCAN_FULL)
		printf("\n%s\n", pb->pb.name);
	    sb = pb->pb.sb[j];
	    if (!sb) {
		printf("ZERO sb!!!!!  Should not happen. index:%x\n", i);
	    }
	    if (fcscan_mode==FCSCAN_FULL) {
		printf("- Port: %s   %s\n", sb->portname, valtxt[sb->valid]);
	    }

	    insert_fcsb(sb);
/*
 * Do an inquiry on lun 0 and get the allocation class.
 */

	    krn$_wait( &scsi_ub_lock );
	    ub = sb->ub.flink;
	    while (ub != &sb->ub.flink)
	      {

		if( sb->state != sb_k_open )
                  {
                   dprintf("sb->state not open - break (ub:%x)\n", p_args1(ub) );
                   break;
                  }

		if(!ub) break;

		inode = ub->inode;
		pWWID = wwid2wwev(wwidtext, 0, 0, ub->wwid);
		/*
		 *  Fill in the Command Out bytes.
		 */
		cmd_len = sizeof( cmd );
		cmd[0] = scsi_k_cmd_inquiry;
		cmd[1] = 0;
		cmd[2] = 0;
		cmd[3] = 0;
		cmd[4] = 1;		/* only need the peripheral qualifier
					 */
		cmd[5] = 0;
		/*
		 * Check to see if the device is connected.  Cannot send
		 * a start_unit to a device that is not connected or we will
		 * fail over an HSGx80 from one controller to another.  afw_eco 1226
		 */
		dat_in_len = sizeof( dat_in );
		t = kg_scsi_send(ub, cmd, cmd_len, 0, 0, dat_in, &dat_in_len);

		udid = 0;
		if(t==msg_success)
		   t = kg_get_udid(ub, &udid);
                else
		   break;

		if (t == msg_success)
		  {
		    d2printf("ub:%x ub->unit:%x  udid:%d \n",
		      p_args3(ub, ub->unit, udid));
		}
		  else
		  {
		    dprintf("ub:%x  No udid available\n", p_args1(ub));
		    udid = -1;
		}
                krn$_wait(&wwids_lock);
		w = find_wwidfilter_from_wwids(ub->wwid, &idx);
		if (!w) {
		    /*
		     *  Should never get here
		     */
		} else {
		    w->udid = udid;

		    /*  if this is another path to the device, record it for
		       presentation only in the wwidmgr.  There is no
		       functional code yet (2-feb-98) to actively use  this
		       pathing information. */

		    for (k = 0; k < MULTIPATH_MAX; k++) {
			if (sb == w->sbs[k]) {
			    w->online[k] = dat_in[0];
			    break;
			}
		    }

		    if (k == MULTIPATH_MAX) {	/* Not found, save it */
			for (k = 0; k < MULTIPATH_MAX; k++) {
			    if (w->sbs[k] == 0) {
				w->sbs[k] = sb;
				w->lun[k] = ub->unit;
				w->online[k] = dat_in[0];
				break;
			    }		/* if */
			}		/* for */

			if (k == MULTIPATH_MAX) {
			    printf("ERROR: Multipathing exceeds %d\n",
			      MULTIPATH_MAX);
			}

		    }			/* if */
		}			/* else */
  	        krn$_post(&wwids_lock);

		if (fcscan_mode==FCSCAN_FULL)
		    printf(" - %s %s\n", inode->name, pWWID);

		if (fcscan_mode==FCSCAN_CONNECTED)
		   if( (dat_in[0] & 0x20) == 0 )
                      printf("%s\n",ub->inode->name);

		ub = ub->ub.flink;
	    }				/* while ubs */
	    krn$_post( &scsi_ub_lock );

	}				/* for sbs */
	if (fcscan_mode==FCSCAN_FULL)
	    printf("\n");

    }					/* for pbs */
    if (fcscan_mode==FCSCAN_FULL)
	printf("\n");

}

void fc_swap_data(unsigned char *dst, unsigned char *src, int len)
{
    src += len;
    while (--len >= 0)
	*dst++ = *--src;
}

void wwidmgr_autoport(int wwididx)
{
    int i, j, k;
    int pidx;
    int duplicate;

    char buf[120];
    unsigned __int64 swap_pn;
    char evname[10];
    struct kgpsa_sb *kgsb, *kgsb2;
    struct registered_wwid *wwid;
    struct registered_portname *p;

    int name_type_cnt = 0;
    NAME_TYPE name_type[8];

    wwid = wwids[wwididx];
    for (i = 0; i < MULTIPATH_MAX; i++)
      {
	if (wwid->sbs[i])
	  {
	    kgsb = wwid->sbs[i];
	    duplicate = 0;
	    for (k = 0; k < i; k++)
	      {
		if (wwid->sbs[k]) {
		    kgsb2 = wwid->sbs[k];
		    if (kgpsa_cmp_name(&kgsb->serv_parm.portName,
		      &kgsb2->serv_parm.portName) == 0) {
			dprintf("Duplicate.\n", p_args0 );
			duplicate = 1;
			break;
		    }
		}
	    }				/* k */
	    if (!duplicate)
		memcpy(&name_type[name_type_cnt++], &kgsb->serv_parm.portName,
		  sizeof(NAME_TYPE));
	}
    }					/* i */


/*
 * TODO: Have to worry about name_type_cnt exceeding the number of N% ev's
 *       Right now I think 4 is the max ports a disk will be hooked up to,
 *       but that is just knowledge of what is today.
 */


    dprintf("name_type_cnt: %d\n", p_args1(name_type_cnt));
    for (j = 0; j < name_type_cnt; j++)
      {

	fc_swap_data(&swap_pn, &name_type[j], sizeof(NAME_TYPE));
	sprintf(&buf, "%16.16x", swap_pn);
	dprintf("%s\n", p_args1(buf));
	sprintf(&evname, "N%d", j + 1);
	ev_write(evname, buf, EV$K_STRING | EV$K_SYSTEM);
    }
}

int wwidmgr_set_item(int idx, int unit, int collision )
{
    int i;
    char evname[10];
    char buf[120];
    struct registered_wwid *pwwid, *w;

    /*
     * Find a wwid<n> ev to store this in.  If the is an attempt
     * to rename/reset then delete first?  Just reuse the ev?
     * What about any inodes hanging around or other pointers.
     */
    if (wwids[idx]->evnum == -1) {
	for (i = 0; i < MAX_WWID_ALIAS_EV; i++) {
	    if (!(wwid_ev_mask & (1 << i)))
		break;
	}
    } else {
	i = wwids[idx]->evnum;
    }

    if (i == MAX_WWID_ALIAS_EV) {
	printf("All the ev's are used up.  Delete something first\n");
	return msg_failure;
    } else {
	wwid_ev_mask |= 1 << i;
	wwids[idx]->evnum = i;
	wwids[idx]->unit = unit;
	wwids[idx]->collision = collision;
	sprintf(&evname, "wwid%d", wwids[idx]->evnum);
	wwid2wwev(buf, unit, collision, &wwids[idx]->wwid);
	dprintf("setting %s to %s\n", p_args2(evname, buf) );
	ev_write(evname, buf, EV$K_STRING | EV$K_SYSTEM);
    }

    return msg_success;
}

void wwidmgr_reachability()
{
    int i, j, k;
    int widx, pidx;

    char buf[120];
    char wwidtxt[100], *pWWID;
    char name[50];
    unsigned __int64 swap_pn;
    char evname[10];
    char string[80];
    struct kgpsa_sb *kgsb, *kgsb2;
    struct registered_wwid *w, *wwid, wwid2, *w2 = &wwid2;
    struct registered_portname *p;
    struct EVNODE evn, *evp = &evn;
    struct kgpsa_pb *pb;

    int name_type_cnt = 0;
    NAME_TYPE name_type[8];

    printf("\nDisk assignment and reachability after next initialization:\n\n");

    for (i = 0; i < 4; i++)
      {
	int unit, collision;
	char wwidev[6] = {'w', 'w', 'i', 'd', '\0', '\0'};

	wwidev[4] = '0' + i;
	if (ev_read(wwidev, &evp, 0) != msg_success)
	    continue;

	pWWID = wwev2wwid(evp->value.string, &unit, &collision, &w2->wwid);
	w = find_wwidfilter_from_evnum(i, &widx);
	if (!w)
	    continue;
	printf("\n%s\n", &pWWID[14]);
	printf("%-24s  %-20s %-20s %-11s\n", "", "via adapter:", "via fc nport:","connected:");

	for (j = 0; j < MULTIPATH_MAX; j++)
	  {
   	    dprintf("sbs[%d] %x\n", p_args2(j, w->sbs[j]) );
	    if (w->sbs[j])
	      {
		kgsb = w->sbs[j];
		if (!kgsb->valid)
		    continue;
		fc_swap_data(&swap_pn, &kgsb->serv_parm.portName,
		  sizeof(NAME_TYPE));
		sprintf(&buf, "%16.16x", swap_pn);
		pb = kgsb->pb;
		for (k = 1; k <= 4; k++)
		  {
		    char n[3] = {'N', '\0', '\0'};

		    n[1] = '0' + k;
		    if (ev_read(n, &evp, 0) != msg_success)
			continue;

		    if (strcmp(&evp->value.string, &buf) == 0) {
			set_io_name(name, "dg", unit, collision * 1000 + k,
			  pb);
			wwn_sprintf(&buf, &kgsb->serv_parm.portName);
			sprintf(string, "%-24s %-20s %-20s %7s", name, pb->pb.name,
			  buf,Yes_No[(w->online[j] & 0x20) != 0]);
			printf("%s\n", string);
		    }			/* if */

		}			/* for k number of portname evs */
	    }				/* if */
	}				/* for j MULTIPATH_MAX */
    }					/* for i number of wwid evs */

}




void wwidmgr_init( struct QSTRUCT *qual )
{
    wwidmgr_scan( qual );
    fcscan(FCSCAN_MIN);
}

void wwidmgr_delete_wwev(int type, int evnum)
{
    int i;
    int idx;
    char evname[10];
    struct registered_wwid *w;

    if (type == WWID_EV)
      {
	wwid_ev_mask = wwid_ev_mask & ~(1 << evnum);
        w = find_wwidfilter_from_evnum(evnum, &idx);
        if(w)
          {
	   sprintf(&evname, "wwid%d", evnum);
	   ev_write(evname, "", EV$K_STRING | EV$K_SYSTEM);
	   wwids[idx]->evnum = -1;
          }
    } else {
	sprintf(&evname, "N%d", evnum);
	ev_write(evname, "", EV$K_STRING | EV$K_SYSTEM);

    }
}


#if 1
void wwidmgr_display_nv( struct kgpsa_pb *pb )
{
  struct kgpsa_nvram_region nvstack, *nvr = &nvstack;
  kgpsa_read_compaq_nvdata( pb, nvr );
  printf("vendor id: %x\n", nvr->vendor_id );
  printf("nvr version: %x\n", nvr->version );
  printf("checksum: %x\n", nvr->checksum );
  printf("topology: %x\n", nvr->topology );
  printf("alpa: %x\n", nvr->alpa );
}
#endif

