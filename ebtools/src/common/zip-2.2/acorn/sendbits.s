;
; Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
; Onno van der Linden, Kai Uwe Rommel, Igor Mandrichenko, Sergio Monesi and
; Karl Davis.
; Permission is granted to any individual or institution to use, copy, or
; redistribute this software so long as all of the original files are included,
; that it is not sold for profit, and that this copyright notice is retained.
;
; sendbits.s for ARM by Sergio Monesi.

r0 RN 0
r1 RN 1
r2 RN 2
r3 RN 3
r4 RN 4
r5 RN 5
r6 RN 6
r7 RN 7
r8 RN 8
r9 RN 9
sl RN 10
fp RN 11
ip RN 12
sp RN 13
lr RN 14
pc RN 15

                AREA    |C$$code|, CODE, READONLY

; r1 = ....

|__bi_buf|
        IMPORT  bi_buf
        DCD     bi_buf
|__bi_valid|
        IMPORT  bi_valid
        DCD     bi_valid
|__out_buf|
        IMPORT  out_buf
        DCD     out_buf
|__out_size|
        IMPORT  out_size
        DCD     out_size
|__out_offset|
        IMPORT out_offset
        DCD    out_offset

        IMPORT  flush_outbuf
        IMPORT  |x$stack_overflow|

        DCB     "send_bits"
        DCB     &00,&00,&00
        DCD     &ff00000c

        EXPORT  send_bits
send_bits
        LDR     r2, [pc, #|__bi_valid|-.-8]
        LDR     r2, [r2]
        RSB     r3, r1, #16
        CMP     r2, r3
        BLE     send_part2

        LDR     r3, [pc, #|__bi_buf|-.-8]
        LDR     ip, [r3]
        ORR     ip, ip, r0, LSL r2
        STR     ip, [r3]

        LDR     r2, [pc, #|__out_offset|-.-8]
        LDR     r2, [r2]
        LDR     r3, [pc, #|__out_size|-.-8]
        LDR     r3, [r3]
        SUB     r3, r3, #1
        CMP     r2, r3
        BCS     send_flush

        STMFD   sp!, {r4-r6,lr}

        MOV     r6, #1

        LDR     r4, [pc, #|__out_buf|-.-8]
        LDR     r4, [r4]

        AND     r5, ip, #&FF
        STRB    r5, [r4, r2]
        ADD     r2, r2, #1
        MOV     r5, ip, LSR #8
        STRB    r5, [r4, r2]
        ADD     r2, r2, #1
        LDR     r4, [pc, #|__out_offset|-.-8]
        STR     r2, [r4]
        B       send_cont

send_flush
        MOV     r3, ip
        MOV     ip, sp
        STMFD   sp!, {r4-r6,fp,ip,lr,pc}
        SUB     fp, ip, #4
        CMPS    sp, sl
        BLLT    |x$stack_overflow|
        MOV     r4, r0
        MOV     r5, r1
        MOV     r0, r3
        MOV     r1, #2
        BL      flush_outbuf
        MOV     r0, r4
        MOV     r1, r5

        MOV     r6, #0

send_cont
        LDR     r2, [pc, #|__bi_valid|-.-8]
        LDR     r3, [r2]
        RSB     r3, r3, #16
        LDR     r4, [pc, #|__bi_buf|-.-8]
        MOV     r5, r0, LSR r3
        STR     r5, [r4]
        LDR     r3, [r2]
        ADD     r3, r3, r1
        SUB     r3, r3, #16
        STR     r3, [r2]

        CMP     r6, #0
        LDMEQEA fp, {r4-r6,fp,sp,pc}^
        LDMNEFD sp!, {r4-r6,pc}^

send_part2
        LDR     r3, [pc, #|__bi_buf|-.-8]
        LDR     ip, [r3]
        ORR     ip, ip, r0, LSL r2
        STR     ip, [r3]

        LDR     r3, [pc, #|__bi_valid|-.-8]
        ADD     r2, r2, r1
        STR     r2, [r3]

        MOVS    pc, lr


        DCB     "bi_reverse"
        DCB     &00,&00
        DCD     &ff00000c

        EXPORT  bi_reverse
bi_reverse
        MOV     r2, #0
rev_cycle
        MOVS    r0, r0, LSR #1
        ADCS    r2, r2, r2
        SUBS    r1, r1, #1
        BNE     rev_cycle
        MOV     r0, r2
        MOVS    pc, lr

        END

