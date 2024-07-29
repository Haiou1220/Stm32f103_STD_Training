#include "flash_internal.h"
#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "print.h"

/*
片内flash操作: 
补充读写接口
半字读写u16
组织块:中容量:64个1K字节/页(中容量)     IROM1: start:0x800 0000 size:0x1 0000(64KB)
最小擦除单位:页;每页1K字节(1024字节)
最小写入数据单位:半字
最大擦除单位:整片擦除

写入前,必须是已经擦除的,否则写入失败,并产生错误状态标志位
在执行闪存写操作时，任何对闪存的读操作都会锁住总线，在写操作完成后读操作才能正确地进行。\
既在进行写或擦除操作时，不能进行代码或数据的读取操作。
*/

#if FLASH_STM_SIZE<256
    #define FLASH_STM_PAGE_SIZE 1024 //字节
#else 
    #define FLASH_STM_PAGE_SIZE	2048
#endif	

#define FLASH_STM32_BASE 0x08000000 	//STM32 FLASH的起始地址
   //64kb容量,最后一个页区地址: 0x0800FC00;
    //512KB容量,最后一个页区地址: 0x0807 F800;
#define FLASH_LAST_PAGE_ADDR  FLASH_STM32_BASE+((FLASH_STM_SIZE/(FLASH_STM_PAGE_SIZE/1024) ) - 1 )*FLASH_STM_PAGE_SIZE
#define FLASH_KEY_PRESS_COUNT_STORE_ADDR           FLASH_LAST_PAGE_ADDR + 0
#define FLASH_MCU_RESET_COUNT_STORE_ADDR           FLASH_LAST_PAGE_ADDR + 2



TaskHandle_t g_flash_internal_task_Handler = NULL; //export
SemaphoreHandle_t flashSTM_semphr_Handler = NULL;  //export
static u16 sg_flash_MCU_reset_count = 0;
static TimerHandle_t print_xTimer_hdl = NULL;

void FLASHSTM_WriteHalfWord(u32 WriteAddr,u16 WriteData);   
u16 flashSTM_ReadhalfWord(u32 faddr);

//static u16 sg_key_press_count = 0;


void flashPrintvTimerCallback( TimerHandle_t xTimer )
{
    print("KEY_PRESS_COUNT_STORE:%d\r\n",flashSTM_ReadhalfWord(FLASH_KEY_PRESS_COUNT_STORE_ADDR));
    print("MCU_RESET_COUNT_STORE:%d\r\n",flashSTM_ReadhalfWord(FLASH_MCU_RESET_COUNT_STORE_ADDR));
}

void FLASHSTM_Write(u32 WriteAddr,u16 *pBuffer,u16 len);

void flash_internal_task(void *pvarg)
{
     uint32_t count = 0;
	 u16 temp;

    // u16 Writeu16Data[10] = {0};
    // for (int i = 0; i < sizeof(Writeu16Data)/sizeof(u16); i++)
    // {
    //     Writeu16Data[i] = 0xffff;
    // }    
    // FLASHSTM_Write(FLASH_LAST_PAGE_ADDR,Writeu16Data,sizeof(Writeu16Data)/sizeof(u16));//写入一个字     (FLASH_LAST_PAGE_ADDR,(u16)(sg_flash_MCU_reset_count + 1));


    //sg_key_press_count = flashSTM_ReadhalfWord(FLASH_KEY_PRESS_COUNT_STORE_ADDR);
    sg_flash_MCU_reset_count = flashSTM_ReadhalfWord(FLASH_MCU_RESET_COUNT_STORE_ADDR);
    //修改mcu上电复位次数写入flash
    FLASHSTM_WriteHalfWord(FLASH_MCU_RESET_COUNT_STORE_ADDR,(u16)(sg_flash_MCU_reset_count + 1));

    flashSTM_semphr_Handler = xSemaphoreCreateBinary();
    print_xTimer_hdl = xTimerCreate("print_xTimer", pdMS_TO_TICKS(2000), pdTRUE, (void *)0, flashPrintvTimerCallback);  
    if(print_xTimer_hdl != NULL ) xTimerStart(print_xTimer_hdl,pdMS_TO_TICKS(2000));

    ASSERT(NULL != print_xTimer_hdl );
    ASSERT(NULL != flashSTM_semphr_Handler);
        
    while (1)
    {        
     
     xSemaphoreTake(flashSTM_semphr_Handler,portMAX_DELAY);
     print("IN flash_internal_task:%d,flashSTM_semphr get!\r\n",count++);	
    
	 temp = flashSTM_ReadhalfWord(FLASH_KEY_PRESS_COUNT_STORE_ADDR);
     FLASHSTM_WriteHalfWord(FLASH_KEY_PRESS_COUNT_STORE_ADDR,(u16)(temp + 1));
          
    }
}



