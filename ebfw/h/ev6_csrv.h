#define	__CSERVE_LOADED	1
/*
 *  $Id: ev6_csrv.h,v 1.1.1.1 1998/12/29 21:36:06 paradis Exp $
 *
 *  MODIFICATION HISTORY:
 *
 *  $Log: ev6_csrv.h,v $
 *  Revision 1.1.1.1  1998/12/29 21:36:06  paradis
 *  Initial CVS checkin
 *
 * Revision 1.1  1997/07/10  00:34:04  fdh
 * Initial revision
 *
 */

/*
** Console Service (cserve) sub-function codes:
*/
#define CSERVE_K_JTOPAL         0x09
#define CSERVE_K_RD_IMPURE      0x0B

#if 0
#define CSERVE_K_LDQP           0x01
#define CSERVE_K_STQP           0x02
#define CSERVE_K_WR_INT         0x0A
#define CSERVE_K_PUTC           0x0F

#define CSERVE_K_WR_ICSR	0x10
#define CSERVE_K_WR_ICCSR	0x10    /* for ev4 backwards compatibility */
#define CSERVE_K_RD_ICSR	0x11
#define CSERVE_K_RD_ICCSR	0x11    /* for ev4 backwards compatibility */
#if defined(DC21164PC)
#define CSERVE_K_RD_BCCFG2	0x12
#else
#define CSERVE_K_RD_BCCTL	0x12
#endif
#define CSERVE_K_RD_BCCFG	0x13

#define CSERVE_K_WR_BCACHE      0x16

#define CSERVE_K_RD_BCCFG_OFF   0x17
#endif /* 0 */

#define CSERVE_K_SROM_INIT      0x18
#define CSERVE_K_SROM_PUTC      0x19
#define CSERVE_K_SROM_GETC      0x20

#if 0
#if defined(DC21164PC)
#define CSERVE_K_RD_MAF_MODE	0x21
#define CSERVE_K_WR_MAF_MODE	0x22
#endif
#endif /* 0 */
