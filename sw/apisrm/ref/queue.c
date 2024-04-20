/* file:	queue.c
 *
 * Copyright (C) 1992 by
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
 *      Doubly linked queue routines.  Callers of the routines are responsible
 *	for any sychronization needed.
 *
 *  AUTHORS:
 *
 *      AJ Beaverson
 *
 *  CREATION DATE:
 *  
 *      20-Jul-1992
 *
 *  MODIFICATION HISTORY:
 *
 *
 *--
 */
#include "cp$src:platform.h"
#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$inc:prototypes.h"

extern struct LOCK spl_kernel;

void insq (struct QUEUE *entry, struct QUEUE *header) {

	entry->flink = header->flink;
	entry->blink = header;

	header->flink = entry;
	entry->flink->blink = entry;
}

void insq_lock (struct QUEUE *entry, struct QUEUE *header) {
	spinlock (&spl_kernel);
	insq (entry, header);
	spinunlock (&spl_kernel);
}

int remq (struct QUEUE *header) {
	header->flink->blink = header->blink;
	header->blink->flink = header->flink;

	return header;
}

int remq_lock (struct QUEUE *header) {
	spinlock (&spl_kernel);
	remq (header);
	spinunlock (&spl_kernel);

	return header;
}

/*
 * Add an element to a sorted queue.  Pass a compare function that is used
 * to implement the sort key.
 *
 * The order of the queue is undefined if an out of sort queue is passed in.
 */
void insq_sorted (struct QUEUE *entry, struct QUEUE *header, int (*cmp) ()) {
	struct QUEUE *q;

	q = header;
	while ((q->flink != header) && ((*cmp)(entry, q->flink) > 0)) {
	    q = q->flink;
	}

	insq (entry, q);
}

#if 0


int queue_validate_sorted (struct QUEUE *header) {
	struct QUEUE *q;

	q = header->flink;
	while (q->flink != header) {
	    if (q >= q->flink) return 1;
	    q = q->flink;
	}
	return 0;
}


/*
 * Following is a simple test for the insq_sorted routine.
 */
int cmp_ints (int a, int b) { return a - b; }

void show_queue (struct QUEUE *header) {
	struct QUEUE *q;

	q = header;
	printf ("%08X: %08X %08X\n\n", q, q->flink, q->blink);

	q = header->flink;
	while (q != header) {
	    printf ("%08X: %08X %08X\n", q, q->flink, q->blink);
	    q = q->flink;
	}
}

void main (int argc, char *argv []) {
	struct QUEUE h,  e1, e2, e3, e4, e5, e6, e7;

	h.flink = & h;
	h.blink = & h;

	insq_sorted (&e1, &h, cmp_ints);
	insq_sorted (&e4, &h, cmp_ints);
	insq_sorted (&e3, &h, cmp_ints);
	insq_sorted (&e2, &h, cmp_ints);

	show_queue (&h);
	printf ("%d\n", queue_validate_sorted (&h));
	insq (&e6, &h);
	insq (&e5, &h);
	insq (&e7, &h);
	printf ("%d\n", queue_validate_sorted (&h));

}
#endif
