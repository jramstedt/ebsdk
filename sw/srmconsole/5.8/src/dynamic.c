/* file:	dynamic.c
 *
 * Copyright (C) 1990,1993 by
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
 *      Console Firmware
 *
 *  MODULE DESCRIPTION:
 *                                                
 *	Dynamic memory allocation/deallocation for EVAX/Alpha
 *	firmware.  This is a functional superset of C's MALLOC
 *	and FREE functions.
 *
 *  AUTHORS:
 *
 *	AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      07-Mar-1990
 *
 *--
 */  

#include	"cp$src:platform.h"
#include	"cp$src:common.h"
#include	"cp$src:kernel_def.h"
#include	"cp$src:parse_def.h"
#include	"cp$src:dynamic_def.h"
#include	"cp$src:msg_def.h"
#include	"cp$src:ev_def.h"
#include	"cp$inc:prototypes.h"

#define	floor(modulus, x) (((x) / (modulus)) * (modulus))
#define ceiling(modulus, x) (floor ((modulus), (x) + (modulus)))

/*
 * Global references
 */
extern int printf ();
extern int pprintf ();
extern int krn$_post ();
extern int krn$_wait ();
extern int null_procedure ();
extern struct PCB *getpcb ();
extern int quadset ();
#if MODULAR
extern int force_boot_reset;		/* force a reset if we try to boot */
extern int print_debug_flag;
int pdebug_save;
#endif
int force_memzone_flag = 0;
extern unsigned __int64 mem_size;

struct ZONE _align (QUADWORD) defzone;
struct SEMAPHORE_OWNER defzone_sync_owner;
#if TURBO
struct ZONE _align (QUADWORD) smallzone;
struct ZONE _align (QUADWORD) tinyzone;
static void zoneexpand(struct ZONE *zp, int size, int expand);
struct SEMAPHORE _align (QUADWORD) expand_zone_sync;
struct SEMAPHORE _align (QUADWORD) malloc_sync;
#endif
struct ZONE *conzone;
struct ZONE *memzone;
struct ZONE *zlist [MAX_ZONES];
int	zcount = 0;		/* number of zones */
int	conzone_inited = 0;

INT	dyn_flood_malloc;
INT	dyn_flood_free;

#if REGATTA || WILDFIRE || FALCON || PC264
extern struct set_param_table ev_heap_table[];
#endif
/*
 * Occasionally we need to know when a particular location is "covered" by
 * an allocated block.  Set this flag to enable this feature.  When set, you
 * have to define at compile time or patch at run time the field dyn_traploc.
 * Once set, the heap will print a message anytime a block is malloc'd or
 * free'd that contains this address.
 */
/*
 * If you want to allow for tracing of malloc/free requests at run time, then
 * set this flag.  Normally this is left off to increase performance.  When
 * set, and the DYN$K_PRINT option is specified, then a trace message is
 * printed to stderr.
 */
/*+
 * ============================================================================
 * = dyn$_init - Initialize the dynamic memory heap.                          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Initializes a memory zone.  Zones start off as physically contigous
 *	chunks of memory.  They may be expanded later on with additional
 *	physically discontiguous memory chunks.
 *
 * FORM OF CALL:
 *
 *  	dyn$_init ( void *p, int n, int iflood )
 *  
 * RETURNS:
 *
 *	x - address of header for heap
 *       
 * ARGUMENTS:
 *
 *      void *p		- a pointer to raw memory that will become the heap.
 *	int n		- the number of bytes to be used.
 *	int iflood	- flood heap with initial background pattern
 *
 * SIDE EFFECTS:
 *
 *	Memory flooded with initialization pattern.
 *
-*/
void dyn$_init (struct ZONE *zp, struct DYNAMIC *p, int n, int iflood) {
	int		i, j;
	struct DYNAMIC	*mp;
	struct PCB *pcb;

	/* clear the zone structure */
	memset (zp, 0, sizeof (*zp));

	pcb = getpcb ();
	krn$_seminit (&zp->sync, 1, "dyn_sync");
	if (zp == &defzone) {
	    defzone.sync.owner = &defzone_sync_owner;
	    defzone_sync_owner.owner_pcb = 0;
	    defzone_sync_owner.new_affinity = -1;
	    defzone_sync_owner.new_priority = 7;
	}
	krn$_seminit (&zp->release, 0, "dyn_release");

#if TURBO
	if (zp == &defzone) {
	    krn$_seminit (&expand_zone_sync, 1, "expand_zone");
	    krn$_seminit (&malloc_sync, 1, "malloc");
	}
#endif

	zp->size = n;

	if ((unsigned int) p & (DYN$K_GRAN-1)) {
	    pprintf ("dyn$_init: Not on a %d byte boundary\n", DYN$K_GRAN);
	    sysfault (30);
	}


	/*
	 * Poison memory with a background pattern
	 */
	memset (&dyn_flood_malloc, 0, sizeof (dyn_flood_malloc));
	memset (&dyn_flood_free, 0xef, sizeof (dyn_flood_free));
	if (iflood) {
	    quadset (p, (INT) dyn_flood_free, (INT) (n >> 3));
	}

	/*
	 * Set up the queue headers for the free and adjacency lists.
	 */
	zp->header.adj_fl  = (void *) &zp->header.adj_fl;
	zp->header.adj_bl  = (void *) &zp->header.adj_fl;
	zp->header.free_fl = (void *) &zp->header.free_fl;
	zp->header.free_bl = (void *) &zp->header.free_fl;
	zp->header.bsize   = - sizeof (struct DYNAMIC);
	zp->header.backlink = zp;
	zp->header.pid = pcb->pcb$l_pid;
	insq ((int) p + adjacent_bias, &zp->header.adj_fl);
	insq ((int) p + free_bias, &zp->header.free_fl);
	p->bsize = n;
	p->backlink = p;

	/*
	 * Register this zone in the zone list
	 */
	zlist [zcount++] = zp;
	zcount &= MAX_ZONES - 1;
}

/*+
 * ============================================================================
 * = dyn$_zfree - Remove a zone.                                              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Deletes a heap.  This entails releasing the semaphores associated with
 *      the zone.  We assume that there are no allocated blocks in the zone
 *      that are on ownership queues.
 *  
 * FORM OF CALL:
 *
 *  	dyn$_zfree ( void struct ZONE *zp )
 *  
 * RETURNS:
 *
 *	x - msg_success
 *       
 * ARGUMENTS:
 *
 *      struct ZONE *zp	- address of zone header
 *
 * SIDE EFFECTS:
 *
 *      None
 *
-*/
int dyn$_zfree (struct ZONE *zp) {
	int	i;

	if (dyn$_badzone (zp)) return msg_dyn_badzone;

	krn$_semrelease (&zp->sync);
	krn$_semrelease (&zp->release);

	/*
	 * make a minor complaint to the event logger if the zone wasn't
	 * empty.
	 */
	if (zp->allocated) {
	    qprintf ("Zone %08X had %d bytes allocated when freed\n", zp, zp->allocated);
	}

	/*
	 * Find the zone and remove it from the zone list
	 */
	for (i=0; i<zcount; i++) {
	    if (zlist [i] == zp) {
		zlist [i] = zlist [--zcount];
		break;
	    }
	}

	return	msg_success;
}

static int compare_integers (int a, int b) {
	return a - b;
}


