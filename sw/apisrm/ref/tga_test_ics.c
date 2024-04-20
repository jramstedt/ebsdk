#include   "cp$src:platform.h"
/*
 *        Copyright (c) Digital Equipment Corporation, 1990
 *        All Rights Reserved.  Unpublished-rights reserved
 *        under the copyright laws of the United States.
 *
 *        The software contained on this media is proprietary to"
 *        and embodies confidential technology of Digital.  Possession,
 *        use, duplication or dissemination of the software and media
 *        is authorized only pursuant to a valid written license from
 *        Digital.
 *
 *        RESTRICTED RIGHTS LEGEND  Use, duplication, or disclosure"
 *        by the U.S. Government is subject to restrictions as set 
 *        forth in subparagraph (c)(1)(ii) of DFARS 252.227-7013, or 
 *        in FAR 52.227-19, as applicable.
 */

/*
 *++
 *  FACILITY:
 *
 *      Color Frame Buffer Test Routines 
 *
 *  ABSTRACT:
 *
 *      BT459 RAMDAC ICS utility 
 *
 *  AUTHORS:
 *
 *      James Peacock
 *      ICS1562 utility used from ICS manufacturer - 
 *
 --*/

/**
** INCLUDES
**/

#include "cp$src:tga_sfbp_include.h"
#include   "cp$src:platform.h"
#include "cp$src:tga_sfbp_callbacks.h"
#include "cp$src:tga_sfbp_prom_defines.h"
#include "cp$src:tga_sfbp_def.h"
#include "cp$src:tga_sfbp_protos.h"
#include "cp$src:tga_sfbp_463.h"
#include "cp$src:tga_sfbp_485.h"
#include "cp$src:tga_sfbp_ext.h"
#include "cp$src:tga_sfbp_crb_def.h"
#include "cp$src:tga_sfbp_ctb_def.h"

#ifdef  ICS_NEEDED

int InitialInformation(void);
int CalculateValues(void);
CalculateDividers(void);
int DetermineCoefficients(void);
int CalculateGain(void);
int DetermineNaturalFrequency(void);
int CalculateN1Modulus(void);
int OutputSingleBit(int,int,int);
int LogData(void);
int PrintData(void);
int InitializeSelections(void);
int GetLegalValue( int, int, int );
int IntValue( char *, int * );
int ObtainPostScaler(void);
int ConvertToBinary( int, int *, int);

typedef struct {
	int ReferenceDivider;
	int FeedbackDivider;
	int ACounter;
	int MCounter;
	float Error;
	float ActualFrequency;
	float NaturalFrequency;
	float Difference;
	} DataStruct;

#define TARGET_NAT_FREQ 15000

#define YES 1

#define NO 0

#define UNKNOWN "UNKNOWN"

#define NUMBER_OF_SELECTIONS 5

#define NUMBER_OF_BITS 56

#define POSTSCALER_SIZE 2
#define N1_SIZE 3
#define N2_SIZE 9
#define GAIN_SIZE 3
#define PHASE_GAIN_SIZE 2
#define M_COUNTER_SIZE 6
#define A_COUNTER_SIZE 4
#define REFERENCE_DIVIDER_SIZE 7

DataStruct  Selection[NUMBER_OF_SELECTIONS+1];

float   Error = 0.0;

double  ActualFrequency = 0.0;

float ReferenceFrequency = 14318180;

float TargetFrequency = 130808000;

float NaturalFrequency = 0.0;

float Accuracy = .1;

int ACounter = 0;

int MCounter = 0;

int ReferenceDivider = 0;

int FeedbackDivider = 0;

int DoubleFrequency = NO;

int N1Modulus = 0;

int N2Modulus = 0;

int Gain = 0;

int PhaseComparatorGain = 2;

int PhaseShift = YES;

int Bit11 = 0;

int Bit12 = 0;

int HoldBits[NUMBER_OF_BITS];

char DummyBuffer[80];

int PortAddress = 0;

#endif


