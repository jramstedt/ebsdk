/*
**  file: dynamic.h. 
**
**  This header file lists the external definitions for 
**  the dynamic.c module. 
*/

#ifndef dynamic_h
#define dynamic_h

struct ZONE;	/* forward declaration */


unsigned int dyn$_realloc (void* p, unsigned int new_size, int option, struct ZONE* zone);           
unsigned int realloc (void* p, unsigned int new_size);
int dyn$_free (void *p, int option);                                                                   
int free (void* p);                                           
                                                                                             
void* malloc (int size) ;
void* malloc_noown (int size) ;
void* dyn$_malloc (int size, int option, int modulus, int remainder, struct ZONE*  zone);                          
                                                                         
#endif 	/* include file wrapper */
