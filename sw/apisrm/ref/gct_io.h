/*** MODULE GCT_IO ***/
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
*	X-1	Andy Kuehnel				13-Nov-1997
*		Add standard header; initial checkin.
*/

#ifndef _GCT_IO_H_
#define _GCT_IO_H_

#define GCT_FILE char

extern GCT_FILE  *gct_io$open_file(char *name, char *access);
extern int        gct_io$close_file(GCT_FILE *stream);
extern char      *gct_io$get_string(char *string, int max, GCT_FILE *stream);
extern int        gct_io$put_string(char *string, GCT_FILE *stream);

#define _GCT_OPEN_FILE(_f,_o) gct_io$open_file(_f, _o);
#define _GCT_GET_STRING(_s,_l,_f) gct_io$get_string(_s, _l, _f);
#define _GCT_PUT_STRING(_s,_f) gct_io$put_string(_s, _f);
#define _GCT_CLOSE_FILE(_s) gct_io$close_file(_s);

#endif /* _GCT_IO_H_ */