/*+
* ===========================================================================
* = sfbp$$ics_test - ICS Utility =
* ===========================================================================
*
* OVERVIEW:
*       ICS Utility
* 
* FORM OF CALL:
*       sfbp$$ics_test (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       scia_ptr    - shared driver pointer
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       This test will be used to determine the oscillator frequency.
*       This will stay in a loop calling the configure video routine
*       to set up the oscillator. This will be used to verify the
*       various switch positions without having to type init each time
*       at the console. Run this and tweek the screwdriver on the switch.
*
--*/
int     test_ics (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{

register int  first=1,last_config,config,done;

/* This reads config data and resets video */
/* Whenever it changes                     */
/* Any key typed gets out out              */

#ifndef HX
last_config = (*gfx_func_ptr->pv_sfbp_read_rom)(scia_ptr,0);
for (done=0;!done;)
 {
 config = (*gfx_func_ptr->pv_sfbp_read_rom)(scia_ptr,0);
 if ((config != last_config) || first )
  {
  (*cp_func_ptr->pv_init_output)(scia_ptr,data_ptr);
  printf("\n\tRom Data = 0x%x --> %d Mhz %d HZ ",
         config,
         data_ptr->sfbp$r_mon_data.sfbp$l_sold_freq,         
         data_ptr->sfbp$r_mon_data.sfbp$l_refresh_rate);         
  last_config = config;
  first       = 0;
  }
 done = io_poll();
 if (done)printf ("\n");
 }
#endif

return (1);
}



/*+
* ===========================================================================
* = sfbp$$ics_test_configure - ICS Utility =
* ===========================================================================
*
* OVERVIEW:
*       ICS Utility
* 
* FORM OF CALL:
*       sfbp$$ics_test_configure (argc,argv,ccv,scia_ptr,data_ptr,param)
*
* ARGUMENTS:
*       scia_ptr    - shared driver pointer
*       argc        - argument count
*       argv        - argument array
*       ccv         - callback vector
*       data_ptr    - driver data pointer
*       param       - parameter array for error information
*
* FUNCTIONAL DESCRIPTION:
*       This is is the utility from ICS floppy utility.
*
--*/
int     test_ics_configure (int argc,char **argv,CCV *ccv,SCIA *scia_ptr, SFBP *data_ptr,int param[])

{
#ifdef  ICS_NEEDED

#ifndef HX
 int Finished = NO,Choice = 0,Count = 0;

 for( Count = 0; Count < NUMBER_OF_BITS; Count++ )
                HoldBits[Count] = 0;

 InitializeSelections();
 InitialInformation();
 CalculateValues();
#endif
#endif

 return(1);
 }

#ifndef HX
#ifdef  ICS_NEEDED



/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int InitialInformation()
   {
   int  freq;
   
   ReferenceFrequency = 14318000;

   printf("Enter the target VCO frequency (e.g. 130808000) ");

   gets (DummyBuffer);
   
   TargetFrequency = (float) (strtol (DummyBuffer,0,0));

   Accuracy = 0.1;

   return(0);

   }



/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int CalculateValues()
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

int Choice = 0;

/* --------------------- B O D Y   O F   C O D E ------------------- */

   CalculateGain();

   CalculateDividers();

   PrintData();

   printf("Press Number of Option to Download; (0 for NONE ) -- >");

   Choice = GetLegalValue( 0, NUMBER_OF_SELECTIONS, 0 );

   return(0);

   }



/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

CalculateDividers()

