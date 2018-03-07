#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <sys/poll.h> 

#include <mdsapi/mds_api.h>

#include "mds_list.h"
#include "board_system.h"


static void _gpio_procfs_read_thread(void);

// ---------------------------------------------------
// GPIO EVT TOOLS : USE PROC FS - POLL EVT
// ---------------------------------------------------
#define GPIO_EVT_RET_SUCCESS    1
#define GPIO_EVT_RET_FAIL       -1

#define GPIO_EVT_MAX_CNT        5
#define POLL_TIMEOUT (30 * 1000) /* 3 seconds */

// ---------------------------------------------------
// type
// ---------------------------------------------------
typedef struct
{
	GPIP_EVT_INFO_T gpio_info[GPIO_EVT_MAX_CNT];
    int gpio_last_val[GPIO_EVT_MAX_CNT];
    int gpio_mgr_cnt;
}GPIP_EVT_INFO_MGR_T;

// ---------------------------------------------------
// global variable
// ---------------------------------------------------
static pthread_t _tid_gpio_procfs_read_thread = NULL;
static int _g_run_gpio_procfs_read_thread = 0;
static pthread_mutex_t gpio_procfs_mutex = PTHREAD_MUTEX_INITIALIZER;

GPIP_EVT_INFO_MGR_T gpio_mgr;

static int (*g_gpio_evt_proc)(GPIP_EVT_INFO_T* evt_res) = NULL;




static int _start_gpio_monintor_thread()
{
    _g_run_gpio_procfs_read_thread = 1;

    if ( _tid_gpio_procfs_read_thread == NULL )
        pthread_create(&_tid_gpio_procfs_read_thread, NULL, _gpio_procfs_read_thread, NULL);

    return 0;
}

int _get_gpio_val_fd(int gpio)
{
    char buf[32] = {0};
    int fd = 0;

	if ( export_chk_gpio_num(gpio) == -1 )
		return -1;

    sprintf(buf, GPIO_VALUE, gpio);

    fd = open(buf, O_RDWR);
	if(fd < 0) {
		printf("gpio sysfs open failed\n");
		return -1;
	}

    return fd;
}


