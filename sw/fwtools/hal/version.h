#ifndef __VERSION_H__
#define __VERSION_H__
/*
	Copyright (c) 1994
	Digital Equipment Corporation, Maynard, Massachusetts 01754

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
/*NOTE: After version 5 we went to a "final letter system" on the version number.
        Basically, the letter is incremented if only bug fixes were done.
        Increment the number when new features are added. */
/* V5.01A       - Added %section string operator
                - Added link_last directive
                - Added link_size directive
                - Added logical AND, OR, and NOT operators
                - Added lexer and parser debug options to .pragma
                - Fixed some bugs around variable args and .break
                - Added parser debug options to .pragma
                - Fixed literal size checking
                - Added concept of "link_region all"
                - Directive ".architecture" actually works now
                - HAL no longer does useless PC alignments with DSECTs
                - DSECT alignment can now be larger than 32 bits*/
/* V5.00D	- Fix problem with using "'" witin a repeat block which is within a macro */
/* V5.00C	- Accidently took out tmp file delete'ing in V5.00A ... put back in */
/* V5.00B	- Fixed a couple bugs */
/* V5.00A	- Various changes -- see user manual document */
/* V4.13	- fix bug in .endc detection with preceding local label on same line. */
/* V4.12	- fix bug in line continuation after a "," in a macro arg in a .iif statement. */
/* V4.11	- add -wa option. */
/* V4.10	- allow dest reg and hint specification for EV6 HW_RET-like inst */
/* V4.09	- .repeat directive should not execute if arg is 0 */
/* V4.08        - Added .include_dir directive */
/* V4.07        - Fix .include to error completely out if file is not found. */
/* V4.06        - Change EV5's HW_LD/ST type bit field size from 4 to 6 bits
                - Reduce some of the buffer sizes to help reduce memory usage
                - Fix problem with macro redefines and the -d option */
/* V4.05        - Allow .link_offset to be used at any time with D or Psects */
/* V4.04        - Force Ra to R31 for SEXT*, CTPOP, UNPK*, and PK* instructions */
/* V4.03        - Allow nesting of .cmd_input blocks
                - Have symbol substition, in .cmd_input blocks, print out capital hex characters */
/* V4.02        - Let RPCC have the option of specifing a Rb register */
/* V4.01        - Change psect memory allocation so hdr sections are very small in size initially */
/* V4.0		- Reworked memory allocation strategy to use malloc and free, and not block up tokens.
   		- Fixed bug when overwriting data already placed in memory which had relocations.
                - Fixed bug in listing files which placed upper LW of QWs in the wrong file
                  postion (usually byte 0). */
/* V3.21        - Changed .link_offset behavior so it doesn't affect other DSECTs */
/* V3.20        - Allow ctpop instruction to take a literal */
/* V3.19        - Allow both link_offset and link_after for a dsect */
/* V3.18	- Fix listing file for relocated symbols in second LW of listing line. */
/* V3.17        - set Ra to R31 for WH64 and ECB
                - change way HAL check for truncating errors */
/* V3.16        - fix do_cmd and do_cmdin symbol declare
                - added .nlist (was .nolist) */
/* V3.15        - fix do_cmd when output is not given */
/* V3.14        - Changed cache instruction
                - Fixed local variables
                - Added ^A operator
                - Added warnings about numbers too large for use */
/* V3.13	- New ITOF opcode values - EV6 IPR access changes */
/* V3.12	- fixed bug in macro processing which caused line continuation followed
   		  by a line beginning with whitespace to add an extra positional argument. */
/* V3.11        - added .link_offset */
/* V3.10        - More changes to EV6 PAL instructions */
/* V3.09        - Change score board bits position in EV6 pal opcodes */
/* V3.08        - fix bug in dsect sorter; allow allow lowercase with -h */
/* V3.07        - fixed itof opcode value */
/* V3.06        - Changed ITOF and FTOI names to S and T types */
/* V3.05        - Fix branch dispplacment when jumping to different psect */
/* V3.04        - Added warning for register expressions not 0 -> 31 in value */
/* V3.03        - Added detection of un defined label use */
/* V3.02        - updated new instructions opcode values
                - added IMPLVER instruction */