/*+
 * ============================================================================
 * = dyn$_expand - Expand a zone with a (possibly) discontiguous memory block =
 * ============================================================================
 *
 * OVERVIEW:
 *
 *	Add a block of memory to a zone.  The block may be anywhere in main
 *	memory, and have any arbitrary positional relationship to the zone.
 *	The block must aligned on a DYN$K_GRAN boundary, and have a size
 *	that is a multiple of DYN$K_GRAN.
 *
 *	Don't check for cases where the block has already been accounted for
 *	by the zone.
 *
 *	In adding the block, we have to maintain the address ordering of the
 *	adjacency list and free list.
 *
 * FORM OF CALL:
 *
 *  	dyn$_expand ( zp, block_address, size, iflood )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *	msg_failure - block size/alignment errors
 *                                               
 * ARGUMENTS:
 *
 *      struct ZONE *zp	- a pointer to the zone to be expanded
 *	void *block_address	- address of block to be added
 *	int size	- the number of bytes to be added.
 *	int iflood	- flood heap with initial background pattern
 *
 *
-*/
int dyn$_expand (struct ZONE *zp, int p, unsigned int size, int iflood) {
	struct DYNAMIC *np;

	/* verify alignment */
	if ((unsigned int) p & (DYN$K_GRAN-1)) {
	    pprintf ("dyn$_expand: not on a %d byte boundary\n", DYN$K_GRAN);
	    return msg_failure;
	}

	/* verify size */
	if (size & (DYN$K_GRAN - 1)) {
	    pprintf ("dyn$_expand: size %d is not a multiple of %d\n", size, DYN$K_GRAN);
	    return msg_failure;
	}
                                                                              
	if (iflood) {
	    quadset (p, (INT) dyn_flood_free, (INT) (size >> 3));
        }

	/* lock the heap */
	krn$_wait (&zp->sync);

	np = p;
	np->bsize = size;
	np->backlink = np;
	zp->size += size;     

	/* Put the block into the adjacency and free lists.  These lists
	 * must be maintained in increasing address order.
	 */
	insq_sorted (&np->adj_fl, &zp->header.adj_fl, compare_integers);
	insq_sorted (&np->free_fl, &zp->header.free_fl, compare_integers);

	/*
	 * The block may be adjacent to a free block, in which case there
	 * is a opportunity to to merge. (The consistency checker will
	 * complain about physically adjacent free blocks).  dyn_merge is
	 * robust and won't merge unless all checks pass.
	 */
	dyn_merge (zp, np, 0);				/* merge with next block */
	dyn_merge (zp, np->adj_bl - adjacent_bias, 0);	/* merge with previous block */

	/* unlock the heap */
	krn$_post (&zp->sync);

	return msg_success;

}

/*+
 * ============================================================================
 * = dyn$_badzone - Verify that an address is a legal zone.                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Verifies that an address points to a legitimate zone.  The heap keeps
 *      track of all defined zones, so we simply walk down the list looking
 *      for a match.
 *  
 * FORM OF CALL:
 *
 *  	dyn$_badzone ( struct ZONE *zp )
 *  
 * RETURNS:
 *
 *	msg_success	- zone is good
 *	msg_dyn_badzone	- zone is bad
 *       
 * ARGUMENTS:
 *
 *      struct ZONE *zp	- address of zone
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int dyn$_badzone (struct ZONE *zp) {
	int	i;

	for (i=0; i<zcount; i++) {
	    if (zp == zlist [i]) return 0;
	}

	return 1;
}

/*+
 * ============================================================================
 * = dyn$_chown - Change the owner of a dynamic memory block.                 =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Changes the owner of a memory block to the current caller.  
 *	When memory is dynamically allocated, the block is put on 
 *	the calling process' ownership queue.  There are occasions when
 *	subsequent processes will want to get ownership.
 *
 *	Assume that the block to be chowned is in the default heap.
 *  
 * FORM OF CALL:
 *  
 *	dyn$_chown ( p )
 *  
 * RETURNS:
 *
 *	x - address of memory block
 *       
 * ARGUMENTS:
 *
 * 	dynamic *p	- pointer to dynamic memory block
 *
 * SIDE EFFECTS:
 *
 *	The block is put on the ownership queue of the invoking process.
 *
-*/
int dyn$_chown (struct DYNAMIC *block) {
	struct PCB	*pcb;
	struct DYNAMIC	*p;
	unsigned int	*bp;
	struct ZONE	*zp;

	pcb = getpcb();
	bp = block;
	bp--;
	bp = (int) bp & ~3;
	p = *bp;

	/*
	 * Remove from other process and insert on our own.  What happens
	 * if the other process has already died ?
	 */
	zp = &defzone;
	krn$_wait (&zp->sync);
	remq (&p->free_fl);
	insq (&p->free_fl, &pcb->pcb$r_dq);
	krn$_post (&zp->sync);

	return (int) block;
}

/*+
 * ============================================================================
 * = dyn$_flush - Free all allocated blocks on an ownership queue.            =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Deallocates all blocks on an ownership queue. When a block of
 *	memory is allocated the block is always put on an ownership queue.
 *	The ownership queue header is found in the PCB.
 *  
 * FORM OF CALL:
 *  
 *	dyn$_flush ( queue_header )
 *  
 * RETURNS:
 *
 *	msg_success - success
 *       
 * ARGUMENTS:
 *
 * 	qh	- ownership queue from which blocks are to be freed.
 *
 * SIDE EFFECTS:
 *
 *	None
 *
-*/
int dyn$_flush (struct QUEUE *qh) {
	struct DYNAMIC	*p;
        struct QUEUE	*qe;

	/*
	 * We assume that only one thread is accessing the ownership queue
	 * at this time, so we don't have to synchronize while traversing
	 * the queue.
	 */
	qe = qh->flink;
	while (qe != qh) {
	    p = (void *) ((int) qe - (free_bias));
	    p->backlink = p;
	    qe = qe->flink;
	    free ((int) p + sizeof (struct DYNAMIC));
	}

	return (int) msg_success;
}

/*+
 * ============================================================================
 * = dyn$_malloc - Allocate a block of memory.                                =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Allocates a block of memory.  This is the basic memory allocation
 *	routine upon which all other allocation routines are built.  It
 *	is functionally similar to the malloc routine found in most C run
 *	time environments.
 *
 *	If there is not enough memory to satisfy the request, this routine
 *	waits on a binary semaphore that is posted whenever memory is
 *	deallocated, so from the caller's perspective this routine always
 *	returns with a valid memory pointer (or if the DYN$K_NOWAIT option 
 *      is selected returns a null pointer).
 *
 *	DYN$_MALLOC has an additional parameter, OPTIONS, that the standard
 *	C run time library does not have.  This option is a bitmask that has
 *	the following values:
 *#o
 *    o DYN$K_NOSYNC - don't make any kernel calls to synchronize.  Only 
 *	useful in certain power up situations where the kernel has not been
 *	fully initialized.
 *
 *    o	DYN$K_NOOWN - don't put the allocated block on the process's ownership
 *	queue.
 *
 *    o	DYN$K_PRINT - print to the polled TTY device the allocation quantity.
 *	This option is useful for tracking down memory leaks etc in an elevated
 *	IPL.
 *
 *    o	DYN$K_ALIGN - force alignment on a boundary. The boundary is determined
 *	by the equation (a % m) = r, where "a" is the address, "m" is a modulus
 *	(not necessarily a power of 2) and "r" is the remainder.  Suitable
 *	choices of m and r allow a user to allocate memory at a particular 
 *	physical address.
 *
 *    o	DYN$K_ZONE - user specified zone instead of the default zone.   Users
 *	may create their own heaps at runtime.
 *
 *    o	DYN$K_FLOOD - flood the block with 0 before returning (this is the
 *	default action).
 *
 *    o	DYN$K_NOWAIT - return a null pointer if there is not enough free 
 *      memory to satisfy the request, as opposed to waiting for it.
 *#
 *  
 * FORM OF CALL:
 *  
 *	dyn$_malloc ( size, option, modulus, remainder, zone )
 *  
 * RETURNS:
 *
 *	x - address of memory block
 *       
 * ARGUMENTS:
 *
 *      unsigned int size	- minimum number of bytes to allocate.
 *	unsigned int option	- options flag
 *	unsigned int modulus	- modulus for allocated address
 *	unsigned int remainder	- remainder for allocated address
 *	struct ZONE *zone       - pointer to user specified zone
 *
 * SIDE EFFECTS:
 *
 *	Other routines may be scheduled as a result of a resource wait.  Caller
 *	may be suspended if not enough memory.
 *
-*/

