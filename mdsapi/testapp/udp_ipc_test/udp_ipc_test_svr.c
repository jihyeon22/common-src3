#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>     // sleep()

#include <mdsapi/mds_udp_ipc.h>

int msg_recv_proc_test(const unsigned char* recv_msg, const int recv_msg_len, unsigned char* resp_msg, int* resp_msg_len)
{
    if (( recv_msg_len == 0 ) || (recv_msg == NULL))
    {
        printf("recv proc timeout....\r\n");
        return 0;
    }
    printf("msg_recv_proc_test call \r\n");
    printf(" - recv_msg [%s] / [%d]\r\n", recv_msg, recv_msg_len);

    // case 1 resp to clnt
    strcpy((char*)resp_msg, "this msg is resp");
    *resp_msg_len = strlen("this msg is resp");
    
    // case 2 not resp to clnt
    /*
    *resp_msg_len = 0;
    resp_msg = NULL;
    */
    return 0;
}

int main(void)
{
    //unsigned char result_buff[128] ={0,};
    
    // test case 1 : use thread
	printf("test start.. \r\n"); 
    udp_ipc_server_start(UDP_IPC_PORT__GPS_NMEA_DATA_CH, msg_recv_proc_test);
    
    // test case 2 : no use thread
    /*
    if ( receive_data_timeout(UDP_IPC_PORT__TEST_APP, 30, result_buff, 128) > 0 )
        printf("result_buff is [%s]\r\n", result_buff);
    else
        printf("receive timeout\r\n");
    */
    
    while(1)
    {
        sleep(1);
    }
    
    
    printf("program bye bye..\r\n");
    return 0;
}