#include "cp$src:kernel_def.h"
#include "cp$src:pb_def.h"
extern struct LOCK spl_kernel;

io_get_vector( struct pb *pb, int vector )
    {
    return( 0x80 + vector );
    }

io_enable_interrupts( struct pb *pb, int vector )
{
    unsigned int irq;

    irq = vector - 0x80;
    if (irq <= 15) {
	spinlock (&spl_kernel);
	if (irq < 8) {
	    outportb (pb, 0x21, inportb (pb, 0x21) & ~(1<<(irq&7)));
	} else {
	    outportb (pb, 0xa1, inportb (pb, 0xa1) & ~(1<<(irq&7)));
	}
	spinunlock (&spl_kernel);
	io_issue_eoi (pb, vector);
    }
}

io_disable_interrupts( struct pb *pb, int vector )
{
    unsigned int irq;

    irq = vector - 0x80;
    if (irq <= 15) {
	spinlock (&spl_kernel);
	if (irq < 8) {
	    outportb (pb, 0x21, inportb (pb, 0x21) | (1<<(irq&7)));
	} else {
	    outportb (pb, 0xa1, inportb (pb, 0xa1) | (1<<(irq&7)));
	}
	spinunlock (&spl_kernel);
	io_issue_eoi (pb, vector);
    }
}

io_issue_eoi( struct pb *pb, int vector )
{
    unsigned int irq;

    irq = vector - 0x80;
    if (irq <= 15) {
	spinlock (&spl_kernel);
	if (irq < 8) {
	    outportb (pb, 0x20, 0x60+(irq&7));
	} else {
	    outportb (pb, 0xa0, 0x60+(irq&7));
	    outportb (pb, 0x20, 0x62);
	}
	spinunlock (&spl_kernel);
    }
}
