/*---------------------------------------------------------------------------

  vmmvs.c (for both VM/CMS and MVS)

  Contains:  vmmvs_open_infile()
             open_outfile()
             find_vms_attrs()
             flush()
             close_outfile()
             close_infile()
             getVMMVSexfield()
             do_wild()
             mapattr()
             mapname()
             checkdir()
             check_for_newer()
             stat()
             version()

  ---------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"


/********************************/
/* Function vmmvs_open_infile() */
/********************************/

FILE *vmmvs_open_infile(__G)
   __GDEF
{
   FILE *fzip;

   G.tempfn = NULL;
   if ((fzip = fopen(G.zipfn,"rb,recfm=fb")) == (FILE *)NULL) {
      size_t cnt;
      char *buf;
      FILE *in, *out;

      if ((buf = (char *)malloc(32768)) == NULL) return NULL;
      if ((G.tempfn = tmpnam(NULL)) == NULL) return NULL;
      if ((in = fopen(G.zipfn,"rb")) != NULL &&
          (out = fopen(G.tempfn,"wb,recfm=fb,lrecl=1")) != NULL) {
         Trace((stdout,"Converting ZIP file to fixed record format...\n"));
         while (!feof(in)) {
            cnt= fread(buf,1,32768,in);
            if (cnt) fwrite(buf,1,cnt,out);
         }
      }
      else {
         free(buf);
         fclose(out);
         fclose(in);
         return NULL;
      }
      free(buf);
      fclose(out);
      fclose(in);

      fzip = fopen(G.tempfn,"rb,recfm=fb");
      if (fzip == (FILE *)NULL) return NULL;

      /* Update the G.ziplen value since it might have changed after
         the reformatting copy. */
      fseek(fzip,0L,SEEK_SET);
      fseek(fzip,0L,SEEK_END);
      G.ziplen = ftell(fzip);
   }

   return fzip;
}


/***************************/
/* Function open_outfile() */
/***************************/

int open_outfile(__G)           /* return 1 if fail */
    __GDEF
{
    char type[100];
    char *mode = NULL;

    if (G.pInfo->textmode)
        mode = FOPWT;
    else {
        if (G.lrec.extra_field_length > EB_HEADSIZE) {
            ush leb_id   = makeword(&G.extra_field[EB_ID]);
            ush leb_dlen = makeword(&G.extra_field[EB_LEN]);

            if ((leb_id == EF_VMCMS || leb_id == EF_MVS) &&
                (leb_dlen <= (G.lrec.extra_field_length - EB_HEADSIZE)) &&
                (getVMMVSexfield(type, G.extra_field, (unsigned)leb_dlen) > 0))
                mode = type;
        }
    }
    if (mode == NULL) mode = FOPW;

    Trace((stderr, "Output file='%s' opening with '%s'\n", G.filename,type));
    if ((G.outfile = fopen(G.filename, mode)) == (FILE *)NULL) {
        Info(slide, 0x401, ((char *)slide, "\nerror:  cannot create %s\n",
             G.filename));
        Trace((stderr, "error %d: '%s'\n", errno, strerror(errno)));
        return 1;
    }
    return 0;
} /* end function open_outfile() */


/****************************/
/* Function close_outfile() */
/****************************/

void close_outfile(__G)
   __GDEF
{
   fclose(G.outfile);
} /* end function close_outfile() */


/****************************/
/* Function close_infile() */
/****************************/

void close_infile(__G)
   __GDEF
{
   fclose(G.zipfd);

   /* If we're working from a temp file, erase it now */
   if (G.tempfn)
      remove(G.tempfn);

} /* end function close_infile() */



/******************************/
/* Function getVMMVSexfield() */
/******************************/

