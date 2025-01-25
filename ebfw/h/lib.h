#ifndef __LIB_H_LOADED
#define __LIB_H_LOADED
/*****************************************************************************

       Copyright © 1993, 1994 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided  
that the copyright notice and this permission notice appear in all copies  
of software and supporting documentation, and that the name of Digital not  
be used in advertising or publicity pertaining to distribution of the software 
without specific, written prior permission. Digital grants this permission 
provided that you prominently mark, as not part of the original, any 
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees  
with regard to this software, including all implied warranties of fitness for 
a particular purpose and merchantability, and makes no representations 
regarding the use of, or the results of the use of, the software and 
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at 
your own risk. 

******************************************************************************/

/*
 *  $Id: lib.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $;
 */

/*
 * $Log: lib.h,v $
 * Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 * Initial CVS checkin
 *
 * Revision 1.85  1998/10/08  13:34:06  gries
 * added:
 * > #ifdef DP264
 * > void srom_init_lcd(void);
 * > void srom_access_lcd(int, int);
 * > #endif
 * >
 *
 * Revision 1.84  1998/08/19  17:16:45  jerzy
 * Removed (commented out) memcpy(), see c source file.
 *
 * Revision 1.83  1998/03/23  21:02:26  gries
 * extern ui mem_size; to extern ul mem_size;
 *
 * Revision 1.82  1997/12/15  20:48:25  pbell
 * Updated for dp264.
 *
 * Revision 1.81  1997/08/01  19:18:06  pbell
 * Changed memtest prototype to use ul addresses.
 * Added PRINTF_sl_ and PRINTF_L_LARGE macros so that code can be written without
 * #if statements when using orintf to display sl and ul quantities.
 *
 * Revision 1.80  1997/06/02  04:37:27  fdh
 * Moved declarations for machine check related stuff
 * to another file.
 *
 * Revision 1.79  1997/05/30  18:57:06  pbell
 * added prototypes for new functions; fputc, fgetc, fungetc, fprintf,
 * fscanf.
 *
 * Revision 1.78  1997/05/29  17:45:47  pbell
 * Added 64 bit string to integer function strtoul64
 *
 * Revision 1.77  1997/05/28  15:22:47  fdh
 * Fixed up conditionals around the inrom() declaration.
 *
 * Revision 1.76  1997/05/06  14:36:22  pbell
 * Added prototype for SMC669Init and added definitions
 * for DC21164PC in sysdata_t.
 *
 * Revision 1.75  1997/04/30  21:18:39  pbell
 * Added a prototype for UngetChar in callback.c.
 *
 * Revision 1.74  1997/04/10  17:01:21  fdh
 * Added _EDATA for GNU C.
 *
 * Revision 1.73  1997/03/27  19:40:00  fdh
 * Removed an obsoleted definition.
 *
 * Revision 1.72  1997/03/26  15:10:15  fdh
 * Removed the Read/Write declarations used for _WIN32.
 *
 * Revision 1.71  1997/03/12  15:58:26  fdh
 * Added calloc.c
 *
 * Revision 1.70  1997/02/21  04:10:52  fdh
 * Added new definitions.
 *
 * Revision 1.69  1996/08/20  02:48:43  fdh
 * Changed a typedef name to avoid a conflict.
 *
 * Revision 1.68  1996/08/06  22:07:54  fdh
 * Updated a prototype...
 *
 * Revision 1.67  1996/07/11  16:03:29  fdh
 * Added iobusini.c and prototypes for PCIBusInit() and ISABusInit().
 *
 * Revision 1.66  1996/07/11  14:28:33  fdh
 * Moved msleep() from beep.c to time.c.
 *
 * Revision 1.65  1996/06/24  18:19:01  fdh
 * Added prototype for abort().
 *
 * Revision 1.64  1996/06/13  03:23:03  fdh
 * Modified data type returned from inmeml.
 *
 * Revision 1.63  1996/06/03  17:44:22  fdh
 * Added declaration for the MessageString array.
 *
 * Revision 1.62  1996/05/26  20:20:35  fdh
 * Added EnableIsaDmaChannel(int channel);
 *
 * Revision 1.61  1996/05/22  21:25:55  fdh
 * Added State_t definition for general configuration states.
 * Added prototypes for new routines and removed obsoleted ones.
 *
 * Revision 1.60  1995/12/23  23:11:41  fdh
 * Removed nttypes.h
 *
 * Revision 1.59  1995/11/29  19:38:08  cruz
 * Added missing fields to sysdata_t for DC21164
 *
 * Revision 1.58  1995/11/16  23:58:11  cruz
 * Updated prototype for flash_main because its arguments were changed.
 *
 * Revision 1.57  1995/11/06  19:58:26  fdh
 * Added some missing datatype definitions.
 *
 * Revision 1.56  1995/10/31  22:46:31  cruz
 * Added compile conditionals around prototypes that were
 * dependent on specific compile switches.
 *
 * Revision 1.55  1995/10/31  18:50:03  cruz
 * Commented out some unused routines.
 *
 * Revision 1.54  1995/10/30  16:58:53  cruz
 * Commented out routine(s) not in use.
 *
 * Revision 1.53  1995/10/27  15:28:23  cruz
 * Added extern reference to mem_size.
 *
 * Revision 1.52  1995/10/27  14:24:53  cruz
 * Added prototype for rand().
 *
 * Revision 1.51  1995/10/26  23:52:17  cruz
 * Added several prototypes and extern declarations of
 * global variables.
 *
 * Revision 1.50  1995/10/20  19:16:04  cruz
 * Added prototypes for strcpy and strcmp functions.
 *
 * Revision 1.49  1995/10/20  13:55:04  fdh
 * Added DumpSysData().
 *
 * Revision 1.48  1995/10/19  18:27:33  fdh
 * Added the iterations argument to memtest.
 *
 * Revision 1.47  1995/10/18  23:04:41  cruz
 * Fixed prototype of memset().
 *
 * Revision 1.46  1995/10/11  21:41:59  fdh
 * Removed some internal function prototypes.
 *
 * Revision 1.45  1995/10/10  20:14:38  fdh
 * Removed some internal prototypes.
 *
 * Revision 1.44  1995/10/10  14:22:49  cruz
 * Changed prototype.
 *
 * Revision 1.43  1995/10/05  01:31:59  fdh
 * Updated some EB64 specific prototypes.
 *
 * Revision 1.42  1995/10/05  01:04:24  fdh
 * Updated function prototypes.
 *
 * Revision 1.41  1995/10/03  03:36:17  fdh
 * Replaced ux #define definitions with typedefs.
 * Changed _edata reference to _fbss reference to work
 * around a Unix 3.0 linker bug.
 * Added BOOLEAN definition.
 *
 * Revision 1.40  1995/10/02  22:10:28  cruz
 * Added new prototype
 *
 * Revision 1.39  1995/09/22  13:09:23  fdh
 * Removed memmove() because it did not handle the case
 * for overlapping memory regions.  Therefore, memcpy()
 * supplants that memmove().
 *
 * Revision 1.38  1995/09/21  20:53:41  cruz
 * Included file.h
 *
 * Revision 1.37  1995/09/15  19:29:23  cruz
 * Updated with file i/o prototypes.
 *
 * Revision 1.36  1995/09/12  21:28:25  fdh
 * Modified GetChar() to return an int instead of a char.
 *
 * Revision 1.35  1995/09/05  19:18:50  fdh
 * Added definitions for _MIN and _MAX.  These can
 * replace the various similar definitions that exist
 * elsewhere.
 *
 * Revision 1.34  1995/09/05  18:41:38  fdh
 * Include prtrace.h for debugging in all modules.
 *
 * Revision 1.33  1995/09/02  07:02:27  fdh
 * Removed WaitUs.
 *
 * Revision 1.32  1995/09/02  03:50:07  fdh
 * Moved some files around.
 *
 * Revision 1.31  1995/08/25  19:37:47  fdh
 * Modified PCI config space prototypes.
 * Modified to support callbacks.
 * Added necessary Bios emulation prototypes.
 *
 * Revision 1.30  1995/07/05  17:44:44  cruz
 * Added prototypes for supporting routines for accessing
 * PCI dense memory space.
 *
 * Revision 1.29  1995/04/05  21:46:10  cruz
 * Added EB164 prototype for wr_bcache().
 *
 * Revision 1.28  1995/04/03  18:35:32  cruz
 * Extended the prototype definition of cserv.
 *
 * Revision 1.27  1995/03/05  04:17:23  fdh
 * Corrected prototype for inrom().
 *
 * Revision 1.26  1995/02/28  03:05:53  fdh
 * Moved rom.c prototypes to rom.h
 *
 * Revision 1.25  1995/02/27  19:21:27  fdh
 * Added prototypes for Ctype routines.
 *
 * Revision 1.24  1995/02/23  21:48:31  fdh
 * Added prototypes for set_haxr() and atoi().
 *
 * Revision 1.23  1995/02/22  22:01:10  fdh
 * Changed types for tolower() and toupper().
 *
 * Revision 1.22  1995/02/19  17:46:01  fdh
 * Changed one of the arguments to read_rom().
 *
 * Revision 1.21  1995/02/10  02:23:20  fdh
 * Added prototype for set_romboot().
 *
 * Revision 1.20  1994/11/19  03:30:51  fdh
 * Added support for romlist command.
 *
 * Revision 1.19  1994/11/18  19:05:31  fdh
 * swpipl returns the current ipl.
 *
 * Revision 1.18  1994/11/08  21:39:20  fdh
 * Added declaration for the flash routine.
 *
 * Revision 1.17  1994/11/01  11:30:01  rusling
 * Changed following PCI-PCI bridge support.
 *
 * Revision 1.16  1994/08/05  20:13:47  fdh
 * Updated Copyright header and RCS $Id: identifier.
 *
 * Revision 1.15  1994/08/03  19:44:23  fdh
 * Fixups around the linker defined symbols _edata and _end.
 * Protect time_t definition with #ifndef _TIME_T.
 *
 * Revision 1.14  1994/07/22  21:02:46  fdh
 * Added extern void rtcBaseInit(void); for EB64 builds.
 *
 * Revision 1.13  1994/07/21  18:10:06  fdh
 * >> Added EnableBCache(), DisableBCache(), and CleanBCache().
 *
 * Revision 1.12  1994/07/13  14:17:07  fdh
 * Added data structure for holding pointers to SROM interface
 * parameters.
 *
 * Revision 1.11  1994/06/28  20:08:21  fdh
 * Modified filenames and build precedure to fit into a FAT filesystem.
 *
 * Revision 1.10  1994/06/22  15:10:34  rusling
 * Fixed up WNT compile warnings.
 *
 * Revision 1.9  1994/06/21  15:27:47  rusling
 * Removed putFloat() prototype.
 *
 * Revision 1.8  1994/06/21  14:18:05  rusling
 * changed definition of loadHeader() in rom.c
 *
 * Revision 1.7  1994/06/21  10:38:53  rusling
 * Added strncmp() for WNT.
 *
 * Revision 1.5  1994/06/20  14:18:59  fdh
 * Fixup header file preprocessor #include conditionals.
 *
 * Revision 1.4  1994/06/17  19:34:01  fdh
 * Clean-up...
 *
 * Revision 1.3  1994/06/13  15:54:35  fdh
 * Added definitions of unsigned amounts as defined in system.h
 * Definitions are also placed here to make the lib subdirectory
 * free-standing from the rest of the source tree.
 *
 * Revision 1.2  1994/06/03  20:18:38  fdh
 * Added protypes for all routines in /lib.
 *
 * Revision 1.1  1994/01/19  10:33:21  rusling
 * Initial revision
 *
 * Revision 1.1  1993/06/08  19:56:14  fdh
 * Initial revision
 *
 */

