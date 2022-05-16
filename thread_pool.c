#include "thread_pool.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
enum {
	TPOOL_READY=0,
	TPOOL_BUSY,
	TPOOL_INITIALIZE,
	TPOOL_ERROR,
	TPOOL_OK,
}Tpool_state_t;

int create_tpool(Tpool_info_t *tpool_info_t,int thread_num){
	BaseType_t Result;
	tpool_info_t->tpool_state = TPOOL_INITIALIZE;
	tpool_info_t->thread_max_num = thread_num;
	printf("开始创建工作线程...\r\n");		
	tpool_info_t->mutex = 0;
	
	tpool_info_t->queue  = xQueueCreate(thread_num,sizeof(Tpool_work_t)); /* 每个消息大小，单位字节 */
	
	tpool_info_t->thread_info_t = (Thread_info_t*)malloc(sizeof(Thread_info_t)*thread_num);
	for(int i = 0; i < tpool_info_t->thread_max_num ; i++){
		(tpool_info_t->thread_info_t+i)->name = "tpool_work_thread";
		(tpool_info_t->thread_info_t+i)->thread_id = i+1;
		(tpool_info_t->thread_info_t+i)->thread_state = TPOOL_INITIALIZE;
		(tpool_info_t->thread_info_t+i)->mutex = &tpool_info_t->mutex;
		(tpool_info_t->thread_info_t+i)->queue = &tpool_info_t->queue;
    Result = xTaskCreate((TaskFunction_t )work_task,     
										 (const char*    )tpool_info_t->thread_info_t->name,   
										 (uint16_t       )128, 
										 (void*          )(tpool_info_t->thread_info_t+i),
										 /*(UBaseType_t    )*/osPriorityHigh,
										 (TaskHandle_t*  )&tpool_info_t->thread_info_t->TaskHandler);
		if(Result!= pdPASS) return TPOOL_ERROR;
		(tpool_info_t->thread_info_t+i)->thread_state = TPOOL_READY;
		osDelay(50);
	}
	tpool_info_t->tpool_state = TPOOL_READY;
	printf("创建工作线程完成\r\n");
	return TPOOL_OK; 
}

int destroy_tpool(Tpool_info_t *tpool_info_t){
	for(int i = 0; i < tpool_info_t->thread_max_num ; i++){
		vTaskDelete(*tpool_info_t->thread_info_t->TaskHandler);
	}
	free(tpool_info_t->thread_info_t);
	return TPOOL_OK; 
}

void work_task(void const *argument){
	while(1){
		Thread_info_t *thread_info_t = (Thread_info_t *)argument;
		Tpool_work_t tpool_work_t;
		printf("work_task is running!  id= %d\r\n",thread_info_t->thread_id);
		if( pdTRUE == xQueueReceive(*thread_info_t->queue, &tpool_work_t, 0))
		{
			printf("运行用户添加的函数\r\n");
			tpool_work_t.fun(tpool_work_t.args);
		}	
		osDelay(400);
	} 
}
int add_task(Tpool_info_t* tpool_info_t, void* (*fun)(void*),void* args){
	
	Tpool_work_t tpool_work_t;
	tpool_work_t.fun = fun;
	tpool_work_t.args = args;
	xQueueSend(tpool_info_t->queue, &tpool_work_t,0);		
	return TPOOL_OK; 
}




