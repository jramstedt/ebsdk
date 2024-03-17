#if defined(DP264)
        .globl srom_write_lcd
        .set noat
        .text
        .align  4
        .ent srom_write_lcd
 #
 #
 #       enter with $16 the work area
 #       enter with $17 the randomed start point
srom_write_lcd:
	bis     $31, $16, $18
	bis     $31, $26, $7
	bis     $31, 0x80, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	and     $18, 0xff, $16
	bis     $31, 0x1, $17
	bsr	$26, srom_access_lcd
	srl	$18, 8, $18
	and     $18, 0xff, $16
	bis     $31, 0x1, $17
	bsr	$26, srom_access_lcd
	srl	$18, 8, $18
	and     $18, 0xff, $16
	bis     $31, 0x1, $17
	bsr	$26, srom_access_lcd
	srl	$18, 8, $18
	and     $18, 0xff, $16
	bis     $31, 0x1, $17
	bsr	$26, srom_access_lcd
	srl	$18, 8, $18
	and     $18, 0xff, $16
	bis     $31, 0x1, $17
	bsr	$26, srom_access_lcd
	srl	$18, 8, $18
	and     $18, 0xff, $16
	bis     $31, 0x1, $17
	bsr	$26, srom_access_lcd
	srl	$18, 8, $18
	and     $18, 0xff, $16
	bis     $31, 0x1, $17
	bsr	$26, srom_access_lcd
	bis     $31, $7, $26
	ret	$31,  ($7)

srom_iic_wait_for_idle:
        lda     $3, 0x1000($31)                //
iic_wait_for_loop:                              //
        ldbu    $5, 1($0)             //
        ldbu    $2, 0($1)             //
        subq    $3, 1, $3                       //
        beq     $3, iic_idle                    //
        blbc    $5, iic_wait_for_loop          //
iic_idle:                                       //
//        bis     $27, 1, $27                     //      pal mode
        ret	$31, ($27)                   /* All synced up                */
iic_wait_for_pin:				// Wait for bus to become idle
	ldah    $3, 0x7($31)
iic_wait_for_pin_loop:
	ldbu    $4, 1($0)
	ldbu    $2, 0($1)
	subq    $3, 1, $3
	beq     $3, iic_pin
	srl     $4, 7, $2
	blbs    $2, iic_wait_for_pin_loop
iic_pin: 
//        bis     $27, 1, $27                     //      pal mode
        ret	$31, ($27)                   /* All synced up                */

iic_set_addresses:
	ldah    $0, 0x4007($31)
	lda     $0, 0xffc0($0)
	sll     $0, 13, $0
	ldah    $1, 0x400f($31)
	lda     $1, 0xe000($1)
	sll     $1, 13, $1
	lda     $1, 0x80($1)
//        bis     $27, 1, $27                     //      pal mode
        ret     $31, ($27)                   /* All synced up                */
.end srom_write_lcd

        .globl srom_write_word
        .set noat
        .text
        .align  4
        .ent srom_write_word
 #
 #
 #       enter with $16 the addr
 #       enter with $17 the data
srom_write_word:
iic_write_word:
	bsr	$27, srom_iic_wait_for_idle
	and     $16, 0xfe, $16
        stb     $16, 0($0)
        ldbu    $2, 0($1)
	bis     $31, 0xc5, $16
	bis	$31, $31, $31
	bis	$31, $31, $31
	bis	$31, $31, $31
	mb
        bis     $31, $31, $31
        bis     $31, $31, $31
        bis     $31, $31, $31

        stb     $16, 1($0)
        ldbu    $2, 0($1)
        bsr     $27, iic_wait_for_pin
        stb     $17, 0($0)
        ldbu    $2, 0($1)
        bsr     $27, iic_wait_for_pin
        bis     $31, 0xc3, $16
        stb     $16, 1($0)
        ldbu    $2, 0($1)
	ret	$31,  ($28)

.end srom_write_word

usdelay:
	lda	$3, 6667($31)
        mulq    $3, 16, $3
usdelay_init:
        rpcc    $2 
        bis     $31, $31, $31
        bis     $31, $31, $31
        bis     $31, $31, $31
usdelay_loop:
        rpcc    $4
        subq    $4, $2, $4
        zapnot  $4, 0x0f, $4
        cmplt   $3, $4, $4
        beq     $4, usdelay_loop
	ret	$31,  ($28)

        .globl srom_access_lcd
        .set noat
        .text
        .align  4
        .ent srom_access_lcd
 #
 #
 #       enter with $16 control
 #       enter with $17 contrast | rs
srom_access_lcd:
	bis     $31, $17, $6
	bsr	$27, iic_set_addresses
	bis     $31, $16, $17
	bis	$31, 0x42, $16
	bsr	$28, srom_write_word
	bsr	$28, usdelay
	bis     $6, 0x60, $17
	bis	$31, 0x40, $16
	bsr	$28, srom_write_word
	bsr	$28, usdelay
	bis     $6, 0x40, $17
	bis	$31, 0x40, $16
	bsr	$28, srom_write_word
	bsr	$28, usdelay
	bis     $31, $16, $17
	bis     $6, 0x60, $17
	bis	$31, 0x40, $16
	bsr	$28, srom_write_word
	bsr	$28, usdelay
	ret	$31,  ($26)

.end srom_access_lcd

        .globl srom_init_lcd
        .set noat
        .text
        .align  4
        .ent srom_init_lcd
 #
 #
srom_init_lcd:
	bis     $31, $26, $7
	ldah	$0, 0x400f($31)
	lda     $0, 0xf000($0)
	sll     $0, 13, $0
	lda     $0, 0x2847($0)
	bis	$31, 0xe0, $1
	stb	$1, ($0)

	bsr	$27, iic_set_addresses

	bsr	$27, srom_iic_wait_for_idle
	bis     $31, 0x80, $16
        stb     $16, 1($0)
        ldbu    $2, 0($1)
        bsr     $27, iic_wait_for_pin

	bis     $31, 0x0, $16
        stb     $16, 1($0)
        ldbu    $2, 0($1)
        bsr     $27, iic_wait_for_pin

	bis     $31, 0x5b, $16
        stb     $16, 0($0)
        ldbu    $2, 0($1)
	bis     $31, 0x20, $16
	bis	$31, $31, $31
	bis	$31, $31, $31
	bis	$31, $31, $31
	mb
        bis     $31, $31, $31
        bis     $31, $31, $31
        bis     $31, $31, $31
        stb     $16, 1($0)
        ldbu    $2, 0($1)
        bsr     $27, iic_wait_for_pin

	bis     $31, 0x14, $16
        stb     $16, 0($0)
        ldbu    $2, 0($1)
	bis     $31, 0xc3, $16
	bis	$31, $31, $31
	bis	$31, $31, $31
	bis	$31, $31, $31
	mb
        bis     $31, $31, $31
        bis     $31, $31, $31
        bis     $31, $31, $31
        stb     $16, 1($0)
        ldbu    $2, 0($1)
        bsr     $27, iic_wait_for_pin

	bis     $31, 0x38, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	bis     $31, 0x38, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	bis     $31, 0x38, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	bis     $31, 0x38, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	bis     $31, 0x0e, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	bis     $31, 0x02, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	bis     $31, 0x01, $16
	bis     $31, $31, $17
	bsr	$26, srom_access_lcd
	bis     $31, $7, $26
	ret	$31,  ($7)
.end srom_init_lcd
#else
	/* dummy entry point to keep NT ASAXP happy */
srom_init_lcd:
	ret	$31,($26)
#endif
