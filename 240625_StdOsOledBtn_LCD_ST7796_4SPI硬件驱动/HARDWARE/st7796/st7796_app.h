#ifndef __ST7796_APP_H
#define __ST7796_APP_H
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"


void st7796_task(void *pvarg);
extern TaskHandle_t g_st7796Task_Handler; //export 声明

#endif
