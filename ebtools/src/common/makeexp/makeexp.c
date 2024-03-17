/*****************************************************************************

       Copyright 1993, 1994, 1995 Digital Equipment Corporation,
                       Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the copyright notice and this permission notice appear in all copies
of software and supporting documentation, and that the name of Digital not
be used in advertising or publicity pertaining to distribution of the software
without specific, written prior permission. Digital grants this permission
provided that you prominently mark, as not part of the original, any
modifications made to this software or documentation.

Digital Equipment Corporation disclaims all warranties and/or guarantees
with regard to this software, including all implied warranties of fitness for
a particular purpose and merchantability, and makes no representations
regarding the use of, or the results of the use of, the software and
documentation in terms of correctness, accuracy, reliability, currentness or
otherwise; and you rely on the software, documentation and results solely at
your own risk.

******************************************************************************/

#ifndef LINT
static char *rcsid = "$Id: makeexp.c,v 1.1.1.1 1998/12/29 21:36:23 paradis Exp $";
#endif

/*
**++
**  FACILITY:	Make file preprocessor
**
**  MODULE DESCRIPTION:
**
**  This program preprocess a make file allowing a single make script to be
**  used on both UNIX and NT programming environments.  This is made possible
**  by adding the few listed commands to the make syntax and not using any
**  other commands that are not supported by both environments.
**
**  The test clause effectively includes only the value set that matches the
**  value of the test macro.  ONLY macro definitions may be listed in the 
**  body of the VALUE group.
**
**	!TEST <macro name>
**	!VALUE <macro value>	    one or more sets of value/definitions
**	    Macro definitions to execute if the TEST macro has the current VALUE
**	!ENDTEST
**
**  The testdef clause sets the definition of the change macro  based on
**  wether the test macro is defined or not.
**
**	!TESTDEF <test macro name> <change macro name> ? <defined value> : <undefined value>
**
**  Include will add the extension .nt or .osf depending on the platform.  
**  The included file is not preprocessed.
**
**	!INCLUDE <file name>
**
**  longer used and is replaced by a huffman encoding algorithm that is more
**  efficient and is unencumbered.
**
**  AUTHOR:	Peter J. Bell
**
**  CREATION DATE:  22-Oct-1997
**
**  MODIFICATION HISTORY:
** $Log: makeexp.c,v $
** Revision 1.1.1.1  1998/12/29 21:36:23  paradis
** Initial CVS checkin
**
 * Revision 1.2  1997/11/19  21:52:18  pbell
 * Cleaned up a unix version error.
 *
 * Revision 1.1  1997/10/22  12:10:50  pbell
 * Initial revision
 *
**
*/


#include <stdio.h>

char * Strcpyn( char * Dst, char * Src, int Len )
{
    strncpy( Dst, Src, Len-1 );
    Dst[Len-1] = 0;
}

/* note that the line buffer must be large enough to hold a line
   after all continuations have been merged. */
#define MAX_LINE	4096
#define MAX_VARS	50
#define MAX_VARLEN	128
#define MAX_VALUELEN	128

#define KEY_TEST	"!TEST"
#define KEY_VALUE	"!VALUE"
#define KEY_END		"!ENDTEST"

#define KEY_TESTDEF	"!TESTDEF"
#define KEY_INCLUDE_VER "!INCLUDE"

int  LineNumber=0;
char LineBuffer[MAX_LINE];
char ItemList[MAX_VARS][MAX_VARLEN];

int  Items;
char * LinePos;

int GetLine( void )
{
    int LineLen;
    LinePos = fgets( LineBuffer, MAX_LINE, stdin );
    LineNumber++;
    while( LinePos != NULL && (LineLen = strlen(LineBuffer)) >= 2 &&
	   LineBuffer[LineLen-2] == '\\' )
    {
	LineLen-=2; /* remove the continuation character */
	LineBuffer[LineLen] = 0;
	LinePos = fgets( LineBuffer+LineLen, MAX_LINE-LineLen, stdin );
	LineNumber++;
    }
    return( LinePos != NULL );
}

void WriteLine( void )
{
    if( LinePos != NULL )
	fputs( LinePos, stdout );
}


void SkipWS( void )
{
    if( LinePos == NULL )
	return;
    while( isspace( *LinePos ) ) LinePos++;
    if( *LinePos == 0 )
	LinePos = NULL;
}


int LineStarts( char * Value )
{
    return( LinePos != NULL && strncmp( Value, LineBuffer, strlen( Value ) ) );
}


