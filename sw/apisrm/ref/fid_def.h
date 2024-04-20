/********************************************************************************************************************************/
/* Created 20-FEB-1991 15:53:39 by VAX SDL V3.2-12     Source:  2-SEP-1989 10:22:30 CLOUD_USER:[SHIRRON.V5CP.SRC]FIDDEF.SDL;1 */
/********************************************************************************************************************************/
 
/*** MODULE $FIDDEF ***/
/*                                                                          */
/* STRUCTURE OF A FILE ID (FID)                                             */
/*                                                                          */
#define FID$K_LENGTH 6
#define FID$C_LENGTH 6
#define FID$C_INDEXF 1                  /* INDEX FILE                       */
#define FID$C_BITMAP 2                  /* STORAGE MAP FILE                 */
#define FID$C_BADBLK 3                  /* BAD BLOCK FILE                   */
#define FID$C_MFD 4                     /* MASTER FILE DIRECTORY            */
#define FID$C_CORIMG 5                  /* CORE IMAGE FILE                  */
#define FID$C_VOLSET 6                  /* VOLUME SET LIST FILE             */
#define FID$C_CONTIN 7                  /* STANRARD CONTINUATION FILE       */
#define FID$C_BACKUP 8                  /* BACKUP LOG FILE                  */
#define FID$C_BADLOG 9                  /* BAD BLOCK LOG FILE               */
#define FID$C_FREFIL 10                 /* FREE SPACE FILE                  */
struct FIDDEF {
    unsigned short int FID$W_NUM;       /* FILE NUMBER                      */
    unsigned short int FID$W_SEQ;       /* FILE SEQUENCE NUMBER             */
    variant_union  {
        unsigned short int FID$W_RVN;   /* RELATIVE VOLUME NUMBER           */
        variant_struct  {
            unsigned char FID$B_RVN;    /* BYTE FORM OF RVN                 */
            unsigned char FID$B_NMX;    /* FILE NUMBER EXTENSION            */
/* FILE ID'S OF THE RESERVED FILES                                          */
            } FID$R_RVN_FIELDS;
        } FID$R_RVN_OVERLAY;
    } ;
