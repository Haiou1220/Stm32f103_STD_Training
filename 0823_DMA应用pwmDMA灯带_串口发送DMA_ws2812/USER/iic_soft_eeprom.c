

#include "iic_soft_eeprom.h"
#include "stm32f10x.h"
#include "global.h"
#include "systick.h"
#include "iic_soft.h"





u8 iic_soft_eeprom_byteRead_wordAddr(uint8_t id,uint16_t WordaddrToRead,uint8_t *store_buf,uint8_t count) //
{
	/*
	1.开始
	2.发id  写                      //读高 写低
	3.等待应答
	4.如果有应答，发送addr 高，等待应答
	5.如果有应答，发送addr 低， 等待应答
	6 开始
	7 发ID 读
	8 等待应答
	9 读一个byte+nack
	10 结束会话
	*/

	
	iic_start();
	iic_send_byte(id);	//发送一个byte
	
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send id+w no ack ) ;
		return 1;
	}
	// id ack ,next   
	
	iic_send_byte((WordaddrToRead & 0xFF00)>>8 );	//高字节
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send Hight_addr no ack ) ;
		return 1;
	}
	// hight addr ack,next
	
	iic_send_byte(WordaddrToRead & 0x00FF );	//低字节
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send Hight_addr no ack ) ;
		return 1;
	}
	// low addr ack,next	
		
	iic_start();
	iic_send_byte(id | 0x01);	// bit,读高
	
	if(iic_wait_ack() ) 
	{
		ErrorInfo(send id+r no ack ) ;
		return 1;
	}
	
	//开始读
	*store_buf =  iic_read_byte( 1); //1=nack
	
	iic_stop( );
	
	return 0;
}



