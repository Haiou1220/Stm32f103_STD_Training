

#include "iic_soft_eeprom.h"
#include "stm32f10x.h"
#include "global.h"
#include "systick.h"
#include "iic_soft.h"





u8 iic_soft_eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) //
{
	/*
	1.��ʼ
	2.��id  д                      //���� д��
	3.�ȴ�Ӧ��
	4.�����Ӧ�𣬷���addr �ߣ��ȴ�Ӧ��
	5.�����Ӧ�𣬷���addr �ͣ� �ȴ�Ӧ��
	6 ��ʼ
	7 ��ID ��
	8 �ȴ�Ӧ��
	9 ��һ��byte+nack
	10 �����Ự
	*/

	
	iic_start();
	iic_send_byte(id);	//����һ��byte
	
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send id+w no ack ) ;
		return 1;
	}
	// id ack ,next   
	
	iic_send_byte((WordaddrToRead & 0xFF00)>>8 );	//���ֽ�
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send Hight_addr no ack ) ;
		return 1;
	}
	// hight addr ack,next
	
	iic_send_byte(WordaddrToRead & 0x00FF );	//���ֽ�
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send Hight_addr no ack ) ;
		return 1;
	}
	// low addr ack,next	
		
	iic_start();
	iic_send_byte(id | 0x01);	// bit,����
	
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send id+r no ack ) ;
		return 1;
	}
	
	//��ʼ��
	*store_buf =  iic_read_byte( 1); //1=nack
	
	iic_stop( );
	
	return 0;
}



