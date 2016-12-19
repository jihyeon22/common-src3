#ifndef __APP_SHM_H__
#define __APP_SHM_H__

#define APP_SHM_KEY_GLOBAL		0x1234
#define APP_SHM_KEY_PER_APP		0x1235

#define MDS_SHM_TRUE    1
#define MDS_SHM_FALSE   0

#define PER_APP_MAX_CNT     10

typedef struct shm_per_app_data
{
    char app_id[128]; // don`t edit!!!
	int test_int;
    unsigned char msg_buff[1024];
}__attribute__((packed))SHM_PER_APP_DATA_T;

typedef struct shm_global_data
{
	int test_int;
    char msg_buff[1024];
}__attribute__((packed))SHM_GLOBAL_DATA_T;

int app_shm_set_global_data(SHM_GLOBAL_DATA_T* data);
int app_shm_get_global_data(SHM_GLOBAL_DATA_T* data);
int app_shm_dbg_print_global_data();


int app_shm_set_per_app_data(char* app_name, SHM_PER_APP_DATA_T* data);
int app_shm_get_per_app_data(char* app_name, SHM_PER_APP_DATA_T* data);
int app_shm_get_per_app_data_all(SHM_PER_APP_DATA_T (*data)[PER_APP_MAX_CNT]);
#endif