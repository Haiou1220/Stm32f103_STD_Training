

 #ifndef __AHT10_I2C_HARD_H__
#define __AHT10_I2C_HARD_H__

#include "stm32f10x.h"

 



u8 AHT10_ReadyMeasureData(void) ;     //触发测量数据 发送数据 0xac+0x33+0x00
u8 AHT10_SoftRestet(void) ;  //软复位  发送数据  0xba
float AHT10_GetTempera(void);  //读取温湿度数据: 状态 湿度1 湿度2 【湿度3（高4位）温度1（低4位）】 温度2 温度3(最低)          4+8+8 = 20bit

u8 AHT10_GetStatusWord(void) ;
	
 u8 AHT10_GetStatus_CALEnableFlag(void) ;
 u8 AHT10_GetStatus_BusyFlag(void) ;
 u8 AHT10_Calibration(void);
 void AHT10_init(void) ;  //初始化
 float AHT10_Task_GetTemperature(void);    //任务-AHT10 GetTemperature
	float ATH10_GetHumidity(void) ; 
	float ATH10_Task_GetHumidity(void);  //任务-AHT10 GetHumidity
 extern	void  sysTickDelay_ms(uint32_t delay_count); //9M ->9*1000 000  
		
	
#endif 
