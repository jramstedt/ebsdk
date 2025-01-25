# 0 "../sromcsrv.s"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/alpha-linux-gnu/include/stdc-predef.h" 1 3
# 0 "<command-line>" 2
# 1 "../sromcsrv.s"
# 73 "../sromcsrv.s"
# 1 "./macros.h" 1
# 74 "../sromcsrv.s" 2
# 1 "../../h/cpu.h" 1
# 58 "../../h/cpu.h"
# 1 "./dc21164.h" 1
# 59 "../../h/cpu.h" 2
# 75 "../sromcsrv.s" 2
# 1 "../mini_dbg.h" 1
# 76 "../sromcsrv.s" 2

 .global Sys_Cserve_Srom_Init
 .global Sys_Cserve_Srom_Putc
 .global Sys_Cserve_Srom_Getc

 .text 3
# 100 "../sromcsrv.s"
 .align 3

Sys_Cserve_Srom_Init:
 bsr $21, autobaud_sync
 lda $0, (1 << 8)($31)
 bis $23, $0, $23
 bis $23, $31, $0
 hw_rei
# 127 "../sromcsrv.s"
 .align 3

Sys_Cserve_Srom_Putc:
 bis $19, $31, $23
 bsr $22, putChar
 hw_rei
# 152 "../sromcsrv.s"
 .align 3

Sys_Cserve_Srom_Getc:
 bis $19, $31, $23
 bsr $22, getChar
 hw_rei
