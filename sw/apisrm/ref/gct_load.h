/*** MODULE GCT_LOAD ***/
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
#ifndef _GCT_LOAD_H_
#define _GCT_LOAD_H_

#define GCT__GENERIC     0
#define GCT__COBRA       2
#define GCT__TURBOLASER  12
#define GCT__AVANTI      13
#define GCT__LYNX        24
#define GCT__WILDFIRE    35

/*
 *  Database of "known" devices
 */
typedef struct _gct_machine {
  GCT_PLATFORM_DATA *machine_data;
  GCT_BINDINGS      *machine_bindings;
} GCT_MACHINE;

#ifndef MACHINE_SYMBOLS_ONLY

/*
 *   cons
 *   base_alloc
 *   base_align
 *   min_alloc
 *   min_align
 *   max_part
 *   max_frag
 *   max_desc
 *   name
 *   system type cod
 */

static GCT_PLATFORM_DATA wildfire_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  ONE_MEG,
  ONE_MEG,
  8,
  16,
  8,
  "Wildfire",
  GCT__WILDFIRE 
};

static GCT_PLATFORM_DATA TL_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  TWO_MEG,
  TWO_MEG,
  4,
  16,
  7,
  "TurboLaser",
  GCT__TURBOLASER
};

static GCT_PLATFORM_DATA cobra_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  ONE_MEG,
  ONE_MEG,
  1,
  16,
  1,
  "Cobra",
  GCT__COBRA
};

static GCT_PLATFORM_DATA lynx_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  ONE_MEG,
  ONE_MEG,
  2,
  16,
  3,
  "Lynx",
  GCT__LYNX
};

static GCT_PLATFORM_DATA avanti_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  ONE_MEG,
  ONE_MEG,
  1,
  16,
  1,
  "Avanti",
  GCT__AVANTI
};

static GCT_PLATFORM_DATA generic_data[] = {
  TWO_MEG,
  32*ONE_MEG,
  32*ONE_MEG,
  ONE_MEG,
  ONE_MEG,
  1,
  16,
  1,
  "Generic",
  GCT__GENERIC
};

static GCT_BINDINGS TL_bindings[] = {
  {NODE_CPU_MODULE,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_CPU,		NODE_CPU_MODULE,	NODE_HW_ROOT,	NODE_CPU_MODULE},
  {NODE_MEMORY_SUB,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_IOP,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_HOSE,		NODE_IOP,		NODE_IOP,	NODE_IOP},
  {NODE_BUS,		NODE_HOSE,		NODE_HOSE,	NODE_HOSE},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_BUS,	NODE_BUS},
  {0}
};

static GCT_BINDINGS wildfire_bindings[] = {
  {NODE_SBB,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_CPU,		NODE_SBB,		NODE_HW_ROOT,	NODE_SBB},
  {NODE_MEMORY_SUB,	NODE_SBB,		NODE_HW_ROOT,	NODE_SBB},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_SBB},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_SBB},
  {NODE_IOP,		NODE_SBB,		NODE_HW_ROOT,	NODE_SBB},
  {NODE_HOSE,		NODE_IOP,		NODE_IOP,	NODE_IOP},
  {NODE_BUS,		NODE_HOSE,		NODE_HOSE,	NODE_HOSE},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_BUS,	NODE_BUS},
  {0}
};

static GCT_BINDINGS cobra_bindings[] = {
  {NODE_CPU,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_SUB,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_IOP,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_HOSE,		NODE_IOP,		NODE_IOP,	NODE_IOP},
  {NODE_BUS,		NODE_HOSE,		NODE_HOSE,	NODE_HOSE},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_BUS,	NODE_BUS},
  {0}
};

static GCT_BINDINGS generic_bindings[] = {
  {NODE_CPU,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_SUB,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_IOP,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_HOSE,		NODE_IOP,		NODE_IOP,	NODE_IOP},
  {NODE_BUS,		NODE_HOSE,		NODE_HOSE,	NODE_HOSE},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_BUS,	NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_BUS,	NODE_BUS},
  {0}
};

static GCT_BINDINGS lynx_bindings[] = {
  {NODE_CPU,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_SUB,	NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_DESC,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_MEMORY_CTRL,	NODE_MEMORY_SUB,	NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_BUS,		NODE_HW_ROOT,		NODE_HW_ROOT,	NODE_HW_ROOT},
  {NODE_IO_CTRL,	NODE_BUS,		NODE_HW_ROOT,   NODE_BUS},
  {NODE_SLOT,		NODE_BUS,		NODE_HW_ROOT,	NODE_BUS},
  {0}
};

GCT_MACHINE machine_database[] = {
  { &generic_data[0],	&generic_bindings[0]},
  { &TL_data[0],	&TL_bindings[0]},
  { &wildfire_data[0],	&wildfire_bindings[0]},
  { &lynx_data[0],	&lynx_bindings[0]},
  { &cobra_data[0],	&cobra_bindings[0]},
  { &avanti_data[0],	&lynx_bindings[0]},
  { 0,			0}
};

#else

extern GCT_MACHINE machine_database[];

#endif /* MACHINE_SYMBOLS_ONLY */

/* extern  int32 gct_load$platform(char *name);
extern uint64 gct$get_platform(char *name);  */

#endif /* _GCT_LOAD_H_ */
