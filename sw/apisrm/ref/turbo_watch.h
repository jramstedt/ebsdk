/*
 * DS1287 WATCH Chip Registers
 */

#define watch_r_seconds		0xb0000000	/* Seconds 0-59 decimal */
#define watch_r_minutes		0xb0000080	/* Minutes 0-59 decimal */
#define watch_r_hours		0xb0000100	/* Hours 0-23 decimal */
#define watch_r_day_of_month	0xb00001c0	/* Day_of_month 0-31 decimal */
#define watch_r_month		0xb0000200	/* Month 0-12 decimal */
#define watch_r_year		0xb0000240	/* Year 0-99 decimal */
#define watch_r_csr_a		0xb0000280	/* CSR A */
#define watch_r_csr_b		0xb00002c0	/* CSR B */
#define watch_r_csr_c		0xb0000300	/* CSR C */
#define watch_r_csr_d		0xb0000340	/* CSR D */

#define WATCH_M_UIP	0x80	/* Update In Progress               */

#define WATCH_M_DSE	0x1	/* Daylight Savings Enable          */
#define WATCH_M_TF_TW	0x2	/* Hour Mode (TF=24,TW=12)          */
#define WATCH_M_DM	0x4	/* Data Mode                        */
#define WATCH_M_SQWE	0x8	/* Square Wave Enable               */
#define WATCH_M_UIE	0x10	/* Update Ended Interrupt Enable    */
#define WATCH_M_AIE	0x20	/* Alarm interrupt enable           */
#define WATCH_M_PIE	0x40	/* Periodic Interrupt Enable        */
#define WATCH_M_SET	0x80	/* SET (for TOY counts)             */

#define WATCH_M_UF	0x10	/* Update Ended Interrupt Flag      */
#define WATCH_M_AF	0x20	/* Alarm interrupt Flag             */
#define WATCH_M_PF	0x40	/* Periodic Interrupt Flag          */
#define WATCH_M_IRQF	0x80	/* OR of UF, AF, PF                 */

#define WATCH_M_VRT	0x80	/* Valid RAM time                   */

#define CSRA_DV$K_4MHZ 0
#define CSRA_DV$K_1MHZ 16
#define CSRA_DV$K_32KHZ 32
#define CSRA_RS$K_0000 0
#define CSRA_RS$K_0001 1
#define CSRA_RS$K_0010 2
#define CSRA_RS$K_0011 3
#define CSRA_RS$K_0100 4
#define CSRA_RS$K_0101 5
#define CSRA_RS$K_0110 6
#define CSRA_RS$K_0111 7
#define CSRA_RS$K_1000 8
#define CSRA_RS$K_1001 9
#define CSRA_RS$K_1010 10
#define CSRA_RS$K_1011 11
#define CSRA_RS$K_1100 12
#define CSRA_RS$K_1101 13
#define CSRA_RS$K_1110 14
#define CSRA_RS$K_1111 15

