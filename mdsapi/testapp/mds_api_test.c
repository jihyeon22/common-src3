
#include <stdio.h>
#include <stdlib.h>

#include <mdsapi/mds_api.h>

void main()
{
    // api test 
    //mds_api_write_data_maxsize("/data/mds/log/log.test.txt","testmsg\r\n",strlen("testmsg\r\n"),128);
    
//    mds_api_poweroff_and_log("testapp", "bye");
	while(1)
	{
		mds_api_led_on("gps", "green");
		sleep(1);
		mds_api_led_off("gps");
		sleep(1);
	}
}