#include <stddef.h>
#include <stdarg.h>

#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

#define _MIN(a,b) (((a)<(b)) ? (a) : (b))
#define _MAX(a,b) (((a)>(b)) ? (a) : (b))

#ifndef RAND_MAX
#define RAND_MAX	2147483647	/* max value returned by rand	*/
#endif

#include "dbmtypes.h"
#include "prtrace.h"
#include "time.h"
#include "asmstuff.h"
#include "file.h"

typedef enum _STATUS {STATUS_FAILURE, STATUS_SUCCESS} DBM_STATUS;


typedef struct {

#ifdef DC21064
  ul *abox_ctl;
  ul *biu_ctl;
#endif /* DC21064 */

#ifdef DC21066
  ul *abox_ctl;
  ui *bcr0;
  ui *bcr1;
  ui *bcr2;
  ui *bcr3;
  ui *bmr0;
  ui *bmr1;
  ui *bmr2;
  ui *bmr3;
#endif /* DC21066 */

#ifdef DC21164
  ul *bc_ctl;
  ul *bc_cfg;
  ul *bc_cfg_off;
#endif /* DC21164 */

#ifdef DC21164PC
  ul *bc_cfg;
  ul *bc_cfg2;
#endif /* DC21164 */

#ifdef DC21264
  ul *dc_ctl;
#endif /* DC21264 */

  ul *srom_rev;
  ul *proc_id;
  ul *mem_size;
  ul *cycle_cnt;
  ul *signature;
  ul *proc_mask;
  ul *sysctx;
  ul valid;
} sysdata_t;


