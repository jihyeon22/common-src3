#ifndef __UTIL_MDS_API_LIST_H__
#define __UTIL_MDS_API_LIST_H__

#include <pthread.h>

#define mds_api_list_for_each(pos, head) \
	for(pos = head; pos != 0; pos = pos->next)

typedef struct mds_dataList mds_dataList_t;
struct mds_dataList
{
	void *data;
	mds_dataList_t *next;
};

typedef struct mds_listInstance mds_listInstance_t;
struct mds_listInstance
{
	mds_dataList_t *head_list;
	mds_dataList_t *tail_list;
	pthread_mutex_t mutex;
	int num;
};

void mds_api_list_free(mds_dataList_t *mds_api_list);
int mds_api_list_add(mds_listInstance_t *inst, void *data);
int mds_api_list_pop(mds_listInstance_t *inst, void **data);
int mds_api_list_del_all(mds_listInstance_t *inst);
int mds_api_list_get_num(mds_listInstance_t *inst);

#endif