#if DYNAMIC_PRINTING
char dyn_process_name [32] = " ";
#endif
/* if dyn_process_name[0] == 1 do_bpt on errors */

int malloc(int size)
{
    int value;
    struct PCB *pcb;

    value = dyn$_malloc (size, DYN$K_SYNC|DYN$K_FLOOD, 0, 0, 0);

#if DYNAMIC_PRINTING
    pcb = getpcb ();
    if (!strcmp_nocase (dyn_process_name, pcb->pcb$b_name)) {
	pprintf( "malloc: pid %x (%s) pc %08X adr %08X\n",
	      pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), value);
    }
#endif

    return value;
}

int malloc_noown(int size)
{
    int value;
    struct PCB *pcb;

    value = dyn$_malloc (size, DYN$K_SYNC|DYN$K_FLOOD|DYN$K_NOOWN, 0, 0, 0);

#if DYNAMIC_PRINTING
    pcb = getpcb ();
    if (!strcmp_nocase (dyn_process_name, pcb->pcb$b_name)) {
	pprintf( "malloc: pid %x (%s) pc %08X adr %08X\n",
	      pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), value);
    }
#endif

    return value;
}

int dyn$_malloc (unsigned int size, unsigned int option,
	protoargs unsigned int modulus, unsigned int remainder, 
	unsigned int zone)
{
	int	i;
	int	j;
	int	id;
	int	found;
	int	zexpand_flag;
	struct DYNAMIC	*fp, *ap; /* pointers to free and allocated blocks */
	struct PCB	*pcb;
	struct ZONE	*zp;
	unsigned int	m, r;
	unsigned int	ap_lower, ap_upper;
	unsigned int	fp_lower, fp_upper;
	unsigned int	*bp;
	int heap_work;

#if RAWHIDE
	zexpand_flag = 0;
#endif
#if TURBO
	if ((option & DYN$M_ZONE) != DYN$K_ZONE) {
	    zexpand_flag = 0;
	    krn$_wait(&malloc_sync);
	}
#endif
	id = whoami();

#if DYNAMIC_PRINTING
	pcb = getpcb ();
	if (!strcmp_nocase (dyn_process_name, pcb->pcb$b_name))
	    option |= DYN$M_PRINT;
#endif

	/* Figure out our alignment */

	if ((option & DYN$M_ALIGN) == DYN$K_ALIGN) {
	    m = modulus;
	    r = remainder;
	} else {
	    m = DYN$K_GRAN;
	    r = 0;
	}
	if (size == 0) size = DYN$K_GRAN;
	
	/* determine what zone to use */

	if (option & DYN$M_ZONE) {
	    zp = (struct ZONE *)zone;
	} else {
	    if (conzone_inited == 0) {
		zp = &defzone;
#if TURBO
		krn$_wait(&expand_zone_sync);
		if (force_memzone_flag) {
		    zp = memzone;
		} else {
		    if ((option & DYN$M_ALIGN) != DYN$K_ALIGN) {
			if (size <= 32) {
			    zp = &tinyzone;
			    zexpand_flag = 1;
			    zoneexpand(zp,64,4*1024);
			} else {
			    if (size <= 256) {
				zp = &smallzone;
				zexpand_flag = 2;
				zoneexpand(zp,512,4*1024);
			    }
			}
		    }
		}
		krn$_post(&expand_zone_sync);
#endif
	    } else {
		zp = conzone;
	    }
	}

#if TURBO
	if ((option & DYN$M_ZONE) != DYN$K_ZONE) {
	    krn$_post(&malloc_sync);
	}
#endif
	/* Serialize access to the data structures */

	if ((option & DYN$M_SYNC) == DYN$K_SYNC) {
	    krn$_wait (&zp->sync);
	}

	/*
	 * Walk down the free list until we find a block big enough
	 * to handle the request.  If there isn't enough memory, then
	 * we have to release the semaphore and:
         * return a null pointer if DYN$K_NOWAIT
         * else,  go into a resource wait state, and repeat the process. 
	 */
	found = 0;
	while (!found) {
	    fp = (void *) ((int) zp->header.free_fl - free_bias);
	    while (((int) fp + free_bias) != (int) &zp->header.free_fl) {

		/*
		 * Calculate the bounds of the current block, and
		 * the bounds of a block that would have the correct alignment.
		 */
		fp_lower = (int) fp + sizeof (struct DYNAMIC);
		fp_upper = (int) fp + fp->bsize;

		ap_lower = floor (m, fp_lower) + r;
		if (ap_lower < (int) fp + sizeof (*fp)) {
		    ap_lower += m;
		}

		ap_upper = ceiling (DYN$K_GRAN, ap_lower + size - 1);
		if ((fp_lower <= ap_lower)  && (ap_lower < fp_upper)  && (ap_upper <= fp_upper)) {

		    /*
		     * We can trim off the bottom and/or top if there are
		     * at least DYN$K_FRAG bytes left over.
		     */
		    ap = floor (DYN$K_GRAN, ap_lower - sizeof (struct DYNAMIC));
		    ap = dyn_split (fp, ap);	/* trim from the bottom */
		    dyn_split (ap, ap_upper);	/* trim from the top */

		    /*
		     * Remove from free list
		     */
		    remq (&ap->free_fl);
		    found = 1;
		    break;
		}
		fp = (void *) ((int) fp->free_fl - free_bias);
	    }

	    /* not enough room in the heap */
	    if (!found) {

		/* caller will handle the not enough room case */
		if ((option & DYN$M_WAIT) == DYN$K_NOWAIT) {
		    if ((option & DYN$M_SYNC) == DYN$K_SYNC) {
		       krn$_post (&zp->sync);
		    }
                    return 0;

		/*
		 * No more room, and caller can't handle not enough room
		 * so we crash (in a firmware context, it's not likely that
		 * room will become available).
		 */
                } else if ((option & DYN$M_SYNC) == DYN$K_SYNC) {
#if TL6
	write_csr_32(0x8e001680, 0);		/* debug trigger */
#endif
		    pprintf("CPU%d: ", id);
		    pprintf (msg_insuff_mem, size);
#if SABLE
		    if (zp == &defzone) {
		       pprintf("Operator intervention required : increase heap_expand environment variable\n");
		    }
#endif
#if REGATTA || WILDFIRE || FALCON || PC264
#define EV_HEAP_EXPAND 0x18
#define EV_HEAP_EXPAND_VALID 0x19
		    /* autmatically increase the value in heap_expand so when the console comes back up there
		     * will be memory available.
		     */
		    if (zp == &defzone) {
			heap_work = rtc_read (EV_HEAP_EXPAND);
			if (((-heap_work)&0xff) == rtc_read(EV_HEAP_EXPAND_VALID)) {
			    for (i=0; ev_heap_table[i].param != 0; i++) {
				if (!strcmp( ev_heap_table[heap_work].param, ev_heap_table[i].param)) {
				    if ((ev_heap_table[i].param) && (ev_heap_table[i+1].param)) {
					i++;
					rtc_write (EV_HEAP_EXPAND, i);
					rtc_write (EV_HEAP_EXPAND_VALID, -i);
					pprintf ("Console heap space will be automatically increased in size by %dKB\n",
							ev_heap_table[i].code);
				    }
				    break;
				}
			    }
			}
		    }
#endif
#if MODULAR
		    krn$_post (&zp->sync);
		    pdebug_save = print_debug_flag;
		    print_debug_flag = 1;
		    pprintf("pid %x (%s) pc %08X x %x\n", pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), zexpand_flag);
		    dyn$_check();			/* show dynamic memory usage */
#endif
		    dyn$_procstat (zp, 1);
#if MODULAR
		    print_debug_flag = pdebug_save;
#endif
		    sysfault (30);
		}
	    }
	}

	pcb = getpcb();
	zp->allocated += ap->bsize;