typedef enum CONFIGSTATES {
  StateOn,
  StateOff,
  StateSystem,
  StateCPU
} State_t;


extern char MessageString[];
extern int isp_simulation;

#ifdef _WIN32
extern unsigned long end;
extern unsigned long edata;	/* Visual C++ */
#define _END end
#define _EDATA edata
#else
extern unsigned long _end;
extern unsigned long _edata;	/* OSF and Linux */
extern unsigned long _fbss;	/* OSF 3.0 bug workaraound */
extern unsigned long _EDATA;	/* GNU C OSF */
#define _END _end
#define _EDATA _edata
#endif

/* To avoid the conflict with the definition in the BIOS Emulator */
#ifndef BIOS_EMULATION
typedef ui BOOLEAN;
#endif

/*
 *  Sysdata is a structure containing system specific data.
 *  CPU_cycles_per_usecs = # of cycles per microsecond.
 *  Their space is allocated in "initdata.c"
*/
extern sysdata_t sysdata;
extern ul CPU_cycles_per_usecs;

/*
 *  mem_size contains the size of the board's main memory.
 *  Its space is allocated in main.c
 */
extern ul mem_size;

/*
 *  If a Digital TGA card is detected during PCI probing, then
 *  the following flag will be set to TRUE and its base stored
 *  in DigitalTGABase
 *  Their space is allocated in "pci.c"
 */