u16 flashSTM_ReadhalfWord(u32 faddr)
{
   return *(vu16*)faddr;
}

/**
 * @brief 从指定地址开始读出指定长度的数据
 * 
 * @param faddr 起始地址
 * @param pBuffer 数据指针
 * @param NumToRead 半字(16位)数
 */
void FLASHSTM_Read(u32 faddr,u16 *pBuffer,u16 NumToRead)
{
     for (u16 i = 0; i < NumToRead; i++)
     {
        pBuffer[i] = flashSTM_ReadhalfWord(faddr);
        faddr += 2; //偏移两个字节
     }     
}


/**
 * @brief 不检查的写入
 * 
 * @param WriteAddr :起始地址
 * @param pBuffer :数据指针
 * @param ToWriteCount :半字(16位)数
 * @return * void 
 */
void FLASHSTM_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 ToWriteCount)   
{ 			 		 
	u16 i;
	for(i=0;i<ToWriteCount;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
} 

static u16 FLASHSTM_BUF[FLASH_STM_PAGE_SIZE/2]; //本地FLASHSTM缓存
/**
 * @brief 从指定地址开始写入指定长度的数据
 * 
 * @param WriteAddr 起始地址
 * @param pBuffer 数据指针
 * @param len 半字(16位)数的长度
 */
void FLASHSTM_Write(u32 WriteAddr,u16 *pBuffer,u16 len)	
{   
	u32 page_pos;	   //页地址
	u16 page_off;	   //页内偏移地址,u16
	u16 to_write_page_remain_num; //页区内剩余地址,u16	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
    u32 ToWriteCount = len; //buffer,u16

	if(WriteAddr<FLASH_STM32_BASE || (WriteAddr>=(FLASH_STM32_BASE+1024*FLASH_STM_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
    
	offaddr=WriteAddr-FLASH_STM32_BASE;		//实际偏移地址.
	page_pos = offaddr/FLASH_STM_PAGE_SIZE;			//页区地址  
	page_off = (offaddr%FLASH_STM_PAGE_SIZE)/2;		//在页区内的偏移,u16
	to_write_page_remain_num = (FLASH_STM_PAGE_SIZE)/2 - page_off;		//页区剩余空间大小,u16   
	if(ToWriteCount<=to_write_page_remain_num)to_write_page_remain_num=ToWriteCount;//不大于该页区范围
	while(1) 
	{	
		FLASHSTM_Read(page_pos*FLASH_STM_PAGE_SIZE+FLASH_STM32_BASE,FLASHSTM_BUF,FLASH_STM_PAGE_SIZE/2);//读出整个页区的内容
		
        for(i=0;i<to_write_page_remain_num;i++)//校验数据
		{
			if(FLASHSTM_BUF[page_off+i]!=0XFFFF)break;//需要擦除  	  
		}
		if(i<to_write_page_remain_num)//需要擦除
		{
			FLASH_ErasePage(page_pos*FLASH_STM_PAGE_SIZE+FLASH_STM32_BASE);//擦除这个页区
			for(i=0;i<to_write_page_remain_num;i++)//更新到本地缓存
			{
				FLASHSTM_BUF[i+page_off]=pBuffer[i];	  
			}
			FLASHSTM_Write_NoCheck(page_pos*FLASH_STM_PAGE_SIZE+FLASH_STM32_BASE,FLASHSTM_BUF,FLASH_STM_PAGE_SIZE/2);//写入整个页区  
		}else FLASHSTM_Write_NoCheck(WriteAddr,pBuffer,to_write_page_remain_num);//写已经擦除了的,直接写入页区剩余区间.

        if (to_write_page_remain_num == ToWriteCount)
        {
            break; // 写入结束了
        }
        else
        { // 写入未结束

            page_pos++;                // 页区地址增1
            page_off = 0;              // 偏移位置为0
            pBuffer += to_write_page_remain_num;    // 指针偏移
            WriteAddr += to_write_page_remain_num;  // 写地址偏移
            ToWriteCount -= to_write_page_remain_num; // byteToWriteCount递减

            if (ToWriteCount > FLASH_STM_PAGE_SIZE/2)
                to_write_page_remain_num = FLASH_STM_PAGE_SIZE/2; // 下一个页区还是写不完
            else
                to_write_page_remain_num = ToWriteCount; // 下一个页区可以写完了
        }
    };	
	FLASH_Lock();//上锁
}


//WriteAddr:起始地址
//WriteData:要写入的数据
void FLASHSTM_WriteHalfWord(u32 WriteAddr,u16 WriteData)   	
{
	FLASHSTM_Write(WriteAddr,&WriteData,1);//写入一个字 
}


 

