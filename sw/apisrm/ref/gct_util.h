/*** MODULE GCT_UTIL ***/
/******************************************************************************/
/**                                                                          **/
/**  Copyright (c) 1997                                                      **/
/**  by DIGITAL Equipment Corporation, Maynard, Mass.                        **/
/**  All rights reserved.                                                    **/
/**                                                                          **/
/**  This software is furnished under a license and may be used and  copied  **/
/**  only  in  accordance  with  the  terms  of  such  license and with the  **/
/**  inclusion of the above copyright notice.  This software or  any  other  **/
/**  copies  thereof may not be provided or otherwise made available to any  **/
/**  other person.  No title to and ownership of  the  software  is  hereby  **/
/**  transferred.                                                            **/
/**                                                                          **/
/**  The information in this software is subject to change  without  notice  **/
/**  and  should  not  be  construed  as  a commitment by DIGITAL Equipment  **/
/**  Corporation.                                                            **/
/**                                                                          **/
/**  DIGITAL assumes no responsibility for the use or  reliability  of  its  **/
/**  software on equipment which is not supplied by DIGITAL.                 **/
/**                                                                          **/
/******************************************************************************/
/*
* Author: Fred Kleinsorge
*
*   Modified by:
*
*	X-2	Andy Kuehnel				13-Nov-1997
*		Add standard header; adopt to VMS build environment.
*/
#ifndef _GCT_UTIL_H_
#define _GCT_UTIL_H_

#define GCT_UTIL$_SUCCESS    1
#define GCT_UTIL$_STRING_MAX 512

/*
 * Flags for gcon_edit_string 
 */
#define GCT_UTIL$_UPCASE                 0x01
#define GCT_UTIL$_STRIP_LEAD_WHITESPACE  0x02
#define GCT_UTIL$_STRIP_TRAIL_SPACES     0x04
#define GCT_UTIL$_STRIP_QUOTES           0x08
#define GCT_UTIL$_COMPRESS_SPACES        0x10
#define GCT_UTIL$_COMPRESS_TABS          0x20
#define GCT_UTIL$_STRIP_COMMENTS         0x40
#define GCT_UTIL$_STRIP_SEMI_COMMENTS    0x80

#define GCT_UTIL$_xTAB                   0x09
#define GCT_UTIL$_xDELETE                0x7F

/*
 * Flags for gcon_tokenize_input
 */
#define GCT_UTIL$_NO_FLAGS         0
#define GCT_UTIL$_RETURN_SEPERATORS 1

/*
 *  TOUPPER and TOLOWER
 */
#define _UPCASE(c)      (((c) >= 'a' && (c) <= 'z') \
                                    ? (c) & 0xDF : (c))

#define _LOWERCASE(c)   (((c) >= 'A' && (c) <= 'Z') \
                                    ? (c) | 0x20 : (c))

/*
 *  Utility routines
 */
extern int   gct_util$edit_string(char *new_string, char *old_string, int max_destination, int flags);
extern int   gct_util$tokenize_input(char *input_string, char **tokens, int max_tokens, int *seperators, int flags);
extern int   gct_util$compare_case_blind( char *str1, char *str2, int max);
extern int   gct_util$append_decimal_string(char *outstr, int bin);
extern int   gct_util$append_hex_string(char *outstr, int bin, int min, char pad, int upper);
extern void  gct_util$out_asciz_string(char *string);
extern void  gct_util$print_unknown_string(char *message, int length);

#ifndef GCT_UTIL$_NO_ALIASES

/*
 *   STRING routines that mimic standard UNIX routines
 */
/* extern int    gct_util$string_length(char *str1); */
/* extern char  *gct_util$string_copy(char *str1, char *str2); */
extern char  *gct_util$string_append(char *str1, char *str2);
extern int    gct_util$string_compare( char *str1, char *str2, int max);
extern unsigned __int64 gct_util$strtouq (char *nptr, char **endptr, int base);


/*
 *   STDIO routines that mimic printf/sprintf/fprintf
 */
#ifndef VMS$GLX
/* extern int   gct_util$printf(char *msg, ...); */
/* extern int   gct_util$fprintf(GCT_FILE *stream, char *msg, ...); */
#endif
/* extern int   gct_util$sprintf(char *outs, char *msg, ...); */

/*
 *   Aliases for standard UNIX routined
 */

#if 0
#define strlen  gct_util$string_length
#define strcpy  gct_util$string_copy 
#define strcat  gct_util$string_append
#define strncmp gct_util$string_compare
#endif

#ifdef strtouq
#undef strtouq
#endif
#define strtouq gct_util$strtouq

#endif /* GCT_UTIL$_NO_ALIASES */

#endif /* _GCT_UTIL_H_ */
