#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "hal.h"
#include "lib.h"
#include "operator.h"

#define CALCFILL(a) (((a % 4) == 0) ? 0 : (4-(a%4)))

/*
** Function to save macro definitions to a save file
*/
void save_macro(void) {

    int status;
    int filestart;
    unsigned int filesize = 0;
    int namelen, bodylen, fnamelen;
    int i, j;
    MACROSAV *ms;
    MACRODEF *m;
    char zero_fill = '\0';

    /*
    ** Write dummy file size and the number of macros defined
    */
    if ((status = fwrite((char *)(&filesize), sizeof(filesize), 1, save_file)) == -1) {
        errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of file size: %s\n",
               strerror(errno));
        return;
    }
    filesize += sizeof(filesize);

    if ((status = fwrite((char *)(&macronum), sizeof(macronum), 1, save_file)) == -1) {
        errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of macro number: %s\n",
               strerror(errno));
        return;
    }
    filesize += sizeof(macronum);

    /*
    ** Write out Macros
    */
    ms = (MACROSAV*)malloc(sizeof(MACROSAV));
    for (i = 0; i < macronum; i++) {

        m = macro_defn[i];
        if (!m)  /* if macro was redefined the old definition was wiped out */
          continue;

        namelen = strlen(m->name) + 1;
        ms->namelen = namelen + CALCFILL(namelen);
        
        bodylen = strlen(m->body) + 1;
        ms->bodylen = bodylen + CALCFILL(bodylen);
        
        ms->narg = m->narg;
        ms->lineno = m->lineno;
        ms->var_arg = m->var_arg;
        ms->skiparglen = m->skiparglen;

        fnamelen = strlen(m->fname) + 1;
        ms->fnamelen = fnamelen + CALCFILL(fnamelen);

        /* write parameters */

        if ((status = fwrite((char *)(ms), sizeof(MACROSAV), 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of parameters: %s\n",
                   strerror(errno));
            return;
        }
        filesize += sizeof(MACROSAV);

        /* write name, body, fname strings */

        if ((status = fwrite((char *)(m->name), namelen, 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of name string: %s\n",
                   strerror(errno));
            return;
        }
        filesize += namelen;
        if ((status = fwrite(&zero_fill, ms->namelen - namelen, 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of name string: %s\n",
                   strerror(errno));
            return;
        }
        filesize += ms->namelen - namelen;
        
        if ((status = fwrite((char *)(m->body), bodylen, 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of body string: %s\n",
                   strerror(errno));
            return;
        }
        filesize += bodylen;
        if ((status = fwrite(&zero_fill, ms->bodylen - bodylen, 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of body string: %s\n",
                   strerror(errno));
            return;
        }
        filesize += ms->bodylen - bodylen;
        
        if ((status = fwrite((char *)(m->fname), fnamelen, 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of fname string: %s\n",
                   strerror(errno));
            return;
        }
        filesize += fnamelen;
        if ((status = fwrite(&zero_fill, ms->fnamelen - fnamelen, 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of fname string: %s\n",
                   strerror(errno));
            return;
        }
        filesize += ms->fnamelen - fnamelen;
        
        /* write skipargck array */
        for (j = 0; j < m->skiparglen; j++) {
          if ((status = fwrite((char *)(&m->skipargck[j]), sizeof(int), 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of skipargck[%d]: %s\n",
                   j, strerror(errno));
            return;
          }
          filesize += sizeof(int);
        }            
        
        /* write argtypes array */
        for (j = 0; j < m->narg; j++) {
          if ((status = fwrite((char *)(&m->argtypes[j]), sizeof(int), 1, save_file)) == -1) {
            errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of argtypes[%d]: %s\n",
                   j, strerror(errno));
            return;
          }
          filesize += sizeof(int);
        }            
        
        /* write arguments, argtypes, and default arguments */
        for (j = 0; j < m->narg; j++) {

            unsigned int arglen, argdeflen;
            unsigned int arglen_z, argdeflen_z;

            arglen = strlen(m->args[j]) + 1;
            arglen_z = arglen + CALCFILL(arglen);
            
            if (m->argdefs[j] != (char *)'?') {
                argdeflen = strlen(m->argdefs[j]) + 1;
                argdeflen_z = argdeflen + CALCFILL(argdeflen);
            }
            else {
                argdeflen = 0;
                argdeflen_z = 0;
            }
        
            if ((status = fwrite((char *)(&arglen_z), sizeof(arglen_z), 1, save_file)) == -1) {
                errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of arglen: %s\n",
                       strerror(errno));
                return;
            }
            filesize += sizeof(arglen_z);
            
            if ((status = fwrite((char *)(&argdeflen_z), sizeof(argdeflen_z), 1, save_file)) == -1) {
                errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of argdeflen: %s\n",
                       strerror(errno));
                return;
            }
            filesize += sizeof(argdeflen_z);
            
            if ((status = fwrite((char *)(m->args[j]), arglen, 1, save_file)) == -1) {
                errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of args[%d]: %s\n",
                       j, strerror(errno));
                return;
            }
            filesize += arglen;
            if ((status = fwrite(&zero_fill, arglen_z - arglen, 1, save_file)) == -1) {
                errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of args[%d]: %s\n",
                       j,strerror(errno));
                return;
            }
            filesize += arglen_z;
            

            if (argdeflen != 0) {
                if ((status = fwrite((char *)(m->argdefs[j]), argdeflen, 1, save_file)) == -1) {
                    errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of argdefs[%d]: %s\n",
                           j, strerror(errno));
                    return;
                }
                filesize += argdeflen;
                if ((status = fwrite(&zero_fill, argdeflen_z - argdeflen, 1, save_file)) == -1) {
                    errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of argdefs[%d]: %s\n",
                           j,strerror(errno));
                    return;
                }
                filesize += argdeflen_z - argdeflen;
            }

        } /* End for (j) */
        
    }  /* end for (i) */

    free(ms);

    /*
    ** Write real file length
    */
    status = fseek(save_file, 0, SEEK_SET);
    if (status == -1) {
        errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file seek problem, aborting seek: %s\n",
               strerror(errno));
        return;
    }
    if ((status = fwrite((char *)(&filesize), sizeof(filesize), 1, save_file)) == -1) {
        errout(E_ERR | E_NO_LNUM, "save_macro", "macro save file write problem, aborting write of filesize: %s\n",
               strerror(errno));
        return;
    }

}

