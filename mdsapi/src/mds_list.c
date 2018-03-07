#include <stdlib.h>
#include <stdio.h>

#include "mds_list.h"

static mds_dataList_t * _mds_api_list_new();

static mds_dataList_t * _mds_api_list_new()
{
	return (mds_dataList_t *)malloc(sizeof(mds_dataList_t));
}

void mds_api_list_free(mds_dataList_t *mds_api_list)
{
	if(mds_api_list != NULL)
	{
		free(mds_api_list);
	}
}

int mds_api_list_add(mds_listInstance_t *inst, void *data)
{
	if(inst == NULL)
	{
		return -1;
	}

	mds_dataList_t *new_list = NULL;
	if((new_list = _mds_api_list_new()) == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&(inst->mutex));

	new_list->data = data;

	new_list->next = NULL;
	if(inst->head_list == NULL)
	{
		inst->head_list = new_list;
	}
	else
	{
		if(inst->tail_list == NULL)
		{
			printf("err mds_api_list about tail %s\n", __FUNCTION__);
		}
		(inst->tail_list)->next = new_list;
	}
	inst->tail_list = new_list;

	inst->num++;

	pthread_mutex_unlock(&(inst->mutex));

	return 0;
}

int mds_api_list_pop(mds_listInstance_t *inst, void **data)
{
	mds_dataList_t *free_list;

	if(inst == NULL)
	{
		return -1;
	}

	if(data == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&(inst->mutex));
	
	if(inst->head_list == NULL)
	{
		if(inst->tail_list != NULL)
		{
			printf("err mds_api_list about tail %s\n", __FUNCTION__);
		}
		
		pthread_mutex_unlock(&(inst->mutex));
		
		return -1;
	}
	
	*data = (inst->head_list)->data;
	free_list = inst->head_list;
	inst->head_list = (inst->head_list)->next;

	mds_api_list_free(free_list);


	if(inst->head_list == NULL)
	{
		inst->tail_list = NULL;
	}

	inst->num--;

	pthread_mutex_unlock(&(inst->mutex));

	return 0;
}

int mds_api_list_del_all(mds_listInstance_t *inst)
{
	mds_dataList_t *free_list;

	if(inst == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&(inst->mutex));

	while(inst->head_list != NULL)
	{
		free_list = inst->head_list;
		free((inst->head_list)->data);
		inst->head_list = (inst->head_list)->next;
		mds_api_list_free(free_list);
	}
	inst->tail_list = NULL;

	inst->num = 0;

	pthread_mutex_unlock(&(inst->mutex));

	return 0;
}

int mds_api_list_get_num(mds_listInstance_t *inst)
{
	if(inst == NULL)
	{
		return -1;
	}

	return inst->num;
}

