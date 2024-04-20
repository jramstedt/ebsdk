/* file:	alphamm_def.h
/*
/* Copyright (C) 1991 by
/* Digital Equipment Corporation, Maynard, Massachusetts.
/* All rights reserved.
/*
/* This software is furnished under a license and may be used and copied
/* only  in  accordance  of  the  terms  of  such  license  and with the
/* inclusion of the above copyright notice. This software or  any  other
/* copies thereof may not be provided or otherwise made available to any
/* other person.  No title to and  ownership of the  software is  hereby
/* transferred.
/*
/* The information in this software is  subject to change without notice
/* and  should  not  be  construed  as a commitment by digital equipment
/* corporation.
/*
/* Digital assumes no responsibility for the use  or  reliability of its
/* software on equipment which is not supplied by digital.
/*
/*
/* Abstract:	ALPHA memory management data structure definitions.
/*
/* Author:	jds - Jim Sawin
/*
/* Modifications:
/*
/*  1	by jds, Wed Feb 27 10:19:27 1991
/*	Initial entry.
 */

/* Virtual address (VA) formats: */

#define VA_V_BWP 0
#define VA_M_BWP 0x00001FFF
#define VA_S_BWP 13
#define VA_S_SEG 10
#define VA_V_SEG3 (VA_V_BWP + VA_S_BWP)
#define VA_V_SEG2 (VA_V_SEG3 + VA_S_SEG)
#define VA_V_SEG1 (VA_V_SEG2 + VA_S_SEG)
#define VA_V_SEG0 (VA_V_SEG1 + VA_S_SEG)
#define VA_M_SEG 0x000003FF

struct VA_8K {					/* 8K page size */
	unsigned long bwp	:13;		/* Byte Within Page */
	unsigned long seg3	:10;		/* Segment 3 index */
	unsigned long seg2	:10;		/* Segment 2 index */
	unsigned long seg1	:10;		/* Segment 1 index */
	unsigned long seg0	:10;		/* Segment 0 index */
	unsigned long filler	:11;
};

struct VA_16K {					/* 16K page size */
	unsigned long bwp	:14;		/* Byte Within Page */
	unsigned long seg3	:11;		/* Segment 3 index */
	unsigned long seg2	:11;		/* Segment 2 index */
	unsigned long seg1	:11;		/* Segment 1 index */
	unsigned long seg0	:11;		/* Segment 0 index */
	unsigned long filler	:6;
};

struct VA_32K {					/* 32K page size */
	unsigned long bwp	:15;		/* Byte Within Page */
	unsigned long seg3	:12;		/* Segment 3 index */
	unsigned long seg2	:12;		/* Segment 2 index */
	unsigned long seg1	:12;		/* Segment 1 index */
	unsigned long seg0	:12;		/* Segment 0 index */
	unsigned long filler	:1;
};

struct VA_64K {					/* 64K page size */
	unsigned long bwp	:16;		/* Byte Within Page */
	unsigned long seg3	:13;		/* Segment 3 index */
	unsigned long seg2	:13;		/* Segment 2 index */
	unsigned long seg1	:13;		/* Segment 1 index */
	unsigned long seg0	:9;		/* Segment 0 index */
};

struct PTE {
	unsigned long V	:1;			/* Valid */
	unsigned long FOR	:1;		/* Fault on Read */
	unsigned long FOW	:1;		/* Fault on Write */
	unsigned long FOE	:1;		/* Fault on Execute */
	unsigned long ASM	:1;		/* Address Space Match */
	unsigned long GH	:2;		/* Granularity Hint */
	unsigned long res0	:1;		/* Reserved */
	unsigned long KRE	:1;		/* Kernel Read Enable */
	unsigned long ERE	:1;		/* Executive Read Enable */
	unsigned long SRE	:1;		/* Supervisor Read Enable */
	unsigned long URE	:1;		/* User Read Enable */
	unsigned long KWE	:1;		/* Kernel Write Enable */
	unsigned long EWE	:1;		/* Executive Write Enable */
	unsigned long SWE	:1;		/* Supervisor Write Enable */
	unsigned long UWE	:1;		/* User Write Enable */
	short int res1;				/* Reserved */
	unsigned int pfn;			/* Page Frame Number */
};
