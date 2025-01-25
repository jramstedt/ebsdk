# 0 "../srom_dbm.s"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/alpha-linux-gnu/include/stdc-predef.h" 1 3
# 0 "<command-line>" 2
# 1 "../srom_dbm.s"
# 74 "../srom_dbm.s"
# 1 "./macros.h" 1
# 75 "../srom_dbm.s" 2
# 1 "../../h/cpu.h" 1
# 58 "../../h/cpu.h"
# 1 "./dc21164.h" 1
# 59 "../../h/cpu.h" 2
# 76 "../srom_dbm.s" 2
# 1 "../mini_dbg.h" 1
# 77 "../srom_dbm.s" 2



.global keypressed
.global getChar
.global putChar
.global autobaud_sync
# 130 "../srom_dbm.s"
keypressed:

 hw_mfpr $0, 0x100
 srl $0, 33, $0
 and $0, 1, $0
 sll $0, 33, $0
 hw_mtpr $0, 0x115
 cmovne $0, 1, $0






pvc$serial_keypressed_ret$4009.1:
 ret $31, ($20)
# 175 "../srom_dbm.s"
getChar:
 lda $2, (0+8)($31)


char_start:
 hw_mfpr $0, 0x117
 bne $0, char_start

pvc$serial_waitbit_1$4000:
 bsr $20, waitBitHalf

get_bits:
pvc$serial_waitbit_2$4000:
 bsr $20, waitBitTime
 hw_mfpr $0, 0x117
 srl $0, 6, $0
 sll $0, $2, $0
 bis $0, $2, $2
 lda $2, 1($2)
 and $2, 0xFF, $0
 cmpult $0, (8+8), $1
 bne $1, get_bits

stop_get:
pvc$serial_waitbit_3$4000:
 bsr $20, waitBitTime

        hw_mfpr $1, 0x117
pvc$serial_keypressed_1$4009:
 bsr $20, keypressed

 extbl $2, 1, $0
 srl $23, 8, $1
 blbc $1, exit_getchar
 xor $0, 0x58, $1
 bne $1, exit_getchar
 lda $1, (1 << 8)($31)
 bic $23, $1, $23

exit_getchar:
pvc$serial_getchar_ret$4002.1:
 ret $31, ($22)
# 250 "../srom_dbm.s"
putChar:
 srl $23, 8, $2
 blbc $2, exit_putChar


 lda $2, 16($31)

 lda $17, 0x3F00($17)
 s4addq $17, 1, $17
 sll $17, (7), $17

data_bit:
 hw_mtpr $17, 0x116

 srl $17, 1, $17
 subq $2, 1, $2
pvc$serial_waitbit_4$4000:
 bsr $20, waitBitTime
 bne $2, data_bit

exit_putChar:
pvc$serial_putchar_ret$4001.1:
 ret $31, ($22)
# 297 "../srom_dbm.s"
waitBitHalf:
 srl $23, (32+1), $0
 br $31, waitBitCont
waitBitTime:
 srl $23, 32, $0
waitBitCont:

wait_init:
 lda $1, 0x1($31)
 sll $1, 32, $1
 hw_mtpr $1, 0x20E
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
wait:
 rpcc $1
 zapnot $1, 0x0f, $1
 cmplt $0, $1, $1
 beq $1, wait

pvc$serial_waitbit_ret$4000.1:
 ret $31, ($20)
# 353 "../srom_dbm.s"
autobaud_sync:
 lda $1, 1($31)
 sll $1, 32, $1
pvc$serial_keypressed_2$4009:
 bsr $20, keypressed


poll_start:
 hw_mfpr $0, 0x117
     bne $0, poll_start
 hw_mtpr $1, 0x20E


poll_h:
 hw_mfpr $0, 0x117
        beq $0, poll_h
 rpcc $1
 zap $1, 0xF0, $1


poll_l:
 rpcc $0
 zap $0, 0xF0, $0
 sll $1, 3, $2
 cmpult $2, $0, $0
 bne $0, end_sync
 hw_mfpr $0, 0x117
        bne $0, poll_l
 br $31, poll_h

end_sync:
 lda $0, 0($31)
simple_div:
 mulq $0, 9, $2
 cmpult $2, $1, $2
 beq $2, set_baud
 addq $0, 1, $0
 br $31, simple_div

set_baud:
 sll $0, 32, $0
 lda $23, (1 << 8)($0)
pvc$serial_autosync_ret$4010.1:
 ret $31, ($21)
