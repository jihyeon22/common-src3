
#include <stdio.h>
#include <stdlib.h>

#include <mdsapi/mds_api.h>

void main()
{
    // api test 
    //mds_api_write_data_maxsize("/data/mds/log/log.test.txt","testmsg\r\n",strlen("testmsg\r\n"),128);
    
//    mds_api_poweroff_and_log("testapp", "bye");
	mds_api_set_network_time(KOR_TIME_ZONE_OFFSET);
}
