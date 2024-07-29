#ifndef __FLASH_INTERNAL_H
#define __FLASH_INTERNAL_H

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define FLASH_STM_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)

extern TaskHandle_t g_flash_internal_task_Handler; //export
extern SemaphoreHandle_t flashSTM_semphr_Handler;  //export


void flash_internal_task(void *pvarg);


#endif

