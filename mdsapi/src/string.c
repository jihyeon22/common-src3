
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>

#include <mdsapi/mds_api.h>


#include "board_system.h"

char * mds_api_strnstr(const char *str, const char *find, int str_len)
{
	char c, sc;
	int len = 0;

	if((c = *find++) != '\0') {
		len = strlen(find);
		do {
			do {
				if((sc = *str++) == '\0' || str_len-- < 1) {
					return (NULL);
				}
			} while(sc != c);
			if(len > str_len) {
				return (NULL);
			}
		} while(strncmp(str, find, len) != 0);
		str--;
	}
	return ((char *)str);
}

char * mds_api_strnchr(char *str, const char ch, const int str_len)
{
	int i;
	for(i = 0; i < str_len; i++)
	{
		if(str[i] == ch)
		{
			return &str[i];
		}
	}
	return NULL;
}

int mds_api_null2space(char *buff, const int data_len)
{
	int i, res = 0;
	for(i = 0; i < data_len; i ++)
	{
		if(buff[i] == 0)
		{
			res++;
			buff[i] = 0x20;
		}
	}
	return res;
}


// tools_itoa doesn't set NULL at end of string.
int mds_api_lftoa_19(char *buf, int out_str_len, const char *format, double value)
{
	//only support 19 characters
	//In the future, if you need more character then you increase array size.
	char tmp_buf[20] = {0};

	if(out_str_len <= 0)
	{
		return -1;
	}
	
	snprintf(tmp_buf, sizeof(tmp_buf)-1, format, value);
	memcpy(buf, tmp_buf, out_str_len);

	return 0;
}



// tools_itoa doesn't set NULL at end of string.
int mds_api_itoa_11(char *buf, int out_str_len, const char *format, int value)
{
	//integer 32bit 's max length is 11.
	char tmp_buf[12] = {0};

	if(out_str_len <= 0)
	{
		return -1;
	}
	
	snprintf(tmp_buf, sizeof(tmp_buf)-1, format, value);
	memcpy(buf, tmp_buf, out_str_len);

	return 0;
}


int mds_api_count_char(const char* str, int str_len, const char c)
{
	int count = 0;
	int i = 0;
	
	for (i = 0 ; i < str_len ; i++ )
	{
		if ( str[i] == c )
			count ++;
	}
	
	printf("count_char ret is [%d]\r\n", count);
	return count;
}


int mds_api_strlen_without_cr(const char *s)
{
	int cnt = 0;

	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( ( *s != '\r' ) && ( *s != '\n' ) )
			cnt++;
		s++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}

int mds_api_remove_cr(const char *s, char* target, int target_len)
{
	int cnt = 0;

	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( ( *s != '\r' ) && ( *s != '\n' ) )
		{
			target[cnt] = *s;
			cnt++;
			
			if (cnt > target_len)
				return -1;
		}
		s++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}


int mds_api_remove_char(const char *s, char* target, int target_len, char targ_char)
{
	int cnt = 0;

	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( ( *s != targ_char ) )
		{
			target[cnt] = *s;
			cnt++;
			
			if (cnt > target_len)
				return -1;
		}
		s++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}

int mds_api_remove_etc_char(const char *s, char* target, int target_len)
{
	int cnt = 0;

    if ( s == NULL ) 
        return -1;

    if ( target == NULL )
        return -1;
        
	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( ( *s >= 33 ) && ( *s <= 125 ) )
		{
            target[cnt] = *s;
            //printf("target[%d] => [0x%x][%c]\r\n", cnt, target[cnt], target[cnt]);
			cnt++;
			
			if (cnt > target_len)
				return -1;
		}
		s++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}



int mds_api_read_line ( const char *cmd, const int cmd_len, char* buff, int buff_len)
{
	int i = 0;
	int j = 0;
	
	for ( i = 0 ; i < cmd_len ; i ++)
	{
		if ( (cmd[i] == '\r') || (cmd[i] == '\n'))
		{
			//printf("buff[j] => [%c] / cmd[i] => [%c]\r\n", buff[j-1] ,cmd[i] );
			break;
		}
		if (j > buff_len)
			return -1;
			
		buff[j++] = cmd[i];
		//printf("buff[j] => [%c] / cmd[i] => [%c]\r\n", buff[j-1] ,cmd[i] );
	}

	return ++i;
}



int mds_api_str_replace ( const char *string, const char *substr, const char *replacement , char* buff, int buff_len)
{
	int total_len = 0;
	
	int input_len = strlen(string);
	int substr_len = strlen(substr);
	int replacement_len = strlen(replacement);
	
	int replace_offset_start = 0;
	int replace_offset_end = 0;
	
	char* tok;
	// check argument
	if ( substr == NULL || replacement == NULL )
	{
		//LOGE(LOG_TARGET, "%s: Argument is null!! \r\n",__FUNCTION__);
		return -1;
	}
	
	// check buff size
	total_len = input_len - substr_len + replacement_len + 1 ;
	//printf("total_len is [%d]\r\n",total_len);
	
	if ( total_len > buff_len  )
	{
		//LOGE(LOG_TARGET, "%s: buffer size is too small!! \r\n",__FUNCTION__);
		return -1;
	}
	
	// find substr index
	tok = strstr ( string, substr );
	replace_offset_start = input_len - strlen(tok);
	replace_offset_end = replace_offset_start + substr_len;
	
	//check buff len
	strncpy( buff, string, replace_offset_start);
	strncpy( buff + replace_offset_start, replacement, replacement_len );
	strncpy( buff + replace_offset_start + replacement_len , string + replace_offset_end, input_len - replace_offset_end);
	
	return 0;
	
}