{

/* ------------------ A U T O   V A R I A B L E S ------------------ */

float   PhaseDetectorFrequency = 0.0;

int     ReferenceDividerOffset = 0;

float   LowValue = 0.0;

float   HighValue = 0.0;

float   LowError = 0.0;

float   HighError = 0.0;

int     DivBy2 = 200;

/* --------------------- B O D Y   O F   C O D E ------------------- */

     if( TargetFrequency >= DivBy2 )
		DoubleFrequency = YES;

	for( FeedbackDivider = 2; FeedbackDivider < 300; FeedbackDivider++ )
	   {
	   PhaseDetectorFrequency = TargetFrequency/FeedbackDivider;
	   ReferenceDivider = ReferenceFrequency/PhaseDetectorFrequency;

	   if( ( ReferenceDivider >= 1 ) && ( ReferenceDivider < 128 )  )
		{
		LowValue = ReferenceFrequency/ReferenceDivider;
		HighValue = ReferenceFrequency/( ReferenceDivider +1  );

		LowError = (PhaseDetectorFrequency-LowValue)/PhaseDetectorFrequency;
		HighError = (PhaseDetectorFrequency-HighValue)/PhaseDetectorFrequency;

		if( HighError < -LowError )
		   {
		   Error = HighError;
		   ReferenceDividerOffset = 1;
		   }
		else
		   {
		   Error = LowError;
		   ReferenceDividerOffset = 0;
		   }

		if( Error < 0.0 )
		   Error = -Error;

		Error = Error * 100.0;

		if( Error <= Accuracy )
		   {
		   ReferenceDivider = ReferenceDivider + ReferenceDividerOffset;
		   ActualFrequency  = ReferenceFrequency / ReferenceDivider;
		   ActualFrequency  = ActualFrequency * FeedbackDivider;

		   LogData();
		  }
	      }

	}

   return(0);

   }



/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int DetermineCoefficients()
  {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

   int Count;

   int Finished = NO;

   int HighValue = 7;

   int LowValue = 6;


/* --------------------- B O D Y   O F   C O D E ------------------- */

   if( DoubleFrequency )
	{
	HighValue = 14;
	LowValue = 12;
	}

   Count = FeedbackDivider;

   ACounter = 0;
   MCounter = 0;

   if( ( Count % HighValue ) == 0 )
	{
	ACounter = 0;
	MCounter = ( Count/HighValue ) - 1;
	Finished = YES;
	}

   while( ( Count > LowValue ) && ( !Finished ) )
	{
	Count = Count - HighValue;
	ACounter = ACounter + 1;

	if( ( Count % LowValue ) == 0)
		{
		MCounter = ACounter + ( Count / LowValue ) - 1;
		Finished = YES;
		}
	}


    if( !Finished )
	{
	MCounter = 0;
	ACounter = 0;
	return(NO);
	}

    return(YES);

    }




/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int CalculateGain()
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

/* --------------------- B O D Y   O F   C O D E ------------------- */
   Gain = 7;

   if( TargetFrequency < 250.1 )
	Gain = 6;
   if( TargetFrequency < 200.1 )
	Gain = 5;
   if( TargetFrequency < 120.1 )
	Gain = 4;

   return(Gain);

   }

/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int DetermineNaturalFrequency()
  {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

double DetectCurrent = 2.0e-6;

static int GainVco[8] =  { 8, 13, 21, 26, 30, 50, 70, 90 };

double  Cap1 = 80.0e-12;

double Result;

double Temp1;
double Temp2;
double Temp3;
double Temp4;

double Pi = 3.1415927;

/* --------------------- B O D Y   O F   C O D E ------------------- */

/* sqrt ( ( I_det * Kvco(* 1000000) ) / ( C1 * Total_FeedBack  ) ) / 2 * pi */


				/* detect_current * 10e+6 * gain_vco[gain] */
  Temp1 = DetectCurrent * GainVco[Gain] * 1000000;

  Temp2 = Cap1 * FeedbackDivider;

  Temp3 = Temp1 / Temp2;

  /* Temp4 = sqrt(Temp3);       */

  Result = Temp4 / (2 * Pi );

  return(Result);
  }



/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int CalculateN1Modulus()
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

 char Buffer[5];


/* --------------------- B O D Y   O F   C O D E ------------------- */

   printf("\n\n");

   printf("Enter Code for N1 Divider Control (3, 4a, 4b, 5, 6, 8a, 8b or 10) --> ");

   gets( Buffer );

   if( strncmp( Buffer, "3",1  )  == 0)N1Modulus = 0;
   if( strncmp( Buffer, "4a",2 )  == 0)N1Modulus = 1;
   if( strncmp( Buffer, "4b",2 )  == 0)N1Modulus = 2;
   if( strncmp( Buffer, "5" ,1 )  == 0)N1Modulus = 3;
   if( strncmp( Buffer, "6" ,1 )  == 0)N1Modulus = 4;
   if( strncmp( Buffer, "8a",2 )  == 0)N1Modulus = 5;
   if( strncmp( Buffer, "8b",2 )  == 0)N1Modulus = 6;
   if( strncmp( Buffer, "10",2 )  == 0)N1Modulus = 7;

   printf("\n");

   return(0);

   }




