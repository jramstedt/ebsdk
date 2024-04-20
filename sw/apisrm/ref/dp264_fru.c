/*
 * file:    dp264_fru.c
 *
 * Copyright (C) 1998 by
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
 *      Alpha SRM Console Firmware
 *
 *  MODULE DESCRIPTION:
 *
 *      This module contains platform-specific routines to access the 
 *	JEDEC-like FRU IIC EEPROMs.
 *                                        
 *  AUTHOR:
 *
 *      Eric Rasmussen
 *
 *  CREATION DATE:
 *  
 *      30-Nov-1998
 *
 *  MODIFICATION HISTORY:
 *
 *	ER	20-AUG-1999	Added gct_sys_serial_write( ).
 *	ER	30-Nov-1998	Adapted from CLIPPER_FRU.C
 *
 *--
 */

#include "cp$src:common.h"
#include "cp$src:kernel_def.h"
#include "cp$src:msg_def.h"
#include "cp$src:gct.h"
#include "cp$src:gct_tree.h"
#include "cp$src:gct_nodeid.h"
#include "cp$src:fru50_def.h"
#include "cp$src:platform_fru.h"
#include "cp$src:iic_def.h"
#include "cp$src:jedec_def.h"

#define DEBUG 0

extern gct_root;

char *device[] = { "iic_cpu0", "iic_cpu1", "iic_smb0" };

#define MAX_DEV ( sizeof( device ) / sizeof( char * ) )

int write_jedec( char *name, char *value, int offset, int style, int flag ) 
{
    struct FILE *fp;
    struct INODE *ip;
    struct IICCB *iicbp;
    unsigned __int64 fru_id = 0;
    GCT_NODE_ID_FRU_DESC *pNodeid;
    char buffer[32];
    int length;
    int i, j;

    if ( support_iic_fru( name, fru_names ) == -1 ) {
 	printf( "write_jedec: Could not find node %s\n", name );
	return( msg_failure );
    }
    if ( support_iic_fru( name, fru_names ) == 0 ) {
 	printf( "write_jedec: Device %s does not support setting FRU values\n", name );
	return( msg_failure );
    }
/*
 * Find the INODE and open the EEPROM device
 */
    fru_id = find_fru_id( name, fru_names );
    pNodeid = &fru_id;
    for ( i = 0;  i < MAX_DEV;  i++ ) {
    	if ( ( fp = fopen( device[i], "rs+" ) ) != 0 ) {
	    ip = fp->ip;
	    iicbp = ( struct IICCB * )ip->misc;
	    if ( iicbp->address == ( char )( pNodeid->node_id_fru_desc_slot & 0xFF ) ) {
/* 
 * Check to see if we are dealing with integers or characters 
 */
		if ( style ) {
		    *buffer = ( char * )value;
		    length = 1;
#if DEBUG
		    printf( "write_jedec: %s offset: %3d value: %x\n", device[i], offset, ( char * )value );
#endif
		} 
		else {
		    sprintf( buffer, "%s", value );
		    length = strlen( buffer );
#if DEBUG
		    for ( j = 0;  j < length;  j++ ) {
			printf( "write_jedec: %s offset: %3d value: %c\n", device[i], offset + j, buffer[j] );
		    }
#endif
		}
/* 
 * Position the file offset and write the data
 */
		fseek( fp, offset, SEEK_SET );
		fwrite( buffer, length, 1, fp );
		fclose( fp );
		return( msg_success );
	    }
	    fclose( fp );
	}
    }
    return( msg_failure );
}

int read_jedec( char *name, char *buffer, int length, int offset, int flag ) 
{
    struct FILE *fp;
    struct INODE *ip;
    struct IICCB *iicbp;
    unsigned __int64 fru_id = 0;
    GCT_NODE_ID_FRU_DESC *pNodeid;
    int i;

    if ( ( support_iic_fru( name, fru_names ) == -1 ) || 
	 ( support_iic_fru( name, fru_names ) == 0 ) ) {
	return( 0 );
    }
/*
 * Find the INODE and open the EEPROM device
 */
    fru_id = find_fru_id( name, fru_names );
    pNodeid = &fru_id;
    for ( i = 0;  i < MAX_DEV;  i++ ) {
    	if ( ( fp = fopen( device[i], "rs" ) ) != 0 ) {
	    ip = fp->ip;
	    iicbp = ( struct IICCB * )ip->misc;
	    if ( iicbp->address == ( char )( pNodeid->node_id_fru_desc_slot & 0xFF ) ) {
/* 
 * Position the file offset and read the data
 */
#if DEBUG
		printf( "read_jedec: %s offset: %3d length: %x\n", device[i], offset, length );
#endif
		fseek( fp, offset, SEEK_SET );
		fread( buffer, length, 1, fp );
		fclose( fp );
		return( msg_success );
	    }
	    fclose( fp );
	}
    }
    return( 0 );
}


int gct_sys_serial_write ( char *sys_name ) {
  int                 i, j, m = 0, status = 0, localOffset;
  GCT_ROOT_NODE       *pRoot;
  GCT_HANDLE          fru_root = 0, node = 0;
  int                 flag=0;
  GCT_NODE            *pNode, *pNext;
  GCT_FRU_DESC_NODE   *pFrudesc;
  FRU_NAME_LIST       *pFru_names; 
  char                *fruname, mfg_sys_serial[17];
  unsigned char       buffer[256];
  unsigned int        return_checksum=0;

  pFru_names = fru_names;
  pRoot = (GCT_ROOT_NODE *) _GCT_MAKE_ADDRESS( 0);
  status = gct$find_node(0,&fru_root, FIND_BY_TYPE,NODE_FRU_ROOT,  0, 0, 0, 0);
  pNode = _GCT_MAKE_ADDRESS(fru_root);
  
  pNext = pNode;
  j = 0;
  while (pNext && (j < 2)) { 
	status = gct$find_node(0,&node, FIND_ANY, pNext , 0, 0, 0, 0);
	pNext = _GCT_MAKE_ADDRESS(node);
	pFrudesc = _GCT_MAKE_ADDRESS(node);

	/* Look only for type=15 (gct_fru_desc nodes)
	*/
	if (pNext->type == 0x15) {
	    m = 0;
	    while ( pFru_names[m].fru_id != 0) {
		if (pNext->id == pFru_names[m].fru_id) {
		    if ( pFru_names[m].iic_support ) {
			fruname = pFru_names[m].name;

			/* Read current string value */
			status = read_jedec(fruname, &buffer, 16, offsetof( struct _JEDEC, jedec_b_sys_serialnumb), flag);
			sprintf(mfg_sys_serial, "%s", sys_name);

			/* Only update if different */
			if ( strncmp ( buffer, mfg_sys_serial, 16) != 0 ) {
			    write_jedec(fruname, mfg_sys_serial, offsetof(struct _JEDEC, jedec_b_sys_serialnumb), 0, flag);
			    write_jedec(fruname, 00, offsetof(struct _JEDEC, jedec_b_sys_serialnumb) + strlen(mfg_sys_serial), 1 ,flag);
			    return_checksum = jedec_checksum( fruname, 0x80, 0x7E);
			    status = write_jedec( fruname, return_checksum, offsetof(struct _JEDEC, jedec_b_checksum128to254), 1, flag);
			    break;
			}
		    }  /* end - if iic support */
		}  /* end - if id match */
	    m++;
	    }  /* end -  while fru id */
	}
	if (pNext->type == 0x14) j++;
  }

  return msg_success;
}
