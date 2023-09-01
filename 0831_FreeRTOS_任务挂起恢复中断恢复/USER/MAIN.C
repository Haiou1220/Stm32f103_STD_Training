/**
  ******************************************************************************
 freertos 系统移值完成，建立了 三个任务： led0 任务，led1任务，串口命令任务。
 串口命令任务：用于控制 前面两个任务的执行状态，挂起与恢复。
 中断处理：在串口中断调用了回调函数，用于处理 对 led0 任务 的挂起恢复 处理。
 RTOS 系统可管理的最高中断优先级 =  5, 所以硬件的抢占优先级不能高于rtos，即是只能5-15 .   
 
 串口命令
 cmd0://cmd0删除任务led0_task
 cmd1://cmd1删除任务led1_task
 
 cmd2://cmd2 挂起  任务led0_task
 cmd3://cmd2 恢复  任务led0_task
 

 cmd5://cmd5 中断 恢复  任务led0_task，中断
 
 对应在rtos系统上 接管 硬件中断的要求：
 分组要求全为抢占，而且 外设中断的抢占优先级要要大于等于5，5-15的硬件中断优先级可以满足条件。
 
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stdio.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_app.h"


 

/*Compiler definec : USE_STDPERIPH_DRIVER,STM32F10X_MD */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void Bsp_init(void)
{

 NVIC_PriorityGroupConfig(  NVIC_PriorityGroup_4);//配置中断分组,4bit为抢占
  usart1_init(115200 ) ;//pa9 tx,  pa10 rx
   led_init( );//led0 led1 pb5 pe5
	
  led0_switch(0);
  led1_switch(0);
	
  printf("Bsp_init finsh \r\n");

}

			
					
int main(void)
{
  

	Bsp_init( ) ;// 板级 硬件 初始化

	freertos_app(); //开启调度

   
   while(1);
}






