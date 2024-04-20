/********************************************************************************************************************************/
/* Created 21-FEB-1991 13:23:15 by VAX SDL V3.2-12     Source: 21-FEB-1991 13:23:06 WRKD$1:[DEAMICIS.LASER.CODE.REFERENCE]GBUS_MI */
/********************************************************************************************************************************/
 
/*** MODULE LEP_GBUS ***/
/* Revision history:                                                        */
/*                                                                          */
/* 1-28-91  Mark DeAmicis                                                   */
/*	- Added GBus$WHAMI bit descriptions/defines.                        */
/*	- Added GBus$PMask bit descriptions/defines.  Note the PHaltEn is new */
/*	  and can not be found in v0.9 of the LEP spec.                     */
/*	- Changed the byte fill from 63 to 60 between GBus registers.  This */
/*	  was necessary because we are defining the registers as byte (char) */
/*	  length and the compiler was then padding three more bytes on to   */
/*	  the address to achieve longword alignment.   We then only have to */
/*	  60 more bytes to get to the next GBus byte register (remember the */
/*	  registers are on 64 byte boundaries).                             */
/*+                                                                         */
/* LEP GBUS Registers                                                       */
/*-                                                                         */
#define GBUS_M_LSB_BAD 4
#define GBUS_M_MFG 8
#define GBUS_M_STP 1
#define GBUS_M_LCON_WIN 2
#define GBUS_M_RUN 4
#define GBUS_M_LED3 8
#define GBUS_M_LED4 16
#define GBUS_M_LED5 32
#define GBUS_M_LED6 64
#define GBUS_M_LED7 128
#define GBUS_M_HALTEN 1
#define GBUS_M_SELCONSTERM 2
#define GBUS_M_PHALTEN 4
#define GBUS_M_DUART0_I 1
#define GBUS_M_DUART1_I 2
#define GBUS_M_LSB0 4
#define GBUS_M_LSB2 32
#define GBUS_M_IP 64
#define GBUS_M_INTIM 128
#define GBUS_M_CTL_P_HALT 64
#define GBUS_M_NHALT 128
#define GBUS_M_EXPSEL0 1
#define GBUS_M_EXPSEL1 2
struct LEPGBUS {
    union  {
        struct  {
            unsigned GBUS_V_NID : 2;    /* LSB node-id (2 bit field)        */
            unsigned GBUS_V_LSB_BAD : 1;
            unsigned GBUS_V_MFG : 1;
            unsigned GBUS_v_rese : 4;
            } GBUS_R_WHAMI_BITS;
        unsigned char GBUS_B_WHAMI;     /* LEP LSB node-id                  */
        } GBUS_R_WHAMI;
/*                                                                          */
#define gbus_r_whami GBUS_R_WHAMI.GBUS_B_WHAMI                            
#define gbus_v_whami GBUS_R_WHAMI.GBUS_R_WHAMI_BITS                       
/*                                                                          */
/*                                                                          */
    char GBUS_b_fill1 [60];
    union  {
        struct  {
            unsigned GBUS_V_STP : 1;    /* Self-test passed LED             */
            unsigned GBUS_V_LCON_WIN : 1; /* Laser Console Win LED          */
            unsigned GBUS_V_RUN : 1;    /* RUN LED                          */
            unsigned GBUS_V_LED3 : 1;   /* LED number 3                     */
            unsigned GBUS_V_LED4 : 1;   /* LED number 4                     */
            unsigned GBUS_V_LED5 : 1;   /* LED number 5                     */
            unsigned GBUS_V_LED6 : 1;   /* LED number 6                     */
            unsigned GBUS_V_LED7 : 1;   /* LED number 7                     */
            } GBUS_R_LEDS_BITS;
        unsigned char GBUS_B_LEDS;      /* leds register                    */
        } GBUS_R_LEDS;
/*                                                                          */
#define gbus_r_leds GBUS_R_LEDS.GBUS_B_LEDS                               
#define gbus_v_leds GBUS_R_LEDS.GBUS_R_LEDS_BITS                          
/*                                                                          */
    char GBUS_b_fill2 [60];
    union  {
        struct  {
            unsigned GBUS_V_HALTEN : 1; /* Processor halt enable            */
            unsigned GBUS_V_SELCONSTERM : 1; /* Select Console Terminal     */
            unsigned GBUS_V_PHALTEN : 1; /* CTL-P halt enable               */
            unsigned GBUS_V_fill_0 : 5;
            } GBUS_R_CTL_P_MASK_BITS;
        unsigned char GBUS_B_CTL_P_MASK; /* CTL_P_mask register             */
        } GBUS_R_CTL_P_MASK;
/*                                                                          */
#define gbus_r_ctl_p_mask GBUS_R_CTL_P_MASK.GBUS_B_CTL_P_MASK             
#define gbus_v_ctl_p_mask GBUS_R_CTL_P_MASK.GBUS_R_CTL_P_MASK_BITS        
/*                                                                          */
    char GBUS_b_fill3 [60];
    union  {
        struct  {
            unsigned GBUS_V_DUART0_I : 1; /* UART0 interrupt                */
            unsigned GBUS_V_DUART1_I : 1; /* UART1 interrupt                */
            unsigned GBUS_V_LSB0 : 1;   /* LSB level 0 interrupt            */
            unsigned GBUS_v_rese : 2;
            unsigned GBUS_V_LSB2 : 1;   /* LSB level 2 interrupt            */
            unsigned GBUS_V_IP : 1;     /* LIPINTR register interrupt       */
            unsigned GBUS_V_INTIM : 1;  /* Watch interval timer interrupt   */
            } GBUS_R_INTR_BITS;
        unsigned char GBUS_B_INTR;      /* intr register                    */
        } GBUS_R_INTR;
/*                                                                          */
#define gbus_r_intr GBUS_R_INTR.GBUS_B_INTR                               
#define gbus_v_intr GBUS_R_INTR.GBUS_R_INTR_BITS                          
/*                                                                          */
    char GBUS_b_fill4 [60];
    union  {
        struct  {
            unsigned GBUS_v_rese : 6;
            unsigned GBUS_V_CTL_P_HALT : 1; /* CTL_P character detected     */
            unsigned GBUS_V_NHALT : 1;  /* Same as LCNR<NHALT> LEVI CSR     */
            } GBUS_R_HALT_BITS;
        unsigned char GBUS_B_HALT;      /* halt register                    */
        } GBUS_R_HALT;
/*                                                                          */
#define gbus_r_halt GBUS_R_HALT.GBUS_B_HALT                               
#define gbus_v_halt GBUS_R_HALT.GBUS_R_HALT_BITS                          
/*                                                                          */
    char GBUS_b_fill5 [60];
    union  {
        struct  {
            unsigned GBUS_v_rese : 8;
            } GBUS_R_LSBRST_BITS;
        unsigned char GBUS_B_LSBRST;    /* Lsbrst, Gbus access              */
        } GBUS_R_LSBRST;
/*                                                                          */
#define gbus_r_lsbrst GBUS_R_LSBRST.GBUS_B_LSBRST                         
#define gbus_v_lsbrst GBUS_R_LSBRST.GBUS_R_LSBRST_BITS                    
/*                                                                          */
    char GBUS_b_fill6 [60];
    union  {
        struct  {
            unsigned GBUS_V_EXPSEL0 : 1; /* Expander sel bit0 (see LEP spec) */
            unsigned GBUS_V_EXPSEL1 : 1; /* Expander sel bit1 (see LEP spec) */
            unsigned GBUS_V_fill_1 : 6;
            } GBUS_R_MISC_BITS;
        unsigned char GBUS_B_MISC;      /* Misc value for Gbus access       */
        } GBUS_R_MISC;
/*                                                                          */
#define gbus_r_misc GBUS_R_MISC.GBUS_B_MISC                               
#define gbus_v_misc GBUS_R_MISC.GBUS_R_MISC_BITS                          
/*                                                                          */
    } ;
