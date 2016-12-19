#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <mdsapi/mds_api.h>
#include <mdsapi/mds_shm.h>
//#include <mds_udp_ipc.h>



int main(int argc, char* argv[])
{
    mds_api_stackdump_init();
    // ----------------------------------------------
    #if 0
    SHM_GLOBAL_DATA_T test_global = {0,};
    
    if (app_shm_get_global_data(&test_global) == MDS_SHM_TRUE)
    {
        printf("get global mem success!\r\n");
        app_shm_dbg_print_global_data();
    }
    else
    {
        printf("get global mem fail! \r\n");
        app_shm_dbg_print_global_data();
    }
    
    test_global.test_int++;
    
    if ( app_shm_set_global_data(&test_global) == MDS_SHM_TRUE )
    {
        printf("set global mem success! \r\n");
        app_shm_dbg_print_global_data();
    }
    else
    {
        printf("set global mem fail! \r\n");
        app_shm_dbg_print_global_data();
    }
    #endif
    // ----------------------------------------------
    {
        int read_cnt = 0;
        int i = 0;
        SHM_PER_APP_DATA_T per_app_data_array[PER_APP_MAX_CNT];
        SHM_PER_APP_DATA_T per_app_temp;
        
        if (argc < 2)
        {
            printf("need to arg!!\r\n");
            exit(0);
        }
        printf("program start...\r\n");
        
        if ( app_shm_get_per_app_data(argv[1], &per_app_temp) == MDS_SHM_TRUE )
        {
            printf("get per app mem success!   \r\n");
            printf("per_app_temp->test_int is [%d] \r\n", per_app_temp.test_int);
        }
        else
        {
            printf("get per app mem fail!  \r\n");
        }
        
        per_app_temp.test_int++;
        if ( app_shm_set_per_app_data(argv[1], &per_app_temp) == MDS_SHM_TRUE )
        {
            printf("set per app mem success!\r\n");
        }
        else
        {
            printf("set per app mem fail ! \r\n");
        } 
        
        memset(&per_app_data_array, 0x00, sizeof(SHM_PER_APP_DATA_T)*PER_APP_MAX_CNT);
        read_cnt = app_shm_get_per_app_data_all(&per_app_data_array);
        
        printf(" ---------------------------- \r\n");
        printf("app data all [%d]    \r\n", read_cnt);
        for ( i = 0 ; i < read_cnt ; i ++)
        {
            printf("per_app_data[%d].app_id  ==> [%s]\r\n", i, per_app_data_array[i].app_id);
            printf("per_app_data[%d].test_int  ==> [%d]\r\n", i, per_app_data_array[i].test_int);
        }
        printf(" ---------------------------- \r\n");
    }
    
    printf("test clnt bye bye...\r\n");   
    return 0;
}