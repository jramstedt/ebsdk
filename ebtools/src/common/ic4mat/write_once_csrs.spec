32_BYTE_IO<0>
Enable 32_byte io mode 

SKEWED_FILL_MODE<0>
Asserted when bcache is 1.5X ratio 

DUP(SKEWED_FILL_MODE)


BC_CLEAN_VICTIM<0>
enable clean victims to the system interface

SYS_BUS_SIZE<1:0>
indicate size of SysAddOut/In bus 

SYS_BUS_FORMAT<0>
indicate bus format for system bus

SYS_CLK_RATIO<4,0>
indicates the speed of system bus
         0 0 0 0 1   |   1.5X
         0 0 0 1 0   |   2.0X
         0 0 1 0 0   |   2.5X
         0 1 0 0 0   |   3.0X
         1 0 0 0 0   |   3.5X

DUP_TAG_ENABLE<0>
enable duplicate tag mode in EV6

PRB_TAG_ONLY<0>
enable probe-tag only mode in EV6

FAST_MODE_DISABLE<0>
when asserted, disables fast data movement mode

BC_RDVICTIM<0>
enables rdvictim mode on the pins

DUP(BC_CLEAN_VICTIM)


RDVIC_ACK_INHIBIT
enable inhibition of incrementing ack counter for Rdvics

SYSBUS_MB_ENABLE
enable MBs off-chip

SYSBUS_ACK_LIMIT<0:4>
sysbus ack limit csr

SYSBUS_VIC_LIMIT<0:2>
limit for victims 

DUP(BC_CLEAN_VICTIM)


BC_RD_WR_BUBBLES<0:5>
read to write GCLK bubbles for the bcache interface

BC_RD_RD_BUBBLES<0:1>
read to read GCLK bubbles for banked bcaches

BC_WR_RD_BUBBLES<0:3>
write to read BCLK bubbles 

DUP(DUP_TAG_ENABLE)


DUP(SKEWED_FILL_MODE)


DUP(BC_RDVICTIM)


DUP(SKEWED_FILL_MODE)


DUP(BC_RDVICTIM)


DUP(BC_CLEAN_VICTIM)


DUP(DUP_TAG_MODE)


DUP(SKEWED_FILL_MODE)


SPEC_READ_ENABLE<0>
enable speculative references to the system port

DUP(SKEWED_FILL_MODE)


DUP(SKEWED_FILL_MODE)


MBOX_BC_PRB_STALL
Must be asserted when BC_RATIO = 4.0X, 5.0X, 6.0X, 7.0X, or 
8.0X.

BC_LAT_DATA_PATTERN<0:31>
bcache data latency pattern

BC_LAT_TAG_PATTERN<0:23>
bcache tag latency pattern

DUP(BC_RDVICTIM)


ENABLE_STC_COMMAND<0>
enable store conditionals to the pins

BC_LATE_WRITE_NUM<0:2>
number of bcache clocks to delay the data for bcache writes.

BC_CPU_LATE_WRITE_NUM<0:1>
number of GCLKs to delay the Bcache clock/data from Index.

BC_BURST_MODE_ENABLE<0>
burst-mode enable signal 

BC_PENTIUM_MODE<0>
enable pentium mode RAM behavior

DUP(SKEWED_FILL_MODE)


BC_FRM_CLK<0>
force all bcache transactions to start on rising edges of the A phase of 
a GCLK. 

DUP(BC_CPU_LATE_WRITE_NUM<0:1>)


BC_DDMR_ENABLE<0>
enables the rising edge of the bcache forwarded clock..always enabled

BC_DDMF_ENABLE<0>
enable the falling edge of the bcache forwarded clock..always enabled.

BC_LATE_WRITE_UPPER<0>
 

BC_TAG_DDM_FALL_EN<0>
enables the update of the EV6 bcache tag outputs based on the falling 
edge of the forwarded clock.

BC_TAG_DDM_RISE_EN<0>
enables the update of the EV6 bcache tag outputs based on the rising 
edge of the forwarded clock.

