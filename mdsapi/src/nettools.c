#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/ether.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

#include <mdsapi/mds_api.h>


mdsapiRet_t mds_api_get_netstate(const char* netdev)
{
	int ret = 0;
	int sock;
	struct ifreq ifr;

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock < 0) {
		return DEFINES_MDS_API_NOK;
	}
	sprintf((char *)&ifr.ifr_name, "%s", netdev);
	if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
	{
		ret = DEFINES_MDS_API_NOK;
	}
	else
	{
		ret = DEFINES_MDS_API_OK;
	}
	close(sock);
	return ret;
}




mdsapiRet_t mds_api_get_domain_to_ip(const char *dn_buff, const int dn_buff_len, char *ip_buff, const int ip_buff_len)
{
	struct hostent *host_entry;

	if(mds_api_check_ip(dn_buff, dn_buff_len) == DEFINES_MDS_API_OK)
	{
		strncpy(ip_buff, dn_buff, 40);
	}
	else
	{
		host_entry = gethostbyname(dn_buff);
		if(host_entry == NULL)
		{
			return DEFINES_MDS_API_NOK;
		}
		sprintf(ip_buff, "%s", inet_ntoa(*(struct in_addr*)host_entry->h_addr_list[0]));
	}
	return mds_api_check_ip(ip_buff, ip_buff_len);
}



mdsapiRet_t mds_api_get_ip(const char* netif_buff, const int netif_buff_len, char *ip_buff, const int ip_buff_len)
{
	int ret = 0;
	int sock;
	struct ifreq ifr;
	struct sockaddr_in * addr;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0) {
		printf("%s:%d> socket open fail\n", __func__, __LINE__);
		return DEFINES_MDS_API_NOK;
	}
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, netif_buff, IFNAMSIZ - 1);
	if(ioctl(sock, SIOCGIFADDR, &ifr) < 0)
	{
		printf("%s:%d> socket ioctl fail\n", __func__, __LINE__);
		ret = DEFINES_MDS_API_NOK;
	}
	else
	{
		addr = (struct sockaddr_in *)&ifr.ifr_addr;
		if(inet_ntop(AF_INET, &addr->sin_addr, ip_buff, ip_buff_len) != NULL)
		{
			ret = DEFINES_MDS_API_OK;
		}
		else
		{
			printf("%s:%d> socket inet_ntop fail\n", __func__, __LINE__);
			ret = DEFINES_MDS_API_NOK;			
		}
	}
	close(sock);
	return ret;
}




#if 0
typedef struct {
	char host_name[256];
	unsigned long int saddr;
}host_by_name_t;

int g_hbn_count = 0;
host_by_name_t g_hbn[10];
pthread_mutex_t mutex_hostbyname = PTHREAD_MUTEX_INITIALIZER;
struct timespec mutex_wait;

unsigned long int gethostbyname_local(const char *host_name)
{
	int i;
	unsigned long int result = 0;
	int mutex_res = 0;

	//pthread_mutex_lock(&mutex_hostbyname);
	clock_gettime(CLOCK_REALTIME , &mutex_wait);
	mutex_wait.tv_sec += 2;
	
	mutex_res = pthread_mutex_timedlock(&mutex_hostbyname, &mutex_wait);
	if(mutex_res != 0)
	{
		printf( "Mutex timeout![err:%d]", mutex_res);
	//	devel_webdm_send_log("dns mutex error!");
	}
	
	for(i = 0; i < g_hbn_count; i++) 
	{
		if(!strcmp(g_hbn[i].host_name, host_name)) {
			printf( "%s:%d> %s local buffer host name %lu\n", __func__, __LINE__, host_name, g_hbn[i].saddr);
			result =  g_hbn[i].saddr;
		}
	}

	if(mutex_res == 0)
	{
		pthread_mutex_unlock(&mutex_hostbyname);
	}

#if LOG_DNS
	{
		char logbuf[255] = {0};
		snprintf(logbuf, sizeof(logbuf)-1, "gethostbyname_local %s %u\n", host_name, result);
		mds_api_write_data("/var/log/dnstry.log", logbuf, strlen(logbuf), 1);
	}
#endif
	
	return result;
}

