#ifndef __STEPPER_APP_H
#define __STEPPER_APP_H
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"


extern TaskHandle_t  g_StepMotorTask_Handler; //export
void StepMotor_task(void* pvarg);

#endif