BC_CLKFWD_ENABLE<0>
enable clock forwarding on the bcache interface.

BC_RCV_MUX_CNT_PRESET<0:1>
initial value for the bcache clock forwarding unload pointer FIFO.

BC_TAG_DDM_RD_FALL_EN<0>
enables the sampling of incoming data on the falling edge of the 
incoming forwarded clock.

BC_TAG_DDM_RD_RISE_EN<0>
enables the sampling of incoming data on the rising edge of the 
incoming forwarded clock.

DUP(BC_LATE_WRITE_UPPER)
asserted when  (BC_LATE_WRITE_NUM > 3) or 
((BC_LATE_WRITE_NUM==3) &     
(BC_CPU_LATE_WRITE_NUM>1))

SYS_DDM_FALL_EN<0>
enables the update of the EV6 system outputs based on the falling 
edge of the system forwarded clock.

SYS_DDM_RISE_EN<0>
enables the update of the EV6 system outputs based on the rising edge 
of the system forwarded clock.

SYS_CLKFWD_ENABLE<0>
enables clock forwarding on the system interface.

SYS_RCV_MUX_CNT_PRESET<0:1>
initial value for the system clock forwarding unload pointer FIFO 

SYS_DDM_RD_FALL_EN<0>
enables the sampling of incoming data on the falling edge of the 
incoming forwarded clock.

SYS_DDM_RD_RISE_EN<0>
enables the sampling of incoming data on the rising edge of the 
incoming forwarded clock.

SYS_CLK_DELAY<0:1>
delay of 0-2 phases between the forwarded clock out and address/data.

SYS_DDMR_ENABLE<0>
enables the rising edge of the system forwarded clock..always enabled.

SYS_DDMF_ENABLE<0>
enables the falling edge of the system forwarded clock..always 
enabled.

BC_DDM_FALL_EN<0>
enables update of data/address on the rising edge of the system 
forwarded clock..

BC_DDM_RISE_EN<0>
enables the update of data/address on the falling edge of the system 
forwarded clock..

DUP(BC_CLKFWD_ENABLE)


DUP(BC_RCV_MUX_CNT_PRESET<0:1>)


BC_DDM_RD_FALL_EN_A<0>
enable the update of the EV6 bcache data outputs based on the falling 
edge of the forwarded clock.

BC_DDM_RD_RISE_EN_A<0>
enables the update of the EV6 bcache data outputs based on the rising 
edge of the forwarded clock.

DUP(BC_CLK_DELAY<0:1>)


DUP(BC_DDMR_ENABLE)


DUP(BC_DDMF_ENABLE)


DUP(SYS_DDM_FALL_EN)


DUP(SYS_DDM_RISE_EN)


DUP(SYS_CLKFWD_ENABLE)


DUP(SYS_RCV_MUX_CNT_PRESET<0:1>)


DUP(SYS_DDM_RD_FALL_EN)


DUP(SYS_DDM_RD_RISE_EN)


DUP(SYS_CLK_DELAY<0:1>)


DUP(SYS_DDMR_ENABLE)


DUP(SYS_DDMF_ENABLE)


DUP(BC_DDM_FALL_EN)


DUP(BC_DDM_RISE_EN)


DUP(BC_CLKFWD_ENABLE)


DUP(BC_RCV_MUX_CNT_PRESET<0:1>)


DUP(BC_DDM_RD_FALL_EN)


DUP(BC_DDM_RD_RISE_EN)


DUP(SYS_DDM_FALL_EN)


DUP(SYS_DDM_RISE_EN)


DUP(SYS_CLKFWD_ENABLE)


DUP(SYS_RCV_MUX_CNT_PRESET<0:1>)


DUP(SYS_DDM_RD_FALL_EN)


DUP(SYS_DDM_RD_RISE_EN)


DUP(SYS_CLK_DELAY<0:1>)


DUP(SYS_DDMR_ENABLE)


