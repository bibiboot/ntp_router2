#include "get_timestamp.h"

void get_timestamp(struct timeval *tv)
{
	gettimeofday(tv,NULL);
}
