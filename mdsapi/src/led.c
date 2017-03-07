#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include <mdsapi/mds_api.h>

#include "board_system.h"

static void _led_set_trigger(const char *name, char *trigger)
{
	int fd;
	char buf[64] = {0};

	sprintf(buf, LED_ROOT_PROC_TRIGER, name);
	fd = open(buf, O_RDWR);
	if(fd < 0) {
		return;
	}

	write(fd, trigger, strlen(trigger));
	close(fd);
}

static void _led_trigger_timer(const char *name, const int delay_on, const int delay_off)
{
	int fd;
	char buf[64] = {0};
	char tmp_itoa[64] = {0,};
	
	sprintf(buf, LED_ROOT_PROC_TRIGER, name);
	//printf("%s - %d -> %s\r\n",__func__, __LINE__, buf);
	fd = open(buf, O_RDWR);
	if(fd < 0) {
		return;
	}

	write(fd, "timer", sizeof("timer"));
	close(fd);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, LED_ROOT_PROC_DELAY_ON, name);
	memset(tmp_itoa, 0x00, 64);
	sprintf(tmp_itoa, "%d", delay_on);
	fd = open(buf, O_RDWR);
	write(fd, tmp_itoa, strlen(tmp_itoa));
	close(fd);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, LED_ROOT_PROC_DELAY_OFF, name);
	memset(tmp_itoa, 0x00, 64);
	sprintf(tmp_itoa, "%d", delay_off);
	fd = open(buf, O_RDWR);
	write(fd, tmp_itoa, strlen(tmp_itoa));
	close(fd);
}

static void _led_set_color(const char *name, const char *color)
{
	int fd;
	char buf[64] = {0};

	sprintf(buf, LED_ROOT_PROC_COLOR, name);
	//printf("%s - %d -> %s\r\n",__func__, __LINE__, buf);

	fd = open(buf, O_RDWR);
	if(fd < 0) {
		return;
	}

	if(color) {
		write(fd, color, strlen(color));
	}
	else {
		write(fd, "none", strlen("none"));
	}

	close(fd);

}

static void _led_set_brightness(const char *name, unsigned char value)
{
	int fd;
	char buf[64] = {0};
	int brightness = value;

	sprintf(buf, LED_ROOT_PROC_BRIGHTNESS, name);
	//printf("%s - %d -> %s\r\n",__func__, __LINE__, buf);

	fd = open(buf, O_RDWR);
	if(fd < 0) {
		return;
	}

	if(brightness) {
		write(fd, "255", 3);
	}
	else {
		write(fd, "0", 1);
	}

	close(fd);
}

static void _led_set_control(const char *name, unsigned char value)
{
	int fd;
	char buf[64] = {0};
//????????????
	//sprintf(buf, LED_ROOT_PROC_MANAGE, name);

	fd = open(buf, O_RDWR);
	if(fd < 0) {
		return;
	}

	write(fd, &value, 1);

	close(fd);
}

// -------------------------------------------------------------------
int mds_api_led_on(const char* led_name, const char* color_name)
{
	// wcdma -----------------------------------------------
	if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "red") == 0 ) )
	{
		_led_set_trigger(LED_NAME_WCDMA_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_WCDMA_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 255);
	} 
	else if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "green") == 0 ) )
	{
		_led_set_trigger(LED_NAME_WCDMA_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_WCDMA_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 0);
	}
	else if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "yellow") == 0 ) )
	{
		_led_set_trigger(LED_NAME_WCDMA_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_WCDMA_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 255);
	}
	else if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "off") == 0 ) )
	{
		_led_set_trigger(LED_NAME_WCDMA_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_WCDMA_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 0);
	}
	// gps -----------------------------------------------
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "red") == 0 ) )
	{
		_led_set_trigger(LED_NAME_GPS_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_GPS_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 255);
	} 
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "green") == 0 ) )
	{
		_led_set_trigger(LED_NAME_GPS_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_GPS_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 0);
	}
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "yellow") == 0 ) )
	{
		_led_set_trigger(LED_NAME_GPS_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_GPS_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 255);
	}
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "off") == 0 ) )
	{
		_led_set_trigger(LED_NAME_GPS_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_GPS_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 0);
	}
	// pwr -----------------------------------------------
	else if ( ( strcmp(led_name, "pwr") == 0) &&  ( strcmp(color_name, "red") == 0 ) )
	{
		_led_set_trigger(LED_NAME_PWR_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 255);
	} 
	else if ( ( strcmp(led_name, "pwr") == 0) && ( strcmp(color_name, "green") == 0 ) )
	{
		_led_set_trigger(LED_NAME_PWR_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 0);
	}
	else if ( ( strcmp(led_name, "pwr") == 0) &&  ( strcmp(color_name, "yellow") == 0 ) )
	{
		_led_set_trigger(LED_NAME_PWR_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 255);
	}
	else if ( ( strcmp(led_name, "pwr") == 0) &&  ( strcmp(color_name, "off") == 0 ) )
	{
		_led_set_trigger(LED_NAME_PWR_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 0);
	}

	else 
		return -1;

	return 1;
}

