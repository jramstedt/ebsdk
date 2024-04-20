/* file:	aputchar.c
 *
 * Copyright (C) 1992, 1993 by
 * Digital Equipment Corporation, Maynard, Massachusetts.
 * All rights reserved.
 *
 * This software is furnished under a license and may be used and copied
 * only  in  accordance  of  the  terms  of  such  license  and with the
 * inclusion of the above copyright notice. This software or  any  other
 * copies thereof may not be provided or otherwise made available to any
 * other person.  No title to and  ownership of the  software is  hereby
 * transferred.
 *
 * The information in this software is  subject to change without notice
 * and  should  not  be  construed  as a commitment by digital equipment
 * corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by digital.
 */

/*
 *++
 *  FACILITY:
 *
 *      Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *
 *	This module implements the aputchar function.
 *	The a putchar function puts an ansi character.
 *
 *  AUTHORS:
 *
 *      J. A.  DeNisco
 *
 *  CREATION DATE:
 *
 *      23-Aug-1993
 *
 *  MODIFICATION HISTORY:
 *
 *	jad	23-aug-1993	Initial Entry.
 *
 *	dwb	08-jul-1994	Fixed line wrap with cr/lf in col 80
 *
 *	rhd	07-jun-1995	Fixed CSI Cursor Up (CASE 'H' & 'A')
 *
 *--
 */

#include	"cp$src:platform.h"
#include "cp$src:common.h"
#include	"cp$src:ansi_def.h"
#include	"cp$src:alpha_arc.h"
#include	"cp$src:chstate.h"
#include	"cp$src:display.h"
#include "cp$inc:prototypes.h"

/*Local Constants*/
#define AP_DEBUG 1

/*External references*/

extern int display_ForegroundColor;
extern int display_BackgroundColor;
extern int display_HighIntensity;
extern int display_UnderScored;
extern int display_ReverseVideo;

#if TGA
extern struct CH_STATE *tga_ch_state;
extern void tga_scroll(struct CH_STATE *chs);
#endif

#if TURBO
extern int both_console;
extern int graphics_enabled;
#endif

extern int ansi_parser (struct ANSI *as, int ch);
extern char paputchar (struct CH_STATE *chs,char ch,unsigned char attribute,
							     int xpos,int ypos);
extern char pagetchar (struct CH_STATE *chs,char *ch,unsigned char *attribute,
							     int xpos,int ypos);

/*Function prototypes*/
void chattributes(struct CH_STATE *chs);
void csi (struct CH_STATE *chs);
void erase(struct CH_STATE *chs,int final);
void esc (struct CH_STATE *chs);
void aputchar (struct CH_STATE *chs,char c);
void scroll (struct CH_STATE *chs);

extern int cbip;
extern UINT cb_time_limit;

/*+
 * ============================================================================
 * = aputchar - Ansi put a character.                                         =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will put a character with support for ansi CSI and
 *  escape sequences. Output is done using the paputchar function.
 *
 * FORM OF CALL:
 *
 *	aputchar (chs,c)
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct CH_STATE *chs - Pointer to the current state
 * 	char c - Character to be output.
 *
-*/

