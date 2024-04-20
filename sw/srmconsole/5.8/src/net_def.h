/*
 * Copyright (C) 1990, 1995 by
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
 * and  should  not  be  construed  as a commitment by Digital Equipment
 * Corporation.
 *
 * Digital assumes no responsibility for the use  or  reliability of its
 * software on equipment which is not supplied by Digital.
 *
 *
 * Abstract:	Net command Definitions
 *
 * Author:	John A. DeNisco
 *
 * Modifications:
 *
 *      dwn     14-Sep-1995	Added NET$K_RSA NET$K_ENV 
 *	pel	23-Jun-1993	add NET$K_STOP
 *	jad	i5-Mar-1990	Initial entry.
*/
 
/*Qualifier constants*/
/*SE and I must be 0 and 1*/
#define NET$K_I 	0
#define NET$K_SA 	1
#define NET$K_S 	2
#define NET$K_L1 	3
#define NET$K_SV 	4
#define NET$K_CM 	5
#define NET$K_IC 	6
#define NET$K_ID 	7
#define NET$K_L 	8
#define NET$K_L0 	9
#define NET$K_LA 	10
#define NET$K_RI 	11
#define NET$K_RB 	12
#define NET$K_ELS 	13
#define NET$K_KLS 	14
#define NET$K_DA 	15
#define NET$K_CSR 	16
#define NET$K_LW 	17
#define NET$K_START 	18
#define NET$K_STOP 	19
#define NET$K_RSA 	20
#define NET$K_ENV 	21

#define NET$K_ARG_MAX 	22

/*Net command defaults*/
#define NET$K_LOOP_COUNT 0		/*Loop Forever*/
