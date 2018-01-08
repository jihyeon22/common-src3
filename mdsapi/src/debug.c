
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

#include <mdsapi/mds_api.h>

#include "board_system.h"

void mds_api_debug_hexdump_buff(unsigned char *buff, const int buff_len)
{
	int i;

	printf("dump buf\n");
	for(i = 0; i < buff_len; i++)
	{
		printf("0x%02x ", buff[i]);
	}
	printf("\n");
}


mdsapiRet_t mds_api_write_time_and_log(const char *filename, char *log)
{
    char buff[1024] = {0,};
    char buff_date[64] = {0};

    int write_size = 0;
    struct timeval tv;
    struct tm *ptm;


    gettimeofday(&tv, NULL);
    ptm = localtime(&tv.tv_sec);
    
    write_size = snprintf(buff, sizeof(buff)-1, "[%02d.%02d.%02d/%02d:%02d:%02d] %s\r\n", ptm->tm_year%100, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, log);

	return mds_api_write_data(filename, (unsigned char*)buff, write_size, 1);

}


mdsapiRet_t mds_api_write_time_and_log_maxsize(const char *filename, char *log, int max_size)
{
    char buff[1024] = {0,};
    char buff_date[64] = {0};

    int write_size = 0;
    struct timeval tv;
    struct tm *ptm;


    gettimeofday(&tv, NULL);
    ptm = localtime(&tv.tv_sec);
    
    write_size = snprintf(buff, sizeof(buff)-1, "[%02d.%02d.%02d/%02d:%02d:%02d] %s\n", ptm->tm_year%100, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, log);

	return mds_api_write_data_maxsize(filename, (unsigned char*)buff, write_size, max_size);
}
