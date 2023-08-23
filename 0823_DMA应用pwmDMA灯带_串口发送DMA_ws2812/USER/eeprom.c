
#include "eeprom.h"
#include "global.h"
#include "systick.h"
#include "i2c_hard.h"
#include "main.h"
 


u8 eeprom_byteRead_byteAddr(uint8_t id,uint8_t addrToRead,uint8_t *store_buf,uint8_t count_read)  //ok
{	
	u32 FlagTimeoutReset = NumCountZero_1s; 
	u32 FlagTimeout ;
	 
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//����id��ַ 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д���� 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//���� Ҫ����д�ļĴ�����ַ 
	I2C_SendData( I2C1,  addrToRead);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}

	//�ڶ���ͨѶ
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while (!I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT))  // ͨѶ��ʼ ev5
	{
		if((FlagTimeout--) == 0) return I2CError();  
	}

	//����id��ַ,������
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1��������
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // ��ַӦ�� ev6
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
		//���ֽڣ�//������� ���ظ� ACK / noAck
	for(u8 i = 0;i<count_read ; i++)
	{
		    FlagTimeout = FlagTimeoutReset;
		if(i == count_read - 1  ){ //���һ���� No ack
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}	
		while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
		{
			if((FlagTimeout--) == 0) return I2CError();
		}
		store_buf[i] = I2C_ReceiveData(I2C1);	
	}		
			
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ;//�ɹ�
}


u8 eeprom_byteWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t byteData)  //ok
{	
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	 
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//����id��ַ 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д���� 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//���� Ҫд�ļĴ�����ַ 
	I2C_SendData( I2C1,  addrToWrite);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}

	//д�ֽ�
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  byteData);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
	
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //�ɹ�
}


u8 eeprom_bufWrite_byteAddr(uint8_t id,uint8_t addrToWrite,uint8_t *bufWriteData,uint8_t count)  //  Ҫ�޸� ��ҳ�ĵ�ַ
{	
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	 
	while(count>0)
	{
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//����id��ַ 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д���� 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//���� Ҫд�ļĴ�����ַ 
	I2C_SendData( I2C1,  addrToWrite);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	while(count>0) //���ͻ������ڲ���8���ֽڵķ�Χ
	{
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  *bufWriteData  );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
			//һ���ֽ����
			count--;//������һ
			bufWriteData++;//Դ��ַ��һ
			addrToWrite++; //Ŀ�ĵ�ַ����,��һ���������
			if((addrToWrite % 8) == 0)  break; //ĿǰĿ�ĵ�ַ�����ڻ�������Χ��  0 - 7;  8-15,16-23,24-31,248-255 
			
	
	}//while ���ͻ�������8���ֽ�
	
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	//��ʱ6ms д����
	 sysTickDelay_ms(6);
	

	}//while �ȴ�ȫ���������
	return 	0 ;//�ɹ�

}