/*
** Function to restore macros from a library file
*/
void rest_macro(FILE *mfile) {
    int status;
    int filesize;
    char *mptr;
    unsigned int num_of_macros = 0;
    MACROSAV *ms;
    MACRODEF *mtmp;
    int i, j;

    /*
    ** Read file size and number of macros
    */
    if ((status = fread((char *)(&filesize), sizeof(filesize), 1, mfile)) == -1) {
        errout(E_ERR | E_NO_LNUM, "rest_macro", "macro save file read problem, aborting read of file size: %s\n",
               strerror(errno));
        fclose(mfile);
        return;
    }
    if (filesize == 0) {
        errout(E_INFO, "rest_macro", "empty macro save file, aborting macro restore\n");
        fclose(mfile);
        return;
    }

    if ((status = fread((char *)(&num_of_macros), sizeof(num_of_macros), 1, mfile)) == -1) {
        errout(E_ERR | E_NO_LNUM, "rest_macro", "macro save file read problem, aborting read of macro number: %s\n",
               strerror(errno));
        fclose(mfile);
        return;
    }

    /*
    ** Allocate memory for file and read in file
    */
    mptr = (char *)malloc(filesize-sizeof(filesize));
    if ((status = fread((char *)(mptr), filesize-sizeof(filesize), 1, mfile)) == -1) {
        errout(E_ERR | E_NO_LNUM, "rest_macro", "macro save file read problem, aborting read of data: %s\n",
               strerror(errno));
        fclose(mfile);
        return;
    }

    /*
    ** Rebuild MACRO table
    */
    ms = (MACROSAV*)malloc(sizeof(MACROSAV));
    for (i = 0; i < num_of_macros; i++) {
        mtmp = ALLOC0(1, MACRODEF);

        ms->namelen = *((unsigned int*)mptr);
        mptr += sizeof(ms->namelen);

        ms->bodylen = *((unsigned int*)mptr);
        mptr += sizeof(ms->bodylen);

        ms->narg = mtmp->narg = *((int*)mptr);
        mptr += sizeof(ms->narg);

        ms->lineno = mtmp->lineno = *((int*)mptr);
        mptr += sizeof(ms->lineno);

        ms->var_arg = mtmp->var_arg = *((int*)mptr);
        mptr += sizeof(ms->lineno);

        ms->skiparglen = mtmp->skiparglen = *((unsigned int*)mptr);
        mptr += sizeof(ms->fnamelen);

        ms->fnamelen = *((unsigned int*)mptr);
        mptr += sizeof(ms->fnamelen);

        mtmp->name = mptr;
        mptr += ms->namelen;

        mtmp->body = mptr;
        mptr += ms->bodylen;
        
        mtmp->fname = mptr;
        mptr += ms->fnamelen;

        mtmp->skipargck = (int*)mptr;
        mptr += ms->skiparglen * sizeof(int);

        mtmp->argtypes = (int*)mptr;
        mptr += ms->narg * sizeof(int);
        
        mtmp->args = ALLOC0(ms->narg, char*);
        mtmp->argdefs = ALLOC0(ms->narg, char*);
        
        for (j = 0; j < ms->narg; j++) {
            unsigned int arglen, argdeflen;

            arglen = *((unsigned int*)mptr);
            mptr += sizeof(arglen);
            argdeflen = *((unsigned int*)mptr);
            mptr += sizeof(argdeflen);

            mtmp->args[j] = mptr;
            mptr += arglen;

            if (argdeflen != 0) {
                mtmp->argdefs[j] = mptr;
                mptr += argdeflen;
            }
            else {
                mtmp->argdefs[j] = (char *)'?';
            }
        }  /* end for (j) */

        create_macro(mtmp, mtmp->name);

    }  /* end for (i) */

    free(ms);
    fclose(mfile);
}

    
