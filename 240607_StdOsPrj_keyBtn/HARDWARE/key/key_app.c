#include "key_app.h"
#include "key.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "print.h"
#include "timers.h"

#include "key_button.h"
/*
不使用以下中断函数,这个按键扫描驱动没有用到中断
EXTI->IMR |= (0xfffff & (1 << KEY_EXTI_Line_NUM)); // // 开启外部中断线,置位 
EXTI->IMR &= (0xfffff & (~(1 << KEY_EXTI_Line_NUM))); // 清零 ,关闭外部中断线
 */

extern uint32_t g_oled_count ; //调试按键,oled 显示

TaskHandle_t g_keyTask_Handler; //export
TimerHandle_t sg_key_timer_handle;

void TimerCallback_keyProcess(TimerHandle_t xTimer);

// new 
enum Button_IDs {
	btn1_id,
    btn2_id,		
};

void BTN1_PRESS_DOWN_Handler(void* btn);
void BTN1_PRESS_UP_Handler(void* btn);
void BTN1_DOUBLE_Click_Handler(void* btn);
void BTN1_LONG_PRESS_HOLD_Handler(void* btn);
void BTN1_PRESS_REPEAT_Handler(void* btn);
void  BTN1_SINGLE_Click_Handler(void* btn);
void BTN1_LONG_PRESS_START_Handler(void* btn);

void button_init(void);
//new

/*
void EXTI0_IRQHandler(void) //不使用中断方法
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    EXTI_ClearITPendingBit(EXTI_Line0);    
   // vTaskNotifyGiveFromISR(g_keyTask_Handler, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
*/

void key_task(void *pvarg)
{   
    BaseType_t xReturn;    
   
    key_init_gpio(); //初始化硬件
    
    button_init(); //初始化button 对象

    //创建定时器,处理按键状态机
    sg_key_timer_handle = xTimerCreate("key_timer",pdMS_TO_TICKS(10),pdTRUE, (void*)1 ,TimerCallback_keyProcess);//uxAutoReload = pdFALSE,单次执行
    ASSERT(NULL != sg_key_timer_handle);
    xReturn = xTimerStart(sg_key_timer_handle,portMAX_DELAY);
    ASSERT(pdFAIL != xReturn);
           
    while (1)
    {        

     vTaskDelay(pdMS_TO_TICKS(1000)); 

    }
}



struct Button btn1;
struct Button btn2;

uint8_t read_button_GPIO(uint8_t button_id)
{
	// you can share the GPIO read function with multiple Buttons
	switch(button_id)
	{
		case btn1_id:
			return  key_getValue();

		default:
			return 0;
	}
}


/*

不要在定时器的回调函数中使用一些导致任务阻塞的函数或代码，例如vTaskDelay()，否则会导致FreeRTOS后台任务进入到阻塞状态。
而且应该尽量让定时器回调函代码简洁高效快速执行。

*/
void TimerCallback_keyProcess(TimerHandle_t xTimer)
{
    // Need to be called cyclically within the specified period,Sample cycle: 5 - 20ms
    button_process_all();

    //xTimerReset(sg_key_timer_handle, portMAX_DELAY);
}

void button_init(void)
{
    button_object_init(&btn1, read_button_GPIO, 0, btn1_id);

    button_attach(&btn1, PRESS_DOWN, BTN1_PRESS_DOWN_Handler);
    button_attach(&btn1, PRESS_UP, BTN1_PRESS_UP_Handler);
    button_attach(&btn1, PRESS_REPEAT, BTN1_PRESS_REPEAT_Handler);
    button_attach(&btn1, SINGLE_CLICK, BTN1_SINGLE_Click_Handler);
    button_attach(&btn1, DOUBLE_CLICK, BTN1_DOUBLE_Click_Handler);
    button_attach(&btn1, LONG_PRESS_START, BTN1_LONG_PRESS_START_Handler);
    button_attach(&btn1, LONG_PRESS_HOLD, BTN1_LONG_PRESS_HOLD_Handler);

    key_button_register(&btn1); //注册进入列表    
}


void BTN1_PRESS_DOWN_Handler(void* btn)
{
	// print("in BTN1_PRESS_DOWN_Handler \r\n");
}

void BTN1_PRESS_UP_Handler(void* btn)
{
	// print("in BTN1_PRESS_UP_Handler \r\n");
}


void BTN1_DOUBLE_Click_Handler(void* btn)
{
	 //print("in BTN1_DOUBLE_Click_Handler \r\n");
     if(g_oled_count>0){g_oled_count--;};          
}

void BTN1_LONG_PRESS_HOLD_Handler(void* btn)
{
	// print("in BTN1_LONG_PRESS_HOLD_Handler \r\n");
     g_oled_count++;
}

void BTN1_PRESS_REPEAT_Handler(void* btn)
{
	// print("in BTN1_PRESS_REPEAT_Handler \r\n");
}
void  BTN1_SINGLE_Click_Handler(void* btn)
{
	// print("in BTN1_SINGLE_Click_Handler \r\n");
     g_oled_count++;
}
void BTN1_LONG_PRESS_START_Handler(void* btn)
{
	// print("in BTN1_LONG_PRESS_START_Handler \r\n");
}


