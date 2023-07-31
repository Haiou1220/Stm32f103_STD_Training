

#include "i2c_hard.h"
#include "stm32f10x.h"
#include "global.h"
#include "systick.h"
#include "stdio.h"

/**/
u8 I2CErrorHandle(uint8_t* file, uint32_t line ,char* message ) //#define Error ErrorPrint((uint8_t*)__FILE__,__LINE__);
{
	printf("I2C Error:%s,%d,%s\r\n",file,line,message);
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	return 1;//ʧ��
}
u8 I2cHard_buf_read_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_read) //û��ַ����buf
{	
	//u32 FlagTimeoutReset = Num_DownToZero_1s;
	//u32 FlagTimeout ;
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	//FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		//if((FlagTimeout--) == 0) return I2CError();
		sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
		if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
	}
	//����id��ַ,������
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1��������
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // ��ַӦ�� ev6
	{
		//if((FlagTimeout--) == 0) return I2CError();
		sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
		if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
	}

	for(u8 i = 0 ;i < num_byte_to_read; i++) ////���ֽ�
	{
		    //FlagTimeout = FlagTimeoutReset;
		//���һ�������ֽڣ��ظ�NACK
		if(i == num_byte_to_read - 1)
		{	
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else
		{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}	
		//�ȴ��������
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
			{
				//if((FlagTimeout--) == 0) return I2CError();
				sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
				if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
			}
			////����
		pbuf[i] = I2C_ReceiveData(I2C1);
	}
			
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //�ɹ�
			
}

u8 I2cHard_buf_write_NoAddr(uint8_t id,uint8_t* pbuf,uint8_t num_byte_to_write) //û��ַд��buf
{
	//u32 FlagTimeoutReset = Num_DownToZero_1s;
	//u32 FlagTimeout ;
	 
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	//FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		//if((FlagTimeout--) == 0) return I2CError();
		sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
		if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
	}
	//����id��ַ 
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д����
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{	
		sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
		if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
	}
	
	for(int i = 0;i < num_byte_to_write;i++ )
	{
		//д�ֽ�
		    //FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  pbuf[i]);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				//if((FlagTimeout--) == 0) return I2CError();
				sysTickDelay_us_NonBlock(1000);//  //FlagTimeout = FlagTimeoutReset;
				if(getFlagSysTickDelayOut() ) return I2CError();  //if((FlagTimeout--) == 0) return I2CError();
			}
	}

	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //�ɹ�
						
}


