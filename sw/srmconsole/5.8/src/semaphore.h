/*
**  file: semaphore.h. 
**
**  This header file lists the external definitions for 
**  the semaphore.c module. 
*/

#ifndef semaphore_h
#define semaphore_h

struct SEMAPHORE;

int krn$_post (struct SEMAPHORE *s, int v);
int krn$_wait (struct SEMAPHORE *s);
int krn$_semrelease (struct SEMAPHORE *s);
int krn$_semreinit (struct SEMAPHORE *s, int n, int v);
int krn$_bpost (struct SEMAPHORE *s, int v);
int krn$_post_wait (struct SEMAPHORE *s0, struct SEMAPHORE *s1, int v);
int krn$_semreinit (struct SEMAPHORE *s, int n, int v);
int krn$_semrelease (struct SEMAPHORE *s);
int krn$_seminit (struct SEMAPHORE *s, int n, char *name);
int krn$_sem_startup ();


#endif 	/* include file wrapper */
