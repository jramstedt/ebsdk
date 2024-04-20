/* stub routine to return false in those cases where the routine is not  */
/* available.  The smcc669_driver has a call to this routine. */

#include    "cp$src:smcc669_def.h"  /* for the prototyping */


int SMC37c669_init_irq_drq_tables(SMC37c669_IRQ_TRANSLATION_ENTRY SMC37c669_default_irq_table,
	SMC37c669_DRQ_TRANSLATION_ENTRY SMC37c669_default_drq_table)
{
    return (0);
}

