# $Id: Makefile,v 1.1.1.1 1998/12/29 21:36:07 paradis Exp $
# $Log: Makefile,v $
# Revision 1.1.1.1  1998/12/29 21:36:07  paradis
# Initial CVS checkin
#
# Revision 1.4  1996/05/22  21:21:44  fdh
# Added an empty depend target.
#
# Revision 1.3  1995/10/31  18:49:27  cruz
# Added dummy lint rule.
#
# Revision 1.2  1994/06/28  20:08:21  fdh
# Modified filenames and build precedure to fit into a FAT filesystem.
#
# Revision 1.1  1994/03/11  05:14:12  fdh
# Initial revision
#

CONFIG_DIR=../..
include $(CONFIG_DIR)/sdkmake.conf

include ../rules.gnu

DELETE	= rm -rf

all:

clean:
	$(DELETE) *~ \#*\# core
	$(CLEAN)

depend:
