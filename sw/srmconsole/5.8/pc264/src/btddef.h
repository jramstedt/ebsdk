/********************************************************************************************************************************/
/* Created: 21-AUG-2000 16:05:05 by OpenVMS SDL EV1-31     */
/* Source:  13-MAY-1991 11:46:34 NIGHTRIDER:[CONSOLE.V58.COMMON.SRC]BTDDEF.SDL;1 */
/********************************************************************************************************************************/
/*** MODULE $BTDDEF ***/
/*+                                                                         */
/*                                                                          */
/* Boot device codes                                                        */
/*                                                                          */
/*-                                                                         */
/* "$K_" added, 8/30/79, CHP                                                */
#define BTD$K_MB 0                      /* Massbus device                   */
/* Types 1-31. reserved for                                                 */
/*  Unibus(Qbus) devices                                                    */
#define BTD$K_DM 1                      /* RK06/7                           */
#define BTD$K_DL 2                      /* RL02                             */
#define BTD$K_DQ 3                      /* RB02/RB80                        */
#define BTD$K_PROM 8                    /* PROM (not copied)                */
#define BTD$K_PROM_COPY 9               /* PROM copied (Mayflower)          */
#define BTD$K_UDA 17                    /* UDA                              */
#define BTD$K_TK50 18                   /* TK50 (MAYA)                      */
#define BTD$K_KFQSA 19                  /* KFQSA                            */
#define BTD$K_QBUS_SCSI 20              /* Qbus SCSI adapter                */
/* End of Unibus(Qbus) devices                                              */
#define BTD$K_HSCCI 32                  /* HSC on CI                        */
#define BTD$K_BDA 33                    /* BI disk adapter                  */
#define BTD$K_BVPSSP 34                 /* BVP Storage Systems ports        */
#define BTD$K_AIE_TK50 35               /* AIE/TK50 port                    */
#define BTD$k_ST506_DISK 36             /* ST506 disk (PVAX/VAXstar)        */
#define BTD$K_KA410_DISK 36             /* VAXstar ST506 disk               */
#define BTD$K_KA420_DISK 36             /* PVAX ST506 disk                  */
#define BTD$K_SCSI_5380_TAPE 37         /* NCR 5380 SCSI tape (PVAX/VAXstar) */
#define BTD$K_KA410_TAPE 37             /* VAXstar SCSI tape                */
#define BTD$K_KA420_TAPE 37             /* PVAX SCSI tape                   */
#define BTD$K_DISK9 38                  /* Disk on 009                      */
#define BTD$K_SII 39                    /* Embedded DSSI controller         */
#define BTD$K_SHAC 41                   /* Single chip DSSI adapter.        */
#define BTD$K_SCSI_5380_DISK 42         /* NCR 5380 SCSI disk (PVAX)        */
#define BTD$K_HSX 43
#define BTD$K_KDM70 43
#define BTD$K_HSXTAPE 44
#define BTD$K_KDM70TAPE 44
#define BTD$K_SWIFT 45                  /* Another embedded DSSI controller-CIRRUS */
#define BTD$K_SCSI_53C94_DISK 46
#define BTD$K_SCSI_53C94_TAPE 47
#define BTD$K_CONSOLE 64                /* Console block storage device     */
/* Network boot devices (96-103)                                            */
#define BTD$K_NET_DLL 96                /* Start of network boot devices    */
/*  Codes 96-127 reserved                                                   */
#define BTD$K_QNA 96                    /* DEQNA                            */
#define BTD$K_UNA 97                    /* DEUNA                            */
#define BTD$K_AIE_NI 98                 /* AIE/NI                           */
#define BTD$K_LANCE 99                  /* LANCE NI chip                    */
#define BTD$K_KA410_NI 99               /* VAXstar NI (LANCE chip)          */
#define BTD$K_KA420_NI 99               /* PVAX NI (LANCE chip)             */
#define BTD$K_SGEC 100                  /* SGEC chip                        */
#define BTD$K_SERVER_DEBNA 101
#define BTD$K_SERVER_DEBNI 102
#define BTD$K_SERVER_XNA 103
#define BTD$K_DEBNI 104                 /* DEBNI                            */
#define BTD$K_DEMNA 105                 /* DEMNA                            */
#define BTD$K_KA520_NI 106              /* CIRRUS NI                        */
#define BTD$K_NISCS 128                 /* SCS disk over NI                 */
 