static void _gpio_procfs_read_thread(void)
{
    int i = 0;
    struct pollfd gpio_fdset[GPIO_EVT_MAX_CNT];
    int gpio_fd[GPIO_EVT_MAX_CNT];

    int timeout = 0;
    int rc;
    unsigned int gpio;
    int len;

    // -------------------------------------------------
    // gpio setting init.
    // -------------------------------------------------
    for ( i = 0 ; i < gpio_mgr.gpio_mgr_cnt ; i ++ )
    {
        // setting gpio 
        // 1. direction : input
        printf("[MDS_APi : GPIO EVT] set dir : [%d] / [%d]\r\n", gpio_mgr.gpio_info[i].gpio_num, eGpioInput);
        mds_api_gpio_set_direction(gpio_mgr.gpio_info[i].gpio_num, eGpioInput);

        // 2. set edge type
        printf("[MDS_APi : GPIO EVT] set edge : [%d] / [%d]\r\n", gpio_mgr.gpio_info[i].gpio_num, gpio_mgr.gpio_info[i].evt_type);
        mds_api_gpio_set_edge(gpio_mgr.gpio_info[i].gpio_num, eGpioEdgeBoth);

        // 3. get fd
        gpio_fd[i] = _get_gpio_val_fd(gpio_mgr.gpio_info[i].gpio_num);
        printf("[MDS_APi : GPIO EVT] get fd gpio [%d] => [%d]\r\n",gpio_mgr.gpio_info[i].gpio_num, gpio_fd[i]);
    }

    // -------------------------------------------------
    // loop : gpio monitor
    // -------------------------------------------------
    while(_g_run_gpio_procfs_read_thread)
    {
        memset((void*)gpio_fdset, 0, sizeof(gpio_fdset));

        // fd clear
        for ( i = 0 ; i < GPIO_EVT_MAX_CNT ; i++ )
        {
            gpio_fdset[i].fd = -1;
        }

        // fd set..
        for ( i = 0 ; i < gpio_mgr.gpio_mgr_cnt ; i++ )
        {
            gpio_fdset[i].fd = gpio_fd[i];
            gpio_fdset[i].events = POLLPRI;
        }
/*
    #define POLLIN      0x0001  // 읽을 데이타가 있다.
    #define POLLPRI     0x0002  // 긴급한 읽을 데이타가 있다.
    #define POLLOUT     0x0004  // 쓰기가 봉쇄(block)가 아니다. 
    #define POLLERR     0x0008  // 에러발생
    #define POLLHUP     0x0010  // 연결이 끊겼음
    #define POLLNVAL    0x0020  // 파일지시자가 열리지 않은것같은
                                // Invalid request (잘못된 요청)
*/
        
        //printf("[MDS_APi : GPIO EVT] poll() time out [%d] \r\n", timeout);

        rc = poll(gpio_fdset, gpio_mgr.gpio_mgr_cnt, timeout);      

        if (rc <= 0) {
            printf("[MDS_APi : GPIO EVT] poll() failed!\r]n");
            timeout = POLL_TIMEOUT;
            continue;
        }

        //printf("[MDS_APi : GPIO EVT] poll() read success ret :  [%d]\r\n", rc);

        for ( i = 0 ; i < gpio_mgr.gpio_mgr_cnt ; i++ )
        {
            // timeout == 0 : first evt proc..
            if ( (gpio_fdset[i].revents & (POLLPRI)) || ( timeout == 0 ) )
            {
                char gpio_val_buf[32] = {0,};
                int cur_gpio_val = 0;
                lseek(gpio_fdset[i].fd, 0, SEEK_SET);
                len = read(gpio_fdset[i].fd, gpio_val_buf, sizeof(gpio_val_buf));
                if ( len <= 0 )
                    continue;
                
                cur_gpio_val = atoi(gpio_val_buf);

                // printf("[MDS_APi : GPIO EVT] last gpio [%d] / cur gpio [%d]\r\n", gpio_mgr.gpio_last_val[i], cur_gpio_val);

                if ( gpio_mgr.gpio_last_val[i] == cur_gpio_val )
                    continue;
                
                
                if ( ( gpio_mgr.gpio_last_val[i] == GPIO_EVT_LEVEL_LOW ) && ( cur_gpio_val == GPIO_EVT_LEVEL_HIGH ) )// rising..
                {
                    // gpio setting 이 해당 edge 일때만 올려준다.
                    if ( (gpio_mgr.gpio_info[i].evt_type == eGpioEdgeRising ) || (gpio_mgr.gpio_info[i].evt_type == eGpioEdgeBoth ))
                    {
                        GPIP_EVT_INFO_T gpio_evt_val = {0,};
                        gpio_evt_val.gpio_num = gpio_mgr.gpio_info[i].gpio_num;
                        gpio_evt_val.gpio_val = cur_gpio_val;
                        gpio_evt_val.evt_type = eGpioEdgeRising;

                        printf("[MDS_APi : GPIO EVT] poll() GPIO %d rising interrupt occurred => val [%s]\n", gpio_mgr.gpio_info[i].gpio_num, gpio_val_buf);
                        if ( g_gpio_evt_proc != NULL )
                            g_gpio_evt_proc(&gpio_evt_val);
                    }
                }

                if ( ( gpio_mgr.gpio_last_val[i] == GPIO_EVT_LEVEL_HIGH ) && ( cur_gpio_val == GPIO_EVT_LEVEL_LOW ) ) // falling..
                {
                    // gpio setting 이 해당 edge 일때만 올려준다.
                    if ( (gpio_mgr.gpio_info[i].evt_type == eGpioEdgeFalling ) || (gpio_mgr.gpio_info[i].evt_type == eGpioEdgeBoth ))
                    {
                        GPIP_EVT_INFO_T gpio_evt_val = {0,};
                        gpio_evt_val.gpio_num = gpio_mgr.gpio_info[i].gpio_num;
                        gpio_evt_val.gpio_val = cur_gpio_val;
                        gpio_evt_val.evt_type = eGpioEdgeFalling;

                        printf("[MDS_APi : GPIO EVT] poll() GPIO %d falling interrupt occurred => val [%s]\n", gpio_mgr.gpio_info[i].gpio_num, gpio_val_buf);

                        if ( g_gpio_evt_proc != NULL )
                            g_gpio_evt_proc(&gpio_evt_val);
                    }
                }

                gpio_mgr.gpio_last_val[i] = cur_gpio_val;
                
            }
        }

        timeout = POLL_TIMEOUT;

    }
}





int mds_api_gpio_evt_start(GPIP_EVT_INFO_T evt_ret[], int (*gpio_evt_proc)(GPIP_EVT_INFO_T* evt_res))
{
    int i = 0;
    
    // init mgr var
    memset(&gpio_mgr, 0x00, sizeof(gpio_mgr));


    gpio_mgr.gpio_mgr_cnt = 0;

    for ( i = 0 ; i < GPIO_EVT_MAX_CNT ; i++ )
    {
        GPIP_EVT_INFO_T evt_tmp = evt_ret[i];

        if (evt_ret[i].gpio_num == -1)
            break;

        memcpy(&gpio_mgr.gpio_info[i], &evt_tmp, sizeof(GPIP_EVT_INFO_T));
        
        gpio_mgr.gpio_last_val[i] = gpio_mgr.gpio_info[i].gpio_val;
        gpio_mgr.gpio_mgr_cnt++;
    }

    g_gpio_evt_proc = gpio_evt_proc;
    _start_gpio_monintor_thread();

}

int flush_gpio_evt()
{

}