u8 eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) //
{

	u32 FlagTimeoutReset = NumCountZero_1s; 
	u32 FlagTimeout ;
	u8 addrByteLow =  WordaddrToRead & 0XFF ;
	u8 addrByteHigh = (WordaddrToRead >> 8) & 0X7F;
	
	//�ж� addrWord ��Χ
	if( !(WordaddrToRead < (1<<15)) ) { return I2CError(); }
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	//����id��ַ 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д���� 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//���� Ҫ����д�ļĴ�����ַ �ߵ�ַ
	I2C_SendData( I2C1,  addrByteHigh);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	//���� Ҫ����д�ļĴ�����ַ �͵�ַ
	I2C_SendData( I2C1,  addrByteLow);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	

	//�ڶ���ͨѶ
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while (!I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT))  // ͨѶ��ʼ ev5
	{
		if((FlagTimeout--) == 0) return I2CError();  
	}

	//����id��ַ,������
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1��������
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // ��ַӦ�� ev6
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
		//���ֽڣ�//������� ���ظ� ACK / noAck
	for(u8 i = 0;i<count ; i++)
	{
		    FlagTimeout = FlagTimeoutReset;
		if(i == count - 1  ){ //���һ���� No ack
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}	
		while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
		{
			if((FlagTimeout--) == 0) return I2CError();
		}
		store_buf[i] = I2C_ReceiveData(I2C1);	
	}		
			
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ;//�ɹ�





}
u8 eeprom_byteWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t byteData)  //
{
	
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	u8 addrByteLow =  WordAddrToWrite & 0XFF ;
	u8 addrByteHigh = (WordAddrToWrite >> 8) & 0X7F;
	 
	//�ж� addrWord ��Χ
	if( !(WordAddrToWrite < (1<<15)) ) { return I2CError(); }
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//����id��ַ 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д���� 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 
	}
	
	//���� Ҫд�ļĴ�����ַ  �ߵ�ַ
	I2C_SendData( I2C1,  addrByteHigh);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	//���� Ҫд�ļĴ�����ַ �͵�ַ
	I2C_SendData( I2C1,  addrByteLow);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}

	//д�ֽ�
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  byteData);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
	
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //�ɹ�


}
u8 eeprom_bufWrite_wordAddr(uint8_t id,uint16_t WordAddrToWrite,uint8_t *bufWriteData,uint8_t count)  
{
	u32 FlagTimeoutReset = NumCountZero_1s;
	u32 FlagTimeout ;
	 
	//�ж� addrWord ��Χ
	if( !(WordAddrToWrite < (1<<15)) ) { return I2CError(); }  
	
	while(count>0)
	{
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//����id��ַ 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д���� 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 	}
	
	//���� Ҫд�ļĴ�����ַ  �ߵ�ַ   (WordAddrToWrite >> 8) & 0X7F;
	I2C_SendData( I2C1,  (WordAddrToWrite >> 8) & 0X7F);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
		//���� Ҫд�ļĴ�����ַ  �͵�ַ //	 WordAddrToWrite & 0XFF ;

	I2C_SendData( I2C1,  WordAddrToWrite & 0XFF);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	while(count>0) //���� page ���ֽ���Ŀ    64���ֽ�   
	{
		    FlagTimeout = FlagTimeoutReset;
			//�ж� addrWord ��Χ
			//if( !(WordAddrToWrite < (1<<15)) ) { return I2CError(); } 
			
			I2C_SendData(  I2C1,  *bufWriteData  );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
			//һ���ֽ����
			count--;//������һ
														gDebug_u8 = count; //����ʹ��
			bufWriteData++;//����Դ��ַ��һ
			WordAddrToWrite++; //Ŀ�ĵ�ַ����,��һ��������� 
																gDebug_u16= WordAddrToWrite;//����
			
			if(((WordAddrToWrite & 0x003f) == 0)   )  //  11 1111(6λ) 0-63 
			{
				break;//��һ����ַ ������߽�  ������
			}

	
	}//while ���ͻ�������64���ֽ�
	
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	//��ʱ6ms д��ҳ����
	sysTickDelay_ms(10);
	

	}//while �ȴ�ȫ���������
	return 	0 ;//�ɹ�


}
 u8 eeprom_bufRead_wordAddr(uint8_t id,uint16_t WordAddrToRead,uint8_t *bufRead_Store,uint8_t count) //ing
{
	u32 FlagTimeoutReset = NumCountZero_1s; 
	u32 FlagTimeout ;
	 
	//�ж� addrWord ��Χ
	if( !(WordAddrToRead < (1<<15)) ) { return I2CError(); }  
	
	//�Ƿ�æ��
	if( I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY))  //0:����  1��æµ
	{	 
		return I2CError();
	}
	//������ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  �ȴ�ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	
		//����id��ַ 
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0��д���� 
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//�ȴ�ev6
	{
		if((FlagTimeout--) == 0) return I2CError(); 	
	}
	
	//���� Ҫ���ļĴ�����ַ  �ߵ�ַ   (WordAddrToWrite >> 8) & 0X7F;
	I2C_SendData( I2C1,  (WordAddrToRead >> 8) & 0X7F);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	//���� Ҫ���ļĴ�����ַ  �͵�ַ //	 WordAddrToWrite & 0XFF ;
	I2C_SendData( I2C1,  WordAddrToRead & 0XFF);
	while (!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED)  )  // ������ģʽ ev8_2 
	{ 
		if((FlagTimeout--) == 0) return I2CError();
	}
	
	//�ڶ��β�����ʼ�ź�
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while (!I2C_CheckEvent( I2C1, I2C_EVENT_MASTER_MODE_SELECT))  // ͨѶ��ʼ ev5
	{
		if((FlagTimeout--) == 0) return I2CError();  
	}

	//����id��ַ,������
	FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Receiver);// 1��������
	//Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
	while ( !I2C_CheckEvent(  I2C1,   I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) // ��ַӦ�� ev6 //�ȴ�ack(0)
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
////////////////////////////////////////////////////////////////////////////////////////////	
	//���ж��� ����ֽڣ����ã��ظ�ACK ���� no Ack
	for(u8 i = 0;i<count ; i++)
	{
		FlagTimeout = FlagTimeoutReset;
		if(i == count - 1  ){ //���һ���� No ack//	(i == count - 1 )?I2C_AcknowledgeConfig(I2C1,DISABLE):I2C_AcknowledgeConfig(I2C1, ENABLE) ;
			I2C_AcknowledgeConfig(	I2C1,  DISABLE);
		}else{
			I2C_AcknowledgeConfig(	I2C1,  ENABLE);
		}
		
		//�ȴ� ������� �ź�
		while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_RECEIVED)	 ) 
		{
			if((FlagTimeout--) == 0) return I2CError();
		}
		//�洢�յ�������
		bufRead_Store[i] = I2C_ReceiveData(I2C1);	
	}		
			
	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);
	
	return 	0 ;//�ɹ�
}	