#ifdef NEEDTGA
extern int DigitalTGADetected;
extern ul  DigitalTGABase;
#endif

/*
 *  Variable for specifying the type of the register names to 
 *  be used: software (1) or hardware names (0).
 *  Its space is allocated in "dis.c"
 */
extern ui regmode;

/*
 *  Address where dissassembly of code should start if no 
 *  address is specified with "dis" cmd.
 *  Its space is allocated in "dis.c"
 */
extern ul DisStartAddress;

/*
 *  Base address of the real time clock.
*/
extern int rtcBase;

/* These functions are not implemented locally (in DBM); they are 
   taken from the host's build environment.
*/
extern char  *strcpy(char *, const char *);
extern char  *strncpy(char *, const char *, size_t );
extern int   strcmp(const char *, const char *);
extern int   strncmp(const char *,const char *,size_t );
extern char *strncat(char *s1, const char *s2, size_t n);
extern char *strcat(char *s1, const char *s2);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern char *strstr(const char *s1, const char *s2);
extern void *memmove(void *s1, const void *s2, size_t n);



/*::::::::::::::
beep.c
::::::::::::::*/
extern void tone(int period , int duration);
extern void Beep(int msec , int freq);
extern void BeepCode(int code);


/*::::::::::::::
c8514.c
::::::::::::::*/
extern int c8514init(void );
extern void c8514erase(void );
extern void c8514putc(char c);
extern void c8514show(void );
extern void c8514hide(void );
extern void c8514insfontchar(int code , ub * s);
extern void initfont(void );


