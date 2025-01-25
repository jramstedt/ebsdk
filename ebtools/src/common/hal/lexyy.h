#ifndef __LEXYY_H__
#define __LEXYY_H__
/*
	The HAL Alpha Assembler is free software; you can redistribute 
	it and/or modify it with the sole restriction that this copyright 
	notice be attached.

	The information in this software is subject to change without notice 
	and should not be construed as a commitment by Digital Equipment
	Corporation.

	The author disclaims all warranties with regard to this software,
	including all implied warranties of merchantability and fitness, in
	no event shall he be liable for any special, indirect or consequential
	damages or any damages whatsoever resulting from loss of use, data, 
	or profits, whether in an action of contract, negligence or other
	tortious action, arising out of or in connection with the use or
	performance of this software.

*/
#include <stdio.h>

#ifndef NO_LEXYY
/* from flex output */
#define YY_NULL 0
#define YY_READ_BUF_SIZE 8192
#define YY_BUF_SIZE (YY_READ_BUF_SIZE * 2) /* size of default input buffer */
#define YY_CHAR char
struct yy_buffer_state
    {
    FILE *yy_input_file;

    YY_CHAR *yy_ch_buf;		/* input buffer */
    YY_CHAR *yy_buf_pos;	/* current position in input buffer */

    /* size of input buffer in bytes, not including room for EOB characters*/
    int yy_buf_size;	

    /* number of characters read into yy_ch_buf, not including EOB characters */
    int yy_n_chars;

    int yy_eof_status;		/* whether we've seen an EOF on this buffer */
    };
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_create_buffer PROTO((FILE *, int));
void yy_switch_to_buffer PROTO((YY_BUFFER_STATE));
void yyrestart PROTO((FILE *));
#endif

#endif