extent getVMMVSexfield(type, ef_block, datalen)
    char *type;
    uch *ef_block;
    unsigned datalen;
{
    fldata_t *fdata = (fldata_t *) &ef_block[4];
    unsigned long lrecl;

    if (datalen < sizeof(fldata_t))
        return 0;

    strcpy(type, "w");
    strcat(type,  fdata->__openmode == __TEXT   ? ""
                 :fdata->__openmode == __BINARY ? "b"
                 :fdata->__openmode == __RECORD ? "b,type=record"
                 : "");
    strcat(type, ",recfm=");
    strcat(type,  fdata->__recfmF? "F"
                 :fdata->__recfmV? "V"
                 :fdata->__recfmU? "U"
                 :                 "?");
    if (fdata->__recfmBlk) strcat(type, "B");
    if (fdata->__recfmS)   strcat(type, "S");
    if (fdata->__recfmASA) strcat(type, "A");
    if (fdata->__recfmM)   strcat(type, "M");
    lrecl = fdata->__recfmV ? fdata->__maxreclen+4
                            : fdata->__maxreclen;
    sprintf(type+strlen(type), ",lrecl=%ld", lrecl);

#ifdef VM_CMS
    /* For CMS, use blocksize for FB files only, otherwise use LRECL */
    if (fdata->__recfmBlk)
       sprintf(type+strlen(type), ",blksize=%ld", fdata->__blksize);
#else
    /* For MVS, always use blocksize */
    sprintf(type+strlen(type), ",blksize=%ld", fdata->__blksize);
#endif

    return strlen(type);
} /* end function getVMMVSexfield() */



#ifndef SFX

/**********************/
/* Function do_wild() */   /* for porting:  dir separator; match(ignore_case) */
/**********************/

char *do_wild(__G__ wld)
    __GDEF
    char *wld;             /* only used first time on a given dir */
{
    static int First = 0;
    static char filename[256];

    if (First == 0) {
       First = 1;
       strcpy( filename, wld );
       return filename;
    }
    else
       return (char *)NULL;

} /* end function do_wild() */

#endif /* !SFX */



/************************/
/*  Function mapattr()  */
/************************/

int mapattr(__G)
     __GDEF
{
    return 0;
}

/************************/
/*  Function mapname()  */
/************************/

int mapname(__G__ renamed)
            /* returns: */
            /* 0 (PK_COOL) if no error, */
            /* 1 (PK_WARN) if caution (filename trunc), */
            /* 2 (PK_ERR)  if warning (skip file because dir doesn't exist), */
            /* 3 (PK_BADERR) if error (skip file), */
            /* 10 if no memory (skip file) */
    __GDEF
    int renamed;
{
    char newname[68], *lbar;
#ifdef MVS
    char *pmember;
#endif
    int name_changed = 0;

#ifdef MVS
    while ((lbar = strrchr(G.filename,'_')) != NULL) {
       strcpy(lbar,(lbar)+1);
       name_changed = 1;
    }
    /* '-' and '+' ARE valid chars for CMS.  --RGH  */
    while ((lbar = strrchr(G.filename,'+')) != NULL) {
       strcpy(lbar,(lbar)+1);
       name_changed = 1;
    }
    while ((lbar = strrchr(G.filename,'-')) != NULL) {
       strcpy(lbar,(lbar)+1);
       name_changed = 1;
    }
#endif

    while ((lbar = strrchr(G.filename,'(')) != NULL) {
       strcpy(lbar,(lbar)+1);
       name_changed = 1;
    }
    while ((lbar = strrchr(G.filename,')')) != NULL) {
       strcpy(lbar,(lbar)+1);
       name_changed = 1;
    }

#ifdef VM_CMS
    if ((lbar = strrchr(G.filename,'/')) != NULL) {
        strcpy((char *)newname,(char *)((lbar)+1));
        printf("WARNING: file '%s' renamed as '%s'\n",G.filename,newname);
        strcpy(G.filename,(char *)newname);
        name_changed = 1;
    }
#else /* MVS */
    if ((pmember = strrchr(G.filename,'/')) == NULL)
        pmember = G.filename;
    else
        pmember++;

    /* search for extension in file name */
    if ((lbar = strrchr(pmember,'.')) != NULL) {
        *lbar++ = '\0';
        strcpy(newname, pmember);
        strcpy(pmember, lbar);
        strcat(pmember, "(");
        strcat(pmember, newname);
        strcat(pmember, ")");
    }

    /* Remove all 'internal' dots '.', to prevent false consideration as
     * MVS path delimiters! */
    while ((lbar = strrchr(G.filename,'.')) != NULL) {
        strcpy(lbar,(lbar)+1);
        name_changed = 1;
    }

    /* Finally, convert path delimiters from internal '/' to external '.' */
    while ((lbar = strchr(G.filename,'/')) != NULL)
        *lbar = '.';
#endif /* ?VM_CMS */

#ifndef MVS
    if ((lbar = strchr(G.filename,'.')) == (char *)NULL) {
        printf("WARNING: file '%s' has NO extension - renamed as '%s.NONAME'\n"\
              ,G.filename,G.filename);
       strcat(G.filename,".NONAME");
       name_changed = 1;
    }
#endif
    checkdir(__G__ G.filename, GETPATH);

    return name_changed;

} /* end function mapname() */