#if DYNAMIC_PRINTING
        if (zp->allocated > zp->size) {
	    if (dyn_process_name[0] != 0x20) {
		pprintf("MALLOC: Allocated is bigger than zone size\n");
		pprintf("  zone %x, size %x, allocated %x\n", zp, zp->size, zp->allocated);
		pprintf("  pid %x (%s) pc %08X requested %5d got %5d adr %08X\n",
		  pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), size, ap->bsize, ap_lower);
		if (dyn_process_name[0] == 1)
		    do_bpt();
	    }
        }
#endif
	if (zp->allocated > zp->hiwater) {
	   zp->hiwater = zp->allocated;
	}

#if DYNAMIC_PRINTING
	if (option & DYN$M_PRINT) {
	    pprintf( "pid %x (%s) pc %08X requested %5d got %5d adr %08X\n",
	      pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), size, ap->bsize, ap_lower);
	}
#endif

	/*
	 * Force the size to be negative, thus indicating that this block
	 * is in use.
	 */
	ap->bsize = - ap->bsize;

	/*
	 * Put the block on this process's ownership queue, or create
	 * a null entry otherwise.
	 */
	if ((option & DYN$M_OWN) == DYN$K_OWN) {
	    insq ((int) ap + free_bias, &pcb->pcb$r_dq);
 	} else {
	    ap->free_fl = (void *) &ap->free_fl;
	    ap->free_bl = (void *) &ap->free_fl;
 	}

	/*
	 * Unlock access to the structures
	 */
	if ((option & DYN$M_SYNC) == DYN$K_SYNC) {
	    krn$_post (&zp->sync);
	}


	/*
	 * Poison newly allocated memory if requested. Poison
	 * the entire block, even if the user is allocating
	 * on a non aligned boundary.
	 */
	if ((option & DYN$M_FLOOD) == DYN$K_FLOOD) {
	    quadset (
		(unsigned int) ap + sizeof (struct DYNAMIC),
		(INT) dyn_flood_malloc,
		(INT) ((-ap->bsize - sizeof (struct DYNAMIC)) >> 3)
	    );
	}
	ap->pid = pcb->pcb$l_pid; /* save requestor PID for catching leaks */
	/*
	 * Install the backlink to the header.  This has to be done after
	 * the poison, as the backlink may be in the block if allocated
	 * on a non aligned boundary.
	 */
	bp = (int) (ap_lower - sizeof (struct DYNAMIC) + offsetof (struct DYNAMIC, backlink)) & ~3;
	*bp = ap;

	/* remember the zone pointer */
	ap->zone = zp;

#if DYNAMIC_TRAPPING
	if (dyn$_trap (ap_lower)) {
	    pprintf ("%08X trapped by dyn$_malloc in %s at %08X\n",
		ap_lower,
		getpcb ()->pcb$b_name,
		callers_pc ()
	    );
	}
#endif
	return ap_lower;
}

/*
 * Local helper routine for  malloc to split a free block.  This routine 
 * runs inside dyn_sync.
 * FP is a free block, and NP is the address where the new block will start.
 *
 * Return the orginal block if no split is possible, otherwise return the
 * higher address block.
 */
int dyn_split (struct DYNAMIC *fp,  struct DYNAMIC *np) {
	int	fp_bsize, np_bsize;

	/*
	 * Don't split if either resulting block would be smaller
	 * than DYN$K_FRAG.
	 */
	fp_bsize = (int) np - (int) fp;
	np_bsize = fp->bsize - fp_bsize;

	if ((fp_bsize < DYN$K_FRAG) || (np_bsize < DYN$K_FRAG)) {
	    return fp;
	}

	insq (&np->adj_fl, &fp->adj_fl);
	insq (&np->free_fl, &fp->free_fl);

	np->bsize = np_bsize;
	fp->bsize = fp_bsize;
	np->backlink = np;

	return np;
}

/*+
 * ============================================================================
 * = dyn$_free - Return a previously allocated block to the heap.             =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Returns a previously allocated block of memory to a heap.
 *	The pointer passed MUST be the same that MALLOC returned, otherwise
 *	the routine would be forced to walk down all the allocated blocks
 *	to figure out on which block the pointer hits.
 *
 *	DYN$_FREE will search the zone list to verify that the zone remembered
 *	in the header is still valid.
 *
 *	DYN$_FREE will also allow null pointers to be freed.
 *
 * FORM OF CALL:
 *  
 *	dyn$_free ( p, options )
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 *      void *p		- pointer to a block of allocated memory.
 *	int option	- run synchronized / unsynchronized.
 *
 * SIDE EFFECTS:
 *
 *      Other tasks may be posted as a result of their waiting for
 *	free memory.  These tasks may be of a higher priority, thus
 *	delaying the completion of this routine for an undetermined
 *	amount of time.
 *
-*/
int free (int p)
{
    int status;
    struct PCB *pcb;

    status = dyn$_free (p, DYN$K_SYNC| DYN$K_FLOOD);

#if DYNAMIC_PRINTING
    pcb = getpcb ();
    if ((!strcmp_nocase (dyn_process_name, pcb->pcb$b_name)) || status != msg_success) {
	pprintf( "free: pid %x (%s) pc %08X adr %08X\n",
	      pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), p);
    }
#endif

    return status;
}

int dyn$_free (void *p, int option) {
	struct DYNAMIC *ap;	/* allocated pointer	*/
	struct DYNAMIC *fp;
	struct ZONE	*zp;
	unsigned int	*bp;
	struct PCB	*pcb;
	int		status = msg_success;

#if DYNAMIC_PRINTING
	pcb = getpcb ();
	if (!strcmp_nocase (dyn_process_name, pcb->pcb$b_name)) {
	    option |= DYN$M_PRINT;
	}
#endif

	/* Complain about null pointers, but still allow it. */
	if (!p) {
	    err_printf ("process %s %08X freeing a null pointer\n",
		getpcb()->pcb$b_name,
		getpcb()->pcb$l_pid
	    );
#if DYNAMIC_PRINTING
	    if (dyn_process_name[0] == 1)
		 do_bpt();
#endif
	    return msg_failure;
	}

#if DYNAMIC_TRAPPING
	if (dyn$_trap (p)) {
	    pprintf ("%08X trapped by dyn$_free in %s at %08X\n",
		p,
		getpcb ()->pcb$b_name,
		callers_pc ()
	    );
	}
#endif

	/* get the pointer to the header */
	bp = ((int) p - 4) & ~3;
	ap = *bp;

	/*
	 * complain if the block isn't in any zone
	 */
	zp = ap->zone;
	if (dyn$_badzone (zp)) {
	    err_printf ("Block %08X is not in any zone\n", p);
#if DYNAMIC_PRINTING
	    if (dyn_process_name[0] == 1)
		 do_bpt();
#endif
	    return msg_failure;
	}

#if DYNAMIC_PRINTING
	if (option & DYN$M_PRINT) {
	    pcb = getpcb();
	    pprintf("pid %x (%s) pc %08X free adr %08X %d\n",
	      pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), ap, -ap->bsize);
	}