void sethostbyname_local(const char *host_name, unsigned long int saddr)
{
	int mutex_res = 0;

	if(g_hbn_count > 9)
	{
		printf("hostbyname_local max count error\n");
		return;
	}
	
	LOGT(LOG_TARGET, "%s:%d> %s local buffer host name %lu\n", __func__, __LINE__, host_name, saddr);

	if(gethostbyname_local(host_name) == 0) {
	
		//pthread_mutex_lock(&mutex_hostbyname);
		clock_gettime(CLOCK_REALTIME , &mutex_wait);
		mutex_wait.tv_sec += 2;
		
		mutex_res = pthread_mutex_timedlock(&mutex_hostbyname, &mutex_wait);
		if(mutex_res != 0)
		{
			printf("Mutex timeout![err:%d]", mutex_res);
			printf("dns mutex error!");
		}
		
		strcpy(g_hbn[g_hbn_count].host_name, host_name);
		g_hbn[g_hbn_count].saddr = saddr;
		g_hbn_count += 1;

		if(mutex_res == 0)
		{
			pthread_mutex_unlock(&mutex_hostbyname);
		}
		
#if LOG_DNS
		{
			char logbuf[255] = {0};
			snprintf(logbuf, sizeof(logbuf)-1, "sethostbyname_local %s %u\n", host_name, saddr);
			mds_api_write_data("/var/log/dnstry.log", logbuf, strlen(logbuf), 1);
		}	
#endif

	}
	return;
}

void nettool_init_hostbyname_func(void)
{
	int mutex_res = 0;

	//pthread_mutex_lock(&mutex_hostbyname);
	clock_gettime(CLOCK_REALTIME , &mutex_wait);
	mutex_wait.tv_sec += 2;	
	
	mutex_res = pthread_mutex_timedlock(&mutex_hostbyname, &mutex_wait);
	if(mutex_res != 0)
	{
		printf "Mutex timeout![err:%d]", mutex_res);
		printf("dns mutex error!");
	}

	memset(&g_hbn, 0, sizeof(g_hbn));
	g_hbn_count = 0;

	if(mutex_res == 0)
	{
		pthread_mutex_unlock(&mutex_hostbyname);
	}

	return;
}





unsigned long int nettool_get_host_name(const char *host_name)
{
	unsigned long int s_addr;

	long int *add;
	struct hostent host_temp;
    struct hostent *host_entry;
	int n_try = 3;

	if(host_name == NULL) {
		printf("!!!!!Wrong format of host name : NULL Error!!!!!");
		return 0;
	}

	if( mds_api_check_ip(host_name, 255) == DEFINES_MDS_OK)
	{
		return inet_addr(host_name);
	}
	
	if(g_hbn_count > 0) {
		s_addr = gethostbyname_local(host_name);
		if(s_addr > 0) {
			return s_addr;
		}
	}
	
	while(n_try-- > 0)
	{
		host_entry = _get_host_by_name(host_name, &host_temp);
		if (!host_entry) {
			LOGE(LOG_TARGET, "!!!!!Wrong format of host name<%s>. #1!!!!! err[%d]", host_name, h_errno);
			LOGE(LOG_TARGET, "-->%s", hstrerror( h_errno));
			if(h_errno == 2) {
				//if gethostbyname was called when ppp device don't load completely
				//socket communication operate wrong unlimitly. untill ppp device re-load
			}
			return 0;
		} else {
			if (*host_entry->h_addr_list != NULL) {
				add = (long int *)*host_entry->h_addr_list;
				s_addr = *add;
#if LOG_DNS
				{
					char logbuf[255] = {0};
					snprintf(logbuf, sizeof(logbuf)-1, "gethostbyname %s %u\n", host_name, s_addr);
					tools_write_data("/var/log/dnstry.log", logbuf, strlen(logbuf), 1);
				}
#endif
				
				if(s_addr == 16777343)
				{
#if LOG_DNS
					tools_write_data("/var/log/dnstry.log", "gethostbyname retry\n", strlen("gethostbyname retry\n"), 1);
#endif
					continue;
				}
				break;
			}
			else {
				LOGE(LOG_TARGET, "!!!!!Wrong format of host name<%s>. #2!!!!![%d]", host_name, errno);
				LOGE(LOG_TARGET, "-->%s", strerror( errno));
				return 0;
			}
		}
	}
	if(n_try < 0)
	{
		return 0;
	}

	sethostbyname_local(host_name, s_addr);
	
	return s_addr;
}

#define SIZE_BUF_HOSTBYNAME 2048
char buf_gethostbyname[2048];

struct hostent *_get_host_by_name(const char *name, struct hostent *hbuf)
{
	int rc, err;
	struct hostent *result = NULL;
	
	if(name == NULL)
	{
		LOGE(LOG_TARGET, "ERROR : Address is NULL.");
		return NULL;
	}
	
	if((rc = gethostbyname_r(name, hbuf, buf_gethostbyname, sizeof(buf_gethostbyname), &result, &err)) == ERANGE)
	{
		LOGE(LOG_TARGET, "ERROR : buf_gethostbyname is not enough");
		return NULL;
	}

	if (0 != rc || NULL == result) {
		return NULL;
	}
	
	return result;
}

#endif