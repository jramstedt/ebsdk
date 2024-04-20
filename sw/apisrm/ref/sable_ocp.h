/* file:	sable_ocp.h                                                 	*/
/*                                                                          	*/
/* Copyright (C) 1993 by                                          		*/
/* Digital Equipment Corporation, Maynard, Massachusetts.                   	*/
/* All rights reserved.                                                     	*/
/*                                                                          	*/
/* This software is furnished under a license and may be used and copied    	*/
/* only  in  accordance  of  the  terms  of  such  license  and with the    	*/
/* inclusion of the above copyright notice. This software or  any  other    	*/
/* copies thereof may not be provided or otherwise made available to any    	*/
/* other person.  No title to and  ownership of the  software is  hereby    	*/
/* transferred.                                                             	*/
/*                                                                          	*/
/* The information in this software is  subject to change without notice    	*/
/* and  should  not  be  construed  as a commitment by digital equipment    	*/
/* corporation.                                                             	*/
/*                                                                          	*/
/* Digital assumes no responsibility for the use  or  reliability of its    	*/
/* software on equipment which is not supplied by digital.                  	*/
/*                                                                          	*/
/*                                                                          	*/
/* Abstract:	Sable OCP definitions.             				*/
/*                                                                          	*/
/* Author:	Marco Ciaffi							*/
/*                                                                          	*/
/* Modifications:                                                           	*/
/*                                                                          	*/
/*                                                                          	*/
#define OCP_WRITE_ENABLE	0x10	/* write enable signal <WR> for the control buffer	*/
#define OCP_ADDRESS_MASK	0x07	/* address bits <A0:A2> for the control buffer		*/
#define OCP_MODE_SELECT		0x08	/* mode selecet signal <A3> for the control buffer	*/
#define OCP_BANK_SELECT		0x20	/* bank select signal <CE> for the control buffer	*/
#define OCP_EXPNDR_OFFSET	0x00	/* offset value for reading/writing the OCP expanders	*/
#define OCP_TYPE		0x02	/* Expander signal which defines OCP type	   	*/
