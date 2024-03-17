
	/*--------------------------------------------------------------*
	 *	These routines are provided for all programs to use	*
	 *	to standardize the timing mechanism for portability	*
	 *--------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/times.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MILLION 1000000.0

#ifndef HZ
#define HZ 60.0
#endif


	/*--------------------------------------------------------------*
	 *	returns the difference of the current cpu time of	*
	 *	the process and the time supplied as the argument.	*
	 *	resolution is 1/100 second, units are seconds		*
	 *--------------------------------------------------------------*/

float SCWctime (last)
     float last;
{
  struct rusage	me,kids;
  static int	first_time = 1;
  static double	first_cpuse = 0.0;
  double	cpuse;

  getrusage (RUSAGE_SELF,	&me);
  getrusage (RUSAGE_CHILDREN,	&kids);
  cpuse = me.ru_utime.tv_sec   + me.ru_utime.tv_usec/MILLION   +
/*
	  me.ru_stime.tv_sec   + me.ru_stime.tv_usec/MILLION   +
*/
	  kids.ru_utime.tv_sec + kids.ru_utime.tv_usec/MILLION;
/*
	  kids.ru_stime.tv_sec + kids.ru_stime.tv_usec/MILLION;
*/
  if (first_time)
    {
      first_cpuse = cpuse;
      first_time = 0;
    }
  return (cpuse - first_cpuse - last);
}


	/*--------------------------------------------------------------*
	 *	returns the difference of the current elapsed wall	*
	 *	clock time and the time supplied as the argument.	*
	 *	resolution is 1/100 second, units are seconds		*
	 *	(if ftime() not available, try sys call gettimeofday())	*
	 *--------------------------------------------------------------*/

float SCWetime (last)
     float last;
{
  struct timeval tval;
  struct timezone tzone;
  struct timeb	elp_time;
  static int	first_time = 1;
  static double	first_elapsed = 0.0;
  double	elapsed;

  /* ftime (&elp_time); */
  /* elapsed = elp_time.time + elp_time.millitm/1000.0; */
  gettimeofday (&tval, &tzone);
  elapsed = tval.tv_sec + tval.tv_usec/1000.0;
  if (first_time)
    {
      first_elapsed = elapsed;
      first_time = 0;
    }
  return (elapsed - first_elapsed - last);
}
