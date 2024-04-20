#ifndef H_PCKBD
#define H_PCKBD

/* Programmable Interrupt Controller (PIC) */
#define ICW1			0x20
#define OCW1			0x21
#define KCM             040
#define	CMD	0x64
#define	RDI	0xC0

#define KBD_INT_ENABLE		0x02	/* 0 = enable */



/* status register bits */
#define	KBD_OBF		0x01
#define	KBD_IBF		0x02
#define	KBD_KBEN	0x10
#define	KBD_ODS		0x20
#define	KBD_GTO		0x40
#define	KBD_PERR	0x80

/* mode register bits */
#define KBD_EKI		0x01
#define KBD_SYS		0x04
#define KBD_EMI		0x02
#define KBD_DKB		0x10
#define KBD_DMS		0x20
#define KBD_KCC		0x40


/* kbd */
#define	KBD_RESET	0xFF
#define	KBD_MAKEBREAK	0xFC
#define	KBD_MAKE	0xFD
#define	KBD_SETLEDS	0xED
#define	KBD_SELECTCODE	0xF0
#define	KBD_ENABLE	0xF4
#define	KBD_ACK		0xFA
#define	KBD_RESEND	0xFE
#define	KBD_DEFAULTS	0xF6

/* rtc */
#define	RTCA		0x0170
#define	RTCD		0x0171
#define	RTCCR1		0x6A
#define	ATKBD		0x02

/* outport */
#define	HDLED		0x0002

/* prefix */
#define	F0SEEN		0x0001
#define	E0SEEN		0x0002

/* shift */
#define	LFLAG		0x0001
#define	RFLAG		0x0002

/* make/break - mode 3 */
#define LSHIFT_MAKE  0x2A
#define LSHIFT_BREAK 0xAA
#define RSHIFT_MAKE  0x36
#define RSHIFT_BREAK 0xB6
#define LCTRL_MAKE   0x38
#define LCTRL_BREAK  0xB8
#define RCTRL_MAKE   0x3A
#define RCTRL_BREAK  0xBA
#define LALT_MAKE    0x71
#define LALT_BREAK   0xF1
#define RALT_MAKE    0x72
#define RALT_BREAK   0xF2


#endif /* H_PCKBD */
