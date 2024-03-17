/*
 * routine to take the nth root of a number
 * arg[0] = number to take root of
 * arg[1] = root
 *
 * James Lo 9/25/89
 */

#include <stdio.h>
#include <math.h>

main (argc, argv)
int argc;
char *argv[];
{
 int i;
 double root, number, answer;
 for (i=0;i<1;i++) {
 scanf ("%lf", &number);
 scanf ("%lf", &root);
 answer = pow(number,1.0/root);
 printf("%5.8lf \n",answer);
 }
}  
