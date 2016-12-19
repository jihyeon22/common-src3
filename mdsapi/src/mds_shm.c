#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>     // sleep()
#include <sys/ipc.h>
#include <sys/shm.h>

#include <pthread.h>
#include <semaphore.h>

#include <mdsapi/mds_shm.h>
#include <mdsapi/mds_udp_ipc.h>

const char empty_app_name[] = "null";
static SHM_GLOBAL_DATA_T* _app_shm_global_init()
{
	key_t key = APP_SHM_KEY_GLOBAL;
    int shmid = -1;
    
    static SHM_GLOBAL_DATA_T* shm_data_p = NULL;
    
    if ( shm_data_p != NULL)
        return shm_data_p;
    
	shmid = shmget(key, sizeof(SHM_GLOBAL_DATA_T), IPC_CREAT | 0666);

	if (shmid < 0)
    {
		printf("[DEBUG:ERROR] %s() : cannot open shm [%d] \r\n", __func__, key);
        return NULL;
    }
    
    shm_data_p = (SHM_GLOBAL_DATA_T*)shmat(shmid, NULL, 0);
    
    // printf("shmid is [%d]\r\n",shmid);
    return shm_data_p;
}


int app_shm_set_global_data(SHM_GLOBAL_DATA_T* data)
{
	SHM_GLOBAL_DATA_T* shm_data_p = _app_shm_global_init();
    
    unsigned char send_msg_buff[128] = {0,};
    int send_len = 0;
    
    
    if (( shm_data_p == NULL) || (data == NULL))
        return MDS_SHM_FALSE;
    
    send_len = sprintf((char *)send_msg_buff, "gmem");
    memcpy(shm_data_p, data, sizeof(SHM_GLOBAL_DATA_T));    
    udp_ipc_broadcast_send(UDP_IPC_PORT__CHK_GLOBAL_MEM, send_msg_buff, send_len);
    return MDS_SHM_TRUE;
}

int app_shm_get_global_data(SHM_GLOBAL_DATA_T* data)
{
	SHM_GLOBAL_DATA_T* shm_data_p = _app_shm_global_init();
    
    if (( shm_data_p == NULL) || (data == NULL))
        return MDS_SHM_FALSE;
    
    memcpy(data, shm_data_p, sizeof(SHM_GLOBAL_DATA_T));
    return MDS_SHM_TRUE;
}

int app_shm_dbg_print_global_data()
{
    SHM_GLOBAL_DATA_T* shm_data_p = _app_shm_global_init();
    if ( shm_data_p == NULL ) 
    {
        printf(" ------- shm global data -------------- \r\n");
        printf(" FAIL !!!\r\n");
        printf(" -------------------------------------- \r\n");

    }
    else
    {
        printf(" ------- shm global data -------------- \r\n");
        printf("  --> test_int [%d] \r\n", shm_data_p->test_int);
        printf("  --> msg_buff [%s] \r\n", shm_data_p->msg_buff);
        printf(" -------------------------------------- \r\n");
    }
    return MDS_SHM_TRUE;
}

// ---------------------------------------------------

typedef struct shm_data
{
    int init_flag;
    char app_name[PER_APP_MAX_CNT][256];
	SHM_PER_APP_DATA_T shm_per_app[PER_APP_MAX_CNT];
}__attribute__((packed))SHM_PER_APP_DATA_MGR_T;


static SHM_PER_APP_DATA_MGR_T* _app_shm_per_app_init()
{
	key_t key = APP_SHM_KEY_PER_APP;
    int shmid = -1;
    
    static SHM_PER_APP_DATA_MGR_T* shm_data_p = NULL;
    
	shmid = shmget(key, sizeof(SHM_PER_APP_DATA_MGR_T), IPC_CREAT | 0666);

	if (shmid < 0)
    {
		printf("[DEBUG:ERROR] %s() : cannot open shm [%d] \r\n", __func__, key);
        return NULL;
    }
    
    shm_data_p = (SHM_PER_APP_DATA_MGR_T*)shmat(shmid, NULL, 0);
    
    if ( shm_data_p == NULL)
        return NULL;
    
    // 메모리영역 초기화
    if ( shm_data_p->init_flag == 0)
    {
        printf("[DEBUG:ERROR] %s() : memory init [%d] \r\n", __func__, key);
        int i = 0 ;
        for ( i = 0 ; i < PER_APP_MAX_CNT ; i ++)
        {
            memset(&shm_data_p->shm_per_app[i], 0x00, sizeof(SHM_PER_APP_DATA_T));
            strcpy(shm_data_p->app_name[i], empty_app_name);
        }
        shm_data_p->init_flag = 1;
    }
    
    // printf("shmid is [%d]\r\n",shmid);
    return shm_data_p;
}