#endif

	/*
	 * Poison data portion of block if requested
	 */
	if ((option & DYN$M_FLOOD) == DYN$K_FLOOD) {
	    quadset (
		(unsigned int) ap + sizeof (struct DYNAMIC),
		(INT) dyn_flood_free,
		(INT) ((abs (ap->bsize) - sizeof (struct DYNAMIC)) >> 3)
	    );
	}

	/*
	 * Lock up access to memory
	 */
	if ((option & DYN$M_SYNC) == DYN$K_SYNC) {
	    krn$_wait (&zp->sync);
	}

	zp->allocated += ap->bsize;

#if DYNAMIC_PRINTING
	if (dyn_process_name[0] != 0x20) {
	    if (ap->bsize > 0) {
		pcb = getpcb();
		pprintf("FREE: Freeing a block with a positive size %08X\n", ap->bsize);
		pprintf("  pid %x (%s) pc %08X free adr %08X\n", pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc(), ap);
		pprintf("  zone %x, size %x, allocated %x\n", zp, zp->size, zp->allocated);
		status = msg_failure;
		if (dyn_process_name[0] == 1)
		   do_bpt();
	    }
        }
#endif
	/*
	 * Release the block from the ownership queue
	 */
	remq (&ap->free_fl);

	/*
	 * Search the free list to find out where this block needs to 
	 * be inserted.  We search the free list instead of the adjacency
	 * list, since the free list is typically two orders of magnitude
	 * smaller than the adjacency list.  Assume that there is always
	 * at least one free block in the heap.
	 */
	fp = (void *) ((int) zp->header.free_bl - free_bias);
	while (((int) fp + free_bias) != (int) &zp->header.free_fl) {
	    if ((unsigned int) fp < (unsigned int) ap) {
		break;
	    }
	    fp = (void *) ((int) fp->free_bl - free_bias);
	}
	insq (&ap->free_fl, &fp->free_fl);

	ap->bsize = abs (ap->bsize);

	/*
	 * See if we can coalesce the newly freed block with the 
	 * logically adjacent blocks.  To do this we have to verify that
	 * the blocks are both adjacent AND free.
	 */
	dyn_merge (zp, ap, option);
	dyn_merge (zp, (int) ap->adj_bl - adjacent_bias, option);

	/*
	 * Release the lock on memory, and post anybody waiting on
	 * free memory (a binary semaphore).
	 */
	if ((option & DYN$M_SYNC) == DYN$K_SYNC) {
	    krn$_post (&zp->sync);
/*	    krn$_bpost (&zp->release); */
	}
	return status;
}

/*
 * dyn_merge
 *
 * Helper routine for dyn$_free that tries to merge the current block
 * with the next logically adjacent block (checking that both are free and
 * physically adjacent).
 */
int dyn_merge (struct ZONE *zp, struct DYNAMIC *ap, int options) {
	struct DYNAMIC *np;

	/*
	 * Verify that both are free
	 */
	np = (void *) ((int) ap->adj_fl - adjacent_bias);
	if (np->bsize < 0) return 0;
	if (ap->bsize < 0) return 0;


	/* We can't merge if the next logical adjacent block isn't
	 * physically contiguous (we allow for holes in memory).  This
	 * test also prevents us from merging with the header block.
	 */
	if (((int) (ap) + ap->bsize) != (int) np) return 0;

	/*
	 * Coalesce, and the next block from the adjacency list and the
	 * free list.
	 */
	ap->bsize += np->bsize;
	remq (&np->free_fl);
	remq (&np->adj_fl);

	/*
	 * Poison the next block's header
	 */
	if ((options & DYN$M_FLOOD) == DYN$K_FLOOD) {
	    quadset (np, (INT) dyn_flood_free, (INT) (sizeof (struct DYNAMIC) >> 3));
	}

	return 0;
}


/*+
 * ============================================================================
 * = dyn$_realloc - Trim or expand a block of memory to a new size.           =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *      Returns a pointer to a block that's smaller or larger than the original
 *      block after copying the contents of the original block to the new one.
 *      Works only with blocks of DYNAMIC type.
 *  
 *      Most calls to realloc in the system are to expand memory, not to
 *	compress.  Since blocks are allocated from the top down, and every
 *      block almost always has adjacent blocks that are used, trying to
 *      extend the block will almost always fail.  For these reasons, we take
 *      the simple approach and layer realloc on top of malloc and free.
 *
 * FORM OF CALL:
 *  
 *	dyn$_realloc ( p, new_size, option, modulus, remainder, zone )
 *  
 * RETURNS:
 *
 *	address of new block
 *       
 * ARGUMENTS:
 *
 * 	void *p               -	pointer to mem block created by dyn$_malloc.
 *	unsigned int new_size - size of data portion of new block to alloc.
 *	int option            - control flags (see dyn$_malloc).
 *	unsigned int modulus	- modulus for allocated address
 *	unsigned int remainder	- remainder for allocated address
 *	struct ZONE *zone     - memory zone from which the block was allocated.
 *
 * SIDE EFFECTS:
 *
 *      Execution time is undetermined due to other processes being scheduled
 *	as a result of resource waits being satisfied.
 *
-*/
int dyn$_realloc (void *p, unsigned int new_size, unsigned int option,
	protoargs unsigned int modulus, unsigned int remainder, 
	unsigned int zone)
{
	struct DYNAMIC	*q, *qp;
        struct ZONE	*zp;
	unsigned int old_size;
	unsigned int *bp;
	int id;
	int zexpand_flag = 0;

	id = whoami();

	if (option & DYN$M_ZONE) {
	    zp = zone;
	} else {
	    if (conzone_inited == 0) {
		zp = &defzone;
#if TURBO
		krn$_wait(&expand_zone_sync);
		if (new_size <= 32) {
		    zp = &tinyzone;
		    zexpand_flag = 3;
		    zoneexpand(zp,64,4*1024);
		} else {
		    if (new_size <= 256) {
			zp = &smallzone;
			zexpand_flag = 4;
			zoneexpand(zp,512,4*1024);
		    }
		}
		krn$_post(&expand_zone_sync);
#endif
	    } else {
		zp = conzone;
	    }
	}
	bp = p;
	bp--;
	bp = (int) bp & ~3;
	q = *bp;
	old_size = abs (q->bsize) - sizeof(struct DYNAMIC);

	q = dyn$_malloc (new_size, option | DYN$M_ZONE, modulus, remainder, zp);
	if (q == 0) {
	    pprintf("CPU%d: realloc ", id);
	    pprintf (msg_insuff_mem, new_size);
	    return 0;
	}
	memcpy(q, p, ((old_size > new_size) ? new_size : old_size));
	dyn$_free (p, option);
	return (unsigned int) q;

}

unsigned int realloc (void *p, int new_size) {
	return dyn$_realloc (p, new_size, DYN$K_SYNC| DYN$K_FLOOD, 0, 0, 0);
}

unsigned int realloc_noown (void *p, int new_size) {
	return dyn$_realloc (p, new_size, DYN$K_SYNC| DYN$K_FLOOD | DYN$K_NOOWN, 0, 0, 0);
}

#if DYNAMIC_TRAPPING
unsigned int dyn_traploc;

/*
 * Report when a block covers a memory location
 */
int dyn$_trap (unsigned int p) {
	struct DYNAMIC *dp;
	unsigned int *bp;
	unsigned int lower;
	unsigned int upper;

	bp = ((int) p - 4) & ~3;
	dp = *bp;

	lower = (unsigned int) dp;
	upper = lower + abs (dp->bsize) - 1;
	if ((lower <= dyn_traploc)  && (dyn_traploc <= upper)) {
	    return 1;
	}
	return 0;
}
#endif

/*
 * Helper routine for DYNAMIC that prints out a usage summary on a per
 * process basis
 *
 *	To get a sorted list of the per process allocation:
 *
 *	dyn -p |grep '^0' |grep -v '\%' | sort 
 */
