
#include "stepper_app.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "print.h"

#include "stepper_drv.h"

TaskHandle_t  g_StepMotorTask_Handler; //export

/*
电机 额定电流1.5A/相
PUL 宽度应大于1.2us。
DIR 方向信号应 先于脉冲信号至少5us建立
ENA 使能输入信号

细分:1 脉冲/圈:200   SW1:0N SW2:ON SW3:OFF
电流:1.5 峰值1.7     SW4:ON SW5:0N SW6:OFF
最大启动频率: 1.5k HZ = 0.66ms = 660 us
最大空载频率: 空载:1.9k Hz = 0.52ms = 520us
*/
void StepMotor_task(void* pvarg)
{
   
    stepper_drv_gpioCtlPin_init(); //gpio en pin ,dir pin,pb10,pb11
    stepper_drv_timerOC_init(); //timer2,channel1,pa0,1ms频率脉冲

    stepper_drv_timerOC_set_ccr((660*1.5)/2);//

    //在 key_app_task 按键按下执行 stepper_drv_timerOC_start();
    while (1)
    {
      

       vTaskDelay(pdMS_TO_TICKS(1000));
     

    }
    

}

