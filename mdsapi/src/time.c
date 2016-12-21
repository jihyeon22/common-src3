
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include <mdsapi/mds_api.h>


#include "board_system.h"

#define RETRY_GET_KERNELTIME 5
time_t mds_api_get_kerneltime()
{
	int n_try = RETRY_GET_KERNELTIME;
	int ret = 0;
	struct timespec ts;
	memset(&ts, 0x00, sizeof(struct timespec));

	while(n_try-- > 0)
	{
		ret = clock_gettime(CLOCK_MONOTONIC, &ts);
		if(ret == 0 && ts.tv_sec > 0)
		{
			break;
		}
		usleep(50000);
	}

	if(n_try < 0)
	{
		printf( "%s> can't get kernel time over 10 times.\n", __FUNCTION__);
		ts.tv_sec = 0;
	}
#if 1
	else if(n_try < RETRY_GET_KERNELTIME-1)
	{
		printf( "%s> warn %d %lu\n", __FUNCTION__, n_try, ts.tv_sec);
	}
#endif

	if(ts.tv_sec <= 0)
	{
		printf(  "%s> ts.tv_sec is 0 %d\n", __FUNCTION__, n_try);
	}
	
	return ts.tv_sec;
}