#define MAXP 128
void dyn$_procstat (struct ZONE *zp, int locked) {
	struct DYNAMIC	*p;
	struct PCB *pcb;
	int	i;
	int (*print) ();
	int (*wait) ();
	int (*post) ();

	struct process {
	    unsigned int	pid;
	    unsigned int	bytes;
	} prstat [MAXP];
	int	 prc;
	extern struct SEMAPHORE spl_kernel;
	char namebuf [MAX_NAME];

	/*
	 * So that this routine may run with the zone sync semaphore
	 * taken out, and also not taken out, we have to redirect certain
	 * routines.
	 */
	if (locked) {
	    print = pprintf;
	    wait = null_procedure;
	    post = null_procedure;
	} else {
	    print = printf;
	    wait = krn$_wait;
	    post = krn$_post;
	}

	/* capture the data */
	memset (prstat, 0, sizeof (prstat));
	prc = 0;
	(*wait) (&zp->sync);
	p = (void *) &zp->header.adj_fl;
	do {
	    p = (void *) ((int) p - adjacent_bias);
	    if (p->bsize < 0) {
		for (i=0; i<prc; i++) {
		    if (prstat [i].pid == p->pid) break;
		}
		if (i >= prc) prc++;
		if (prc > MAXP) break;
		prstat [i].pid = p->pid;
		prstat [i].bytes += abs (p->bsize);
	    }
	    p = p->adj_fl;
	} while (p != (void *) &zp->header.adj_fl);
	(*post) (&zp->sync);

	/*
	 * print it out.
	 */
	(*print) ("   PID       bytes  name\n");
	(*print) ("-------- ---------- ----\n");
	for (i=0; i<prc; i++) {
	    (*print) ("%08X %10d ", prstat [i].pid, prstat [i].bytes);
	    if (pcb = krn$_findpcb (prstat [i].pid)) {
		strcpy (namebuf, pcb->pcb$b_name);
		spinunlock (&spl_kernel);
		(*print) ("%s\n", namebuf);
	    } else {
		(*print) ("????\n");
	    }
	}
	
}
#undef MAXP

#if MODULAR
int conzone_init()
{
    unsigned int i, j;

/* If CONZONE was already initialized, then just return */

    if (conzone_inited) {
	return msg_success;
    }

/* Set force boot reset flag to insure that things are cleaned up */

    force_boot_reset = 1;		/* force a reset if we try to boot */

/* No memory present, so don't allow init */

    if (mem_size <= 0x1000000) {
	pprintf("Unable to initialize CONZONE. Insufficient memory.\n");
	return msg_failure;;
    }

    j = 8 * 1024 * 1024;		/* default size of 8Mb */

    i = 8 * 1024 * 1024;		/* default base at 8Mb */

    qprintf("Initializing CONZONE starting at address %x of size %x\n", i, j);

    conzone = dyn$_malloc(sizeof(struct ZONE),DYN$K_SYNC | DYN$K_NOOWN,
	0, 0, 0);

    if (conzone == 0) {
	pprintf("Unable to malloc Conzone pointer\n");
	return msg_failure;
    }

    dyn$_init(conzone, i, j, 1);

    conzone_inited = 1;
    return msg_success;
}

int conzone_remove()
{
    struct PCB *pcb = getpcb ();

/* If no CONZONE then just return */

    if (!conzone)
	return msg_success;

    qprintf("Removing CONZONE, High Water %d\n", conzone->hiwater);

    if( conzone->allocated ) {
	pprintf("Warning -- console zone is not free, alloc = %d\n", conzone->allocated);
	pdebug_save = print_debug_flag;
	print_debug_flag = 1;
	pprintf("pid %x (%s) pc %08X\n", pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc());
	dyn$_check();			/* show dynamic memory usage */
	print_debug_flag = pdebug_save;
#if DYNAMIC_PRINTING
	if (dyn_process_name[0] == 1)
	    do_bpt();
#endif
    } else {
	dyn$_zfree(conzone);
	free(conzone);
	conzone = 0;
    }
    conzone_inited = 0;
}
#endif

#if TURBO
static void zoneexpand(struct ZONE *zp, int size, int expand)
{
    int i;
    int found;
    struct DYNAMIC *fp, *ap;		/* pointers to free and allocated blocks */
    struct PCB *pcb;
    u_int m = 32;
    u_int r = 0;
    u_int ap_lower, ap_upper;
    u_int fp_lower, fp_upper;
    u_int *bp;

    pcb = getpcb();

    found = 0;

    while (!found) {

	krn$_wait (&zp->sync);
	fp = (void *) ((int) zp->header.free_fl - free_bias);

	while (((int) fp + free_bias) != (int) &zp->header.free_fl) {
	    /*
	     * Calculate the bounds of the current block, and
	     * the bounds of a block that would have the correct alignment.
	     */
	    fp_lower = (int) fp + sizeof (struct DYNAMIC);
	    fp_upper = (int) fp + fp->bsize;

	    ap_lower = floor(m, fp_lower) + r;
	    if (ap_lower < (int) fp + sizeof (*fp))
		ap_lower += m;

	    ap_upper = ceiling(DYN$K_GRAN, ap_lower + size - 1);

	    if ((fp_lower <= ap_lower) && (ap_lower < fp_upper) && (ap_upper <= fp_upper)) {
		found = 1;
		break;
	    }
	    fp = (void *) ((int) fp->free_fl - free_bias);
	}
	krn$_post (&zp->sync);

	if (!found) {			/* not enough room in the heap */
#if DYNAMIC_PRINTING
	    if (dyn_process_name[0] == 1) {
		pprintf("zoneexpand: %x %d %d\n", zp, size, expand);
		pprintf("  pid %x (%s) pc %08X\n", pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc());
	    }
#endif
	    if (conzone_inited == 0)		/* get room from default zone */
		i = dyn$_malloc (expand, DYN$K_SYNC|DYN$K_ZONE|DYN$K_NOOWN, 0, 0, &defzone);
	    else
		i = dyn$_malloc (expand, DYN$K_SYNC|DYN$K_ZONE|DYN$K_NOOWN, 0, 0, conzone);
	    if (i != 0)
		i = dyn$_expand(zp, i, expand, 1);
	    else
		i = msg_failure;
	    if (i != msg_success) {
		pprintf("Unable to expand zone %x\n", zp);
		pdebug_save = print_debug_flag;
		print_debug_flag = 1;
		pprintf("pid %x (%s) pc %08X\n", pcb->pcb$l_pid, pcb->pcb$b_name, callers_pc());
		dyn$_check();			/* show dynamic memory usage */
		print_debug_flag = pdebug_save;
		sysfault(30);
	    }
	}
    }
}
#endif