void LineValue( char * Value )
{
    char * Pos;

    Value[0] = 0;
    if( LinePos == NULL )
	return;
    while( *LinePos != 0 && *LinePos != '=' && !isspace(*LinePos) ) LinePos++;
    while( isspace(*LinePos) ) LinePos++;
    Strcpyn( Value, LinePos, MAX_VALUELEN );
    Pos = Value + strlen( Value );
    for( Pos--; isspace( *Pos ); Pos-- ) Pos = 0;
}

void GetName( char * Name )
{
    char * Pos;

    Name[0] = 0;
    if( LinePos == NULL )
	return;
    while( isspace(*LinePos) ) LinePos++;
    Pos = LinePos;
    while( *LinePos != 0 && *LinePos != '=' && !isspace(*LinePos) &&
	   *LinePos != '?' && *LinePos != ':' )
	LinePos++;
    if( Pos == LinePos )
	return;
    Strcpyn( Name, Pos, (LinePos - Pos) + 1 );
}


void GetExp( char * Exp, char LeaderChar )
{
    char * Pos;

    Exp[0] = 0;
    if( LinePos == NULL )
	return;
    while( isspace(*LinePos) ) LinePos++;
    if( LeaderChar != 0 && *LinePos != LeaderChar )
    {
	fprintf( stderr, "\nERROR(%d) Syntax error looking for '%c'\n",
		 LineNumber, LeaderChar );
	/* the following code geneartes a make syntax error to terminate the build */
#	ifdef _WIN32
	printf( "$(JUNK$(JUNK$(JUNK)))\n" );
#	else
	printf( "!IFDEF JUNK" );
#	endif
	return;
    }

    LinePos++;
    while( isspace(*LinePos) ) LinePos++;
    Pos = LinePos;
    while( *LinePos != 0 && *LinePos != '\n' && *LinePos != '\r' &&
	   *LinePos != '?' && *LinePos != ':' )
	LinePos++;
    if( Pos == LinePos )
	return;
    Strcpyn( Exp, Pos, (LinePos - Pos) + 1 );
}


void WriteValue( char * Item )
{
    char Value[MAX_VALUELEN];
    SkipWS();
    LineValue( Value );
#ifdef _WIN32
    printf( "!IF %s == %s\n", Item, Value );

    while( GetLine() && !LineStarts(KEY_VALUE) && !LineStarts(KEY_END) )
	WriteLine();

    printf( "!ENDIF" );
#else	/* Unix */
    while( GetLine() && !LineStarts(KEY_VALUE) && !LineStarts(KEY_END) )
    {
	GetName( ItemList[Items] );
	fputs( stdout, ItemList[Items] );
	fputs( stdout, Value );
	WriteLine();
	Items++;
    }
#endif
}

void WriteTA( void )
{
    char Item[MAX_VALUELEN];

    Items = 0;
    LineValue( Item );
    GetLine();

    while( LineStarts(KEY_VALUE) )
	WriteValue( Item );

#ifndef _WIN32
    {
	int i;
	for( i=0; i<Items; i++ )
	    printf( "%s=$(%s$(%s)?$(%s(%s)):$(%s)\n",
		    ItemList[i],ItemList[i],Item,ItemList[i],Item,ItemList[i] );
    }
#endif
}

void WriteTD( void )
{
    char Item[MAX_VALUELEN];
    char define[MAX_VALUELEN];
    char V1[MAX_LINE];
    char V2[MAX_LINE];
    GetName( Item );
    GetName( define );
    GetExp( V1, '?' );
    GetExp( V2, ':' );

#ifdef _WIN32
    printf( "!IFDEF %s\n%s=%s\n!ELSE\n%s=%s\n!ENDIF\n",
	    Item, define, V1, define, V2 );
#else
    printf( "%s=$(%s?%s:%s)\n", define, Item, V1, V2 );
#endif
}

void WriteInclude( void )
{
    char Include[MAX_VALUELEN];
    GetName( Include );

#ifdef _WIN32
    printf( "include %s.nt\n", Include );
#else
    printf( "include %s.osf\n", Include );
#endif
}


main()
{
    while( GetLine() )
    {
	if( LineStarts( KEY_TEST ) )
	    WriteTA();
	else if( LineStarts( KEY_TESTDEF ) )
	    WriteTD();
	else if( LineStarts( KEY_INCLUDE_VER ) )
	    WriteInclude();
	else
	    WriteLine();
    }
}


