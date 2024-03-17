/*  ===========================================================================*/
/*        Written by:   Martin Lea, Mark Richer, Bruce Tanner*/

/*  This file is a header file for C and assembler programs, and describes  the*/
/*  structure of a node address, including group and functional addresses.*/

/*  The 6 byte node address is defined as a union to make it easy to access and*/
/*  also so that it can be manipulated as a single object.*/

/*  ===========================================================================*/

/*  19/3/92 CRH	This file gets included in other header files, to avoid*/
/* 			problems we need to check NODEADDR_INCLUDED just below*/


#ifndef NODEADDR_INCLUDED
#define NODEADDR_INCLUDED TRUE



#if defined(BIT_REVERSE)		/*   This is necessary because of the  
					 */
#define natural_order      trn		/*   different byte ordering on the    
					 */
#define bit_swapped_order  eth		/*   68000 and therefore the different 
					 */
#else					/*   bit ordering used by lattice 'C'  
					 */
#define natural_order      eth		/*   in bitfields.                     
					 */
#define bit_swapped_order  trn
#endif



/*  ===========================================================================*/


typedef union UNION_FUNCTIONAL_ADDRESS FUNCTIONAL_ADDRESS;
typedef union UNION_GROUP_ADDRESS GROUP_ADDRESS;
typedef union UNION_NODE_ADDRESS NODE_ADDRESS;



/*  ===========================================================================*/


union UNION_FUNCTIONAL_ADDRESS
  {
    DWORD all;
    BYTE byte[4];

    struct STRUCT_I_FUNCTIONAL_ADDRESS
      {
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD rpl : 1;
	WORD lrm : 1;			/*  This isn't really a functional
					 * address */

	WORD hlm_ser : 1;
	WORD hlm_res_man : 1;
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD netware : 1;

	WORD bridge : 1;
	WORD : 1;
	WORD : 1;
	WORD : 1;
	WORD hlm_iau : 1;
	WORD lanman : 1;
	WORD : 1;
	WORD : 1;

	WORD active : 1;
	WORD rps : 1;
	WORD hlm_hb : 1;
	WORD rem : 1;
	WORD crs : 1;
	WORD : 1;
	WORD hlm_non_ser : 1;
	WORD netbios : 1;
    } flags;
};




#define FUNCTIONAL_ADDRESS_HLM_HB 0x04000000L
#define FUNCTIONAL_ADDRESS_HLM_NON_SER 0x40000000L
#define FUNCTIONAL_ADDRESS_HLM_SER 0x00000100L
#define FUNCTIONAL_ADDRESS_HLM_RES_MAN 0x00000200L
#define FUNCTIONAL_ADDRESS_HLM_IAU 0x00100000L
#define FUNCTIONAL_ADDRESS_ACTIVE 0x01000000L
#define FUNCTIONAL_ADDRESS_RPS 0x02000000L
#define FUNCTIONAL_ADDRESS_REM 0x08000000L
#define FUNCTIONAL_ADDRESS_CRS 0x10000000L
#define FUNCTIONAL_ADDRESS_NETBIOS 0x80000000L
#define FUNCTIONAL_ADDRESS_BRIDGE 0x00010000L
#define FUNCTIONAL_ADDRESS_LANMAN 0x00200000L
#define FUNCTIONAL_ADDRESS_NETWARE 0x00008000L
#define FUNCTIONAL_ADDRESS_RPL 0x00000040L

#define FUNCTIONAL_ADDRESS_PREFIX 0x00C0


/*  ===========================================================================*/

union UNION_GROUP_ADDRESS
  {
    DWORD all;
    BYTE byte[4];
};


#define GROUP_ADDRESS_PREFIX 0x00C0


/*  ===========================================================================*/











union UNION_NODE_ADDRESS
  {
    BYTE byte[6];
    WORD word[3];
    union
      {
	struct STRUCT_I_NA_ROUTING
	  {
	    BYTE r : 7;
	    BYTE flag : 1;
	} routing;
	struct STRUCT_I_NA_MULTICAST
	  {
	    BYTE r : 7;
	    BYTE flag : 1;
	    BYTE : 8;
	    BYTE : 7;
	    BYTE group : 1;
	} multicast;
    } bit_swapped_order;		/*  TRN normally, ETH for 68k  */
    union
      {
	struct STRUCT_I_NA_ROUTING1
	  {
	    BYTE flag : 1;
	} routing;
	struct STRUCT_I_NA_MULTICAST1
	  {
	    BYTE flag : 1;
	    BYTE : 7;
	    BYTE : 8;
	    BYTE group : 1;
	} multicast;
    } natural_order;			/*  ETH normally, TRN for 68k  */
    struct STRUCT_I_NA_FUNCTIONAL
      {
	WORD prefix;
	FUNCTIONAL_ADDRESS addr;
    } fun;
    struct STRUCT_I_NA_GROUP
      {
	WORD prefix;
	GROUP_ADDRESS addr;
    } group;
};



#endif					/* ifndef NODEADDR_INCLUDED */


/*  ========================  End of NODEADDR.CON  ============================*/
