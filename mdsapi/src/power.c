
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

#include "board_system.h"


static void devel_log_poweroff(const char *log)
{
    char buff[192] = {0,};
    char buff_date[64] = {0};

    int write_size = 0;
    struct timeval tv;
    struct tm *ptm;


    gettimeofday(&tv, NULL);
    ptm = localtime(&tv.tv_sec);
    
    write_size = snprintf(buff, sizeof(buff)-1, "[%02d.%02d.%02d/%02d:%02d:%02d PWR LOG] %.120s\n", ptm->tm_year%100, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, log);

    mds_api_write_data_maxsize(LOG_PWR_PATH, (unsigned char *)buff, write_size, LOG_PWR_LOG_MAX_SIZE);
}

mdsapiRet_t mds_api_poweroff()
{
    int power_off_cnt = 0;
    while(1)
    {
        system("poweroff &");
        sleep(10);
        if(power_off_cnt++ > 10) {
            system("echo c > /proc/sysrq-trigger");
        }
    }

    return DEFINES_MDS_API_NOK;
}

mdsapiRet_t mds_api_poweroff_and_log(char* process_name, char* msg)
{
    char log_buff[1024] = {0,};
    int log_strlen = 0;

    log_strlen = sprintf(log_buff, "%s > %s \r\n", process_name, msg);
    devel_log_poweroff(log_buff);
    mds_api_poweroff();

    return DEFINES_MDS_API_NOK;
}

