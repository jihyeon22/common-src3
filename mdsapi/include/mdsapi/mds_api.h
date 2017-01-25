#ifndef __H_MDS_API__
#define __H_MDS_API__

typedef enum mdsapiRet mdsapiRet_t;
enum mdsapiRet
{
    DEFINES_MDS_API_NOK = -1,
    DEFINES_MDS_API_OK = 0,
};


// crc tools
unsigned short mds_api_crc16_get( unsigned char *buf, unsigned int len);

// --------------------------------------
// power 
// --------------------------------------
mdsapiRet_t mds_api_poweroff();   // never return..
mdsapiRet_t mds_api_poweroff_and_log(char* process_name, char* msg); // // never return..

// --------------------------------------
// file 
// --------------------------------------
#define ERR_NONE                0
#define ERR_FILE_NOT_EXIST      1
#define ERR_FILE_OPEN           2
#define ERR_MEM_ALLOC           3
#define ERR_FILE_READ           4
#define ERR_DATA_CRC            5

mdsapiRet_t mds_api_write_data(const char *filename, unsigned char *buff, const int data_len, int append);
mdsapiRet_t mds_api_write_data_maxsize(const char *filename, unsigned char *buff, const int data_len, int max_size);
int mds_api_read_data(const char *filename, unsigned char *buff, const int buff_len);
int mds_api_get_file_size(const char* file_path);
unsigned char mds_api_checksum_xor(const unsigned char *buff, const int data_len);
mdsapiRet_t mds_api_check_exist_file(const char *filename, int timeout);
mdsapiRet_t mds_api_cp(const char *from, const char *to,  int overwrite);
void mds_api_rm_all(char *dir);

int mds_api_storage_load_file(char *file, void *pdata, int length);
int mds_api_storage_save_file(const char *file, void *pdata, int length);


// --------------------------------------
// proc 
// --------------------------------------
int mds_api_get_available_memory(void);
mdsapiRet_t mds_api_get_module_list(const char* module_name);
void mds_api_write_procfs(char* value, char* procfs_path);

// --------------------------------------
// string 
// --------------------------------------
char * mds_api_strnstr(const char *str, const char *find, int str_len);
char * mds_api_strnchr(char *str, const char ch, const int str_len);
int mds_api_null2space(char *buff, const int data_len);
int mds_api_lftoa_19(char *buf, int out_str_len, const char *format, double value);
int mds_api_itoa_11(char *buf, int out_str_len, const char *format, int value);
int mds_api_strlen_without_cr(const char *s);
int mds_api_remove_cr(const char *s, char* target, int target_len);
int mds_api_remove_char(const char *s, char* target, int target_len, char targ_char);
int mds_api_read_line ( const char *cmd, const int cmd_len, char* buff, int buff_len);
int mds_api_str_replace ( const char *string, const char *substr, const char *replacement , char* buff, int buff_len);
int mds_api_count_char(const char* str, int str_len, const char c);

// --------------------------------------
// gpio 
// --------------------------------------
typedef enum gpioDirection gpioDirection_t;
enum gpioDirection
{
    eGpioInput = 0,
    eGpioOutput
};
int mds_api_gpio_set_value(const int gpio, const int value);
int mds_api_gpio_get_value(const int gpio);
int mds_api_gpio_set_direction(const int gpio, gpioDirection_t direction);

// --------------------------------------
// led
// --------------------------------------
int mds_api_led_on(const char* led_name, const char* color_name);
int mds_api_led_off(const char* led_name);
int mds_api_led_blink(const char* led_name, const char* color_name, const int delay_on, const int delay_off);


// --------------------------------------
// process 
// --------------------------------------
int mds_api_proc_find(const char* name);    // default max count : 4
int mds_api_proc_find_cnt(int max_cnt, const char* name);
int mds_api_system_fork(const char *command, int deamon, char *argv[]);

// --------------------------------------
// statck dump 
// --------------------------------------
void mds_api_stackdump_init(void);

// --------------------------------------
// debug 
// --------------------------------------
void mds_api_debug_hexdump_buff(unsigned char *buff, const int buff_len);

// --------------------------------------
// uart
// --------------------------------------
int mds_api_init_uart(const char* dev, const int baud);
int mds_api_uart_flush(int fd, int ftimes, int ftimeu);
int mds_api_uart_check(int fd, int ftime, int retry);
int mds_api_uart_size_read(int fd, unsigned char *buf, int nbytes, int ftime);
int mds_api_uart_read(int fd, unsigned char *buf, int nbytes, int ftime);
int mds_api_uart_write(int fd, const void *buf, size_t nbytes);
void mds_api_uart_close(int fd);

// --------------------------------------
// validation
// --------------------------------------
mdsapiRet_t mds_api_check_phonenum(const char *buff, const int buff_len);
mdsapiRet_t mds_api_check_imei(const char *buff, const int buff_len);
mdsapiRet_t mds_api_check_ip(const char *buff, const int buff_len);
mdsapiRet_t mds_api_check_dns_addr(const char *buff, const int buff_len);
mdsapiRet_t mds_api_check_is_num(const char *buff, const int buff_len);
mdsapiRet_t validation_check_lat_lon(float lat, float lon);

//---------------------------------------
// net tool
// --------------------------------------
mdsapiRet_t mds_api_get_ip(const char* netif_buff, const int netif_buff_len, char *ip_buff, const int ip_buff_len);
mdsapiRet_t mds_api_get_domain_to_ip(const char *dn_buff, const int dn_buff_len, char *ip_buff, const int ip_buff_len);
mdsapiRet_t mds_api_get_netstate(const char* netdev);

//---------------------------------------
// gpsd tool
// --------------------------------------
#define MDS_API_GPS_TOOLS__TYPE_COLD    0
#define MDS_API_GPS_TOOLS__TYPE_WARM    1

int gpsd_start(int type);
int gpsd_stop();
int gpsd_reset(int type);
int gpsd_get_nmea(char* buff, int buff_len);

#endif // __H_MDS_API__