/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int LogData()
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

int Count = 0;

float Difference = 0.0;

/* --------------------- B O D Y   O F   C O D E ------------------- */

   if( DetermineCoefficients() )
      {
      NaturalFrequency = DetermineNaturalFrequency();

      Difference = TARGET_NAT_FREQ - NaturalFrequency;

      if( Difference < 0 )
      	Difference = -Difference;

      for( Count = NUMBER_OF_SELECTIONS-1; Count >= 0; Count-- )
         {
      	if( Difference < Selection[Count].Difference )
	         {
      	   Selection[Count+1].ReferenceDivider = Selection[Count].ReferenceDivider;
	         Selection[Count+1].FeedbackDivider = Selection[Count].FeedbackDivider;
      	   Selection[Count+1].ACounter = Selection[Count].ACounter;
	         Selection[Count+1].MCounter = Selection[Count].MCounter;
      	   Selection[Count+1].Error = Selection[Count].Error;
	         Selection[Count+1].ActualFrequency = Selection[Count].ActualFrequency;
      	   Selection[Count+1].NaturalFrequency = Selection[Count].NaturalFrequency;
	         Selection[Count+1].Difference = Selection[Count].Difference;

      	   Selection[Count].ReferenceDivider = ReferenceDivider;
	         Selection[Count].FeedbackDivider = FeedbackDivider;
      	   Selection[Count].ACounter = ACounter;
	         Selection[Count].MCounter = MCounter;
      	   Selection[Count].Error = Error;
	         Selection[Count].ActualFrequency = ActualFrequency;
      	   Selection[Count].NaturalFrequency = NaturalFrequency;
      	   Selection[Count].Difference = Difference;
	         }
      	}

     }

   return(0);

   }



/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int PrintData()
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

int Count = 0;

/* --------------------- B O D Y   O F   C O D E ------------------- */

  printf( "The target VCO frequency is %d HZ \n", TargetFrequency );
  printf( "Some suggested options are : \n" );

  for( Count = 0; Count < NUMBER_OF_SELECTIONS; Count++ )
	{
	if( Selection[Count].ReferenceDivider | Selection[Count].FeedbackDivider )
	   {
	   printf( "\t Actual Frequency   --> %f \n", Selection[Count].ActualFrequency );
	   printf( "\t Input Divider      --> %d \n", Selection[Count].ReferenceDivider );
	   printf( "\t Feedback Divider   --> %d \n", Selection[Count].FeedbackDivider );
	   printf( "\t A-Counter          --> %d \n", Selection[Count].ACounter );
	   printf( "\t M-Counter          --> %d \n", Selection[Count].MCounter );
	   printf( "\t Error              --> %f \n", Selection[Count].Error );
	   printf( "\t Natural Frequency  --> %f \n", Selection[Count].NaturalFrequency );
	   printf( "\n");
	   }
	}

  printf( "The Target Frequency is %f MHz \n", TargetFrequency );
  printf( "The Gain is %d \n", Gain );
  printf( "Some suggested options are : \n" );

  for( Count = 0; Count < NUMBER_OF_SELECTIONS; Count++ )
	{
	if( Selection[Count].ReferenceDivider | Selection[Count].FeedbackDivider )
	   {
	   printf("\t Option  #%d \n", Count+1 );
	   printf("\t Actual Frequency  --> %f \t Error             --> %5.4f%% \n",
                         Selection[Count].ActualFrequency, Selection[Count].Error  );
	   printf("\t Input Divider     --> %d \t\t Feedback Divider  --> %d \n", 
                        Selection[Count].ReferenceDivider , Selection[Count].FeedbackDivider );
           /* printf("\t A-Counter         --> %d \t M-Counter         --> %d \n", 
                        Selection[Count].ACounter, Selection[Count].MCounter );         */
	   printf("\n");
	   }
	}

  return(0);

  }