void aputchar (struct CH_STATE *chs,char c)
{
unsigned char uc;

/* Feed the character to the parser, and if we have an	*/
/* accepting state, process it.				*/
	if (ansi_parser (&(chs->as), c) == ANSI_NEEDMORE) {
		return;
	}
	uc = (unsigned char) chs->as.c;

/*Handle the characters*/

	/* BELL */
	if (uc == 0x07) {
	   return;
	}
	/* NULL */
	else if (uc == '\0') {
	   return;
	}
	else if (uc == '\t') {
	   chs->ds.CursorXPosition = (chs->ds.CursorXPosition + 8) & ~7;
	}
	else if (uc == '\b') {
	   if (chs->ds.CursorXPosition) {
		chs->ds.CursorXPosition--;
	   }
	}
	else if (uc == '\n') {
	   chs->ds.CursorYPosition++;
	}
	else if (uc == '\r') {
	   chs->ds.CursorXPosition = 0;
	}
	else if (uc == CSI) {
	   csi(chs);
	}
	else if (uc == esc) {
	   esc(chs);
	}
	else {	/*Just display this character*/
	   if (chs->ds.CursorXPosition > chs->ds.CursorMaxXPosition){
		chs->ds.CursorYPosition++;
		chs->ds.CursorXPosition = 0;
	   }
	   if (chs->ds.CursorYPosition > chs->ds.CursorMaxYPosition){
#if TGA
		if (chs != tga_ch_state)
			scroll(chs);           
             	else       
			tga_scroll(chs);
#else           
      	     	scroll(chs);           
#endif
	     	chs->ds.CursorYPosition = chs->ds.CursorMaxYPosition -
							chs->scroll_jump;
	   }

	   ((PPF)chs->putc)(chs,uc,chs->CurrentAttribute,
			chs->ds.CursorXPosition,chs->ds.CursorYPosition);
			chs->ds.CursorXPosition++;
	}

	if (chs->ds.CursorYPosition > chs->ds.CursorMaxYPosition){
#if TGA
	    if (chs != tga_ch_state)
		scroll(chs);           
	    else       
		tga_scroll(chs);
#else           
	    scroll(chs);           
#endif
	    chs->ds.CursorYPosition = chs->ds.CursorMaxYPosition
			    - chs->scroll_jump;
	}

/*Set the cursor position*/
	((PPF)chs->putc)(chs,0,0,chs->ds.CursorXPosition,chs->ds.CursorYPosition);
}


/*+
 * ============================================================================
 * = scroll - Scroll the screen 1 line            							  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will scroll the entire screen 1 line.
 *
 * FORM OF CALL:
 *
 *	scroll (chs)
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct CH_STATE *chs - Pointer to the current state
 *
-*/

void scroll (struct CH_STATE *chs)
{
int r,c,i;
char ch;
unsigned char attr;
UINT time;

/*Copy the screen up 1 row*/

	/*Initialize*/
	if (chs->scroll_in_prog) {
	    r = chs->row;
	    c = chs->col;
	    chs->scroll_in_prog = 0;
	} else {
	    r = 1;
	    c = 0;
	}
	i = 0;

	/*Scroll one line*/
	for (; r<=chs->ds.CursorMaxYPosition; r++) {
	    if (i) c = 0;
	    for (; c<=chs->ds.CursorMaxXPosition; c++) {
		/*Quit if we've done enough*/

#if TURBO
		if (!(both_console && graphics_enabled)) {
		    if (i && cbip) {
			rscc (&time);

			if (time > cb_time_limit) {
			    chs->row = r;
			    chs->col = c;
			    chs->scroll_in_prog = 1;
			    return;
			}
		    }
		}
#endif

		/*Get the character and attribute*/
		((PGF)chs->getc) (chs,&ch,&attr,c,r);

		/*Move it up a row*/
		((PPF)chs->putc) (chs,ch,attr,c,r-1);

		/*Clear char if the last line*/
		if (r == chs->ds.CursorMaxYPosition) {
	            ((PPF)chs->putc) (chs,' ',chs->CurrentAttribute,c,chs->ds.CursorMaxYPosition);
                    }

		/*Show we've made progress*/
		i++;
	    }

	}
}



/*+
 * ============================================================================
 * = csi - Handle a csi code.                							      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will handle a csi code.
 *
 * FORM OF CALL:
 *
 *	csi (chs)
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct CH_STATE *chs - Pointer to the current state
 *
-*/

