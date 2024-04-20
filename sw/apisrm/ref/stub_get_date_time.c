#include "cp$src:time.h"

get_date_time(struct toy_date_time *toy)
{
	toy->day_of_month = 1;
	toy->month = 1;
	toy->year = 2000;

	toy->hours = 12;
	toy->minutes = 0;
	toy->seconds = 1;
}
