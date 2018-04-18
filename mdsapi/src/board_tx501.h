#pragma once

// --------------------------------------------------------
// tools
// --------------------------------------------------------
#define CONCAT_STR(X, Y)				X Y
#define MAKE_CMD_STR(X, Y)				X " " Y


#define LOG_PWR_PATH "/data/mds/log/power.log"
#define LOG_PWR_LOG_MAX_SIZE    10240

#define MDS_API_CALLSTACK_LOG_PATH "/data/mds/log/abort.log"


// --------------------------------------------------------
// process name check routine
// --------------------------------------------------------
#define MAX_PROCESS_NAME_LEN       15

// --------------------------------------------------------
// led setting.
// --------------------------------------------------------

// /sys/class/leds/led2_red_17


#define LED_NAME_GPS_RED_NUMBER_NAME          "led1_red_28"
#define LED_NAME_GPS_GREEN_NUMBER_NAME        "led1_green_59"
#define LED_NAME_PWR_RED_NUMBER_NAME          "led2_red_17"
#define LED_NAME_PWR_GREEN_NUMBER_NAME        "led2_green_29"
#define LED_NAME_WCDMA_RED_NUMBER_NAME        "led3_red_27"
#define LED_NAME_WCDMA_GREEN_NUMBER_NAME      "led3_green_16"

#define LED_ROOT_DIR				"/sys/class/leds"
#define LED_ROOT_PROC_TRIGER		CONCAT_STR(LED_ROOT_DIR, "/%s/trigger")
#define LED_ROOT_PROC_DELAY_ON		CONCAT_STR(LED_ROOT_DIR, "/%s/delay_on")
#define LED_ROOT_PROC_DELAY_OFF		CONCAT_STR(LED_ROOT_DIR, "/%s/delay_off")
#define LED_ROOT_PROC_COLOR			CONCAT_STR(LED_ROOT_DIR, "/%s/color")
#define LED_ROOT_PROC_BRIGHTNESS	CONCAT_STR(LED_ROOT_DIR, "/%s/brightness")
// TODO :: check..
//#define LED_ROOT_PROC_MANAGE		CONCAT_STR(LED_ROOT_DIR, "/neow200::%s/manage")

// --------------------------------------------------------
// gpio setting.
// --------------------------------------------------------
#define GPIO_ROOT_DIR					"/sys/class/gpio"

#define GPIO_EXPORT                     CONCAT_STR(GPIO_ROOT_DIR, "/export")
#define GPIO_VALUE						CONCAT_STR(GPIO_ROOT_DIR, "/gpio%d/value")
#define GPIO_DIRECTION					CONCAT_STR(GPIO_ROOT_DIR, "/gpio%d/direction")
#define GPIO_EDGE						CONCAT_STR(GPIO_ROOT_DIR, "/gpio%d/edge")

#define GPS_ANT_CHK_GPIO_NUMBER         7
#define GPS_ANT_CHK_CONN_VAL            (1)
#define GPS_ANT_CHK_DISCONN_VAL         (0);
