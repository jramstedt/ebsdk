/* file:	alphaps_def.h
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
/* Facility:	Cobra/Ruby/Neon console
/*
/* Abstract:	Alpha Processor Status register.
/*
/* Author:	jds - Jim Sawin
/*
/* Revision History:
/* 
/*  1	by jds, Wed Mar  6 15:10:29 1991
/*	Initial entry.
/*
 */

struct PS
{   unsigned sw		:3;			/* reserved for software */
    unsigned cm		:2;			/* current mode */
    unsigned rsvd1	:2;			/* Reserved; MBZ */
    unsigned vmm	:1;			/* Virtual Machine Monitor */
    unsigned ipl	:5;			/* Interrupt Priority Level */
    unsigned rsvd2	:19;			/* Reserved; MBZ */
    unsigned rsvd3	:24;			/* Reserved; MBZ */
    unsigned sp_align	:6;			/* Stack Alignment (valid after exception or interrupt) */
    unsigned rsvd4	:2;			/* Reserved; MBZ */
};