int checkdir(__G__ pathcomp, flag)
    __GDEF
    char *pathcomp;
    int flag;
/*
 * returns:  1 - (on APPEND_NAME) truncated filename
 *           2 - path doesn't exist, not allowed to create
 *           3 - path doesn't exist, tried to create and failed; or
 *               path exists and is not a directory, but is supposed to be
 *           4 - path is too long
 *          10 - can't allocate memory for filename buffers
 */
{
    static int rootlen = 0;      /* length of rootpath */
    static char *rootpath;       /* user's "extract-to" directory */

#   define FN_MASK   7
#   define FUNCTION  (flag & FN_MASK)

#if (!defined(SFX) || defined(SFX_EXDIR))
    if (FUNCTION == ROOT) {
        Trace((stderr, "initializing root path to [%s]\n", pathcomp));
        if (pathcomp == (char *)NULL) {
            rootlen = 0;
        }
        else if ((rootlen = strlen(pathcomp)) > 0) {
            if ((rootpath = (char *)malloc(rootlen+1)) == NULL) {
                rootlen = 0;
                return 10;
            }
            strcpy(rootpath, pathcomp);
            Trace((stderr, "rootpath now = [%s]\n", rootpath));
        }
        return 0;
    }
#endif /* !SFX || SFX_EXDIR */

/*---------------------------------------------------------------------------
    GETPATH:  copy full path to the string pointed at by pathcomp, and free
    buildpath.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == GETPATH) {
        if (rootlen > 0) {
#ifdef VM_CMS                     /* put the exdir after the filename */
           strcat(pathcomp,".");       /* used as minidisk to be save on  */
           strcat(pathcomp,rootpath);
#else /* MVS */
           char newfilename[PATH_MAX];
           char *start_fname;

           strcpy(newfilename,rootpath);
           if (strchr(pathcomp,'(') == NULL) {
              if ((start_fname = strrchr(pathcomp,'.')) == NULL) {
                 start_fname = pathcomp;
              }
              else {
                 *start_fname++ = '\0';
                 strcat(newfilename, ".");
                 strcat(newfilename, pathcomp);
              }
              strcat(newfilename,"(");
              strcat(newfilename,start_fname);
              strcat(newfilename,")");
           }
           else {
              strcat(newfilename,".");
              strcat(newfilename,pathcomp);
           }
           Trace((stdout, "new dataset : %s\n", newfilename));
           strcpy(pathcomp,newfilename);
#endif /* ?VM_CMS */
        }
        return 0;
    }

/*---------------------------------------------------------------------------
    END:  free rootpath, immediately prior to program exit.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == END) {
        Trace((stderr, "freeing rootpath\n"));
        if (rootlen > 0) {
            free(rootpath);
            rootlen = 0;
        }
        return 0;
    }

    return 99;  /* should never reach */

} /* end function checkdir() */


/******************************/
/* Function check_for_newer() */  /* used for overwriting/freshening/updating */
/******************************/

