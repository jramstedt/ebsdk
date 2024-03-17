/* os2acl.h
 *
 * Author:  Kai Uwe Rommel <rommel@ars.de>
 * Created: Fri Mar 29 1996
 */
 
/* $Id: os2acl.h,v 1.1 1999/01/26 18:13:00 paradis Exp $ */

/*
 * $Log: os2acl.h,v $
 * Revision 1.1  1999/01/26 18:13:00  paradis
 * New files for zip/unzip
 *
 * Revision 1.1  1996/03/30 09:35:00  rommel
 * Initial revision
 * 
 */

#ifndef _OS2ACL_H
#define _OS2ACL_H

#define ACL_BUFFERSIZE 4096

int acl_get(char *server, const char *resource, char *buffer);
int acl_set(char *server, const char *resource, char *buffer);

#endif /* _OS2ACL_H */

/* end of os2acl.h */
