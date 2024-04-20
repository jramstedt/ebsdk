/*
**  file: strfunc.h. 
**
**  This header file lists the external definitions for 
**  the strfunc.c module. 
*/

#ifndef strfunc_h
#define strfunc_h


size_t strlen(char *s);
char *strcpy(char *s1, char *s2);
char *strncpy(char *s1, char *s2, size_t n);
char *strcat(char *s1, char *s2);
char *strncat(char *s1, char *s2, size_t n);
size_t strcspn(char *s1, char *s2);
size_t strspn(char *s1, char *s2);
char *strpbrk(char *s1, char *s2);                                  
char *strstr(unsigned char *s1, unsigned char *s2);                 
char *strchr (char *s0, char c);
char *strrchr (char *s0, char c);
int strcount (char *s, char *classification);
int strcmp (char unsigned *s0, char unsigned *s1);
int strncmp (char unsigned *s0, char unsigned *s1, int length);
int strcmp_nocase (char unsigned *s0, char unsigned *s1);    
int strwcmp_nocase(char unsigned *s0, char unsigned *s1); 
int strncmp_nocase (char unsigned *s0, char unsigned *s1, int length);
int substr (int s, int t);
int substr_nocase (int s, int t);
int strelement (char *string, int n, char *set, char *element);

#endif 	/* include file wrapper */