int app_shm_set_per_app_data(char* app_name, SHM_PER_APP_DATA_T* data)
{
	SHM_PER_APP_DATA_MGR_T* shm_data_p = _app_shm_per_app_init();
//    SHM_PER_APP_DATA_T* shm_per_app_p = NULL;
    
    int i = 0;
    int found_app = 0;
    
    unsigned char send_msg_buff[128] = {0,};
    int send_len = 0;
    
    
    if (( shm_data_p == NULL) || (data == NULL))
        return MDS_SHM_FALSE;
    
    for ( i = 0 ; i < PER_APP_MAX_CNT ; i ++ )
    {
        if ( strcmp(app_name, shm_data_p->app_name[i]) == 0 )
        {
            found_app = 1;
            break;
        }
    }
    
    send_len = sprintf((char *)send_msg_buff, "amem-%s", app_name);
    
    // 기존의 데이터를 찾았다.
    if ( found_app == 1 )
    {
        printf("[DEBUG:MSG] %s() : app[%s] is idx[%d]\r\n", __func__, app_name, i);
        memcpy(&shm_data_p->shm_per_app[i], data, sizeof(SHM_PER_APP_DATA_T));
        udp_ipc_broadcast_send(UDP_IPC_PORT__CHK_APP_MEM, send_msg_buff, send_len);
        return MDS_SHM_TRUE;
    }
    
    for ( i = 0 ; i < PER_APP_MAX_CNT ; i ++ )
    {
        if ( strcmp(shm_data_p->app_name[i], empty_app_name) == 0 )
        {
            found_app = 1;
            break;
        }
    }
    
    // 새로운 index 를 찾았고 그곳에 저장
    if ( found_app == 1 )
    {
        printf("[DEBUG:MSG] %s() 1 : app[%s] have not idx -> new idx [%d]\r\n", __func__, app_name, i);

        memcpy(&shm_data_p->shm_per_app[i], data, sizeof(SHM_PER_APP_DATA_T));
        
        strcpy(shm_data_p->app_name[i], app_name);
        strcpy(shm_data_p->shm_per_app[i].app_id, app_name);
        
        udp_ipc_broadcast_send(UDP_IPC_PORT__CHK_APP_MEM, send_msg_buff, send_len);

        return MDS_SHM_TRUE;
    }
    
    udp_ipc_broadcast_send(UDP_IPC_PORT__CHK_APP_MEM, send_msg_buff, send_len);
    printf("[DEBUG:ERROR] %s() : app[%s] idx full \r\n", __func__, app_name);
    
    return MDS_SHM_TRUE;
}

int app_shm_get_per_app_data(char* app_name, SHM_PER_APP_DATA_T* data)
{
	SHM_PER_APP_DATA_MGR_T* shm_data_p = _app_shm_per_app_init();
//    SHM_PER_APP_DATA_T* shm_per_app_p = NULL;
    
    int i = 0;
    int found_app = 0;
    
    if (( shm_data_p == NULL) || (data == NULL))
        return MDS_SHM_FALSE;
    
    for ( i = 0 ; i < PER_APP_MAX_CNT ; i ++ )
    {
        if ( strcmp(app_name, shm_data_p->app_name[i]) == 0 )
        {
            found_app = 1;
            break;
        }
    }
    
    // 기존의 데이터를 찾았다.
    if ( found_app == 1 )
    {
        strcpy(shm_data_p->shm_per_app[i].app_id, app_name);
        printf("[DEBUG:MSG] %s() : app[%s] is idx[%d]\r\n", __func__, app_name, i);
        memcpy(data, &shm_data_p->shm_per_app[i], sizeof(SHM_PER_APP_DATA_T));
        return MDS_SHM_TRUE;
    }
    else
    {
        printf("[DEBUG:ERROR] %s() : app[%s] idx cannot find\r\n", __func__, app_name);
    }
    return MDS_SHM_FALSE;
}

int app_shm_get_per_app_data_all(SHM_PER_APP_DATA_T (*data)[PER_APP_MAX_CNT])
{
    SHM_PER_APP_DATA_MGR_T* shm_data_p = _app_shm_per_app_init();
    //SHM_PER_APP_DATA_T* shm_per_app_p = NULL;
    
    int i = 0;
    int j = 0;
    //int found_app = 0;
    
    if (( shm_data_p == NULL) || (data == NULL))
        return MDS_SHM_FALSE;
    
    for ( i = 0 ; i < PER_APP_MAX_CNT ; i ++ )
    {
        //printf("[DEBUG:MSG] %s() 1 : app[%s] is idx[%d]\r\n", __func__, shm_data_p->app_name[i], i);
        
        if ( strcmp("null", shm_data_p->app_name[i]) != 0 )
        {
            //memset(&data[j], 0x00, sizeof(SHM_PER_APP_DATA_T));
           
            strcpy(shm_data_p->shm_per_app[i].app_id, shm_data_p->app_name[i]);
            printf("[DEBUG:MSG] %s() found : app[%s] is idx[%d]\r\n", __func__, shm_data_p->shm_per_app[i].app_id, i);
            memcpy((data[0])+j, &shm_data_p->shm_per_app[i], sizeof(SHM_PER_APP_DATA_T) );
            j++;
            //printf("[DEBUG:MSG] %s() 2 found : app[%s] is idx[%d]\r\n", __func__, *data[j]->app_id, i);            
        }
    }
    
    return j;
}




