#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>

#include <mdsapi/mds_udp_ipc.h>
#include <mdsapi/mds_api.h>
#include "board_system.h"

// ------------------------------------
#define GPS_IPC_MSG__SET_GPS_COLD_START         "set_start_c"
#define GPS_IPC_MSG__SET_GPS_WARM_START         "set_start_w"
#define GPS_IPC_MSG__GET_GPS_STAT               "get_stat"
#define GPS_IPC_MSG__SET_GPS_STOP               "set_stop"
#define GPS_IPC_MSG__SET_GPS_RESET_COLD_BOOT    "set_reset_c"
#define GPS_IPC_MSG__SET_GPS_RESET_WARM_BOOT    "set_reset_w"
// ------------------------------------

int mds_api_gpsd_start(int type)
{
    int i = 10;
    int send_len = 0;
	
	unsigned char send_buff[IPC_BUFF_SIZE] = {0,};
    unsigned char recv_buff[IPC_BUFF_SIZE] = {0,};
	
	if ( type == MDS_API_GPS_TOOLS__TYPE_COLD )
	{
		memset(send_buff, 0x00, IPC_BUFF_SIZE);
		send_len = sprintf((char*)send_buff,"%s", GPS_IPC_MSG__SET_GPS_COLD_START);
    	udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
	}

	if ( type == MDS_API_GPS_TOOLS__TYPE_WARM )
	{
		memset(send_buff, 0x00, IPC_BUFF_SIZE);
		send_len = sprintf((char*)send_buff,"%s", GPS_IPC_MSG__SET_GPS_WARM_START);
    	udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 3, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
	}
	
	return 0;
}

int mds_api_gpsd_stop()
{
    int i = 10;
    int send_len = 0;
	
	unsigned char send_buff[IPC_BUFF_SIZE] = {0,};
    unsigned char recv_buff[IPC_BUFF_SIZE] = {0,};
	
	memset(send_buff, 0x00, IPC_BUFF_SIZE);
	send_len = sprintf((char*)send_buff,"%s", GPS_IPC_MSG__SET_GPS_STOP);
	// 7초정도.. ㅋㅋ
    udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 7, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
	
	return 0;
}

int mds_api_gpsd_reset(int type)
{
    int i = 10;
    int send_len = 0;
	
	unsigned char send_buff[IPC_BUFF_SIZE] = {0,};
    unsigned char recv_buff[IPC_BUFF_SIZE] = {0,};
	
	if ( type == MDS_API_GPS_TOOLS__TYPE_COLD )
	{
		memset(send_buff, 0x00, IPC_BUFF_SIZE);
		send_len = sprintf((char*)send_buff,"%s", GPS_IPC_MSG__SET_GPS_RESET_COLD_BOOT);
    	udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 7, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
	}

	if ( type == MDS_API_GPS_TOOLS__TYPE_WARM )
	{
		memset(send_buff, 0x00, IPC_BUFF_SIZE);
		send_len = sprintf((char*)send_buff,"%s", GPS_IPC_MSG__SET_GPS_RESET_WARM_BOOT);
    	udp_ipc_broadcast_send_recv(UDP_IPC_PORT__GPS_MGR_CH, 7, send_buff, send_len ,recv_buff, IPC_BUFF_SIZE) ;
	}

	return 0;
}

int mds_api_gpsd_get_nmea(char* buff, int buff_len)
{
	unsigned char recv_buff[IPC_BUFF_SIZE] = {0,};
	int recv_len = 0;
	
	// 데이터를 1 초당보내기때문에 타임아웃을 2초로 한다.
	recv_len = udp_ipc_data_recv_timeout(UDP_IPC_PORT__GPS_NMEA_DATA_CH, 2, recv_buff, IPC_BUFF_SIZE);
	
	if ( recv_len > 0 )
	{
		memcpy(buff, recv_buff, recv_len);
		return recv_len;
	}
	else
	{
		return -1;
	}
}

mdsapiRet_t mds_api_gps_util_get_gps_ant()
{
	if ( mds_api_gpio_get_value(GPS_ANT_CHK_GPIO_NUMBER) == GPS_ANT_CHK_CONN_VAL )
		return DEFINES_MDS_API_OK;
	else 
		return DEFINES_MDS_API_NOK;
}