/*::::::::::::::
callbacks.c
::::::::::::::*/
extern void PutChar(char c);
extern int GetChar(void );
extern void UngetChar(char c);
extern int CharAv(void );
extern void malloc_summary(char * string);
extern void * malloc(size_t size);
extern void free(void * pointer);
extern void * realloc(void * pointer , size_t size);
extern void ladbx_poll(void );
extern time_t time(time_t * out);


/*::::::::::::::
calloc.c
::::::::::::::*/
extern void *calloc(size_t num_of_elts, size_t elt_size);
void cfree(void *pointer);


/*::::::::::::::
ctype.c
::::::::::::::*/
extern int isalnum(int c);
extern int isalpha(int c);
extern int isascii(int c);
extern int iscntrl(int c);
extern int isdigit(int c);
extern int isgraph(int c);
extern int islower(int c);
extern int isprint(int c);
extern int ispunct(int c);
extern int isspace(int c);
extern int isupper(int c);
extern int isxdigit(int c);
extern int tolower(int c);
extern int toupper(int c);


/*::::::::::::::
ebxx_io.c
::::::::::::::*/
extern void outportb(ui p , ui d);
extern void outportw(ui p , ui d);
extern void outportl(ui p , ui d);

#ifdef NEEDPCI
extern void outcfgb(ui bus , ui dev , ui func , ui reg , ui data);
extern void outcfgw(ui bus , ui dev , ui func , ui reg , ui data);
extern void outcfgl(ui bus , ui dev , ui func , ui reg , ui data);

extern ui incfgb(ui bus , ui dev , ui func , ui reg);
extern ui incfgw(ui bus , ui dev , ui func , ui reg);
extern ui incfgl(ui bus , ui dev , ui func , ui reg);

extern void PCIBusInit(void);

extern ul IOPCIClearNODEV(void );

#endif

extern ui inportb(ui p);
extern ui inportw(ui p);
extern ui inportl(ui p);
extern void outLed(ui d);
extern void out_ioc_csr(ui p , ul d);
extern ul in_ioc_csr(ui p);
extern void outmemb(ui p , ui d);
extern void outmemw(ui p , ui d);
extern void outmeml(ui p , ui d);
extern ui inmemb(ui p);
extern ui inmemw(ui p);
extern ui inmeml(ui p);
extern ui inIack(void );

extern void outdmemb(ui p , ui d);
extern void outdmemw(ui p , ui d);
extern void outdmeml(ui p , ui d);
extern ui indmemb(ui p);
extern ui indmemw(ui p);
extern ui indmeml(ui p);

extern void ISABusInit(void);

extern ui SetHAE(ui Value);

#ifdef EB64
extern void rtcBaseInit(void);
extern ui insctl(void);
extern void outsctl(ui d);
#endif

#if defined(NEEDEB64SPECIALROMACCESS) || defined(NEEDPRIVATEFLASHBUS)
extern ub inrom(ui p);
#endif

/*::::::::::::::
exit.c
::::::::::::::*/
extern void exit (int status);
extern void abort (void);


/*::::::::::::::
iobusini.c
::::::::::::::*/
extern void IOBusInit(void );


/*::::::::::::::
ebxxmem.c
::::::::::::::*/
extern void memdetect(void);

#ifdef EB66
extern int check_mem_esr(int silent);
extern void out_mem_csr(ui p , ul d);
extern ul in_mem_csr(ui p);
#elif EB64P
extern ui in_mem_csr(ui p);
#endif

