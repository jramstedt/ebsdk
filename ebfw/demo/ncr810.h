/* for read_inputs.c */
#define STAT_OK 10
#define STAT_REPEAT 20
#define ERROR_CANCEL 30


#define NCR_CFG_BASE		0x10000	        /* for slot 1, bit 16 */
#define NCR_MEM_BASE		0x81000000      /* at 2 gig   */

/*
   Relative offsets for the configuration registers in the 53C810 SCSI
   controller when doing a config read with IDSEL on the chip selected
   along with the appropriate CBE bits set.
*/

#define NCR$W_VID_OFF		0x00
#define NCR$W_DEVID_OFF		0x02
#define NCR$W_COMMAND_OFF	0x04
#define NCR$W_STATUS_OFF	0x06
#define NCR$B_REV_OFF		0x08
#define NCR$B_TIMER_OFF		0x0D
#define NCR$L_IOBASE_OFF	0x10
#define NCR$L_MEMBASE_OFF	0x14

#define NCR$CFG_W_VID		NCR_CFG_BASE+NCR$W_VID_OFF
#define NCR$CFG_W_DEVID		NCR_CFG_BASE+NCR$W_DEVID_OFF
#define NCR$CFG_W_COMMAND	NCR_CFG_BASE+NCR$W_COMMAND_OFF
#define NCR$CFG_W_STATUS	NCR_CFG_BASE+NCR$W_STATUS_OFF
#define NCR$CFG_B_REV		NCR_CFG_BASE+NCR$B_REV_OFF
#define NCR$CFG_B_TIMER		NCR_CFG_BASE+NCR$B_TIMER_OFF
#define NCR$CFG_L_IOBASE	NCR_CFG_BASE+NCR$L_IOBASE_OFF
#define NCR$CFG_L_MEMBASE	NCR_CFG_BASE+NCR$L_MEMBASE_OFF

/*
   Bit definitions for the DMA status register (DSTAT) and 
   interrupt status register (ISTAT)
*/
#define D_DFE	0x80
#define D_MDPE	0x40
#define D_BF	0x20
#define D_ABRT	0x10
#define D_SSI	0x08
#define D_SIR	0x04
#define D_IID	0x01

#define I_ABRT	0x80
#define I_SRST  0x40
#define I_SIGP  0x20
#define I_SEM   0x10
#define I_CON	0x08
#define I_INTF	0x04
#define I_SIP	0x02
#define I_DIP	0x01

#define SIST0_M_A	0x80
#define SIST0_CMP	0x40
#define SIST0_SEL	0x20
#define SIST0_RSL	0x10
#define SIST0_SGE	0x08
#define SIST0_UDC	0x04
#define SIST0_RST	0x02
#define SIST0_PAR	0x01

#define SIST1_STO	0x04
#define SIST1_GEN	0x02
#define SIST1_HTH	0x01

/*
   Relative offsets for the registers in the 53C810 SCSI controller 
   from the PCI address defined in either the Base I/O address or
   Base Memory address
*/
#define NCR$SCNTL0	0x0
#define NCR$SCNTL1	0x1
#define NCR$SCNTL2	0x2
#define NCR$SCNTL3	0x3
#define NCR$SCID	0x4
#define NCR$SXFER	0x5
#define NCR$SDID	0x6
#define NCR$GPREG	0x7
#define NCR$SFBR	0x8
#define NCR$SOCL	0x9
#define NCR$SSID	0xa
#define NCR$SBCL    	0xb
#define NCR$DSTAT   	0xc
#define NCR$SSTAT0  	0xd
#define NCR$SSTAT1  	0xe
#define NCR$SSTAT2  	0xf
#define NCR$DSA     	0x10
#define NCR$ISTAT   	0x14
#define NCR$CTEST0  	0x18
#define NCR$CTEST1  	0x19
#define NCR$CTEST2  	0x1a
#define NCR$CTEST3  	0x1b
#define NCR$TEMP	0x1c
#define NCR$DFIFO   	0x20
#define NCR$CTEST4  	0x21
#define NCR$CTEST5  	0x22
#define NCR$CTEST6  	0x23
#define NCR$DBC 	0x24
#define NCR$DCMD	0x27
#define NCR$DNAD	0x28
#define NCR$DSP		0x2c
#define NCR$DSPS	0x30
#define NCR$SCRATCHA 	0x34
#define NCR$DMODE   	0x38
#define NCR$DIEN	0x39
#define NCR$DWT		0x3a
#define NCR$DCNTL	0x3b
#define NCR$ADDER	0x3c
#define NCR$SIEN0	0x40
#define NCR$SIEN1	0x41
#define NCR$SIST0	0x42
#define NCR$SIST1   	0x43
#define NCR$SLPAR	0x44
#define NCR$MACNTL	0x46
#define NCR$GPCNTL	0x47
#define NCR$STIME0	0x48
#define NCR$STIME1	0x49
#define NCR$RESPID	0x4a
#define NCR$STEST0	0x4c
#define NCR$STEST1	0x4d
#define NCR$STEST2	0x4e
#define NCR$STEST3	0x4f
#define NCR$SIDL	0x50
#define NCR$SODL	0x54
#define NCR$SBDL	0x58
#define NCR$SCRATCHB	0x5c
#if 0
typedef struct {
	char	scntl0;
	char	scntl1;
	char	scntl2;
	char	scntl3;
	char	scid;
	char	sxfer;
	char	sdid;
	char	gpreg;
	char	sfbr;
	char	socl;
	char	ssid;
	char	sbcl;
	char	dstat;
	char	sstat0;
	char	sstat1;
	char	sstat2;
	int	dsa;
	char	istat;
	char	buf1[3];
	char	ctest0;
	char	ctest1;
	char	ctest2;
	char	ctest3;
	int	temp;
	char	dfifo;
	char	ctest4;
	char	ctest5;
	char	ctest6;
	int 	dcfield;
	int	dnad;
	int	dsp;
	int	dsps;
	int	scratcha;
	char	dmode;
	char	dien;
	char	dwt;
	char	dcntl;
        int	adder;
	char	sien0;
	char	sien1;
	char	sist0;
	char	sist1;
} ncr_regs;
#endif
