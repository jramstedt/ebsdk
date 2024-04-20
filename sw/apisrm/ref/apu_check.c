/* file:	apu_check.c
 *                                                                       
 * Copyright (C) 1991, 1992 , 1994 by
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
 *      Avanti Firmware 
 *              
 *  MODULE DESCRIPTION:
 *
 *  AUTHORS:
 *      Jim Peacock
 *
 *  May 6th 1996	sm	Renamed to APU_CHECK to allow it to reside in
 *				the same image as the FWUPDATE command, for
 *				AlphaPCI64 system.
 *                  
 *--                   
 */                                                

#include "cp$src:platform.h"
#include 	"cp$src:prdef.h"
#include	"cp$src:kernel_def.h"          
#include 	"cp$src:dynamic_def.h"
#include	"cp$src:parse_def.h"    
#include	"cp$src:version.h"
#include	"cp$src:dynamic_def.h"
#include 	"cp$src:ctype.h"                                           
#include      	"cp$src:common.h"                 
#include 	"cp$src:diag_def.h"                
#include      	"cp$src:diag_groups.h"
#include 	"cp$src:msg_def.h"                               

extern int apu_length,apu_start;

int	BootBytes = 32*8192;

#define malloc(x) dyn$_malloc(x,DYN$K_FLOOD|DYN$K_SYNC|DYN$K_NOOWN)
#define free(x) dyn$_free (x,DYN$K_SYNC|DYN$K_NOOWN)

int	UpdxComplete=0,UpdxBootOk=0;
int	CheckForUpdate ();
int 	execute_script(char *script_name);
extern	int shell_stack;
extern 	sh();
extern	shell_startup ();
extern  struct ZONE *memzone;          
extern  struct ZONE defzone;
extern 	spl_kernel;

#define ApuBase	  0x600000
#define TwoMeg    (2*1024*1024)
#define StartCode 0x180000
#define PteStart  0x900000		/* console pte's with update    */
#define PfnStart  0xD00000		/* update  l2 pte's its malloc  */
#define ZeroSize  0x300000		/* zero from 13 mG to 16 M	*/

                                                 
/*+                                                                  
 * ============================================================================
 * = update - Update the console firmware =
 * ============================================================================
 *                                                
 * OVERVIEW:                                     
 *
 *  COMMAND FORM:                                             
 *        
 *   	apu_update 
 *                                                         
 *   COMMAND TAG: apu_update 0 RXBZ apu_update
 *
 *   COMMAND ARGUMENT(S):
 *                                                  
 *   COMMAND OPTION(S):                                                
 *                                            
 *   COMMAND EXAMPLE(S):                                                
 *                                            
 *   COMMAND REFERENCES:                                                
 *
 *                                            
 * FORM OF CALL:                                           
 *                                  
 *	update ( argc, *argv[] )                  
 *                                                                        
 * RETURN CODES:                                  
 *                                                           
 * ARGUMENTS:                                                                
 *                                                                
 * SIDE EFFECTS:                                         
 *                                                                
 *                              
-*/                                                                
int apu_update (int argc, char *argv [])
{               
static int  fill_array [8] = 
	{
	0x47FF041F,
	0xC3E00008,
	0x00003088,
	0x00000000,
	0x00000200,
	0x00000000,
	0x00000000,
	0x00000000,
	};

int	 i,offset,*fill_ptr,*ptr;
char	 *src,*dst;
struct   FILE	*fp;
char	 boot[80];

 if (apu_start)
 {
 UpdxBootOk = 0;
 fill_ptr = (int *)apu_start;

 /* Fill in the first 32 bytes...boot scribbles 	*/
 for (i=0;i<8;i++)*fill_ptr++= fill_array[i];	

 /* So that console does not put the page tables */
 /* On top of us, lets move the update code	*/
 /* out farther in memory			*/
 /* Since this runs in physical mode, no	*/
 /* problem.					*/

 src  = (char *)apu_start;
 dst  = (char *)apu_start;
 src += TwoMeg;
 dst += TwoMeg;

 /* Align the Start at StartCode boundary	*/
 offset = StartCode-apu_start;
 dst   += offset;

 spinlock (&spl_kernel);

 /* Copy the image BootBytes farther */
 /* Starting from the end	     */
 /* Just copy 2 Mbytes 		     */
 /* which is plenty		     */
 for (i=0,--dst,--src;i<TwoMeg;i++)
	*dst-- = *src--;
  
 /* Zero Out Space past the console  */
 memset( apu_start, 0, offset);

 spinunlock (&spl_kernel); 

 /* Now lets open a script file that has boot	*/
 /* command embedded in it			*/
 /* We need a placeholder so I can fill it	*/
 /* And when done, the updx script will then 	*/
 /* read that file and execute it. Sounds good  */
 /* in theory...lets try it			*/
 
 fp = (struct FILE *)fopen ("updx_boot" ,"w+");

 if (fp) 
   {
   for (i=0;i<80;i++)boot[i]=0;
   sprintf ( boot,"boot pmem:%x\n",StartCode);
   fwrite ( boot,1,strlen(boot),fp);
   UpdxBootOk=1;
   fclose (fp);
   } else return (0);
 }

 return (1);
 }


/*+
 * ============================================================================
 * = CheckForUpdate - CheckForUpdate  =
 * ============================================================================
 *
 * OVERVIEW:
 *	Look at symbol apu_start. If this is set to a non zero value
 *	then the merge utility has built an image for update purposes.  
 *  
 * FORM OF CALL:
 *  
 *	CheckForUpdate ()
 *  
 *
-*/
int CheckForUpdate () 

{
int	status=0;

if (apu_start)
 {

 /* Standalone Image for Update		*/
 /* updx script executes update command	*/
 /* Which calls apu_check:		*/
 /* Which then creates an update script */
 /* with where to boot the merged image */

 /*We have to guarantee after we copy	*/
 /*the code that it will not get trashed*/

 execute_script("apu_updx");

 if (UpdxBootOk)
	{
	/* Execute the boot command	*/
	execute_script("updx_boot");
	/* Clear apu_start so this 	*/
	/* only runs once after boot 	*/
	/* and not after init		*/
	apu_start 	= 0;
	status 		= 1;
	}
 }
return (status);
}


/*+
 * ============================================================================
 * = Execute_script - execute script					      =
 * ============================================================================
 *
 * OVERVIEW:
 *  
 *
 *	Execute script. 
 *
 *
 * FORM OF CALL:
 *  
 *	execute_script(script_name)   
 *  
 * RETURNS:
 *
 * ARGUMENTS:
 *
 *	script_name - name of script
 *
 * SIDE EFFECTS:
 *
 *	Executes script
 *
-*/
int execute_script(char *script_name)
   {
    struct SEMAPHORE sem;
    char **shv;	
    int id = whoami();

    shv = malloc (2 * sizeof (char *));
    shv [0] = mk_dynamic ("shell");
    shv [1] = 0;
    krn$_seminit (&sem, 0, "updx_done");
    krn$_create (sh, 
		shell_startup, 
		&sem, 
		6,				
		1<<id,
		shell_stack,
		script_name,	   
		script_name, "r", "tt", "w", "tt", "w", 1, shv );

    /* Wait for Script to Finish	*/
    krn$_wait (&sem);
    krn$_semrelease (&sem);

    return (1);

    }
