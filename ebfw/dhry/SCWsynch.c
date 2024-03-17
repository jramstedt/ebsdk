
	/*--------------------------------------------------------------*
	 *	These routines are provided for all programs to use	*
	 *	to coordinate the start and end of image execution.	*
	 *	The SCW_start routine must appear before the first	*
	 *	statement of the code to be measured, and SCW_end	*
	 *	must appear after the last statement of interest.	*
	 *--------------------------------------------------------------*/

#include <stdio.h>

static float SCWcpu_start, SCWcpu_end, SCWelp_start, SCWelp_end,
	SCWcpu_percent;

float SCWctime(), SCWetime();

SCWstart (str)
     char *str;
{
  /*  any synchronize stuff goes here... */
  SCWcpu_start = SCWctime (0.0);
  SCWelp_start = SCWetime (0.0);
}


SCWend (str)
     char *str;
{
  FILE *fp;

  SCWcpu_end = SCWctime (SCWcpu_start);
  SCWelp_end = SCWetime (SCWelp_start);
  /*  any synchronize stuff goes here... */
  if (!(fp = fopen ("SCW.data", "w")))
    {
      printf ("\n ===== %s ===== ", str);
      printf ("CPU: %9.3f ===== ", SCWcpu_end);
      printf ("ELAPSED: %9.3f =====\n\n", SCWelp_end);
    }
  else
    {
      fprintf (fp, "%-32.32s    ", str);
      fprintf (fp, "%9.3f    ", SCWcpu_end);
      fprintf (fp, "%9.3f    ", SCWelp_end);
      if (SCWelp_end != 0.0)
	SCWcpu_percent = SCWcpu_end/SCWelp_end;
      else
	SCWcpu_percent = 1.0;
      fprintf (fp, "%6.2f%%\n", 100.0*SCWcpu_percent);
      fclose (fp);
    }
}
