/*

 Copyright (C) 1990-1996 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel, Onno van der Linden, George Petrov and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 * MVS specific things
 */
#include "zip.h"
#include "mvs.h"

static int gen_node( DIR *dirp, RECORD *recptr )
{
   char *ptr, *name, ttr[TTRLEN];
   int skip, count = 2;
   unsigned int info_byte, alias, ttrn;
   struct dirent *new;

   ptr = recptr->rest;
   while (count < recptr->count) {
      if (!memcmp( ptr, endmark, NAMELEN ))
         return 1;
      name = ptr;                    /* member name */
      ptr += NAMELEN;
      memcpy( ttr, ptr, TTRLEN );    /* ttr name    */
      ptr += TTRLEN;
      info_byte = (unsigned int) (*ptr);   /* info byte */
      if ( !(info_byte & ALIAS_MASK) ) {   /* no alias  */
         new = malloc( sizeof(struct dirent) );
         if (dirp->D_list == NULL)
            dirp->D_list = dirp->D_curpos = new;
         else
            dirp->D_curpos = (dirp->D_curpos->d_next = new);
         new->d_next = NULL;
         memcpy( new->d_name, name, NAMELEN );
         new->d_name[NAMELEN] = '\0';
         if ((name = strchr( new->d_name, ' ' )) != NULL)
            *name = '\0';      /* skip trailing blanks */
      }
      skip = (info_byte & SKIP_MASK) * 2 + 1;
      ptr += skip;
      count += (TTRLEN + NAMELEN + skip);
   }
   return 0;
}

DIR *opendir(const char *dirname)
{
   int bytes, list_end = 0;
   DIR *dirp;
   FILE *fp;
   RECORD rec;

   fp = fopen( dirname, "rb" );
   if (fp != NULL) {
      dirp = malloc( sizeof(DIR) );
      if (dirp != NULL) {
         dirp->D_list = dirp->D_curpos = NULL;
         strcpy( dirp->D_path, dirname );
         do {
            bytes = fread( &rec, 1, sizeof(rec), fp );
            if (bytes == sizeof(rec))
               list_end = gen_node( dirp, &rec );
         } while (!feof(fp) && !list_end);
         fclose( fp );
         dirp->D_curpos = dirp->D_list;
         return dirp;
      }
      fclose( fp );
   }
   return NULL;
}

struct dirent *readdir(DIR *dirp)
{
   struct dirent *cur;

   cur = dirp->D_curpos;
   dirp->D_curpos = dirp->D_curpos->d_next;
   return cur;
}

void rewinddir(DIR *dirp)
{
   dirp->D_curpos = dirp->D_list;
}

int closedir(DIR *dirp)
{
   struct dirent *node;

   while (dirp->D_list != NULL) {
      node = dirp->D_list;
      dirp->D_list = dirp->D_list->d_next;
      free( node );
   }
   free( dirp );
   return 0;
}

local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e;

  e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}

int procname(n)
char *n;                /* name to process */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  DIR *d;               /* directory stream from opendir() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0);
  else if (LSSTAT(n, &s))
  {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname))
      {
        z->mark = pcount ? filter(z->zname) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->name);
        m = 0;
      }
    }
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0)) != ZE_OK)
      return m;
  } else {
    /* Add trailing / to the directory name */
    if ((p = malloc(strlen(n)+2)) == NULL)
      return ZE_MEM;
    if (strcmp(n, ".") == 0) {
      *p = '\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      strcpy(p, n);
      a = p + strlen(p);
      if (a[-1] != '/')
        strcpy(a, "/");
      if (dirnames && (m = newname(p, 1)) != ZE_OK) {
        free((zvoid *)p);
        return m;
      }
    }
    /* recurse into directory */
    if (recurse && (d = opendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if (strcmp(e, ".") && strcmp(e, ".."))
        {
          if ((a = malloc(strlen(p) + strlen(e) + 1)) == NULL)
          {
            closedir(d);
            free((zvoid *)p);
            return ZE_MEM;
          }
          strcat(strcpy(a, p), e);
          if ((m = procname(a)) != ZE_OK)   /* recurse on name */
          {
            if (m == ZE_MISS)
              zipwarn("name not matched: ", a);
            else
              ziperr(m, a);
          }
          free((zvoid *)a);
        }
      }
      closedir(d);
    }
    free((zvoid *)p);
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}