/* These two functions are temporarily disabled for EB164 */
#ifndef EB164
extern void EnableBCache(void);
extern void DisableBCache(void);
#endif

#ifdef EB164
void wr_bcache (ui argc, ul arg1, ul arg2, ui select);
#endif

#ifdef DP264
void srom_init_lcd(void);
void srom_access_lcd(int, int);
#endif


/*::::::::::::::
fileio.c
::::::::::::::*/
extern FILE * fopen(char * Filename , char * Mode);
extern size_t fread(void * ptr , size_t size , size_t nobj , FILE * stream);
extern size_t fwrite(const void * ptr , size_t size , size_t nobj , FILE * stream);
extern int    fgetc( FILE * stream );
extern int    fungetc( int c, FILE * stream );
extern int    fputc( int c, FILE * stream );
extern int    fclose(const FILE * stream);
extern int    feof(FILE * stream);
extern void   perror(const char * s);
extern char * fdir(const FILE * stream , const char * FormatString);
extern size_t fload(FILE * stream , char * Destination);
extern char * fcd(char * DirectoryString);
extern char * fdr(char * DriveString);

/*::::::::::::::
mflash.c
::::::::::::::*/
#ifdef NEEDFLASHMEMORY
extern int flash_main(ui ImageAddress, int DestinationOffset, ui UserSpecifiedSize);
#endif

/*::::::::::::::
ident.c
::::::::::::::*/
extern int ident(ul first , ul last);


/*::::::::::::::
initdata.c
::::::::::::::*/
extern void initdata(int process);
extern void DumpSysData(void);

/*::::::::::::::
int.c
::::::::::::::*/
extern void intr_enable(int int_level);

/*::::::::::::::
isa_dma.c
::::::::::::::*/
extern void EnableIsaDmaChannel(int channel, int mode);

/*::::::::::::::
kbd.c
::::::::::::::*/
extern int kbd_init(void );
#ifdef NOT_IN_USE
extern void kbd_reset_state(void );
#endif
extern int kbd_charav(void );
extern int kbd_getc(void );


/*::::::::::::::
memt.c
::::::::::::::*/
extern int mt( int argc, char *argv[] );


/*::::::::::::::
memtest.c
::::::::::::::*/
extern void memtest( ul min, ul max, int inc, int iterations, State_t McheckState, int drivers);



/*::::::::::::::
p8514.c
::::::::::::::*/
extern void pwgaFillSolid(ui fg , ui alu , ui planemask , int xDst , int yDst , int wDst , int hDst);
extern void pwgaDrawColorImage(int xDst , int yDst , int wDst , int hDst , ub * pSrc , int widthSrc , ui alu , ui planemask);
extern void pwgaBlit(int xSrc , int ySrc , int wSrc , int hSrc , int xDst , int yDst , ui alu , ui planemask);
extern int DisplayOpen(int mode1024);
extern void DisplayClose(void );
extern void byteoutport(int p , int d);
extern void short_delay(void );
extern void InitLUT(void );
extern int pwgaExists(void );
extern ui pwgaHWInit(int mode1024);
extern void outwords(short * wSrc , int wcount);
extern void outwblock(ub * pSrc , int w , int h , int widthSrc);


/*::::::::::::::
pr.c
::::::::::::::*/
extern void PQ(ul x);
extern void PL(ui x);
extern void PW(uw x);
extern void PB(ub x);
extern void PutSpace(void );
extern void PutCR(void );


/*::::::::::::::
printf.c
::::::::::::::*/

/* use the PRINTF_sl_ macro in strings where the value of an sl of ul must
   be displayed instead of hard coding the character in the string. */
#if _WIN32
#define PRINTF_L_LARGE
#define PRINTF_sl_  "L"
#else
#define PRINTF_sl_  "l"
#endif
extern int printf(const char * f , ...);
extern int sprintf(char * str , const char * f , ...);
extern int fprintf( FILE * OutFile, const char * CtrlStr, ... );
extern int vprintf(const char * f , va_list ap);


/*::::::::::::::
puts.c
::::::::::::::*/
extern int puts(const char *f);


