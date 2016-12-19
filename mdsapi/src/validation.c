#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <ctype.h>

#include <mdsapi/mds_api.h>

#define DEFINES_IP_LEN	15
#define DEFINES_IP_BUFF_LEN	(DEFINES_IP_LEN + 1)


mdsapiRet_t mds_api_check_phonenum(const char *buff, const int buff_len)
{
	int i;
	int len = strlen(buff);

	if(buff_len < 7) {
		return DEFINES_MDS_API_NOK;
	}

	if(len < 7) {
		return DEFINES_MDS_API_NOK;
	}

	for(i = 0; i < len; i++) {
		if(!isdigit(buff[i])) {
			return DEFINES_MDS_API_NOK;
		}
	}

	return DEFINES_MDS_API_OK;
}

mdsapiRet_t mds_api_check_imei(const char *buff, const int buff_len)
{
	int i;
	int len = strlen(buff);

	if(buff_len < 15) {
		return DEFINES_MDS_API_NOK;
	}

	if(len != 15) {
		return DEFINES_MDS_API_NOK;
	}

	for(i = 0; i < len; i++) {
		if(!isdigit(buff[i])) {
			return DEFINES_MDS_API_NOK;
		}
	}

	return DEFINES_MDS_API_OK;
}

mdsapiRet_t mds_api_check_ip(const char *buff, const int buff_len)
{
	int nNumCount = 0;
	int nDotCount = 0;
	int i = 0;
	int len = strnlen(buff, DEFINES_IP_LEN);

	if(buff_len < 7) {
		return DEFINES_MDS_API_NOK;
	}

	if(len > 15 || len < 7) {
		return DEFINES_MDS_API_NOK;
	}

	for(i = 0; i < len; i++)
	{
		if(buff[i] < '0' || buff[i] > '9')
		{
			if(buff[i] == '.')
			{
				++nDotCount;
				nNumCount = 0;
			}
			else {
				return DEFINES_MDS_API_NOK;
			}
		}
		else
		{
			if(++nNumCount > 3) {
				return DEFINES_MDS_API_NOK;
			}
		}
	}

	if(nDotCount != 3)
	{
		return DEFINES_MDS_API_NOK;
	}

	return DEFINES_MDS_API_OK;
}


static int _isalpha_apn(char ch)
{
	if (( ch >= 'a' && ch <= 'z') || ( ch >= 'A' && ch <= 'Z') || ( ch >= '0' && ch <= '9') || ( ch == '.') )
		return 1;
	
	return 0;
	
}
mdsapiRet_t mds_api_check_dns_addr(const char *buff, const int buff_len)
{
	int i = 0;

	for(i = 0; i < buff_len; i++)
	{
		//printf(" isalpha2() [%c] => [%d]\r\n",buff[i], isalpha2(buff[i]));
		if ( _isalpha_apn(buff[i]) == 0 ) 
		{
			//printf("dns invalid :: buff[i] is [%c]\r\n",buff[i]);
			return DEFINES_MDS_API_NOK;
		}
	}

	return DEFINES_MDS_API_OK;
}

// input apn addr is \"kt.wing.com\"
mdsapiRet_t mds_api_check_apn_addr(const unsigned char *buff, const int buff_len)
{
	int i = 0;
	int j = 0;
	int chk_cnt = 0;
	char apn_addr[512] = {0,};


	// check '"' count..
	for(i = 0; i < buff_len; i++)
	{
		if ( buff[i] == '"' )
			chk_cnt++;
		else
			apn_addr[j++] = buff[i];
	}
	
	printf ("input buff [%s] \" count [%d]\r\n", buff, chk_cnt);
	
	if (chk_cnt > 2)
	{
		printf("invalid apn addr");
		return DEFINES_MDS_API_NOK;
	}
	
	if ( j == 0 )
		return DEFINES_MDS_API_NOK;
	
	printf("real apn is [%s] / [%d] \r\n", apn_addr, j);
	return mds_api_check_dns_addr(apn_addr, j);
}

mdsapiRet_t mds_api_check_is_num(const char *buff, const int buff_len)
{
	int i;
	int num_valid=1;

    for (i = 0; i < buff_len ; i++)
	{
		if (isalpha(buff[i]))
		{
			num_valid = 0;
			break;
		}
	}

	if (num_valid)
	{
		return DEFINES_MDS_API_OK;
	}
	else
	{
		return DEFINES_MDS_API_NOK;
	}

}

mdsapiRet_t validation_check_lat_lon(float lat, float lon)
{
	if(lat < 33.059704 || lat > 38.625083 ||
	        lon < 124.904331 || lon > 131.876004)
	{
		return DEFINES_MDS_API_NOK;
	}

	return DEFINES_MDS_API_OK;
}
