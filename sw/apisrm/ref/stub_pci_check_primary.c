/*+
 * ============================================================================
 * = pci_check_primary - routine to check if ppb is primary                   =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *  	This routine will determine if the ppb is the primary.
 *
 * FORM OF CALL:
 *  
 *	pci_check_primary(hose, bus, slot);
 *  
 * RETURNS:
 *
 *	0 - not primary bridge.  This requires routing
 *	
 *	1 - this is the primary bridge that needs routing.
 *       
 * ARGUMENTS:
 *
 *	hose - hose number
 *
 *	bus - bus number
 *
 *	slot - slot number
 *              
 * SIDE EFFECTS:
 *
 *	None
 *
-*/

int pci_check_primary(int hose, int bus, int slot)
{
    return(0);
}
