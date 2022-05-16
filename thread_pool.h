#ifndef _thread_pool_h
#define _thread_pool_h
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef struct{
	void *(*fun)(void *);
	void *args;
}Tpool_work_t;

typedef struct{
	bool thread_state;
	int thread_id;
	char *name;
	
	uint8_t *mutex;
	QueueHandle_t *queue;
	TaskHandle_t*TaskHandler;
}Thread_info_t;

typedef struct{
	uint8_t tpool_state;
	int thread_max_num;//最大工作线程数量
	uint8_t mutex;
	QueueHandle_t queue;
	Thread_info_t *thread_info_t;//
}Tpool_info_t;





int create_tpool(Tpool_info_t *tpool_info_t,int thread_num);
int destroy_tpool(Tpool_info_t *tpool_info_t);
void work_task(void const * argument);

int add_task(Tpool_info_t* tpool_info_t, void* (*fun)(void*),void* args);
#endif


