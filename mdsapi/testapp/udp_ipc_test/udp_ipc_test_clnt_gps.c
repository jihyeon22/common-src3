#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>     // sleep()

#include <mdsapi/mds_udp_ipc.h>
#include "gps_ipc.h"

int main(int argc, char* argv[])
{
    int i = 10;
    int send_len = 0;
	
    unsigned char send_buff[IPC_BUFF_SIZE] = {0,};
    unsigned char recv_buff[IPC_BUFF_SIZE] = {0,};
    
	
	if(strcmp(argv[1], "start1") == 0)
    {
		printf("[UDP-CLNT-GPS-APP] clnt send msg [%s]\r\n",GPS_IPC_MSG__SET_GPS_COLD_START);
        memset(send_buff, 0x00, IPC_BUFF_SIZE);
        send_len = sprintf(send_buff,"%s", GPS_IPC_MSG__SET_GPS_COLD_START);
        udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
    
        printf("[UDP-CLNT-GPS-APP] clnt recv msg [%s]\r\n", recv_buff);
    }
	else if(strcmp(argv[1], "start2") == 0)
    {
		printf("[UDP-CLNT-GPS-APP] clnt send msg [%s]\r\n",GPS_IPC_MSG__SET_GPS_WARM_START);
        memset(send_buff, 0x00, IPC_BUFF_SIZE);
        send_len = sprintf(send_buff,"%s", GPS_IPC_MSG__SET_GPS_WARM_START);
        udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
    
        printf("[UDP-CLNT-GPS-APP] clnt recv msg [%s]\r\n", recv_buff);
    }
	else if(strcmp(argv[1], "stat") == 0)
    {
		printf("[UDP-CLNT-GPS-APP] clnt send msg [%s]\r\n",GPS_IPC_MSG__GET_GPS_STAT);
        memset(send_buff, 0x00, IPC_BUFF_SIZE);
        send_len = sprintf(send_buff,"%s", GPS_IPC_MSG__GET_GPS_STAT);
        udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
    
        printf("[UDP-CLNT-GPS-APP] clnt recv msg [%s]\r\n", recv_buff);
    }
	else if(strcmp(argv[1], "stop") == 0)
    {
		printf("[UDP-CLNT-GPS-APP] clnt send msg [%s]\r\n",GPS_IPC_MSG__SET_GPS_STOP);
        memset(send_buff, 0x00, IPC_BUFF_SIZE);
        send_len = sprintf(send_buff,"%s", GPS_IPC_MSG__SET_GPS_STOP);
        udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
    
        printf("[UDP-CLNT-GPS-APP] clnt recv msg [%s]\r\n", recv_buff);
    }
	else if(strcmp(argv[1], "reset1") == 0)
    {
		printf("[UDP-CLNT-GPS-APP] clnt send msg [%s]\r\n",GPS_IPC_MSG__SET_GPS_RESET_COLD_BOOT);
        memset(send_buff, 0x00, IPC_BUFF_SIZE);
        send_len = sprintf(send_buff,"%s", GPS_IPC_MSG__SET_GPS_RESET_COLD_BOOT);
        udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
    
        printf("[UDP-CLNT-GPS-APP] clnt recv msg [%s]\r\n", recv_buff);
    }
	else if(strcmp(argv[1], "reset2") == 0)
    {
		printf("[UDP-CLNT-GPS-APP] clnt send msg [%s]\r\n",GPS_IPC_MSG__SET_GPS_RESET_WARM_BOOT);
        memset(send_buff, 0x00, IPC_BUFF_SIZE);
        send_len = sprintf(send_buff,"%s", GPS_IPC_MSG__SET_GPS_RESET_WARM_BOOT);
        udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
    
        printf("[UDP-CLNT-GPS-APP] clnt recv msg [%s]\r\n", recv_buff);
    }
	
}