/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int InitializeSelections()
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

int Count = 0;

/* --------------------- B O D Y   O F   C O D E ------------------- */
  for( Count = 0; Count < NUMBER_OF_SELECTIONS+1; Count++ )
	{
	Selection[Count].ActualFrequency = 0.0;
	Selection[Count].ReferenceDivider = 0;
	Selection[Count].FeedbackDivider = 0;
	Selection[Count].ACounter = 0;
	Selection[Count].MCounter = 0;
	Selection[Count].Error = 0.0;
	Selection[Count].NaturalFrequency = 0.0;
	Selection[Count].Difference = TARGET_NAT_FREQ;
	}

  return(0);

  }




/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int GetLegalValue( MinBoundary, MaxBoundary, DefaultValue )
   int MinBoundary;
   int MaxBoundary;
   int DefaultValue;
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

   int Valid = NO;

   int Value = 0;

   int Legal = NO;

/* --------------------- B O D Y   O F   C O D E ------------------- */

  while( !Valid )
      {
      gets( DummyBuffer);

      printf("\n" );

      if( DummyBuffer[0] == '\0' )
	 return(DefaultValue);
      else
	 {
	 Legal = IntValue( DummyBuffer, &Value );
	 if( Legal )
	   Valid = ( ( Value >= MinBoundary ) && ( Value <= MaxBoundary ) );

	 if( !Valid )
	   {
	   printf("\t The Value must be between %d and %d \n", MinBoundary, MaxBoundary );
	   printf("\n" );
	   printf("Enter a New Value -- > " );
	   }
	 }
      }

   return(Value);

   }



int IntValue( Str, IntPtr )
   char *Str;
   int *IntPtr;
   {

   int Length = 0;
   int Count = 0;

   int Total = 0;
   int ValidNumber = NO;

   Length = strlen( Str );

   while( ( Str[Count] == ' ') && (Count < Length) )
	Count++;

   while( ( isdigit(Str[Count]) ) && (Count < Length) )
	{
	Total = (Total  * 10) + ( Str[Count] - 0x30  );
	ValidNumber = YES;
	Count++;
	}

   *IntPtr = Total;

   return(ValidNumber);

   }


/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int ObtainPostScaler()
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

int Response = 0;

int PostDivider = 0;

/* --------------------- B O D Y   O F   C O D E ------------------- */
  

   printf("\t Enter Post Scaler Value (1, 2, 4 or 9 (AUX) ) --> ");

   Response = GetLegalValue( 1, 9, 1 );

   switch(Response)
	{
	case 1:
	   PostDivider = 0;
	   break;

	case 2:
	   PostDivider = 1;
	   break;

	case 4:
	   PostDivider = 2;
	   break;

	case 9:
	   PostDivider = 3;
	   break;

	default:
	   printf("Illegal Selection.  Post Divider set to 1 \n");
	   PostDivider = 0;

	} /* End Switch */

   return(PostDivider);

   }

/* ------------- F U N C T I O N   D E F I N I T I O N ------------- */

int ConvertToBinary( int Value, int *StartPosition, int Size )
   {

/* ------------------ A U T O   V A R I A B L E S ------------------ */

   int Finished = NO;
   int Position = 0;
   int Count;
   int Temp;

/* --------------------- B O D Y   O F   C O D E ------------------- */

   Position = *StartPosition;

   for( Count = 0; Count < Size; Count++ )
      HoldBits[Position++] = 0;

   Position = *StartPosition;

   while( !Finished )
      {
      Temp = Value % 2;
      
      if( Temp ) 
         HoldBits[Position] = 1;

      Value = Value / 2;

      Position++;

      if( Value == 0 )
         Finished = YES;
      }

   *StartPosition += Size;

   return(0);

   }

int isdigit (char c)
{
return ( (c >= 0x30 && c <= 0x39 ) ? 1 : 0);
}


#endif
#endif