/* V3.01        - Allow reg #'s to be absexpr too */ 
/* V3.00        - Added division, reworked shell command stuff */
/* V2.9.4	- fixed allign pc where the "ic" was incorrect when -s was a large pc */
/* V2.9.3	- fixed retrurn of failure status if input file not found. */
/* V2.9.2	- fixed bug in errout which caused core dumps when called during input file
   		  stack processing. */
/* V2.9.1      	- added POPC, FF** instructions */
/* V2.9		- added .link_first directive
                - add Dsects to both avl and dll for quick lookup
                - redo dll sort algorithm so .link_after is loose and doesn't cause unneccessary overlap
                - add dsect overlap checking
                - change dsect hdr's so not in dll -- dsect struct points to it instead */
/* V2.8		- add UNIX environment variables support
                - added .cmd_input, .endi, and .do_cmd directives
                - added EXCB,SQRTx,NOP,FNOP,UNOP,SEXTx,LDB,LDW,STB,STW,CALL_PAL instructions
                - provide suport for '\' operator on macro call arguments
                - print out warning if link_address ignore in favor of alignment argument
                - added .minclude directive */
/* V2.7		- Chris added osf objects, .dsect stuff, .macro saving
   		- David fixed argument concat operator handling in macros. */
/* V2.6		- fix handling of "." changes which need to allocate more memory but don't
                  immediately write the memory array.  now the memory array is increased
                  any time "." moves beyond its end.
                - fixed problem with increasing psect by more than 1 multiple of
                  DEFAULT_PSECT_LENGTH.
                - changed to use fopen/fwrite for the image file. */
/* V2.5		- change handling of absolute expressions.  relocateable symbol may now be
   		  in absolute expressions.  if psect relocation alters the expression's
                  value, a warning is printed.
                - fix mt/mf_fpcr for both one register and three register formats. */
/* V2.4		- add mt_/mf_fpcr (synonyms for mt_/mf_fpctl) */
/* V2.3		- made macros with a "/" in name lose the slash prior to definition.
                - fixed handling of .iif bodies for "-" characters in comments
                - made usage of ^f operator print a warning and use a zero */
/* V2.2		- removed reserved ipr tokens.  hw_mtpr,hw_mfpr now accept absolute
   		  expressions.
/* V2.1		- fixed bug with NB .if condition.
   		- fixed listing file line numbers for macro definitions whose
                  argument list uses line continuation. */
/* V2.0.1	- added HA's osf.c, and beginnings of osf object module support. */
/* V2.0		- first merged version */
/* V1.7.5	- fix string parsing in .asci[iz] to not use leading whitespace of line */
/* V1.7.4	- add idn,dif conditions for if statements, 
                  (required rewrite of if condition parsing) */
/* V1.7.3	- fix nb,b conditions for .if/.iif directive
   		- fix label on same line as macro call */
/* V1.7.2	- fix macro arguments of different cases */
/* V1.7.1       - fix {ld/st}{ql}_{ucl} translations */
/* V1.7		- make WS work like commas where MACRO32 allows that */
/* V1.6.8       - make "." in branch instructions create a global label
                  add options to allow core dumps */
/* V1.6.7       - fixed macro definitions to override any previous definition 
                  of the same name */
/* V1.6         - fixed macro idents to contain "."'s everywhere, 
		  added support of [ld/st][lq]_[luc] */
/* V1.5		- added many, many things (including line continuation) */
/* V1.4		- added '*' (multiplication) operator */
/* V1.3		- improved memory management */
/* V1.2		- added support of macros, .ascii, .asciz, .iif */
/* V1.1		- added support of commaed expressions in .long, .quad and .address */

#ifndef DEBUG
#define VERSION		"V5.01A"
#else
#define VERSION		"V5.01A-Debug"
#endif

#endif
