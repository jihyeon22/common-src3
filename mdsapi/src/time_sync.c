#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <time.h>
#include <locale.h>

#include <mdsapi/mds_api.h>

static char* time_server_list[] =
{
    "time.bora.net",
    "time.nist.gov",
    "time-a.nist.gov",
    "time-b.nist.gov",
};

static int print_cur_time()
{
    char buffer[64];
    time_t t0 = time(0);
    struct tm *lt = localtime(&t0);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", lt);
    //LOGT(eSVC_COMMON, "[TIMESNYC] cur time is : [%s]\r\n", buffer);
    printf( "[TIMESNYC] cur time is : [%s]\r\n", buffer);
    return 0;
}

static int print_timeinfo(time_t* time_info)
{
    char buffer[64];
    struct tm *lt = localtime(time_info);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", lt);
    //LOGT(eSVC_COMMON, "[TIMESNYC] time info is : [%s]\r\n", buffer);
    printf( "[TIMESNYC] time info is : [%s]\r\n", buffer);
    return 0;
}

static int print_timeinfo_tm(struct tm* cur_time)
{
    printf("[TIMESNYC] ------------------------------------------\n");
    printf("[TIMESNYC] tm_hour:  %d\n",cur_time->tm_hour);
    printf("[TIMESNYC] tm_min:  %d\n",cur_time->tm_min);
    printf("[TIMESNYC] tm_sec:  %d\n",cur_time->tm_sec);
    printf("[TIMESNYC] tm_mon:  %d\n",cur_time->tm_mon);
    printf("[TIMESNYC] tm_mday:  %d\n",cur_time->tm_mday);
    printf("[TIMESNYC] tm_year:  %d\n",cur_time->tm_year);
    printf("[TIMESNYC] tm_yday:  %d\n",cur_time->tm_yday);
    printf("[TIMESNYC] tm_wday:  %d\n",cur_time->tm_wday);
    printf("[TIMESNYC] ------------------------------------------\n");
    
    return 0;
}

static int get_cur_invalid_chk()
{
    time_t t0 = time(0);
    struct tm *lt = localtime(&t0);
    int cur_year = lt->tm_year+1900 ;
    
    int ret_val = DEFINES_MDS_API_NOK;

    //LOGT(eSVC_COMMON, "[TIMESNYC] current time is [%d]\r\n", cur_year);
    printf( "[TIMESNYC] current time is [%d]\r\n", cur_year);

    if ( cur_year > 2016 )
        ret_val = DEFINES_MDS_API_OK;
    else
        ret_val = DEFINES_MDS_API_NOK;

    return ret_val;
}


static int get_rdate_utc_time(char* time_server, struct tm* cur_time) // BLOCK FUNC!!
{
    char time_str[1024] = { 0 };
    char rdate_cmd[1024] = { 0 };

    if (( time_server == NULL ) || (cur_time == NULL))
        return DEFINES_MDS_API_NOK;

    sprintf(rdate_cmd, "rdate -p %s", time_server);
    mds_api_popen(rdate_cmd, time_str, sizeof(time_str));

    printf("[TIMESNYC] server (%s) : get time_str is [%s]\r\n", time_server,  time_str);
    //char* timeptr_p = "Wed Jan 17 06:19:19 2018";
    // Wed Jan 17 06:19:19 2018

    if( strptime(time_str, "%a %b %d %T %Y", cur_time) == NULL)
    {
        return DEFINES_MDS_API_NOK;
    }

    print_timeinfo_tm(cur_time);

    return DEFINES_MDS_API_OK;
}

static char* get_system_env(char* key)
{
    char* value = getenv(key);
    return value;
}

static int set_network_time(int timezone_adj_hour)
{
    struct tm cur_time;
    time_t tmp_timet = 0;
    struct timeval tmp_timeval = {0,};

    char* time_server_p;

    int i = 0;
    int get_time_info = 0 ;


    do 
    {
        time_server_p = time_server_list[i++];
        if ( ( time_server_p != NULL ) && get_rdate_utc_time(time_server_p, &cur_time) == DEFINES_MDS_API_OK )
        {
            get_time_info = 1;
            printf( "[TIMESNYC] get time info success..\r\n");
            break;
        }
    }
    while(time_server_p != NULL);

    if ( get_time_info == 0 ) 
    {
        printf( "[TIMESNYC] get time info fail..\r\n");
        return DEFINES_MDS_API_NOK;
    }
    
    tmp_timet = mktime(&cur_time) + (timezone_adj_hour * 60 * 60);

    tmp_timeval.tv_sec = tmp_timet;
    settimeofday(&tmp_timeval, 0);

    return DEFINES_MDS_API_OK;
}



int mds_api_set_network_time(int tz_hour)
{
    int target_time_zone = tz_hour;
    print_cur_time();

    if (get_cur_invalid_chk() == DEFINES_MDS_API_OK)
    {
        //LOGT(eSVC_COMMON, "[TIMESNYC] already sync .. do nothing..\r\n");
        printf( "[TIMESNYC] already sync .. do nothing..\r\n");
        return DEFINES_MDS_API_OK;
    }

    // check tz
    // NO SETTING TZ
    if ( ( get_system_env("TZ") != NULL ) || ( mds_api_check_exist_file(CHK_TZ_LOCAL_FILE, 2) == DEFINES_MDS_API_OK ) )
        target_time_zone = 0;
    else
        target_time_zone = tz_hour;

    print_cur_time();
    //LOGI(eSVC_COMMON, "[TIMESNYC] TARGET TZ SETTING [%d]\r\n", target_time_zone);
    printf( "[TIMESNYC] TARGET TZ SETTING [%d]\r\n", target_time_zone);

    if ( set_network_time(target_time_zone) == DEFINES_MDS_API_OK ) 
    {
        print_cur_time();
        //LOGT(eSVC_COMMON, "[TIMESNYC] TIME SYNC SUCCESS!!..\r\n");
        printf( "[TIMESNYC] TIME SYNC SUCCESS!!..\r\n");
        return DEFINES_MDS_API_OK;
    }

    //LOGE(eSVC_COMMON, "[TIMESNYC] TIME SYNC FAIL !!..\r\n");
    printf( "[TIMESNYC] TIME SYNC FAIL !!..\r\n");
    return DEFINES_MDS_API_NOK;

}

