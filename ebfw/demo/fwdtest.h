/*
**
**		    FWDTEST.H
**
**		    Copyright Madge Networks, 1993
**
**
**  Test harness for IPX forwarding code. 
**
**  Author: Trevor Warwick
**  Creation Date: 24 November 1993
**
*/

#define MAX_NETWORKS 100
#define MAX_FRAME_SIZE 128
#define MAX_TX_FRAMES 10
#define MAX_RIF_SIZE 32
#ifndef MAX_ADAPTERS
#define MAX_ADAPTERS 2
#endif

STRUCTDEF(TABLE)
{
    IPX_CONFIG config;
    IPX_ROUTING_ENTRY table[MAX_NETWORKS];
}
;

declare WORD cdecl far ipxb_route_frame_c(BYTE far *far *pframe,
  WORD rif_length, 
  int far *frame_length, WORD adapter_number);

#define MAKE_BIGENDIAN_32(a_longword) swap_dword(a_longword)
#define ATO32(param) atol(param)

/*end*/