/*::::::::::::::
rand.c
::::::::::::::*/
extern int random(void);
extern int srandom( unsigned int seed);
extern int rand (void);
void srand( unsigned int seed);


/*::::::::::::::
rw.c
::::::::::::::*/
#ifdef _WIN32
/*
** defined in asmstuff.h and asmstuff.s because NT uses 32 bit pointers
** and we support 64 bit!
*/
#else
#define WriteB(address,value)	    (*(ub *)(address)=(ub)(value))
#define ReadB(address)		    (*(ub *)(address))
#define WriteW(address,value)	    (*(uw *)(address)=(uw)(value))
#define ReadW(address)		    (*(uw *)(address))
#define WriteL(address,value)	    (*(ui *)(address)=(ui)(value))
#define ReadL(address)		    (*(ui *)(address))
#define WriteQ(address,value)	    (*(ul *)(address)=(ul)(value))
#define ReadQ(address)		    (*(ul *)(address))
#define WriteFloat(address,value)   (*(float *)(address)=(float)(value))
#define ReadFloat(address)	    (*(float *)(address))
#define WriteDouble(address,value)  (*(double *)(address)=(double)(value))
#define ReadDouble(address)	    (*(double *)(address))
#endif

/*::::::::::::::
scanf.c
::::::::::::::*/
extern int scanf(const char *f, ... );
extern int sscanf( const char *str, const char *f, ...);
extern int fscanf( FILE * OutFile, const char * CtrlStr, ... );


/*::::::::::::::
scb.c
::::::::::::::*/
extern char *SCB_vector_string(int SCB_vector);


/*::::::::::::::
smc.c
::::::::::::::*/
extern void SMCInit(void);


/*::::::::::::::
smc669.c
::::::::::::::*/
extern void SMC669Init(void);


/*::::::::::::::
sum.c
::::::::::::::*/
extern int ChecksumMem(ul first, ul last);


/*::::::::::::::
search.c
::::::::::::::*/
extern int search(ul first , ul last , int size , char * valstr , int inverse);
extern void ParseVal(char * s , ul * val , ul * mask , int size);


/*::::::::::::::
strtod.c
::::::::::::::*/
extern double atof(const char *nptr);
extern double strtod(const char *nptr, char **endptr);


/*::::::::::::::
strtoul.c
::::::::::::::*/
extern ul strtoul64( const char *nptr, char **endptr, int base);
extern unsigned long int strtoul( const char *nptr, char **endptr, int base);
extern long int strtol( const char *nptr, char **endptr, int base);
extern int atoi(const char * nptr);
extern long int atol(const char *nptr);


/*::::::::::::::
string.c
::::::::::::::*/
extern size_t strlen(char * s);
extern void bzero(char * s , int count);
extern int memcmp(const void * pcs , const void * pct , size_t n);
extern void * memset(void * ps , int c , size_t n);
extern void * memcpy(void * ps , const void * pct , size_t n);
extern void * memchr(const void* ptr, int ch, size_t count);

 
/*::::::::::::::
time.c
::::::::::::::*/
extern struct tm * localtime(const time_t * tp);
extern void msleep(int ms);
extern void usleep(unsigned int usecs);
extern ui sleep(ui seconds);
extern time_t gettime(void );


/*::::::::::::::
vga.c
::::::::::::::*/
extern void vgaerase(void );
extern void vgalcgen(void );
extern void vgasetloc(void );
extern void vgaputc(register int c);
extern void vgainit(void );
extern void wrDacReg(int reg , ui donne);
extern void wrTitanReg(int reg , ui donne);
extern void wrDubicReg(int reg , ui donne);
extern int rdTitanReg(int reg);
extern void SetVgaEn(void);
extern int locate_matrox(void);
extern void vgasetup(void);

#ifdef NOT_IN_USE
extern void setupDac(void);
#endif

/*::::::::::::::
fflush.c
::::::::::::::*/
extern int fflush(FILE *);

#endif /* __LIB_H_LOADED */