DUP(SYS_DDMF_ENABLE)


DUP(BC_DDM_FALL_EN)


DUP(BC_DDM_RISE_EN)


DUP(BC_CLKFWD_ENABLE)


DUP(BC_RCV_MUX_CNT_PRESET<0:1>)


DUP(BC_DDM_RD_FALL_EN)


DUP(BC_DDM_RD_RISE_EN)


DUP(BC_CLK_DELAY<0:1>)


DUP(BC_DDMR_ENABLE)


DUP(BC_DDMF_ENABLE)


DUP(SYS_DDM_FALL_EN)


DUP(SYS_DDM_RISE_EN)


DUP(SYS_CLKFWD_ENABLE)


DUP(SYS_RCV_MUX_CNT_PRESET<0:1>)


DUP(SYS_DDM_RD_FALL_EN)


DUP(SYS_DDM_RD_RISE_EN)


DUP(SYS_CLK_DELAY<0:1>)


DUP(SYS_DDMR_ENABLE)


DUP(SYS_DDMF_ENABLE)


DUP(BC_DDM_FALL_EN)


DUP(BC_DDM_RISE_EN)


DUP(BC_CLKFWD_ENABLE)


DUP(BC_RCV_MUX_CNT_PRESET<0:1>)


DUP(BC_DDM_RD_FALL_EN)


DUP(BC_DDM_RD_RISE_EN)


DUP(SYS_DDM_FALL_EN)


DUP(SYS_DDM_RISE_EN)


DUP(SYS_CLKFWD_ENABLE)


DUP(SYS_RCV_MUX_CNT_PRESET<0:1>)


DUP(SYS_DDM_RD_FALL_EN)


DUP(SYS_DDM_RD_RISE_EN)


DUP(SYS_CLK_DELAY<0:1>)


DUP(SYS_DDMR_ENABLE)


DUP(SYS_DDMF_ENABLE)


DUP(BC_DDM_FALL_EN)


DUP(BC_DDM_RISE_EN)


DUP(BC_CLKFWD_ENABLE)


DUP(BC_RCV_MUX_CNT_PRESET<0:1>)


DUP(BC_DDM_RD_FALL_EN)


DUP(BC_DDM_RD_RISE_EN)


CFR_GCLK_DELAY<0:2>


CFR_EV6CLK_DELAY<0:2>


CFR_FRMCLK_DELAY<0:1>


DUP(BC_LATE_WRITE_NUM<0:2>


DUP(BC_CPU_LATE_WRT_NUM<0:1>


JITTER_CMD<0>
add one GCLK to the SYSDC write path.

DUP(FAST_MODE_DISABLE<0>)


SYSDC_DELAY<3:0>
number of  GCLKs to delay SYSDC fill commands before action by 
the CBOX.

DATA_VALID_DLY<1:0>
number of bcache clock ticks to delay SYS_DATA_IN_VALID before 
sample by the CBOX.

DUP(BC_DDM_FALL_EN)


DUP(BC_DDM_RISE_EN)


DUP(BC_CPU_CLK_DELAY<0:1>)


BC_FDBK_EN<0:7>
CSR to program the Bcache fwded clock shift register feedback points

BC_CLK_LD_VECTOR<0:15>
CSR  to program the Bcache fwded clock shift register load values

BC_BPHASE_LD_VECTOR<0:3>
CSR to program the Bcache fwded clock bphase enables.

DUP(SYS_DDM_FALL_EN)


DUP(SYS_DDM_RISE_EN)


DUP(SYS_CPU_CLK_DELAY<0:1>)


SYS_FDBK_EN<0:7>
CSR to program the system fwded clock shift register feedback points

SYS_CLK_LD_VECTOR<0:15>
CSR to program the system fwded clock shift register load values.

SYS_BPHASE_LD_VECTOR<0:3>
CSR to program the system fwded clock bhpase enables.

SYS_FRAME_LD_VECTOR<0:4>
CSR to program the ratio between frame clock and system fwd clock.