void csi(struct CH_STATE *chs)
{
int final;

#if AP_DEBUG  && 0
	printf ("CSI %c %d %d\n",chs->as.buf [chs->as.bufix-1],chs->as.pval[0],
															chs->as.pval[1]);
#endif

/*Get the final character*/
	final = chs->as.buf [chs->as.bufix-1];

/*Handle the CSI function*/
	switch (final){
	/*Erase functions*/
		case 'J':
		case 'K':
			erase (chs,final);
			break;
	/*Cursor position functions*/
		case 'H':
		/*Absolute postion*/
                        if ( chs->as.pval[1] < 1)
                        {
                           chs->as.pval[1] = 1 ;
                        }
                        else
                        {
                           chs->ds.CursorXPosition = chs->as.pval[1] - 1 ;
                        }

                        if ( chs->as.pval[0] < 1)
                        {
                           chs->as.pval[0] = 1 ;
                        }
                        else
                        {
                           chs->ds.CursorYPosition = chs->as.pval[0] - 1 ;
                        }

                        break;

		case 'A':
		/*Cursor up*/
                        chs->ds.CursorYPosition -= chs->as.pval [0];
                        if (chs->ds.CursorYPosition < 0 )
                                chs->ds.CursorYPosition = 0;
  			break;
		case 'B':
		/*Cursor Down*/
			chs->ds.CursorYPosition += chs->as.pval [0];
			if (chs->ds.CursorYPosition  > chs->ds.CursorMaxYPosition )
				chs->ds.CursorYPosition = chs->ds.CursorMaxYPosition ;
			break;
		case 'C':
		/*Cursor right*/
			chs->ds.CursorXPosition += chs->as.pval [0];
			if (chs->ds.CursorXPosition  > chs->ds.CursorMaxXPosition )
				chs->ds.CursorXPosition = chs->ds.CursorMaxXPosition ;
			break;
		case 'D':
		/*Cursor left*/
			if((short)chs->ds.CursorXPosition-chs->as.pval [0] < 0)
				chs->ds.CursorXPosition = 0;
			else
				chs->ds.CursorXPosition -= chs->as.pval [0];

			break;
		case 'h':
		/*Enable cursor*/
			((PICF)chs->ioctl)(ENA_CURSOR);
			break;
		case 'l':
		/*Disable cursor*/
			((PICF)chs->ioctl)(DIS_CURSOR);
			break;
		/*Character attribiute functions*/
		case 'm':
			chattributes (chs);
			break;
	}
}


/*+
 * ============================================================================
 * = esc - Handle an escape sequence.            							  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will Handle an escape sequence.
 *
 * FORM OF CALL:
 *
 *	esc (chs)
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct CH_STATE *chs - Pointer to the current state
 *
-*/

void esc(struct CH_STATE *chs)
{
int final;

#if AP_DEBUG && 0
	printf ("ESC %c\n",(char)final);
#endif

/*Get the final character*/
	final = chs->as.buf [chs->as.bufix-1];

/*Handle the sequence*/
	switch (final){
		case '7':
		/*Push the cursor*/
			chs->PushedXPosition = chs->ds.CursorXPosition;
			chs->PushedYPosition = chs->ds.CursorYPosition;
			break;
		case '8':
		/*Pop the Cursor*/
			chs->ds.CursorXPosition = chs->PushedXPosition;
			chs->ds.CursorYPosition = chs->PushedYPosition;
			break;
	}
}


/*+
 * ============================================================================
 * = erase - Erase line or screen functions.            					  =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will perform the CSI erase functions.
 *
 * FORM OF CALL:
 *
 *	esc (chs,final)
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct CH_STATE *chs - Pointer to the current state
 *	int final - Final character.
 *
-*/

void erase(struct CH_STATE *chs,int final)
{
USHORT sr,er,sc,ec;			/*starting and ending columns and rows*/
USHORT r,c;

#if AP_DEBUG && 0
	printf ("ERASE: final: %c p0: %d\n",final,chs->as.pval [0] );
#endif

/*Get the actual command*/
	switch (final){
	/*Clear screen functions*/
		case 'J':
			switch (chs->as.pval [0] ){
				case 0:
				/*From the cursor to the end of the screen*/
					sr = chs->ds.CursorYPosition;
					er = chs->ds.CursorMaxYPosition;
					sc = chs->ds.CursorXPosition;
					ec = chs->ds.CursorMaxXPosition;
					break;
				case 1:
				/*From the begining of the screen to the cursor*/
					sr = 0;
					er = chs->ds.CursorYPosition;
					sc = 0;
					ec = chs->ds.CursorXPosition;
					break;
				case 2:
				/*The entire screen*/
					sr = 0;
					er = chs->ds.CursorMaxYPosition;
					sc = 0;
					ec = chs->ds.CursorMaxXPosition;
					break;
			}
		break;
	/*Clear Line functions*/
		case 'K':
			switch (chs->as.pval [0] ){
				/*From the cursor to the end of the line*/
				case 0:
					sr = chs->ds.CursorYPosition;
					er = chs->ds.CursorYPosition;
					sc = chs->ds.CursorXPosition;
					ec = chs->ds.CursorMaxXPosition;
					break;
				case 1:
				/*From the begining of the line to the cursor*/
					sr = chs->ds.CursorYPosition;
					er = chs->ds.CursorYPosition;
					sc = 0;
					ec = chs->ds.CursorXPosition;
					break;
				case 2:
				/*The entire line*/
					sr = chs->ds.CursorYPosition;
					er = chs->ds.CursorYPosition;
					sc = 0;
					ec = chs->ds.CursorMaxXPosition;
					break;
			}
		break;
	}

/*Do the clear*/
	for (r = sr ;r <= er; r++)
		for (c=sc ;c <= ec; c++)
			((PPF)chs->putc)(chs,' ',chs->CurrentAttribute,c,r);
}


