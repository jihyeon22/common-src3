
#include <stdio.h>
#include <stdlib.h>

#include <mdsapi/mds_api.h>

int gpio_evt_proc(GPIP_EVT_INFO_T* evt_res)
{
    printf("APP GPIO EVT RECV ++ \r\n");
    printf("evt_res->gpio_num [%d] \r\n", evt_res->gpio_num);
    printf("evt_res->gpio_val [%d] \r\n", evt_res->gpio_val);
    printf("evt_res->evt_type [%d] \r\n", evt_res->evt_type);
    printf("APP GPIO EVT RECV --  \r\n");
}

void main()
{
    // api test 
    //mds_api_write_data_maxsize("/data/mds/log/log.test.txt","testmsg\r\n",strlen("testmsg\r\n"),128);
    
//    mds_api_poweroff_and_log("testapp", "bye");
	//mds_api_set_network_time(KOR_TIME_ZONE_OFFSET);
    GPIP_EVT_INFO_T gpio_evt[] = {
        {13, GPIO_EVT_LEVEL_HIGH, eGpioEdgeFalling}, 
        {-1, 0, 0, 0}, 
    };

    printf("TIME [%s]\r\n", __TIME__);

    printf("main -1 gpio_evt[0].gpio_num [%d]\r\n", gpio_evt[0].gpio_num);

    mds_api_gpio_evt_start(gpio_evt, gpio_evt_proc);
    
    printf("main -2\r\n");

    while(1)
        sleep(1);
}
