#ifndef __KEY_APP_H
#define __KEY_APP_H
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"

void key_task(void *pvarg);
extern TaskHandle_t g_keyTask_Handler;


#endif

