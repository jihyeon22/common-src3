#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>     // sleep()

#include <mdsapi/mds_udp_ipc.h>

int main()
{
    int i = 10;
    int send_len = 0;
    unsigned char send_buff[IPC_BUFF_SIZE] = {0,};
    unsigned char recv_buff[IPC_BUFF_SIZE] = {0,};
    
    while (i--)
    {
        memset(send_buff, 0x00, IPC_BUFF_SIZE);
        send_len = sprintf(send_buff,"hello msg -> [%d]", i);
        udp_ipc_broadcast_send_recv(UDP_IPC_PORT__TEST_APP, 10, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
    
        printf("[UDP-CLNT-TEST-APP] clnt recv msg [%s]\r\n", recv_buff);
    }
}