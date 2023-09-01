#ifndef __GLOBAL_H__
#define __GLOBAL_H__
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"


typedef struct{

TaskFunction_t pxTaskCode;
 char *  pcName;
 uint16_t usStackDepth;
void *  pvParameters;
UBaseType_t uxPriority;
TaskHandle_t *  pxTaskHandle;
				
					
}TaskStruct_t;


 

#endif






