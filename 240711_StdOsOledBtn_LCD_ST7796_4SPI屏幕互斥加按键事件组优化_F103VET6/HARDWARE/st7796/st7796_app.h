#ifndef __ST7796_APP_H
#define __ST7796_APP_H
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void st7796_task(void *pvarg);
extern TaskHandle_t g_st7796Task_Handler; //export 声明
extern SemaphoreHandle_t g_st7796_mutex_handle; //export

#define ST7796_USE_MUTEX

#ifdef ST7796_USE_MUTEX
#define ST7796_MUTEX_TAKE() xSemaphoreTake(g_st7796_mutex_handle,portMAX_DELAY)
#define ST7796_MUTEX_GIVE() xSemaphoreGive(g_st7796_mutex_handle)
#else
#define ST7796_MUTEX_TAKE() ((void)0)
#define ST7796_MUTEX_GIVE() ((void)0)
#endif



#endif