/*+
 * ============================================================================
 * = chattributes - Character attribute functions.    					      =
 * ============================================================================
 *
 * OVERVIEW:
 *
 * 	This function will perform character attribute functions.
 *
 * FORM OF CALL:
 *
 *	chattributes (chs)
 *
 * RETURNS:
 *
 *	None
 *
 * ARGUMENTS:
 *
 *	struct CH_STATE *chs - Pointer to the current state
 *
-*/

#if !MODULAR && !ARC_SUPPORT
void chattributes(struct CH_STATE *chs)
{
    int i;

/*Handle the attributes for each parameter*/

    for (i=0; i<chs->as.pvc; i++) {

	switch (chs->as.pval [i]) {
	    case 0:			/*No attributes*/
		chs->CurrentAttribute = (NORMAL_VIDEO&(~BLINK_ATTRIBUTE));
		break;
	    case 5:			/*Blink attribute*/
		chs->CurrentAttribute |= BLINK_ATTRIBUTE;
		break;
	    case 7:			/*Reverse video*/
		chs->CurrentAttribute = REVERSE_VIDEO;
		break;
	}
    }
}
#endif

#if MODULAR || ARC_SUPPORT
void chattributes(struct CH_STATE *chs)
{
    int i;

/*Handle the attributes for each parameter*/

    for (i=0; i<chs->as.pvc; i++) {

	switch (chs->as.pval [i]) {

	    case 0: 			/* Attributes off. */
		display_HighIntensity = 0;
		display_UnderScored = 0;
		display_ReverseVideo = 0;
		chs->CurrentAttribute = (display_BackgroundColor<<4) | (display_ForegroundColor);
		break;

	    case 1: 			/* High Intensity. */
		display_HighIntensity = 1;
		chs->CurrentAttribute |= 0x8;
		break;

	    case 4: 			/* UnderScored. */
		display_UnderScored = 1;
		break;

	    case 7: 			/* Reverse Video. */
		display_ReverseVideo = 1;
		chs->CurrentAttribute = (display_ForegroundColor<<4) | (display_BackgroundColor);
		if (display_HighIntensity)
		    chs->CurrentAttribute |= 0x8;
		break;

	    case 10: 			/* Font selection, not implemented yet. */
	    case 11: 
	    case 12: 
	    case 13: 
	    case 14: 
	    case 15: 
	    case 16: 
	    case 17: 
	    case 18: 
	    case 19: 
		break;

	    case 30: 			/* Foreground Color. */
	    case 31: 
	    case 32: 
	    case 33: 
	    case 34: 
	    case 35: 
	    case 36: 
	    case 37: 
		display_ForegroundColor = chs->as.pval [i] - 30;
		chs->CurrentAttribute = (display_BackgroundColor<<4) | (display_ForegroundColor);
		if (display_HighIntensity)
		    chs->CurrentAttribute |= 0x8;
		break;

	    case 40: 			/* Background Color. */
	    case 41: 
	    case 42: 
	    case 43: 
	    case 44: 
	    case 45: 
	    case 46: 
	    case 47: 
		display_BackgroundColor = chs->as.pval [i] - 40;
		chs->CurrentAttribute = (display_BackgroundColor<<4) | (display_ForegroundColor);
		if (display_HighIntensity)
		    chs->CurrentAttribute |= 0x8;
		break;

	    default: 
		break;
	}
    }
}
#endif
