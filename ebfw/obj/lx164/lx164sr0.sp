# 0 "srom.s"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/alpha-linux-gnu/include/stdc-predef.h" 1 3
# 0 "<command-line>" 2
# 1 "srom.s"
# 290 "srom.s"
# 1 "../../palcode/lx164/dc21164.h" 1
# 291 "srom.s" 2
# 1 "io.h" 1
# 59 "io.h"
# 1 "ascitab.h" 1
# 60 "io.h" 2
# 292 "srom.s" 2
# 1 "../../h/pyxis.h" 1
# 293 "srom.s" 2


# 1 "/home/janne/ebsdk/ebfw/obj/lx164/rcsv.h" 1
# 296 "srom.s" 2
# 309 "srom.s"
.align 3
# 323 "srom.s"
initIPRs:
 hw_mtpr $31, 0x210
 hw_mtpr $31, 0x216
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
# 360 "srom.s"
 lda $2, ((( (1 << (17 -16)) | (1 << (25 -16)) | (1 << (26 -16)) | (1 << (27 -16)) | (0 << (30 -16)) | (1 << (33 -16)) | (1 << (39 -16)))&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((( (1 << (17 -16)) | (1 << (25 -16)) | (1 << (26 -16)) | (1 << (27 -16)) | (0 << (30 -16)) | (1 << (33 -16)) | (1 << (39 -16)))+0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 sll $2, 16, $2
 hw_mtpr $2, 0x118

 hw_mtpr $31, 0x20D
 lda $1, 0x1($31)
 sll $1, 32, $1
 hw_mtpr $1, 0x20E
 hw_mtpr $31, 0x10F
 hw_mtpr $31, 0x105
 hw_mtpr $31, 0x20A
 hw_mtpr $31, 0x201
 hw_mtpr $31, 0x200
 hw_mtpr $31, 0x103
 hw_mtpr $31, 0x10A
 hw_mtpr $31, 0x109
 hw_mtpr $31, 0x108
# 390 "srom.s"
initGlobals:
 ldah $0, ((0xfff0 ^ 0x8000) - 0x8000)($31)
 zap $0, 0xE0, $13
 lda $14, 0x874($31); sll $14, 28, $14;
 bsr $28, ClearCPUErrors
# 440 "srom.s"
SetScache:



 bsr $28, SCacheSets


 lda $16, ((1<<1) | (1<<12))($31)
 bis $1, $16, $16
 mb
 hw_stq/p $16, 0x0A8($13)
 mb

 hw_ldq/p $2, 0x0E8($13)
 hw_ldq/p $2, 0x168($13)






 lda $16, ((200000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $16, (((200000 +0x8000)>>16 ^ 0x8000) - 0x8000)($16) ;
 bsr $27, wait_n_cycles


 ldah $16, 0x4000($14)
 lda $8, 0x900($16)
 lda $17, ((0xDEAD&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((0xDEAD +0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 hw_stl/p $17, 0($8)
 mb

 lda $16, ((2000000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $16, (((2000000 +0x8000)>>16 ^ 0x8000) - 0x8000)($16) ;
 bsr $27, wait_n_cycles






 lda $17, ((0xAE170027&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((0xAE170027 +0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 ldah $16, 0x1000($14)
 lda $16, 0x500($16)
 hw_stl/p $17, 0($16)
 mb

 ldah $16, 0x1000($14)
 lda $8, 0x40($16)
 lda $17, ((0x10ff&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((0x10ff +0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 hw_stl/p $17, 0($8)
 mb







InitPCI:

 bis $31, $31, $17
 lda $16, 0x876($31); sll $16, 28, $16;
 lda $16, 0x0400($16)
 lda $7, 4($31)

wbase_loop:
 bsr $26, srom_wrtest
 lda $16, 0x100($16)
 lda $7, -1($7)
 bgt $7, wbase_loop

 lda $16, 0x2000($14)
 bsr $26, srom_wrtest
 lda $16, 0x440($14)
 bsr $26, srom_wrtest

 lda $16, 0xC0($14)
 lda $17, ((0xFF00&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((0xFF00 +0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 bsr $26, srom_wrtest
# 539 "srom.s"
 lda $16, 0x100($14)
 lda $17, ((((1 << 0) | (0 << 2) | (0 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (0 << 8) | (0 << 9) | (0 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (0 << 20) | (0 << 24) | (0 << 28) )&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((((1 << 0) | (0 << 2) | (0 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (0 << 8) | (0 << 9) | (0 << 10) | (0 << 11) | (0 << 12) | (0 << 13) | (0 << 20) | (0 << 24) | (0 << 28) )+0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 bsr $26, srom_wrtest






 lda $16, 0x140($14)
 bis $31, (1 << 0), $17
 bsr $26, srom_wrtest

 lda $16, 0x200($14)
 bis $31, $31, $17
 bsr $26, srom_wrtest
# 563 "srom.s"
 ldah $8, 0x6000($14)
 lda $16, 0x140($8)
 bis $31, ((1 << 0) | (1 << 1) | (1 << 2)), $17


 bsr $26, InitSIO
 bis $31, 0x20, $2 ; bsr $29, WriteLEDS ;







# 1 "../../h/smc.h" 1
# 578 "srom.s" 2
InitSMC:
 lda $16, 0x55($31)
 bsr $29, WriteSMC_Config1
 bsr $29, WriteSMC_Config1

 lda $16, 0x07($31)
 bsr $29, WriteSMC_Index1
 lda $16, 6($31)
 bsr $29, WriteSMC_Data1

 lda $16, 0x70($31)
 bsr $29, WriteSMC_Index1
 lda $16, 8($31)
 bsr $29, WriteSMC_Data1

 lda $16, 0x30($31)
 bsr $29, WriteSMC_Index1
 lda $16, 0x01($31)
 bsr $29, WriteSMC_Data1
 lda $16, 0xAA($31)
 bsr $29, WriteSMC_Config1
# 607 "srom.s"
DetectSpeed:
# 621 "srom.s"
 bsr $28, GetCPUSpeed
 bis $0, $0, $17
 lda $16, 9600($31)
 bsr $26, nintdivide


 lda $29, 5($31); bis $31, $0, $28; bsr $0, minidebugger ;


 lda $29, 1($31); lda $28, ((( (0x41 << 24) | (0x34 << 16) | (0x36 << 8) | (0x31 << 0) )&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $28, (((( (0x41 << 24) | (0x34 << 16) | (0x36 << 8) | (0x31 << 0) )+0x8000)>>16 ^ 0x8000) - 0x8000)($28) ;; sll $28, 32, $28; lda $28, ((( (0x31 << 24) | (0x32 << 16) | (0x0d << 8) | (0x0a << 0) )&0xffff ^ 0x8000) - 0x8000)($28); ldah $28, (((( (0x31 << 24) | (0x32 << 16) | (0x0d << 8) | (0x0a << 0) )+0x8000)>>16 ^ 0x8000) - 0x8000)($28);; bsr $0, minidebugger ;
 bis $31, 0x01, $2 ; bsr $29, WriteLEDS ;




 bis $17, $17, $16
 lda $3, ((0xe8d4a510&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $3, (((0xe8d4a510 +0x8000)>>16 ^ 0x8000) - 0x8000)($3) ;
 zap $3, 0xf0, $17
 sll $17, 8, $17
 bsr $26, nintdivide
 bis $0, $0, $24
  bis $31, 0x02, $2 ; bsr $29, WriteLEDS ;


 lda $29, 6($31); bsr $0, minidebugger ;
# 778 "srom.s"
ReadConfigJumpers:
# 830 "srom.s"
 bsr $29, ReadConf1
 ornot $31, $0, $1
 and $1, 0x80, $30
 and $0, 0x40, $1
 bis $30, $1, $30
 bis $30, 0x20, $30
 sll $30, 17, $30
 bis $31, $31, $3
 and $0, 6, $1
 cmpeq $1, 4, $2
 cmovne $2, 2, $3
 cmpeq $1, 6, $2
 cmovne $2, 3, $3
 cmpeq $1, 2, $2
 cmovne $2, 4, $3
 sll $3, 16, $3
 bis $3, $30, $30



 bis $31, 0x03, $2 ; bsr $29, WriteLEDS ;
# 884 "srom.s"
 bsr $29, ReadConf1
 bis $0, $0, $6
 lda $4, ((0x0C400&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $4, (((0x0C400 +0x8000)>>16 ^ 0x8000) - 0x8000)($4) ;
 lda $5, 6($31)
 lda $3, ((0x04406&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $3, (((0x04406 +0x8000)>>16 ^ 0x8000) - 0x8000)($3) ;
 lda $21, ((0x3F24690&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $21, (((0x3F24690 +0x8000)>>16 ^ 0x8000) - 0x8000)($21) ;
 bis $4, $4, $8


 lda $3, 7($31)
 lda $0, 2189($31)
 lda $1, ((0x3F346A0&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0x3F346A0 +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 cmpult $24, $0, $0
 cmovne $0, $1, $21
 cmovne $0, $4, $8
 cmovne $0, $3, $5

 lda $4, ((0x0C600&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $4, (((0x0C600 +0x8000)>>16 ^ 0x8000) - 0x8000)($4) ;

 lda $3, 8($31)
 lda $0, 1914($31)
 lda $1, ((0x7F457C0&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0x7F457C0 +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 cmpult $24, $0, $0
 cmovne $0, $1, $21
 cmovne $0, $4, $8
 cmovne $0, $3, $5

 lda $3, 9($31)
 lda $0, 1700($31)
 lda $1, ((0x7F457D0&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0x7F457D0 +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 cmpult $24, $0, $0
 cmovne $0, $1, $21
 cmovne $0, $3, $5

 lda $3, 10($31)
 lda $0, 1531($31)
 lda $1, ((0xFF458F0&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0xFF458F0 +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 cmpult $24, $0, $0
 cmovne $0, $1, $21
 cmovne $0, $3, $5
SkipCpuSpeeds:
 blbc $6, CpuSpeedOK
 lda $3, ((0x3F34690&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $3, (((0x3F34690 +0x8000)>>16 ^ 0x8000) - 0x8000)($3) ;
 cmpeq $21, $3, $3
 bne $3, CpuSpeedError
 lda $3, ((0xFF448F0&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $3, (((0xFF448F0 +0x8000)>>16 ^ 0x8000) - 0x8000)($3) ;
 cmpeq $21, $3, $3
 beq $3, CpuSpeedOK

CpuSpeedError:
 lda $8, 0x04($31)
 br $31, FatalError

CpuSpeedOK:
# 956 "srom.s"
set_bc_size:
 srl $30, 16, $2

 and $2, ((((1<<3)-1) << 16) >> 16), $2
 subq $2, 1, $3
 cmoveq $2, 0, $3
 sll $3, 0, $3
 bis $21, $3, $21
 bne $2, bc_enabled

bc_disabled:
 bsr $28, BCnfgOff
 bis $0, $0, $21
bc_enabled:


 zap $21, 0xf0, $21
 bis $31, 0x04, $2 ; bsr $29, WriteLEDS ;
# 992 "srom.s"
 lda $20, ((((1<<15) | (1<<6) | (1<<4))&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $20, (((((1<<15) | (1<<6) | (1<<4))+0x8000)>>16 ^ 0x8000) - 0x8000)($20) ;
 zap $20, 0xF0, $20

 srl $30, 16, $0

 and $0, ((((1<<3)-1) << 16) >> 16), $0
 lda $3, (1<<0)($31)
 cmoveq $0, $31, $3
 bis $20, $3, $20


 sll $8, 16, $8
 bis $20, $8, $20



 bis $31, 0x05, $2 ; bsr $29, WriteLEDS ;


 lda $29, 6($31); bsr $0, minidebugger ;
# 1026 "srom.s"
 bsr $28, BCnfgOff
 bis $0, $0, $22

 bis $31, 0x06, $2 ; bsr $29, WriteLEDS ;


 lda $29, 6($31); bsr $0, minidebugger ;
# 1058 "srom.s"
 lda $14, 0x874($31); sll $14, 28, $14;
 ldah $8, 0x4000($14)
 lda $16, 0x000($8)
 lda $17, ((((1 << 0) | (3 << 4) | (2 << 8) | (1 << 10) | (0 << 12) | (0 << 16) | (0 << 17) | (1 << 18) | (0x29 << 24) )&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((((1 << 0) | (3 << 4) | (2 << 8) | (1 << 10) | (0 << 12) | (0 << 16) | (0 << 17) | (1 << 18) | (0x29 << 24) )+0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 bsr $26, srom_wrtest

 lda $16, 20000($31)
 bsr $27, wait_n_cycles


 lda $17, ((0x332&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((0x332 +0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 ldah $8, 0x1000($14)
 lda $16, 0x200($8)
 bsr $26, srom_wrtest

 lda $17, ((0x750&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((0x750 +0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 lda $16, 0x300($8)
 bsr $26, srom_wrtest




 bis $31, 1, $4
 sll $4, 15, $4
 bis $17, $4, $17



 lda $5, 8($31)
ForceRef:
 lda $16, 0x300($8)
 bsr $26, srom_wrtest

 lda $4, 100($31)
ForceRefWait:
 hw_mfpr $31, 0x140
 subq $4, 1, $4
 bne $4, ForceRefWait

 subq $5, 1, $5
 bne $5, ForceRef

 bis $17, $31, $5







 ldah $16, 0x1000($14)
 lda $17, ((0x3A1401&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $17, (((0x3A1401 +0x8000)>>16 ^ 0x8000) - 0x8000)($17) ;
 bsr $26, srom_wrtest




 bis $5, $31, $17
 lda $5, 8($31)
ForceRef1:
 lda $16, 0x300($8)
 bsr $26, srom_wrtest

 lda $4, 100($31)
ForceRefWait1:
 hw_mfpr $31, 0x140
 subq $4, 1, $4
 bne $4, ForceRefWait1

 subq $5, 1, $5
 bne $5, ForceRef1




 bis $31, $31, $17
 lda $16, 0x600($8)
 bsr $26, srom_wrtest

 lda $17, 0x100($31)
 lda $16, 0x640($8)
 bsr $26, srom_wrtest

 lda $17, 0x22($31)
 lda $16, 0xA00($8)
 bsr $26, srom_wrtest

 lda $16, 0xA40($8)
 bsr $26, srom_wrtest

 lda $17, 0x28($31)
 lda $16, 0x800($8)
 bsr $26, srom_wrtest

 lda $16, 0x840($8)
 bsr $26, srom_wrtest

 bis $17, 1, $17
 lda $16, 0x800($8)
 bsr $26, srom_wrtest

 lda $16, 0x840($8)
 bsr $26, srom_wrtest

 lda $23, ((0x8000000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $23, (((0x8000000 +0x8000)>>16 ^ 0x8000) - 0x8000)($23) ;

 bis $31, 0x07, $2 ; bsr $29, WriteLEDS ;


 lda $29, 6($31); bsr $0, minidebugger ;
# 2248 "srom.s"
 bis $31, 0x0C, $2 ; bsr $29, WriteLEDS ;


 lda $29, 6($31); bsr $0, minidebugger ;
# 2270 "srom.s"
 lda $2, ((1<<0))($31)
 hw_mtpr $2, 0x216
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
 hw_mfpr $31, 0x140
# 2317 "srom.s"
 bis $21, $21, $18
 bis $20, $20, $19





 lda $2, ((((1<<7) | (1<<15) | (1<<6) | (5<<8))&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((((1<<7) | (1<<15) | (1<<6) | (5<<8))+0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;




 bis $19, $2, $19
 bsr $28, BCacheOnOff
 bis $31, 0x0F, $2 ; bsr $29, WriteLEDS ;


 lda $29, 6($31); bsr $0, minidebugger ;
# 2357 "srom.s"
 bis $31, 2, $5
SweepMemory:
 bis $23, $23, $7
SweepMemory1:
 subq $7, 8, $7
 hw_ldq/p $2, 0x00($7)
 hw_stq/p $2, 0x00($7)
 bgt $7, SweepMemory1

 subq $5, 1, $5
 bne $5, SweepMemory

 bis $21, $21, $18
 bis $20, $20, $19
 bsr $28, BCacheOnOff

 bis $31, 0x13, $2 ; bsr $29, WriteLEDS ;







 bsr $28, ClearMemContErrors
 bsr $28, ClearCPUErrors

 bis $23, $23, $7
SweepMemory2:
 subq $7, 8, $7
 hw_ldq/p $2, 0x00($7)
 hw_stq/p $2, 0x00($7)
 bgt $7, SweepMemory2

 bis $31, 0x14, $2 ; bsr $29, WriteLEDS ;


 lda $29, 6($31); bsr $0, minidebugger ;






 bis $30, $30, $16
 bsr $14, LoadSystemCode
 bis $31, 0x17, $2 ; bsr $29, WriteLEDS ;
# 2421 "srom.s"
 bsr $28, SCacheSets


 lda $2, ((1<<12))($31)
 bis $0, $2, $2
 mb
 hw_stq/p $2, 0x0A8($13)
 mb
# 2472 "srom.s"
 lda $2, 0x4000($31)
 cmpult $8, $2, $0
 cmovne $0, $2, $8

 bis $8, 1, $0
 hw_mtpr $0, 0x10B

 lda $2, ((0x77FF0119&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((0x77FF0119 +0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 hw_stl/p $2, 0($8)

 lda $2, ((0xC0000001&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((0xC0000001 +0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 hw_stl/p $2, 4($8)
 hw_stl/p $25, 8($8)
 lda $2, ((0x6C008000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((0x6C008000 +0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 hw_stl/p $2, 12($8)

 lda $2, ((0x47FF041F&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((0x47FF041F +0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 lda $5, 44($31)
 lda $8, 16($8)
loadInst:
 hw_stl/p $2, 0($8)
 subq $5, 1, $5
 lda $8, 4($8)
 bne $5, loadInst

 lda $2, ((0x6BE00000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((0x6BE00000 +0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 hw_stl/p $2, 0($8)
 mb

 bis $31, 0x18, $2 ; bsr $29, WriteLEDS ;


 bsr $28, ClearMemContErrors
 bsr $28, ClearCPUErrors

 srl $0, 24, $2
 and $2, 0xF, $2



 lda $1, ((0x00000000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0x00000000 +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;; sll $1, 32, $1; lda $1, ((0x00000100&0xffff ^ 0x8000) - 0x8000)($1); ldah $1, (((0x00000100 +0x8000)>>16 ^ 0x8000) - 0x8000)($1);
 extbl $1, $2, $1

EV56_Detected:
 sll $1, 32, $1
 bis $1, 7, $25
# 2551 "srom.s"
NextLevel:
 lda $2, 0x19($31)
 srl $30, 23, $7
 cmovlbc $7, 0x3A, $2
 bis $31, $2, $2 ; bsr $29, WriteLEDS ;

 bis $20, $20, $1
 bis $21, $21, $2
 bis $22, $22, $3
 lda $15, ((0x56000000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $15, (((0x56000000 +0x8000)>>16 ^ 0x8000) - 0x8000)($15) ;; sll $15, 32, $15; lda $15, ((0x01010101&0xffff ^ 0x8000) - 0x8000)($15); ldah $15, (((0x01010101 +0x8000)>>16 ^ 0x8000) - 0x8000)($15);
 bis $25, $25, $16
 bis $23, $23, $17
 bis $24, $24, $18
 lda $19, ((0xDECB0001&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $19, (((0xDECB0001 +0x8000)>>16 ^ 0x8000) - 0x8000)($19) ;
 zap $19, 0xf0, $19
 lda $20, 1($31)
 lda $21, 0($31)

 blbs $7, no_minidbg
 lda $29, 0($31); bsr $0, minidebugger ;
no_minidbg:

 hw_rei
# 2608 "srom.s"
SCacheSets:

 lda $1, ((((7 << 13) | (1<<12))&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((((7 << 13) | (1<<12))+0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 mb
 hw_stq/p $1, 0x0A8($13)
 mb
 hw_ldq/p $2, 0x0A8($13)

 srl $2, 13, $2
 and $2, 7, $0

 lda $1, (((1 << 13)&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, ((((1 << 13)+0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 cmplt $0, 7, $2
 beq $2, AllSetsOn

OneSetLoop:
 and $0, $2, $1
 sll $2, 1, $2
 beq $1, OneSetLoop
  sll $1, 13, $1
AllSetsOn:
  sll $0, 13, $0
 ret $31, ($28)
# 2661 "srom.s"
BCnfgOff:
 sll $5, 4, $1
 bis $5, $1, $0
 sll $0, 4, $0
 lda $1, 0x0FFF($31)
 bic $21, $1, $1
 bis $1, $0, $0
 ret $31, ($28)
# 2695 "srom.s"
BCacheOnOff:



 ldah $1, 0x1000($14)
 hw_ldl/p $0, ((0x000 ^ 0x200) - 0x200)($1)
 lda $2, ((((1 << 10) | (1 << 12))&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, (((((1 << 10) | (1 << 12))+0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 bic $0, $2, $0
 blbc $19, mcr_bc_off
 bis $0, $2, $0
mcr_bc_off:
 ldah $1, 0x1000($14)
 hw_stl/p $0, ((0x000 ^ 0x200) - 0x200)($1)
 mb
 hw_ldl/p $0, ((0x000 ^ 0x200) - 0x200)($1)

 hw_ldl/p $0, ((0x200 ^ 0x200) - 0x200)($1)
 lda $2, (((7 << 8)&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, ((((7 << 8)+0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 bic $0, $2, $0
 blbc $19, gtr_bc_off
 lda $2, (((3<<8)&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $2, ((((3<<8)+0x8000)>>16 ^ 0x8000) - 0x8000)($2) ;
 bis $0, $2, $0
gtr_bc_off:
 hw_stl/p $0, ((0x200 ^ 0x200) - 0x200)($1)
 mb
 hw_ldl/p $0, ((0x200 ^ 0x200) - 0x200)($1)

 mb
 hw_stq/p $19, 0x128($13)
 mb
 hw_stq/p $18, 0x1C8($13)
 mb

 ret $31, ($28)
# 3464 "srom.s"
ClearCPUErrors:
 hw_mtpr $31, 0x10C

 hw_mfpr $31, 0x206


 lda $2, ((1<<11) | (1<<12) | (1<<13))($31)
 hw_mtpr $2, 0x11A


 lda $2, ((1<<1) | (1<<0))($31)
 hw_mtpr $2, 0x212





 ldah $2, ((( (1<<27) | (1<<28) | (1<<29) | (1<<32) | (1<<33)) >> 16 ) >> 16)($31)
 sll $2, 16, $2
 ldah $2, ((( (1<<27) | (1<<28) | (1<<29) | (1<<32) | (1<<33)) >> 16 ) & 0xffff)($2)

 hw_mtpr $2, 0x115

 hw_ldq/p $31, 0x0E8($13)
 hw_ldq/p $0, 0x168($13)
 ret $31, ($28)
# 3510 "srom.s"
ClearMemContErrors:
 mb
 lda $14, 0x874($31); sll $14, 28, $14;
 lda $2, 0x4000($14)
 lda $2, 0x4200($2)
 hw_ldl/p $1, 0x0($2)
 hw_stl/p $1, 0x0($2)
 mb
 hw_ldl/p $1, 0x0($2)
 ret $31, ($28)
# 3552 "srom.s"
InitSIO:
# 3561 "srom.s"
 lda $16, 0x8($31)
 bsr $29, WriteSIO_ICD

 lda $16, 1000($31)
 bsr $27, wait_n_cycles

 lda $16, 0x40($31)
 bsr $29, WriteSIO_ICD
# 3582 "srom.s"
 lda $16, ((1 << 6) | (1 << 7))($31)
 bsr $29, WriteSIO_UBCSA

 lda $16, ((1<<7) | (1<<6) | (3<<4)| (3<<2) | (3<<0))($31)
 bsr $29, WriteSIO_UBCSB

 lda $16, 0xC($31)
 bsr $29, WriteSIO_PAC
 lda $16, 0xF0($31)
 bsr $29, WriteSIO_PAPC

 ret $31, ($26)
# 3621 "srom.s"
wait_n_cycles:
 lda $1, 1($31)
 sll $1, 32, $1
 rpcc $3
time_loop:
 rpcc $0
 subl $0, $3, $0
 bge $0, nowrap
 addq $1, $0, $0
nowrap:
 cmpult $0, $16, $2
 bne $2, time_loop
 ret $31, ($27)
# 3658 "srom.s"
WriteSIO_ICD:
 lda $1, 0x4D($31)
 br $31, WriteConf

WriteSIO_PAC:
 lda $1, 0x41($31)
 br $31, WriteConf
WriteSIO_PAPC:
 lda $1, 0x42($31)
 br $31, WriteConf

WriteSIO_UBCSA:
 lda $1, 0x4E($31)
 br $31, WriteConf
WriteSIO_UBCSB:
 lda $1, 0x4F($31)

WriteConf:
 and $1, 3, $3
 insbl $16, $3, $2
 sll $1, 5, $1
 lda $3, 0x870($31); sll $3, 28, $3;
 ldah $16, (19-11)($31)
 bis $3, $16, $3
 br $31, WriteData

WriteLEDS:
 cmpeq $2, 0x20, $1
 bne $1, skipSromPort

 bis $29, $29, $1
 lda $29, 3($31); lda $28, 0x2e($31); bsr $0, minidebugger ;
 lda $29, 7($31); bis $31, $2, $28; bsr $0, minidebugger ;
 bis $1, $1, $29
skipSromPort:
 lda $1, 0x80($31)
 br $31, WriteCont
WriteFBank:
 lda $1, 0x800($31)
 br $31, WriteCont

WriteSMC_Config1:
WriteSMC_Index1:
 lda $1, 0x370($31)
 br $31, WriteCont_std
WriteSMC_Data1:
 lda $1, 0x371($31)
 br $31, WriteCont_std

WriteRTCAddr:
 lda $1, 0x70($31)
 br $31, WriteCont_std
WriteRTCData:
 lda $1, 0x71($31)
 br $31, WriteCont_std
WriteSIO_NMISC:
 lda $1, 0x61($31)
 br $31, WriteCont_std
WriteSIO_TMR2:
 lda $1, 0x42($31)
 br $31, WriteCont_std
WriteSIO_TCW:
 lda $1, 0x43($31)

WriteCont_std:
 bis $16, $16, $2
WriteCont:

 and $1, 3, $3
 insbl $2, $3, $2
 sll $1, 5, $1
 lda $3, 0x858($31); sll $3, 28, $3;

WriteData:
 bis $1, $3, $1
 hw_stl/p $2, 0x00($1)
 mb
 ret $31, ($29)
# 3762 "srom.s"
ReadRTCData:
 lda $1, 0x71($31)
 br $31, ReadCont
ReadConf2:
 lda $1, 0x802($31)
 br $31, ReadCont
ReadConf1:
 lda $1, 0x801($31)

ReadCont:
 sll $1, 5, $2
 lda $3, 0x858($31); sll $3, 28, $3;
ReadData:
 bis $2, $3, $2
 hw_ldl/p $0, 0x00($2)
 and $1, 3, $1
 extbl $0, $1, $0
 ret $31, ($29)
# 3819 "srom.s"
intdivide:
 bis $31, $31, $0
 cmplt $17, $16, $1
 bne $1, divide_done
divide:
 lda $1, 1($31)
divide1:
 addq $0, $1, $4
 mulq $4, $16, $2
 sll $1, 1, $1
 cmplt $2, $17, $3
 bne $3, divide1
 srl $1, 2, $1
 addq $0, $1, $0
 cmplt $1, 2, $3
 beq $3, divide
divide_done:
 ret $31, ($27)
# 3851 "srom.s"
updivide:
 bsr $27, intdivide
 mulq $16, $0, $1
 cmplt $1, $17, $2
 blbc $2, updivide_done
 addq $0, 1, $0
updivide_done:
 ret $31, ($26)
# 3875 "srom.s"
nintdivide:
 bsr $27, intdivide
 mulq $16, $0, $1
 cmplt $1, $17, $2
 blbc $2, nintdivide_done
 addq $0, 1, $0
 mulq $16, $0, $2
 subq $2, $17, $2
 subq $17, $1, $3
 cmplt $3, $2, $1
 blbc $1, nintdivide_done
 subq $0, 1, $0
nintdivide_done:
 ret $31, ($26)
# 3911 "srom.s"
srom_wrtest:
 zap $17, 0xF0, $17
 hw_stl/p $17, 0($16)
 mb
srom_rdtest:
 hw_ldl/p $18, 0($16)
 zap $18, 0xF0, $18
 xor $18, $17, $19
# 3938 "srom.s"
 ret $31, ($26)
# 3967 "srom.s"
FatalError:
 bis $31, 0x3F, $2 ; bsr $29, WriteLEDS ;
 lda $17, 500($31)
 bsr $27, wait_for_pif
 bis $31, $8, $2 ; bsr $29, WriteLEDS ;
 bis $8, $8, $19
 bsr $28, Beep
 lda $17, 1000($31)
 bsr $27, wait_for_pif

 srl $30, 23, $1
 blbs $1, FatalError

 lda $29, 0($31); bsr $0, minidebugger ;
 br $31, FatalError
# 4005 "srom.s"
Beep:
 lda $16, 3($31)
 bsr $29, WriteSIO_NMISC
 lda $16, 0xB6($31)
 bsr $29, WriteSIO_TCW
 lda $16, 0xE2($31)

 bsr $29, WriteSIO_TMR2
 lda $16, 3($31)
 bsr $29, WriteSIO_TMR2

 lda $17, 100($31)
 bsr $27, wait_for_pif

 lda $16, 0xB2($31)
 bsr $29, WriteSIO_TCW
 lda $16, 0($31)
 bsr $29, WriteSIO_TMR2
 bsr $29, WriteSIO_TMR2
 bsr $29, WriteSIO_NMISC

 lda $17, 100($31)
 bsr $27, wait_for_pif
 subq $19, 1, $19
 bne $19, Beep
 ret $31, ($28)
# 4061 "srom.s"
GetCPUSpeed:
# 4075 "srom.s"
 lda $16, 0xA($31)
 bsr $29, WriteRTCAddr
 lda $16, ((2 << 4) | (7 << 0))($31)
 bsr $29, WriteRTCData






 lda $16, 0xB($31)
 bsr $29, WriteRTCAddr

 lda $16, ((1 << 6) | (1 << 2) | (1 << 1))($31)
 bsr $29, WriteRTCData





 lda $17, 10($31)
 bsr $27, wait_for_pif






 lda $17, 64($31)
 rpcc $4
 bsr $27, wait_for_pif
 rpcc $0

 subl $0, $4, $0
 bge $0, noshft
 bis $31, 1, $1
 sll $1, 32, $1
 addq $1, $0, $0

noshft:

 sll $0, 3, $0


 ret $31, ($28)
# 4142 "srom.s"
wait_for_pif:
 lda $16, 0xC($31)
 bsr $29, WriteRTCAddr
 bsr $29, ReadRTCData
 srl $0, 6, $0
 blbs $0, wait_for_pif

wait_assert:
 bsr $29, WriteRTCAddr
 bsr $29, ReadRTCData
 srl $0, 6, $0
 blbc $0, wait_assert
 subq $17, 1, $17
 bne $17, wait_for_pif
 ret $31, ($27)
# 4420 "srom.s"
LoadSystemCode:

 lda $10, ((1 << 8) | 4)($31)
 lda $1, ((2 << 8) | 4)($31)
 srl $16, 24, $0
 cmovlbc $0, $1, $10

scanROM:
 bis $31, $31, $12
 bis $31, $31, $6

rescanROM:
 bis $6, $6, $17
 bis $31, $31, $7
 bic $10, 1, $10

 lda $0, ((0x100000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $0, (((0x100000 +0x8000)>>16 ^ 0x8000) - 0x8000)($0) ;
 cmpult $17, $0, $0
 blbs $0, ValidationPattern







DoneWithROM:







 and $10, 4, $0
 beq $0, NoHeader






 and $10, 8, $0
 lda $10, ((1 << 8) | 8) ($31)
 bne $0, scanROM
 bis $10, 4, $10
 br $31, scanROM

ValidationPattern:
 bsr $28, read_flash

 bis $17, $17, $6
 lda $1, ((0x5a5ac3c3&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0x5a5ac3c3 +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 cmpeq $0, $1, $1
 bne $1, InversePattern
 lda $1, ((0xc3e0&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0xc3e0 +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 extwl $0, 2, $0
 cmpeq $0, $1, $0
 beq $0, rescanROM

InversePattern:
 bsr $28, read_flash

 lda $1, ((0xa5a53c3c&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $1, (((0xa5a53c3c +0x8000)>>16 ^ 0x8000) - 0x8000)($1) ;
 zap $1, 0xF0, $1
 cmpeq $0, $1, $0
 beq $0, rescanROM

HeaderSize:
 bsr $28, read_flash

 cmplt $0, 0x34, $1
 bis $10, $1, $10
 blbs $10, HeaderIsValid

 subq $0, 0x10, $5
 srl $5, 2, $5

HeaderCheckSum:
 bsr $28, read_flash
 subq $5, 1, $5
 bne $5, HeaderCheckSum

 bis $7, $7, $5
 bsr $28, read_flash
 addq $6, (0xC-0x4), $17

 srl $10, 2, $1
 blbc $1, HeaderIsValid
 cmpeq $0, $5, $0
 beq $0, rescanROM

HeaderIsValid:
 addq $12, 1, $12

 bsr $28, read_flash
 bis $0, $0, $11
 bsr $28, read_flash
 bis $0, $0, $9
 bsr $28, read_flash
 bis $0, $0, $5
 bsr $28, read_flash
 bis $0, $0, $8
 bsr $28, read_flash
 sll $0, 32, $0
 bis $8, $0, $8

 srl $10, 1, $1
 blbc $1, PositionDependent

FirmwareID:

 blbs $10, rescanROM
 addq $6, (0x20-0x4), $17
 bsr $28, read_flash
 extbl $0, 1, $1

PositionDependent:
 srl $10, 1, $2
 cmovlbc $2, $12, $1
 extbl $10, 1, $0
 cmpeq $1, $0, $0
 beq $0, rescanROM

 addq $6, (0x24-0x4), $17
 bsr $28, read_flash
 cmovlbc $10, $0, $9

 addq $6, (0x8-0x4), $17
 bsr $28, read_flash
 subq $6, 0x4, $17
 addq $0, $17, $17
 addq $17, $9, $9
 br $31, headerEnd

NoHeader:
 lda $19, 0x4($31)
 bsr $28, Beep

  bis $31, $31, $8
 bis $31, $31, $5
 bis $31, $31, $17
 lda $9, ((0x100000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $9, (((0x100000 +0x8000)>>16 ^ 0x8000) - 0x8000)($9) ;
 bic $10, 4, $10

headerEnd:
 bis $8, $8, $25
 bis $31, $31, $7
 blbs $5, LoadComp
# 4584 "srom.s"
 bis $31, 0x15, $2 ; bsr $29, WriteLEDS ;
LoadUncomp:
 bsr $28, read_flash

 hw_stl/p $0, 0($8)
 addq $8, 4, $8
 cmpult $17, $9, $0
 bne $0, LoadUncomp


 br $31, ImageCheckSums

LoadComp:
 bis $31, 0x16, $2 ; bsr $29, WriteLEDS ;
 bis $31, 4, $18
  bsr $26, getRecord

LoadCInit:
 bis $31, $31, $27
 bis $31, $31, $5

LoadCLoop:
 bsr $26, getByte
 insbl $16, $5, $0
 bis $0, $27, $27
 addq $5, 1, $5
 cmpeq $5, 4, $0
 beq $0, LoadCLoop

 hw_stl/p $27, 0($8)
 addq $8, 4, $8
 bne $19, LoadCInit

DecompressionDone:
 lda $5, 1($31)

ImageCheckSums:
 blbs $10, codeLoaded
 srl $10, 2, $0
 blbc $0, codeLoaded
 cmpeq $7, $11, $1
 beq $1, SumMismatch


 blbs $5, codeLoaded

 ldah $0, 0x100($31)
flush:
 hw_ldq/p $31, 0x0($0)
 subq $0, 0x20, $0
 bge $0, flush

 bis $31, $31, $7
 bis $25, $25, $17
secondcheck:
 hw_ldl/p $0, 0x0($17)
 bsr $28, ComputeCheckSum
 addq $17, 4, $17
 cmpult $17, $8, $1
 bne $1, secondcheck
 cmpeq $7, $11, $1
 bne $1, codeLoaded

 lda $8, 0x06($31)
 br $31, FatalError

SumMismatch:
 subq $12, 1, $12
 br $31, rescanROM
codeLoaded:
 ret $31, ($14)
# 4684 "srom.s"
read_flash:
 ldah $0, (0x100000>>16)($31)
 cmpult $17, $0, $0
 cmoveq $0, $17, $6
 beq $0, rescanROM

 ldah $0, (0x80000>>16)($31)
 cmple $0, $17, $2
 sll $2, 19, $3
 subq $17, $3, $4

 bsr $29, WriteFBank

 ldah $0, ((0xFFF80000>>16 ^ 0x8000) - 0x8000)($31)
 zap $0, 0xF0, $0
 addq $4, $0, $4

 lda $0, 0x860($31); sll $0, 28, $0;
 addq $0, $4, $4
 hw_ldl/p $0, 0($4)
 zap $0, 0xF0, $0
 addq $17, 4, $17
# 4733 "srom.s"
ComputeCheckSum:
 lda $2, 0($31)
CheckSumLoop:
 srl $7, 1, $3
 lda $4, ((0x8000&0xffff ^ 0x8000) - 0x8000)($31) ; ldah $4, (((0x8000 +0x8000)>>16 ^ 0x8000) - 0x8000)($4) ;
 addq $3, $4, $4
 cmovlbs $7, $4, $3
 extbl $0, $2, $4
 addq $3, $4, $7
 zapnot $7, 0x03, $7
 addq $2, 1, $2
 cmpeq $2, 4, $4
 beq $4, CheckSumLoop
 ret $31, ($28)
# 4785 "srom.s"
getByte:
 blt $19, repByte
 bsr $28, ReadFByte
 bis $2, $2, $16
 lda $19, -1($19)
 beq $19, getRecord
 ret $31, ($26)

repByte:
 bis $2, $2, $16
 lda $19, 1($19)
 beq $19, getRecord
 ret $31, ($26)

getRecord:
 bsr $28, ReadFByte
 sll $2, 56, $19
 sra $19, 56, $19
 bge $19, norepeat
 bsr $28, ReadFByte

norepeat:
 ret $31, ($26)
# 4839 "srom.s"
ReadFByte:
 cmpeq $18, 4, $0
 beq $0, skipRd

 cmpult $17, $9, $0
 beq $0, DecompressionDone

 bis $31, $31, $18

 bis $28, $28, $15
 bsr $28, read_flash
 bis $15, $15, $28

 bis $0, $0, $15
skipRd:
 extbl $15, $18, $2
 lda $18, 1($18)
 ret $31, ($28)

.align 3
minidebugger:



.end
