#include "usart_app.h"
#include "stm32f10x.h"
#include "usart.h"
#include "print.h"
#include "FreeRTOS.h"
#include "task.h"

void usart1_task(void *argv) 
{
	uint32_t count = 0;
	usart1_init(115200); // pa9 tx,  pa10 rx
	while (1)
	{
		vTaskDelay(pdMS_TO_TICKS(1000));
		//print("IN usart1_task:%d\r\n",count++);		
	}
}