int check_for_newer(__G__ filename)  /* return 1 if existing file is newer */
    __GDEF                           /*  or equal; 0 if older; -1 if doesn't */
    char *filename;                  /*  exist yet */
{
    FILE *stream;

    if ((stream = fopen(filename, "r")) != (FILE *)NULL) {
       fclose(stream);
       /* File exists, assume it is "newer" than archive entry. */
       return EXISTS_AND_NEWER;
    }
    /* File does not exist. */
    return DOES_NOT_EXIST;
} /* end function check_for_newer() */


/*********************/
/*  Function stat()  */
/*********************/

int stat(const char *path, struct stat *buf)
{
   FILE *fp;
   char fname[PATH_MAX];
   time_t ltime;

   if ((fp = fopen(path, "rb")) != NULL) {
      fldata_t fdata;
      if (fldata( fp, fname, &fdata ) == 0) {
         buf->st_dev  = fdata.__device;
         buf->st_mode = *(short *)(&fdata);
      }

      /* Determine file size by seeking to EOF */
      fseek(fp,0L,SEEK_END);
      buf->st_size = ftell(fp);
      fclose(fp);

      /* set time fields in stat buf to current time. */
      time(&ltime);
      buf->st_atime =
      buf->st_mtime =
      buf->st_ctime = ltime;

      /* File exists, return success */
      return 0;
   }
   return 1;
}



/***************************/
/*  Function main_vmmvs()  */
/***************************/

/* This function is called as main() to parse arguments                */
/* into argc and argv.  This is required for stand-alone               */
/* execution.  This calls the "real" main() when done.                 */

#ifdef STAND_ALONE
int MAIN_VMMVS(void)
   {
    int  argc=0;
    char *argv[50];

    int  iArgLen;
    char argstr[256];
    char **pEPLIST, *pCmdStart, *pArgStart, *pArgEnd;

   /* Get address of extended parameter list from S/370 Register 0 */
   pEPLIST = (char **)__xregs(0);

   /* Null-terminate the argument string */
   pCmdStart = *(pEPLIST+0);
   pArgStart = *(pEPLIST+1);
   pArgEnd   = *(pEPLIST+2);
   iArgLen   = pArgEnd - pCmdStart + 1;

   /* Make a copy of the command string */
   memcpy(argstr, pCmdStart, iArgLen);
   argstr[iArgLen] = '\0';  /* Null-terminate */

   /* Store first token (cmd) */
   argv[argc++] = strtok(argstr, " ");

   /* Store the rest (args) */
   while (argv[argc-1])
      argv[argc++] = strtok(NULL, " ");
   argc--;  /* Back off last NULL entry */

   /* Call "real" main() function */
   return MAIN(argc, argv);
}
#endif  /* STAND_ALONE */



#ifndef SFX

/************************/
/*  Function version()  */
/************************/

void version(__G)
    __GDEF
{
    int len;
    char liblvlmsg [50+1];

    union {
       unsigned int iVRM;
       struct {
          unsigned int v:8;
          unsigned int r:8;
          unsigned int m:8;
       } xVRM;
    } VRM;

    /* Break down the runtime library level */
    VRM.iVRM = __librel();
    sprintf(liblvlmsg, ".  Runtime level V%dR%dM%d",
            VRM.xVRM.v, VRM.xVRM.r, VRM.xVRM.m);


    /* Output is in the form "Compiled with %s%s for %s%s%s%s" */

    len = sprintf((char *)slide, LoadFarString(CompiledWith),

    /* Add compiler name and level */
      "C/370", "",          /* Assumed.  Can't get compiler lvl(?) */

    /* Add compile environment */
#ifdef VM_CMS
      "VM/CMS",
#else
      "MVS",
#endif

    /* Add timestamp */
#ifdef __DATE__
      " on " __DATE__
#ifdef __TIME__
     " at " __TIME__
#endif
     ".\n", "",
#else
      "", "",
#endif
      liblvlmsg
    );

    (*G.message)((zvoid *)&G, slide, (ulg)len, 0);

} /* end function version() */

#endif /* !SFX */
