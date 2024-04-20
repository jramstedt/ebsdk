#include "cp$src:kernel_def.h"
#include "cp$src:dynamic_def.h"

extern struct ZONE *memzone;

struct ZONE *set_malloc_zone(struct FILE *fp)
{
	return memzone;
}