int mds_api_led_off(const char* led_name)
{

	// wcdma -----------------------------------------------
	if ( strcmp(led_name, "wcdma") == 0) 
	{
		_led_set_trigger(LED_NAME_WCDMA_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_WCDMA_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 0);
	} 
	// gps -----------------------------------------------
	else if ( strcmp(led_name, "gps") == 0 )
	{
		_led_set_trigger(LED_NAME_GPS_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_GPS_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 0);
	} 
	// pwr -----------------------------------------------
	else if ( strcmp(led_name, "pwr") == 0 )
	{
		_led_set_trigger(LED_NAME_PWR_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 0);
	} 
	else
		return -1;

	return 0;
}

int mds_api_led_blink(const char* led_name, const char* color_name, const int delay_on, const int delay_off)
{
	// wcdma -----------------------------------------------
	if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "red") == 0 ) )
	{
		_led_set_trigger(LED_NAME_WCDMA_GREEN_NUMBER_NAME, "none");
		_led_trigger_timer(LED_NAME_WCDMA_RED_NUMBER_NAME, delay_on, delay_off);
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 255);
	} 
	else if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "green") == 0 ) )
	{
		_led_trigger_timer(LED_NAME_WCDMA_GREEN_NUMBER_NAME, delay_on, delay_off);
		_led_set_trigger(LED_NAME_WCDMA_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 0);
	}
	else if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "yellow") == 0 ) )
	{
		_led_trigger_timer(LED_NAME_WCDMA_GREEN_NUMBER_NAME, delay_on, delay_off);
		_led_trigger_timer(LED_NAME_WCDMA_RED_NUMBER_NAME, delay_on, delay_off);
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 255);
	}
	else if ( ( strcmp(led_name, "wcdma") == 0) &&  ( strcmp(color_name, "off") == 0 ) )
	{
		_led_set_trigger(LED_NAME_WCDMA_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_WCDMA_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_WCDMA_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_WCDMA_RED_NUMBER_NAME, 0);
	}
	// gps -----------------------------------------------
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "red") == 0 ) )
	{
		_led_set_trigger(LED_NAME_GPS_GREEN_NUMBER_NAME, "none");
		_led_trigger_timer(LED_NAME_GPS_RED_NUMBER_NAME, delay_on, delay_off);
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 255);
	} 
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "green") == 0 ) )
	{
		_led_trigger_timer(LED_NAME_GPS_GREEN_NUMBER_NAME, delay_on, delay_off);
		_led_set_trigger(LED_NAME_GPS_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 0);
	}
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "yellow") == 0 ) )
	{
		_led_trigger_timer(LED_NAME_GPS_GREEN_NUMBER_NAME, delay_on, delay_off);
		_led_trigger_timer(LED_NAME_GPS_RED_NUMBER_NAME, delay_on, delay_off);
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 255);
	}
	else if ( ( strcmp(led_name, "gps") == 0) &&  ( strcmp(color_name, "off") == 0 ) )
	{
		_led_set_trigger(LED_NAME_GPS_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_GPS_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_GPS_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_GPS_RED_NUMBER_NAME, 0);
	}
	// pwr -----------------------------------------------
	else if ( ( strcmp(led_name, "pwr") == 0) &&  ( strcmp(color_name, "red") == 0 ) )
	{
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		_led_trigger_timer(LED_NAME_PWR_RED_NUMBER_NAME, delay_on, delay_off);
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 255);
	} 
	else if ( ( strcmp(led_name, "pwr") == 0) && ( strcmp(color_name, "green") == 0 ) )
	{
		_led_trigger_timer(LED_NAME_PWR_GREEN_NUMBER_NAME, delay_on, delay_off);
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 0);
	}
	else if ( ( strcmp(led_name, "pwr") == 0) &&  ( strcmp(color_name, "yellow") == 0 ) )
	{
		_led_trigger_timer(LED_NAME_PWR_GREEN_NUMBER_NAME, delay_on, delay_off);
		_led_trigger_timer(LED_NAME_PWR_RED_NUMBER_NAME, delay_on, delay_off);
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 255);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 255);
	}
	else if ( ( strcmp(led_name, "pwr") == 0) &&  ( strcmp(color_name, "off") == 0 ) )
	{
		_led_set_trigger(LED_NAME_PWR_GREEN_NUMBER_NAME, "none");
		_led_set_trigger(LED_NAME_PWR_RED_NUMBER_NAME, "none");
		//_led_set_color(LED_NAME_WCDMA_RED_NUMBER_NAME, color_name);
		_led_set_brightness(LED_NAME_PWR_GREEN_NUMBER_NAME, 0);
		_led_set_brightness(LED_NAME_PWR_RED_NUMBER_NAME, 0);
	}

	else 
		return -1;
	
}



