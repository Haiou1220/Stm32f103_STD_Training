

 #ifndef __AHT10_I2C_HARD_H__
#define __AHT10_I2C_HARD_H__

#include "stm32f10x.h"

 



u8 AHT10_ReadyMeasureData(void) ;     //������������ �������� 0xac+0x33+0x00
u8 AHT10_SoftRestet(void) ;  //��λ  ��������  0xba
float AHT10_GetTempera(void);  //��ȡ��ʪ������: ״̬ ʪ��1 ʪ��2 ��ʪ��3����4λ���¶�1����4λ���� �¶�2 �¶�3(���)          4+8+8 = 20bit

u8 AHT10_GetStatusWord(void) ;
	
 u8 AHT10_GetStatus_CALEnableFlag(void) ;
 u8 AHT10_GetStatus_BusyFlag(void) ;
 u8 AHT10_Calibration(void);
 void AHT10_init(void) ;  //��ʼ��
 float AHT10_Task_GetTemperature(void);    //����-AHT10 GetTemperature
	float ATH10_GetHumidity(void) ; 
	float ATH10_Task_GetHumidity(void);  //����-AHT10 GetHumidity
 extern	void  sysTickDelay_ms(uint32_t delay_count); //9M ->9*1000 000  
		
	
#endif 
