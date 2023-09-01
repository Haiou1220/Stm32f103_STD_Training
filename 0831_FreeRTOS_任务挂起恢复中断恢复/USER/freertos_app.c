#include "freertos_app.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "global.h"



TaskStruct_t gAppCreate_task_st;
TaskStruct_t gLed0_task_st ;
TaskStruct_t fLed1_task_st ;
 

uint16_t		 AppCreate_task_StackDepth =128;						
UBaseType_t		 AppCreate_task_Priority = 1;						
TaskHandle_t     AppCreate_task_TaskHandle;
void AppCreate_task(void* pvParameters);

uint16_t 		 led0_task_StackDepth =128;						
UBaseType_t		 led0_task_Priority = 2;		
TaskHandle_t     led0_task_TaskHandle;
void led0_blink_task(void* pvParameters);

uint16_t 		 led1_task_StackDepth =128;						
UBaseType_t		 led1_task_Priority = 3;	
TaskHandle_t     led1_task_TaskHandle;
void led1_blink_task(void* pvParameters);

uint16_t 		 usartCmd_task_StackDepth =128;						
UBaseType_t		 usartCmd_task_Priority = 0;
TaskHandle_t     usartCmd_task_TaskHandle;
void usartCmd_task(void* pvParameters);


 
 

void freertos_app(void) //��������
{
	BaseType_t BaseType ;
	
 
		
	
	BaseType = xTaskCreate( (TaskFunction_t) AppCreate_task,
							( char *)  "AppCreate_task",
							( uint16_t) AppCreate_task_StackDepth,
							(void *)   NULL,
							(UBaseType_t) AppCreate_task_Priority,
							(TaskHandle_t *)  &AppCreate_task_TaskHandle);
							
		if(BaseType  ==  pdPASS )
		{	
			printf(" AppCreate_task  Create   ----OK  \r\n");
			vTaskStartScheduler();//��������
			
		}
		
		
	
}



void AppCreate_task(void* pvParameters)
{
	
	
	BaseType_t BaseType0,BaseType1,BaseType2;

	taskENTER_CRITICAL(); //�����ٽ���
	
	BaseType0 =  xTaskCreate( (TaskFunction_t) led0_blink_task,
							( char *)  "led0_blink_task",
							( uint16_t) led0_task_StackDepth,
							(void *)   NULL,
							(UBaseType_t) led0_task_Priority,
							(TaskHandle_t *)  &led0_task_TaskHandle);
							
				
							
	if(BaseType0 == pdPASS)
	{
	
		printf(" led0_blink_task    Create \r\n");
		
	}
	
							
	BaseType1 =  xTaskCreate( (TaskFunction_t) led1_blink_task,
							( char *)  "led1_blink_task",
							( uint16_t) led1_task_StackDepth,
							(void *)   NULL,
							(UBaseType_t) led1_task_Priority,
							(TaskHandle_t *)  &led1_task_TaskHandle);
	
	if(BaseType1 == pdPASS)
	{
	
		printf(" led1_blink_task  Create \r\n");
		
	}
	
	
	BaseType2 = xTaskCreate( (TaskFunction_t) usartCmd_task,
							( char *)  "usartCmd_task",
							( uint16_t) usartCmd_task_StackDepth,
							(void *)   NULL,
							(UBaseType_t) usartCmd_task_Priority,
							(TaskHandle_t *)  &usartCmd_task_TaskHandle);
							
							
	if(BaseType2 == pdPASS)
	{
	
		printf(" usartCmd_task  ---------------Create \r\n");
		
	}
				
	
	

	 
	  printf(" vTaskDelete(AppCreate_task_TaskHandle) -----------ok  \r\n");
	vTaskDelete(AppCreate_task_TaskHandle);//ɾ��start ����
 
	taskEXIT_CRITICAL(); //�˳��ٽ���
  
}




void usartCmd_task(void* pvParameters) //��������ɾ�� ����
{

	printf("in usartCmd_task\r\n");
	
	while(1)
	{

	  vTaskDelay(10);//1000ms
		
 	if(gUSART1_receive_IT_u8falg)
	{
    	gUSART1_receive_IT_u8falg = 0;
		printf("\r\n usart receive data:%c \r\n",gUSART1_receive_IT_u8data);
		
		if(gUSART1_receive_IT_u8data == '0') //cmd0ɾ������led0_task
		{	
			if(led0_task_TaskHandle != NULL) //�ж��Ƿ�ǿھ��
			{
					vTaskDelete(led0_task_TaskHandle);
					led0_task_TaskHandle  = NULL;
					printf("vTaskDelete(led0_task_TaskHandle); \r\n" );
			}
					 
		
		}else if(gUSART1_receive_IT_u8data == '1')////cmd1ɾ������led1_task
		{
				if(led1_task_TaskHandle != NULL)
				{
				
				vTaskDelete(led1_task_TaskHandle);
					led1_task_TaskHandle = NULL;
				printf("vTaskDelete(led1_task_TaskHandle); \r\n" );
				
				}
				 
		}else if(gUSART1_receive_IT_u8data == '2')////cmd2  ��������led0_task
		{
			
			printf("��������  led0_task_TaskHandle \r\n");
			vTaskSuspend( led0_task_TaskHandle );//��������
		   
		
		
		
		}else if(gUSART1_receive_IT_u8data == '3')////cmd3  �ָ�����led0_task
		{
		
		
		    printf("�ָ�����  led0_task_TaskHandle \r\n");
		
			vTaskResume(led0_task_TaskHandle); // �ָ�����
		
			
		
		
		
		}

	
	}
		

	}
}
	