/*+
 * ============================================================================
 * = dyn$_headers - Dump the headers in the heap in a readable form.          =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Dumps the state of the heap in a readable form to standard output. 
 *	Various options control the rendering of the ownership and contents
 *	of each individual block.
 *
 *	We have to snapshot the state of the heap into a private buffer, and
 *	report out of that private buffer.  (consider the situation where
 *	we take out the dyn_sync semaphore and the output of this routine
 *	is being directed to a file that will be extended with an allocation
 *	from the heap).
 *
 *	Since the heap may discontiguous, print delimiters on the
 *	discontinuities.
 *  
 * FORM OF CALL:
 *  
 *	dyn$_headers ( zone )
 *  
 * RETURNS:
 *
 *	void
 *       
 * ARGUMENTS:
 *
 * 	None
 *
 * SIDE EFFECTS:
 *
 *	The actual printing of data may cause blocks of memory to be allocated
 *	and deallocated.  Other higher priority tasks may also be modifying 
 *	the heap underneath this routine.  For these reasons, the dyn_sync
 *	semaphore can't be taken out, and we have to be careful when traversing
 *	queues that are changing.
 *
 *	When we clone the heap headers, we clone them into a structure that
 *	also includes the address of the header.  (once the header has been
 *	moved from its home, there is no way to find out where it came from 
 *	without a race condition).
 *
-*/
void dyn$_headers (struct ZONE *zp) {
	int	i;
	struct DYNAMIC	*p;
	int	blocks, blocks_found;
	struct CLONE_DYNAMIC {
		struct DYNAMIC p;
		int	address;
	} *cp, *clone;

	/*
	 * Count the number of blocks in the heap so we can estimate
	 * how much room we'll need for the snapshot.
	 */
	blocks = 0;
	krn$_wait (&zp->sync);
	p = (void *) &zp->header.adj_fl;
	do {
	    blocks++;
	    p = (void *) ((int) p - adjacent_bias);
	    p = p->adj_fl;
	} while (p != (void *) &zp->header.adj_fl);
	krn$_post (&zp->sync);

	/*
	 * Allocate enough room so that we can snapshot the headers.
	 * Over allocate by roughly 8%.  Allocate from the default heap,
	 * and not the heap being shown.
	 */
	blocks += blocks/16 + 4;
	clone = (void *) malloc (blocks * sizeof (struct CLONE_DYNAMIC));

	/*
	 * Clone the currently defined headers.  A consistent snapshot is more
	 * important than having the most up to date state.
	 */
	blocks_found = 0;
	cp = clone;
	krn$_wait (&zp->sync);
	p = (void *) &zp->header.adj_fl;
	do {
	    blocks_found++;
	    if (blocks_found >= blocks) break;
	    p = (void *) ((int) p - adjacent_bias);
	    memcpy (&cp->p, p, sizeof (struct DYNAMIC));
	    cp->address = p;
	    cp++;
	    p = (void *) p->adj_fl;
	} while (p != (void *) &zp->header.adj_fl);
	krn$_post (&zp->sync);

	/*
	 * Now we can finally print out the state at our leisure from the
	 * cloned copy.
	 */
	cp = (void *) clone;
	for (i=0; i<blocks_found; i++) {
	    int hole_address;
	    int hole_size;
	    if (killpending ()) break;

	    /* Check for discontinuity in the heap.  Don't count the
	     * the discontinuity between the header and the first block
	     */
	    if (i >= 2) {
		hole_address = (cp-1)->address + abs ((cp-1)->p.bsize);
		hole_size = cp->address - hole_address;
		if (hole_size) {
		    printf ("h %08X %d\n", hole_address, hole_size);
		}
	    }

	    printf ("%s %08X %08X_%08X %08X_%08X %08X %08X %d\n",
		cp->p.bsize > 0 ? "f" : "a",
		cp->address,
		cp->p.adj_fl,
		cp->p.adj_bl,
		cp->p.free_fl,
		cp->p.free_bl,
		cp->p.pid,
		cp->p.backlink,
		abs (cp->p.bsize)
	    );
	    cp++;
	}

	if (blocks_found >= blocks) {
	    printf ("%d unreported blocks\n", blocks_found - blocks);
	}

	/*
	 * Release the clone
	 */
	free (clone);
}

int queue_validate_sorted (struct QUEUE *header) {
	struct QUEUE *q;

	q = header->flink;
	while (q->flink != header) {
	    if (q >= q->flink) return 1;
	    q = q->flink;
	}
	return 0;
}

/*+
 * ============================================================================
 * = dyn$_consistency - Perform a consistency check on the heap.              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Walks the data structures that implement the dynamic 
 *	memory and verifies the consistency of those data structures (as 
 *	much as it can). This routine must run with the DYN_SYNC semaphore
 *	taken out, and may take a fair amount of time depending on the heap
 *	size.
 *
 *	By default, the routine performs its actions on all known zones.
 *	Only one zone is operated on if the z qualifier is present.
 *
 *	A typical failure is for free blocks to not be poisoned.  This can
 *	arise from 1) a program still using a block of memory that it free'd or
 *	2) a program freeing memory and explicitly using DYN$K_NOFLOOD.  In 
 *	either case, the consistency checker reports this and will repair
 *	the block if the r qualifier is present.  It repairs the block by
 *	flooding with the free pattern.
 *
 *	Other checks that the dyn$_consistency performs are:
 *##
 *		1  That all adjacent blocks are really adjacent.
 *		2  That the free list is in ascending order.
 *		3  That every byte in the range is on either the
 *	   	   free list or is allocated (or that all blocks cover the
 *		   range).
 *		4  That free blocks are poisoned with the appropriate
 *		   value.
 *		5  There can't be two blocks that are adjacent and free.
 *#
 *
 * FORM OF CALL:
 *  
 *	dyn$_consistency (zp, repair, zone_specified)
 *  
 * RETURNS:
 *
 *	msg_success		- success
 *	msg_dyn_free_lnk	- poorly formed free list
 *	msg_dyn_adj_lnk		- poorly formed adjacency list
 *	msg_dyn_free_seq	- free blocks not in correct sequence
 *	msg_dyn_adj_seq		- adjacent blocks not in correct sequence
 *	msg_dyn_poison		- block not poisoned correctly
 *       
 * ARGUMENTS:
 *
 * 	struct ZONE *zp		- pointer to zone descriptor
 *	int repair		- reflood free blocks if not poisoned.
 *
 * SIDE EFFECTS:
 *
 *	System may bugcheck if this routine fails.
 *
-*/
int dyn$_consistency (struct ZONE *zp, int repair, int zone_specified) {
	struct DYNAMIC	*p, *np;
	INT		*ip;
	unsigned int	len;
	int		i;
	int		corrupt;
	INT		flood_pattern;
        unsigned int	max_bound;

	krn$_wait (&zp->sync);

	flood_pattern = dyn_flood_free;
	if (zp == memzone)
		flood_pattern = 0;

#if RAWHIDE
/*
 * Rawhide has holes in memory
 * so check must allow for highest memory address
 */
	max_bound = 0x7FFFFFFF;
#else
        if (mem_size > 0x7FFFFFFF) {
            max_bound = 0x7FFFFFFF;
	} else {
	    max_bound = mem_size;
	}                     
#endif

	/* consistency check on the adjacency list */
	if (queue_validate (
		&zp->header.adj_fl,
		0,
		max_bound,
		1,
		zp->size / DYN$K_FRAG
		)) goto failure;

	/* consistency check on the adjacency list */
	if (queue_validate (
		&zp->header.free_fl,
		0,
		max_bound,
		1,
		zp->size / DYN$K_FRAG
		)) goto failure;

	/* verify that the adjacency list is in monotonically increasing order */
        if (queue_validate_sorted (&zp->header.adj_fl)) {
	    pprintf ("adjaceny list not in ascending order\n");
	    goto failure;
	}

	/* verify that the free list is in monotonically increasing order */
        if (queue_validate_sorted (&zp->header.free_fl)) {
	    pprintf ("free list not in ascending order\n");
	    goto failure;
	}

	/*
	 * Walk the free list.
	 */
	p = zp->header.free_fl;
	do {
	    p = (void *) ((int) p - free_bias);
	    np = p->free_fl;
	    np = (void *) ((int) np - free_bias);
/*
 * Do not perform a verify or repair any corruption in the block for MEMZONE
 * unless the -z qualifier was specified in the command line.
 */
	    if ((zone_specified) || (zp != memzone))
	    {       
	    /*
	     * Verify that this free block hasn't been corrupted.
	     */
	    ip = (void *) ((int) p  + sizeof (struct DYNAMIC));
	    corrupt = 0;
	    len = (p->bsize - sizeof (struct DYNAMIC)) / sizeof (INT);
	    for (i=0; i<len; i++) {
		if (*ip++ != flood_pattern) {
		    corrupt = 1;
		    ip--;
		    break;
		}
	    }

	    /*
	     * Report the corruption and optionally repair the block.
	     */
	    if (corrupt) {
     		printf ("corruption at address %08X in block %08X", ip, p);
		if (repair) {
		    quadset (
			(int) p  + sizeof (struct DYNAMIC),
			(INT) flood_pattern,
			(INT) ((p->bsize - sizeof (struct DYNAMIC)) >> 3)
		    );
		    printf (" repaired\n");
		} else {
		    printf ("\n");
		    goto failure;
		}
	    }
	    }		/* end of memzone check */

	    /*
	     * size should be negative for free blocks
	     */
	    if (p->bsize < 0) {
		pprintf (msg_dyn_size, p);
		goto failure;
	    }
	
	    p = p->free_fl;
	    if (killpending ()) break;
	} while (p != (void *) &zp->header.free_fl);

	/*
	 * The ownership queue for every allocated block should be 
	 * consistent.  Walk down the adjacency list and look at allocated
	 * blocks.
	 */
	p = zp->header.adj_fl;
	do {
	    p = (void *) ((int) p - adjacent_bias);
	    if (p->bsize > 0) {
		if (queue_validate (
		    &p->free_fl,
		    0,
		    max_bound,
		    1,
		    zp->size / DYN$K_FRAG
		)) {
		    pprintf ("broken ownership queue for block %08X\n", p);
		    goto failure;
		}
	    }
	    p = p->adj_fl;
	} while (p != (void *) &zp->header.adj_fl);

success:	
	krn$_post (&zp->sync);
	return (int) msg_success;

failure:
	krn$_post (&zp->sync);
	return (int) msg_failure;
}

