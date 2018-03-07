
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

#include "board_system.h"

int export_chk_gpio_num(const int gpio)
{
	char buf[32] = {0};
	char write_gpio[32] = {0,};
	if (gpio <= 0)
		return -1;
	
	sprintf(buf, GPIO_VALUE, gpio);

	// export 되어있지 않은경우 체크
	if (mds_api_check_exist_file(buf, 0) == DEFINES_MDS_API_OK)
	{
		//printf("donot exist file >> [%s]\r\n", buf);
		return 0;
	}
	
	printf(" >> do not gpio :: write to gpio export [%s]\r\n", GPIO_EXPORT);
	sprintf(write_gpio, "echo %d > %s &", gpio, GPIO_EXPORT);
	system(write_gpio);
	//mds_api_write_procfs(write_gpio, GPIO_EXPORT);
	// sleep(1);

	if (mds_api_check_exist_file(buf, 1) == DEFINES_MDS_API_OK)
	{
		printf(" gpio export success\r\n");
		return 0;
	}
	else
	{
		printf(" gpio export fail\r\n");
		return -1;
	}
}

int mds_api_gpio_get_value(const int gpio)
{
	int fd = 0, value = 0;
	char status[2] = {0};
	char buf[32] = {0};

	/* check valied gpio number */
    /*
	if(!_valid_gpio(gpio)) {
		fprintf(stderr, "Gpio[%d] is not valied\n", gpio);
		goto err;
	}
    */
	if ( export_chk_gpio_num(gpio) == -1 )
		return -1;
	
	sprintf(buf, GPIO_VALUE, gpio);

	/* open gpio sysfs */
	fd = open(buf, O_RDWR);
	if(fd < 0) {
		fprintf(stderr, "Could not open gpio sysfs\n");
		goto err;
	}

	/* read gpio value */
	read(fd, status, 1);
	value = atoi(status);

	/* close gpio sysfs */
	close(fd);
	return value;

err:
	return -1;
}



int mds_api_gpio_set_value(const int gpio, const int value)
{
	int fd = 0;
	char buf[32] = {0};

	/* check valied gpio number */
    /*
	if(!_valid_gpio(gpio)) {
		printf("gpio[%d] is not valied\n", gpio);
		return -1;
	}*/

	if ( export_chk_gpio_num(gpio) == -1 )
		return -1;

	sprintf(buf, GPIO_VALUE, gpio);

	fd = open(buf, O_RDWR);
	if(fd < 0) {
		printf("gpio sysfs open failed\n");
		return -1;
	}

	if(value) {
		write(fd, "1", 1);
	}
	else {
		write(fd, "0", 1);
	}

	close(fd);
	return 0;
}

int mds_api_gpio_set_direction(const int gpio, gpioDirection_t direction)
{
	int fd = 0;
	char buf[32] = {0};

	/* check valied gpio number */
    /*
	if(!_valid_gpio(gpio)) {
		printf("gpio[%d] is not valied\n", gpio);
		return -1;
	}*/

	if ( export_chk_gpio_num(gpio) == -1 )
		return -1;

	sprintf(buf, GPIO_DIRECTION, gpio);

	fd = open(buf, O_RDWR);
	if(fd < 0) {
		printf("gpio sysfs open failed\n");
		return -1;
	}

	if(direction) {
		write(fd, "out", sizeof("out"));
	}
	else {
		write(fd, "in", sizeof("in"));
	}

	close(fd);
	return 0;
}


int mds_api_gpio_set_edge(const int gpio, gpioEdge_t edge)
{
	int fd = 0;
	char buf[32] = {0};

	/* check valied gpio number */
    /*
	if(!_valid_gpio(gpio)) {
		printf("gpio[%d] is not valied\n", gpio);
		return -1;
	}*/

	if ( export_chk_gpio_num(gpio) == -1 )
		return -1;

	sprintf(buf, GPIO_EDGE, gpio);

	fd = open(buf, O_RDWR);
	if(fd < 0) {
		printf("gpio sysfs open failed\n");
		return -1;
	}

    switch(edge)
    {
        case eGpioEdgeNone:
        {
            write(fd, "none", sizeof("none"));
            break;
        }
        case eGpioEdgeRising:
        {
            write(fd, "rising", sizeof("rising"));
            break;
        }
        case eGpioEdgeFalling:
        {
            write(fd, "falling", sizeof("falling"));
            break;
        }
        case eGpioEdgeBoth:
        {
            write(fd, "both", sizeof("both"));
            break;
        }
        default:
            close(fd);
            return -1;
            break;
    }

	close(fd);

	return 0;
}

