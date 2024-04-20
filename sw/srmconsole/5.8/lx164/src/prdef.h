/********************************************************************************************************************************/
/* Created: 21-JUN-2000 10:27:16 by OpenVMS SDL EV1-33     */
/* Source:  17-MAY-1991 10:31:30 AFW_USER$:[CONSOLE.V58.COMMON.SRC]PRDEF.SDL;1 */
/********************************************************************************************************************************/
/*** MODULE $PRDEF ***/
/*+                                                                         */
/*  PROCESSOR REGISTER DEFINITIONS                                          */
/*-                                                                         */
#define PR$_KSP 0                       /*KERNEL STACK POINTER              */
#define PR$_ESP 1                       /*EXECUTIVE STACK POINTER           */
#define PR$_SSP 2                       /*SUPERVISOR STACK POINTER          */
#define PR$_USP 3                       /*USER STACK POINTER                */
#define PR$_ISP 4                       /*INTERRUPT STACK POINTER           */
#define PR$_ASN 6                       /*ADDRESS SPACE NUMBER REGISTER     */
#define PR$_SPTEP 7                     /*SYSTEM PTE PROTOTYPE REGISTER     */
#define PR$_P0BR 8                      /*P0 BASE REGISTER                  */
#define PR$_P0LR 9                      /*P0 LIMIT REGISTER                 */
#define PR$_P1BR 10                     /*P1 BASE REGISTER                  */
#define PR$_P1LR 11                     /*P1 LIMIT REGISTER                 */
#define PR$_SBR 12                      /*SYSTEM BASE REGISTER              */
#define PR$_SLR 13                      /*SYSTEM LIMIT REGISTER             */
#define PR$_CPUID 14                    /*CPU IDENTIFIER REGISTER           */
#define PR$_WHAMI 15                    /*WHo AM I REGISTER                 */
#define PR$_PCBB 16                     /*PROCESS CONTROL BLOCK BASE        */
#define PR$_SCBB 17                     /*SYSTEM CONTROL BLOCK BASE         */
#define PR$_IPL 18                      /*INTERRUPT PRIORITY LEVEL REGISTER  */
#define PR$_ASTLVL 19                   /*AST LEVEL REGISTER                */
#define PR$_SIRR 20                     /*SOFTWARE INTERRUPT REQUEST REGISTER  */
#define PR$_SISR 21                     /*SOFTWARE INTERRUPT SUMMARY REGISTER  */
#define PR$_ICCS 24                     /* INTERVAL CLOCK CONTROL STATUS REGISTER  */
#define PR$_RXCS 32                     /* CONSOLE RECIEVER CONTROL STATUS REGISTER  */
#define PR$_RXDB 33                     /* CONSOLE RECEIVER DATA BUFFER REGISTER  */
#define PR$_TXCS 34                     /* CONSOLE TRANSMIT CONTROL STATUS REGISTER  */
#define PR$_TXDB 35                     /* CONSOLE TRANSMIT DATA BUFFER REGISTER  */
#define PR$_MAPEN 56                    /* MAPPING ENABLE REGISTER          */
#define PR$_TBIA 57                     /* TRANSLATION BUFFER INVALIDATE: ALL */
#define PR$_TBIS 58                     /*   TB INVALIDATE: SINGLE          */
#define PR$_TBIASN 59                   /*   TB INVALIDATE: ADDRESS SPACE NUMBER */
#define PR$_TBISYS 60                   /*   TB INVALIDATE: SYSTEM          */
#define PR$_SID 62                      /* SYSTEM IDENTIFICATION REGISTER   */
#define PR$_TBCHK 63                    /* TRANSLATION BUFFER VALID CHECK   */
#define PR$_VPSR 144                    /* VECTOR PROCESSOR STATUS REGISTER */
#define PR$_VAER 145                    /* VECTOR ARITHMETIC EXCEPTION REGISTER */
#define PR$_VMAC 146                    /* VECTOR MEMORY ACCESS CHECK REGISTER */
#define PR$_VTBIA 147                   /* VECTOR TB INVALIDATE ALL         */
#define PR$_VSAR 148                    /* VECTOR STATE ADDRESS REGISTER    */
#define PR$_SID_TYP780 1                /* VAX 11/780                       */
#define PR$_SID_TYP750 2                /* VAX 11/750                       */
#define PR$_SID_TYP730 3                /* VAX 11/730                       */
#define PR$_SID_TYP790 4                /* VAX 11/790                       */
#define PR$_SID_TYP8SS 5                /* Scorpio for now                  */
#define PR$_SID_TYP8NN 6                /* Nautilus for now                 */
#define PR$_SID_TYPUV1 7                /* MicroVAX I                       */
#define PR$_SID_TYPUV2 8                /* MicroVAX II                      */
#define PR$_SID_TYP410 8                /* VAXstar                          */
#define PR$_SID_TYP009 9                /* Virtual VAX                      */
#define PR$_SID_TYP420 10               /* PVAX                             */
#define PR$_SID_TYP520 10               /* Cirrus I                         */
#define PR$_SID_TYP650 10               /* Mayfair                          */
#define PR$_SID_TYP9CC 10               /* Calypso/XCP                      */
#define PR$_SID_TYP9CI 10
#define PR$_SID_TYP60 10                /* Firefox                          */
#define PR$_SID_TYP670 11               /* KA670 (Pele)                     */
#define PR$_SID_TYP9RR 11               /* XRP                              */
#define PR$_SID_TYP43 11                /* KA43 (RigelMAX)                  */
#define PR$_SID_TYP9AQ 14               /* Aquarius                         */
#define PR$_SID_TYP8PS 17               /* Polarstar                        */
#define PR$_SID_TYP1202 18              /* Mariah/XMP                       */
#define PR$_SID_TYP46 18                /* PV-Mariah                        */
#define PR$_SID_TYP690 19
#define PR$_SID_TYP1302 19
#define PR$_SID_TYP660 20               /* KA660 (Spitfire)                 */
#define PR$_SID_TYP440 20               /* PVAX2                            */
#define PR$_SID_TYP550 20               /* Cirrus II                        */
#define PR$_SID_TYP1701 23              /* Laser/Neon                       */
#define PR$_SID_TYPMAX 23               /* MAX LEGAL CPU TYPE               */
/*Chip CPU types                                                            */
#define PR$_SID_TYPUV 8                 /* MicroVAX chip                    */
/*MicroVAX chip CPU Subtypes                                                */
#define PR$_XSID_UV_UV 0                /* Generic MicroVAX (unused subtype) */
#define PR$_XSID_UV_UV2 1               /* MicroVAX II                      */
#define PR$_XSID_UV_410 4               /* VAXstar                          */
#define PR$_SID_TYPCV 10                /* CVAX chip                        */
/*CVAX chip CPU Subtypes                                                    */
#define PR$_XSID_CV_CV 0                /* Generic CVAX (unused subtype)    */
#define PR$_XSID_CV_650 1               /* Mayfair                          */
#define PR$_XSID_CV_9CC 2               /* Calypso/XCP                      */
#define PR$_XSID_CV_60 3                /* Firefox                          */
#define PR$_XSID_CV_420 4               /* PVAX                             */
#define PR$_XSID_CV_9CI 5
#define PR$_XSID_CV_520 7               /* CIRRUS I                         */
#define PR$_SID_TYPRV 11                /* Rigel chip                       */
/* Rigel chip CPU Subtypes                                                  */
#define PR$_XSID_RV_RV 0                /* Generic Rigel (unused subtype)   */
#define PR$_XSID_RV_670 1               /* KA670 (Pele)                     */
#define PR$_XSID_RV_9RR 2               /* Calypso/XRP                      */
#define PR$_XSID_RV_43 4                /* KA43 (RigelMAX)                  */
#define PR$_SID_TYPV12 18               /* Mariah chip set                  */
/* Mariah chip CPU Subtypes                                                 */
#define PR$_XSID_V12_V12 0              /* Generic Mariah (unused subtype)  */
#define PR$_XSID_V12_1202 2             /* MARIAH/XMP                       */
#define PR$_XSID_V12_46 4               /* PVAX- mariah subtype             */
#define PR$_SID_TYPV13 19
#define PR$_XSID_V13_V13 0
#define PR$_XSID_V13_690 1
#define PR$_XSID_V13_1302 2
#define PR$_SID_TYPV14 20               /* SOC Chip SID                     */
/* SOC chip CPU subtypes                                                    */
#define PR$_XSID_V14_V14 0              /* unused subtype                   */
#define PR$_XSID_V14_660 1              /* KA660 (Spitfire)                 */
#define PR$_XSID_V14_440 4              /* PVAX2 subtype                    */
#define PR$_XSID_V14_550 7              /* CIRRUS II                        */
#define PR$_SID_TYPV17 23               /* NVAX+ Chip SID                   */
/* NVAX+ chip CPU subtypes                                                  */
#define PR$_XSID_V17_V17 0              /* unused subtype                   */
#define PR$_XSID_V17_1701 1             /* Laser/Neon                       */
/*Nautilus CPU Subtypes                                                     */
#define PR$_XSID_N8800 0                /* VAX 8800                         */
#define PR$_XSID_N8700 1                /* VAX 8700                         */
#define PR$_XSID_N2 2                   /* Undefined Nautilus CPU           */
#define PR$_XSID_N3 3                   /* Undefined Nautilus CPU           */
#define PR$_XSID_N4 4                   /* Undefined Nautilus CPU           */
#define PR$_XSID_N5 5                   /* Undefined Nautilus CPU           */
#define PR$_XSID_N8550 6                /* VAX 8550                         */
#define PR$_XSID_N8500 7                /* VAX 8500                         */
#define PR$_XSID_N8NNN -1               /* Unknown Nautilus CPU             */
/*VAX 11/780 IPR'S:                                                         */
#define PR$_WCSA 44                     /* WCS ADDRESS REGISTER             */
#define PR$_WCSD 45                     /* WCS DATA REGISTER                */
#define PR$_SBIFS 48                    /* SBI FAULT STATUS REGISTER        */
#define PR$_SBIS 49                     /* SBI SILO REGISTER                */
#define PR$_SBISC 50                    /* SBI COMPARATOR REGISTER          */
#define PR$_SBIMT 51                    /* SBI MAINTENANCE REGISTER         */
#define PR$_SBIER 52                    /* SBI ERROR REGISTER               */
#define PR$_SBITA 53                    /* SBI TIMEOUT ADDRESS REGISTER     */
#define PR$_SBIQC 54                    /* SBI QUADWORD CLEAR REGISTER      */
/*END OF VAX 11/780-SPECIFIC IPR'S                                          */
#define PR$_CMIERR 23                   /* CMI ERROR SUMMARY REGISTER       */
#define PR$_CSRS 28                     /* CONSOLE BLK STORE RCV STATUS     */
#define PR$_CSRD 29                     /* CONSOLE BLK STORE RCV DATA       */
#define PR$_CSTS 30                     /* CONSOLE BLK STORE XMIT STATUS    */
#define PR$_CSTD 31                     /* CONSOLE BLK STORE XMIT DATA      */
#define PR$_TBDR 36                     /* TB DISABLE REGISTER              */
#define PR$_CADR 37                     /* CACHE DISABLE REGISTER           */
#define PR$_MCESR 38                    /* MACHINE CHECK ERROR SUMMARY REG  */
#define PR$_CAER 39                     /* CACHE ERROR REGISTER             */
#define PR$_UBRESET 55                  /* UNIBUS I/O RESET REGISTER        */
/*END OF 11/750 AND 11/730 IPR'S                                            */
#define PR$_PAMACC 64                   /* PAMM ACCESS                      */
#define PR$_PAMLOC 65                   /* PAMM LOCATION                    */
#define PR$_CSWP 66                     /* CACHE SWEEP REGISTER             */
#define PR$_MDECC 67                    /* MBOX DATA ECC REGISTER           */
#define PR$_MENA 68                     /* MBOX ERROR ENABLE REGISTER       */
#define PR$_MDCTL 69                    /* MBOX DATA CONTROL REGISTER       */
#define PR$_MCCTL 70                    /* MBOX MCC CONTROL REGISTER        */
#define PR$_MERG 71                     /* MBOX ERROR GENERATOR REGISTER    */
#define PR$_CRBT 72                     /* CONSOLE REBOOT                   */
#define PR$_DFI 73                      /* DIAGNOSTIC FAULT INSERTION       */
#define PR$_EHSR 74                     /* ERROR HANDLING STATUS REGISTER   */
#define PR$_ACCS790 75                  /* ACCELERATOR STATUS REGISTER      */
#define PR$_STXCS 76                    /* CONSOLE STORAGE CONTROL REG      */
#define PR$_STXDB 77                    /* CONSOLE STORAGE DATA REGISTER    */
#define PR$_LSPA 78                     /* SCRATCHPAD ADDRESS               */
#define PR$_RSPD 79                     /* SCRATCHPAD DATA                  */
/*END OF 11/790 PROCESSOR-SPECIFIC IPRS                                     */
union PRDEF {
    struct  {
        unsigned PR$V_SID_SN : 12;      /* SERIAL NUMBER FIELD              */
        unsigned PR$V_SID_PL : 3;       /* PLANT ID                         */
        unsigned PR$V_SID_ECO : 9;      /* ECO LEVEL                        */
        unsigned PR$V_SID_TYPE : 8;     /* CPU TYPE CODE                    */
        } PR$R_PRDEF_BITS;
    struct  {
        unsigned PR$V_FILL_XSID_BITS : 24; /* CPU-SPECIFIC XSID BITS        */
        unsigned PR$V_XSID_TYPE : 8;    /* CPU SUBTYPE CODE                 */
        } PR$R_PRDEF_XBITS;
/* SYSTEM ID REGISTER CPU TYPES                                             */
/* Number assignments are                                                   */
/* based upon the jumpers                                                   */
/* read by the console from                                                 */
/* the MPS backplane                                                        */
/*VAX 11/750 AND 11/730 IPR'S:                                              */
/*VAX 11/790 PROCESSOR-SPECIFIC IPRS                                        */
    } ;
 
