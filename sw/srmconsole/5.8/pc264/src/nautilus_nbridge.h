/*++

Copyright (c) 1997  Digital Equipment Corporation

Module Name:

    Nautilus_NBridge.h 

Abstract:

    Interface to Nautilus Northbridge.c

Author:

    Dick Bissen     21-July-1998

Revision History:

--*/

#ifndef _NBRIDGE_H_
#define _NBRIDGE_H_


/*
// Configuration space addresses of the M1543C registers accessed here.  See the
// Chip Specification for details.
*/

extern void IrongateInitialize(void);

void NBridgeWritePciConfig(
unsigned long deviceNumber,
unsigned long offset,
unsigned long uLongVal,
unsigned long size);

unsigned long NBridgeReadPciConfig(
unsigned long deviceNumber,
unsigned long offset,
unsigned long size);

#endif