#define	LOG_POINTERS	8
#define	LOG_CYCLES	7
#define	LOG_SIZE	6

#define	MAX_POINTERS	(1 << LOG_POINTERS)
#define	MAX_CYCLES	(1 << LOG_CYCLES)
#define	MAX_SIZE	(1 << LOG_SIZE)

#define	BLOCK_SIZE	((MAX_POINTERS + sizeof (struct DYNAMIC)) * MAX_SIZE)

/*
 * The following values are used as the modulus field
 */
unsigned int moduli [] = {
	0, 1, 2, 64,
	DYN$K_FRAG - 1, DYN$K_FRAG, DYN$K_FRAG + 1,
	DYN$K_GRAN - 1, DYN$K_GRAN, DYN$K_GRAN + 1
};

/*
 * Remainders are generated automatically from the modulus fields.
 */

/*+
 * ============================================================================
 * = dyn$_validate - Test heap allocation.                  	              =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *	Allocates and deallocates various size packets to verify that
 *	the dynamic memory package is working correctly.  
 *  
 * FORM OF CALL:
 *  
 *	dyn$_validate ( )              
 *  
 * RETURNS:
 *
 *	0 - success
 *       
 * ARGUMENTS:
 *
 * 	None
 *
 * SIDE EFFECTS:
 *
 *	System may bugcheck if this routine encounters an error.
 *
-*/

#if !VALIDATE

void dyn$_validate (struct ZONE *zp)
{
    printf("-v not supported\n");
}

#else

void dyn$_validate (struct ZONE *zp) {
	int		i;
	int		size;
	int		victim;
	unsigned char	*pointers [MAX_POINTERS];
	unsigned int	mx, m, r;

	/*
	 * Allocate MAX_POINTERS blocks of random sizes.
	 */
	size = 0;
	for (i=0; i<MAX_POINTERS; i++) {
	    pointers [i] = (void *) dyn$_malloc (
		size + 1,
		DYN$K_SYNC | DYN$M_ZONE,
		0,
		0,
		zp
	    );
	    size = random (size, LOG_SIZE);
	}

	/*
	 * Do a FREE and a MALLOC for MAX_CYCLES before
	 * releasing all the pointers.
	 */
	victim = 0;
	size = 0;
	for (i=0; i < MAX_CYCLES; i++) {
	    free (pointers [victim]);
	    pointers [victim] = (void *) dyn$_malloc (
		size + 1,
		DYN$K_SYNC | DYN$M_ZONE,
		0,
		0,
		zp
	    );
	    victim = random (victim, LOG_POINTERS);
	    size = random (size, LOG_SIZE);
 	}

	/*
	 * Perform MAX_CYCLES reallocations on the pointers
	 */
	victim = 15;	/* an arbitrary value */
	size = 41;
	for (i=0; i<MAX_CYCLES; i++) {
	    pointers [victim] = (void *) dyn$_realloc (
		pointers [victim],
		size+1,
		DYN$K_SYNC | DYN$M_ZONE,
		0,
		0,
		zp
	    );
	    victim = random (victim, LOG_POINTERS);
	    size = random (size, LOG_SIZE);
	}

	/*
	 * Allocate various size blocks, moduli and remainders
	 */
	victim = 15;
	size = 41;
	for (i=0; i<MAX_SIZE; i++) {
	    for (mx=0; mx < sizeof (moduli) / sizeof (moduli [0]); mx++) {
		m = moduli [mx];
		for (r=0; r < m; r++) {
		    free (pointers [victim]);
		    pointers [victim] = dyn$_malloc (
			size + 1,
			DYN$K_SYNC | DYN$K_ALIGN | DYN$M_ZONE,
			m,
			r,
			zp
		    );
		    if (((unsigned int) pointers [victim] % m) != r) {
			printf ("validation error a=%08X m=%08X r=%08X \n",
			    pointers [victim], m, r);
			return;
		    }
		    victim = random (victim, LOG_POINTERS);
		    size = random (size, LOG_SIZE);
		    if (killpending ()) break;
		}
	    }
	}

	/*
	 * Now deallocate all the pointers
	 */
	for (i=0; i<MAX_POINTERS; i++) {
	    free (pointers [i]);
	}
}
#endif

/*----------*/
unsigned int percentage (UINT x, UINT total) {
	UINT percentage_point;

	if (total == 0) {
	    return 0;
	};

	if (x > (x * 100)) {
	    percentage_point = total / 100;
	    return x / percentage_point;
	} else {
	    return (x * 100) / total;
	}
}

/*
 * Helper routine for DYNAMIC that prints out a usage summary for a zone
 */
void dyn$_status (struct ZONE *zp, int *header) {
	int	i;
	struct DYNAMIC	*p;
        int	free_bytes, used_bytes;
	int	free_blocks, used_blocks;

	free_bytes = used_bytes = 0;
	free_blocks = used_blocks = 0;
	krn$_wait (&zp->sync);
	p = (void *) &zp->header.adj_fl;

	/*
	 * walk down the list
	 */
	do {
	    p = (void *) ((int) p - adjacent_bias);
	    if (p->bsize > 0) {
		free_bytes += p->bsize;
		free_blocks++;
	    } else {
		used_bytes += abs (p->bsize);
		used_blocks++;
	    }
	    p = p->adj_fl;
	    if (killpending ()) break;
	} while (p != (void *) &zp->header.adj_fl);
	krn$_post (&zp->sync);

	if (*header == 0) {
	    printf ("%s%s%s",
"zone     zone       used    used       free    free       utili-  high\n",
"address  size       blocks  bytes      blocks  bytes      zation  water\n",
"-------- ---------- ------- ---------- ------- ---------- ------- ----------\n"
);
	}

	printf ("%08X %-10d %-7d %-10d %-7d %-10d %3d %%   %-10d\n",
	    zp, zp->size,
	    used_blocks, used_bytes,
	    free_blocks, free_bytes,
	    percentage (used_bytes, free_bytes + used_bytes),
	    zp->hiwater
	);

	*header = 1; 
}
void dyn$_check(void)
{
    int i;
    int header;
    struct ZONE *zp;

    header = 0;

    for (i=0; i<zcount; i++) {
	zp = zlist[i];
	dyn$_status(zp, &header);
	dyn$_consistency(zp, 0, 0);
    }
}

