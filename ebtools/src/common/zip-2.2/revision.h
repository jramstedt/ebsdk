/*

 Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 *  revision.h by Mark Adler.
 */

#ifndef __revision_h
#define __revision_h 1

/* For api version checking */
#define Z_MAJORVER   2
#define Z_MINORVER   2
#define Z_PATCHLEVEL 0
#define Z_BETALEVEL ""

#define VERSION "2.2"
#define REVDATE "November 3rd 1997"

#define DW_MAJORVER    Z_MAJORVER
#define DW_MINORVER    Z_MINORVER
#define DW_PATCHLEVEL  Z_PATCHLEVEL

#ifndef WINDLL
/* Copyright notice for binary executables--this notice only applies to
 * those (zip, zipcloak, zipsplit, and zipnote), not to this file
 * (revision.h).
 */

#ifdef NOCPYRT                       /* copyright[] gets defined only once ! */
extern ZCONST char *copyright[3];    /* keep array sizes in sync with number */
extern ZCONST char *disclaimer[9];   /*  of text line in definition below !! */
extern ZCONST char *versinfolines[7];

#else /* !NOCPYRT */

ZCONST char *copyright[] = {

#ifdef TANDEM
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden, and Dave Smith.",
"Type '%s \"-L\"' for software license."
#endif

#ifdef VMS
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden, Christian Spieler and Igor Mandrichenko.",
"Type '%s \"-L\"' for software license."
#endif

#ifdef AMIGA
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden, John Bush and Paul Kienitz.",
"Type '%s -L' for the software License."
#  ifdef AZTEC_C
     ,        /* extremely lame compiler bug workaround */
#  endif
#endif

#ifdef __BEOS__
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden, and Chris Herborth.",
"Type '%s -L' for the software License."
#endif

#ifdef QDOS
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden and Jonathan Hudson.",
"Type '%s -L' for the software License."
#endif

#if defined(__arm) || defined(__riscos) || defined(RISCOS)
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden, Karl Davis and Sergio Monesi.",
"Type '%s \"-L\"' for software Licence."
#endif

#ifdef DOS
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden, Christian Spieler and Kai Uwe Rommel.",
"Type '%s -L' for the software License."
#endif

#ifdef CMS_MVS
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden, George Petrov and Kai Uwe Rommel.",
"Type '%s -L' for the software License."
#endif

#if defined(OS2) || defined(WIN32) || defined(UNIX)
"Copyright (C) 1990-1997 Mark Adler, Richard B. Wales, Jean-loup Gailly,",
"Onno van der Linden and Kai Uwe Rommel.",
"Type '%s -L' for the software License."
#endif
};

ZCONST char *versinfolines[] = {
"This is %s %s (%s), by Info-ZIP.",
"Currently maintained by Onno van der Linden. Please send bug reports to",
"the authors at Zip-Bugs@lists.wku.edu; see README for details.",
"",
"Latest sources and executables are at ftp://ftp.cdrom.com/pub/infozip, as of",
"above date; see http://www.cdrom.com/pub/infozip/Zip.html for other sites",
""
};

ZCONST char *disclaimer[] = {
"",
"Permission is granted to any individual or institution to use, copy, or",
"redistribute this executable so long as it is not modified and that it is",
"not sold for profit.",
"",
"LIKE ANYTHING ELSE THAT'S FREE, ZIP AND ITS ASSOCIATED UTILITIES ARE",
"PROVIDED AS IS AND COME WITH NO WARRANTY OF ANY KIND, EITHER EXPRESSED OR",
"IMPLIED. IN NO EVENT WILL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY DAMAGES",
"RESULTING FROM THE USE OF THIS SOFTWARE."
};
#endif /* !NOCPYRT */
#endif /* !WINDLL */
#endif /* !__revision_h */
