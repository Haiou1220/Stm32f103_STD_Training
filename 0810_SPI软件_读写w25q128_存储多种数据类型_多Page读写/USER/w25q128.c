#include "stm32f10x.h"
#include "w25q128.h"
#include "global.h"
#include "w25q128_instructions.h"
#include "systick.h"

#define w25q128_CS_PIN			 GPIO_Pin_4   //pc4-CS
#define w25q128_CS_GPIO			 GPIOC   //pc4-CS

#define w25q128_MOSI_PIN 		 		GPIO_Pin_7 //PA7-MO
#define w25q128_MOSI_GPIO		 		GPIOA //PA7-MO

#define w25q128_MISO_PIN				GPIO_Pin_6  //PA6-MI
#define w25q128_MISO_GPIO				GPIOA  //PA6-MI

#define w25q128_SCK_PIN					GPIO_Pin_5  //PA5-SCK
#define w25q128_SCK_GPIO				GPIOA  //PA5-SCK

	//Ĭ�ϵ�ƽ spiģʽ0 ����ʱ�Ӽ���0��ʱ����λ��0��sck����Ϊ0��sck��һ�����ز���
#define W25Q128_SCK_HIGH		 GPIO_WriteBit( w25q128_SCK_GPIO,  w25q128_SCK_PIN,  Bit_SET)
#define W25Q128_SCK_LOW	     GPIO_WriteBit( w25q128_SCK_GPIO,  w25q128_SCK_PIN,  Bit_RESET)
// CS�͵�ƽѡ��
#define W25Q128_CS_LOW	     GPIO_WriteBit( w25q128_CS_GPIO,  w25q128_CS_PIN,  Bit_RESET)
#define W25Q128_CS_HIGH	     GPIO_WriteBit( w25q128_CS_GPIO,  w25q128_CS_PIN,  Bit_SET)

u8 SPI_SOFT_MODE = 0xff;

void w25q128_soft_spi_init_mode0(void);
void w25q128_soft_spi_init_mode3(void);
u8 w25q128_soft_spi_swapByte_mode3(u8 sendByte); //ģʽ3����1������ ������ ����2������ �ɼ����ݣ�SCK����=1
u8 w25q128_soft_spi_swapByte_mode0(u8 sendByte); //ģʽ0��receiveByte sendByte //spi��λ�Ĵ�������λ����    //ģʽ0����1�����ز����������أ�����0������CS�����ݣ��½��أ�

u8 w25q128_soft_spi_init(s8 mode)  //MODE; 0 1 2 3
{
	u8 ret;
	SPI_SOFT_MODE = mode;
	//������
	if((mode<=3) && (mode>=0))
	{
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,  ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//cs -PC4 Ƭѡ 
	GPIO_InitStruct.GPIO_Pin = w25q128_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( w25q128_CS_GPIO, &GPIO_InitStruct);

	
	//PA5-SCK  �����ֲ�������ԣ� Tsclh = Tsckl >=45%PC ;PC= 1/Fc = 1/133M = 7518us ;3383us 
	GPIO_InitStruct.GPIO_Pin = w25q128_SCK_PIN;        //                      
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init( w25q128_SCK_GPIO, &GPIO_InitStruct);
	
	//PA7-MO OUT-PP
	GPIO_InitStruct.GPIO_Pin = w25q128_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( w25q128_MOSI_GPIO, &GPIO_InitStruct);
	
	////PA6-MI  INPUT-UP
	GPIO_InitStruct.GPIO_Pin = w25q128_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( w25q128_MISO_GPIO, &GPIO_InitStruct);
	
	}
	
		  if(mode == 0)
		  {
			w25q128_soft_spi_init_mode0( );
			  ret = 0;
		  }else if(mode == 1)
		  {
			 ret = 1;
		  
		  }else if(mode == 2)
		  {
			ret = 1;
		  }else if(mode ==3 )
		  {
			w25q128_soft_spi_init_mode3( );
				ret = 0;
		  }else
		  {
			Info( ERROR:SPI soft mode arg is out of range);
		  }
		 
			if(ret)
			{
				  Info( ERROR:SPI soft mode init mistake);
			}
			
			
	return ret;
}
	

void w25q128_soft_spi_init_mode0(void)
{
	//Ĭ�ϵ�ƽ spiģʽ0 ����ʱ�Ӽ���0��ʱ����λ��0��sck����Ϊ0��sck��1�����ز���
	W25Q128_CS_HIGH; //ʧ�ܣ��͵�ƽ��Ч 
	W25Q128_SCK_LOW; //sck����Ϊ0��

}
	
void w25q128_soft_spi_init_mode3(void)
{
	// spiģʽ3 ����ʱ�Ӽ���1��ʱ����λ1 ��sck����Ϊ1��  sck��2�����ز���
	W25Q128_CS_HIGH; //ʧ�ܣ��͵�ƽ��Ч 
	W25Q128_SCK_HIGH; //sck����Ϊ1��
	
}


void w25q128_MOSI_SendBit(u8 bit)
{
	if(bit)
	{
		GPIO_WriteBit( w25q128_MOSI_GPIO,  w25q128_MOSI_PIN,  Bit_SET);
	}else
	{
		GPIO_WriteBit( w25q128_MOSI_GPIO,  w25q128_MOSI_PIN,  Bit_RESET);
	}	
		
}

BitAction w25q128_MISO_ReceiveBit(void)
{
	if(    GPIO_ReadInputDataBit(w25q128_MISO_GPIO,w25q128_MISO_PIN)   )
	{
		return Bit_SET;
	}else
	{
		return Bit_RESET;
	}
	
}
	

void w25q128_soft_spi_CS(u8 bit) //CS�ź�
{
	
	if(bit)
	{	
		W25Q128_CS_HIGH;
	
	}else
	{	
		W25Q128_CS_LOW;
	}
}


u8 w25q128_soft_spi_swapByte(u8 Byte)
{
	u8 temp ;
	switch(SPI_SOFT_MODE)
	{
		case 0:
		temp = w25q128_soft_spi_swapByte_mode0(Byte) ;//ģʽ0��receiveByte sendByte //spi��λ�Ĵ�������λ����    //ģʽ0����1�����ز����������أ�����0������CS�����ݣ��½��أ�

			break;
		case 1:
			
			break;
		case 2:
			
			break;
	
		case 3:
			temp = w25q128_soft_spi_swapByte_mode3(Byte); //ģʽ3����1������ ������ ����2������ �ɼ����ݣ�SCK����=1
			break;
		
		default: 

		Info( w25q128_soft_spi_swapByte have error );
		temp = 0;

	} 
	return  temp;

}



//��дģʽ3 

u8 w25q128_soft_spi_swapByte_mode3(u8 sendByte) //ģʽ3����1������ ������ ����2������ �ɼ����ݣ�SCK����=1
{													
	for(u8 i = 0;i < 8;i++)
	{
	
	//�½��أ���һ������ ������
	W25Q128_SCK_LOW;
	w25q128_MOSI_SendBit(  sendByte & 0X80);//������,MSB
	sendByte <<= 1;
	
	//�����أ��ڶ����زɼ� ��
	W25Q128_SCK_HIGH;
	if( w25q128_MISO_ReceiveBit())  sendByte |= 0x01;
		
	//���������ѭ��
		
	}
		
	return sendByte;
}


u8 w25q128_soft_spi_swapByte_mode0(u8 sendByte) //ģʽ0��receiveByte sendByte //spi��λ�Ĵ�������λ����    //ģʽ0����1�����ز����������أ�����0������CS�����ݣ��½��أ�
{
	for(u8 i = 0;i < 8;i++)
	{
	//������,MSB
	 w25q128_MOSI_SendBit(  sendByte & 0X80);
	sendByte <<= 1;
	//�����أ���һ�����ز���
	W25Q128_SCK_HIGH;
	if( w25q128_MISO_ReceiveBit())  sendByte |= 0x01;
	
	//�½��ؽ���������ѭ��
	W25Q128_SCK_LOW;
	}
	
	return sendByte;
}



u8  W25Q128_soft_Get_JEDEC_ID( u8 *MANUFACTURER_ID,u16 *Device_ID) 
{
	
 w25q128_soft_spi_CS(0) ;    //�����ź�

	w25q128_soft_spi_swapByte(JEDEC_ID);
	*MANUFACTURER_ID = w25q128_soft_spi_swapByte(0XFF);
	*Device_ID = w25q128_soft_spi_swapByte(0XFF);
	*Device_ID <<= 8;
		*Device_ID |= w25q128_soft_spi_swapByte(0XFF);

	
 w25q128_soft_spi_CS(1) ;   //ֹͣ�ź�

return 0;
}

u8 W25Q128_soft_Read_Status_Register_1(void ) // xx SEC TB BP2 BP1 BP0 WEL BUSY
{
	u8 Status_byte;
	
	 w25q128_soft_spi_CS(0) ;    //�����ź�

	w25q128_soft_spi_swapByte(Read_Status_Register_1);
	
	 Status_byte = w25q128_soft_spi_swapByte(DUMMY_BYTE);
	
	w25q128_soft_spi_CS(1) ;   //ֹͣ�ź�

	return Status_byte;
}


u8 W25Q128_soft_Write_Enable(void)
{
	w25q128_soft_spi_CS(0) ;    //�����ź�
	w25q128_soft_spi_swapByte(Write_Enable);
 w25q128_soft_spi_CS(1) ;   //ֹͣ�ź�
	
return 0;
	
}
 
//	//����WEL_BIT 
//	if( !( W25Q128_soft_Read_Status_Register_1() & 0x02)  )  // xx SEC TB BP2 BP1 BP0 WEL BUSY
//	{
//		return 1; //����WEL_BIT = 0; ʹ��ʧ��
//	}	
//	return 0;
	
	
	/*
busy=1  ;doing:Page Program,, Sector Erase, Block Erase, Chip Erase, Write Status Register or 
Erase/Program Security Register instruction.

 ����ִ��  Read Status Register and Erase/Program Suspend instruction (see tW, tPP, tSE, tBE, and 
tCE in AC Characteristics).

Sector_Erase_4KB :400ms ,Page Program Time:3ms;

*/
u8 W25Q128_soft_Wait_BUSY(void)  
{ 
	 u8 busy;
	u8  busy_count = 0;
	
	do
	{
		busy = ( W25Q128_soft_Read_Status_Register_1() & 0x01); //busy
		if(busy)
		{
			sysTickDelay_ms( 50); 
			busy_count++;
			if(busy_count>= 20) //100
			{
				ErrorInfo(  W25Q128_soft_Wait_BUSY out time more then 1000ms  );//5000ms ,1000ms
				return 1;
			}
		}else
		{
			break;
		}
			
	}while(busy); 
	

 return 0;
}

//һֱ��ѯ״̬�������߼�ʱ�����㿴ʱ��
//u8 W25Q128_soft_Wait_BUSY(void)  
//{ 
//	 u8 busy,Status_byte;
//	w25q128_soft_spi_CS(0) ;    //�����ź�
//	do
//	{
//	w25q128_soft_spi_swapByte(Read_Status_Register_1);
//		Status_byte = w25q128_soft_spi_swapByte(DUMMY_BYTE);
// 		busy = ( Status_byte & 0x01); //busy
//	}while(busy); 
//		w25q128_soft_spi_CS(1) ;   //ֹͣ�ź�
// return 0;
//}

u8 w25q128_soft_Sector_Erase_4KB( u32 addr_24bit)
{
	 W25Q128_soft_Wait_BUSY() ;
	
	 W25Q128_soft_Write_Enable();//ʹ��
	
w25q128_soft_spi_CS(0) ;    //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	w25q128_soft_spi_swapByte(Sector_Erase_4KB);
	//A23-A16 
	w25q128_soft_spi_swapByte( (addr_24bit >>16) );
	
	//A15-A8
	w25q128_soft_spi_swapByte( (addr_24bit >>8)&0XFF );
	// A7-A0
	w25q128_soft_spi_swapByte(addr_24bit & 0XFF );
 w25q128_soft_spi_CS(1) ;   //ֹͣ�ź�

	return 0;
	
}










void W25Q128_SOFT_Sector_Write_4K(u32 addr_24bit , u8 *WriteByte,u16 count)
{
	u16 total = count, index;
	
	W25Q128_soft_Wait_BUSY();//�ȴ�����
	w25q128_soft_Sector_Erase_4KB(addr_24bit);//��������
	
	while(count )// ִ�� Buf д��
	{
		 W25Q128_soft_Wait_BUSY();//�ȴ�����
		 W25Q128_soft_Write_Enable();//д��ʹ�� ��WEL = 1;
		 w25q128_soft_spi_CS(0);   //pageд����
		
		w25q128_soft_spi_swapByte(Page_Program); //02h  A23-A16 A15-A8 A7-A0 ��D7-D0 D7-D0(3) 
		w25q128_soft_spi_swapByte( (addr_24bit >>16)&0XFF );////A23-A16 
		w25q128_soft_spi_swapByte( (addr_24bit >>8)&0XFF );//A15-A8
		w25q128_soft_spi_swapByte(addr_24bit & 0XFF );// A7-A0
		
		while(count)//ִ�� page д����,д���߽������������count������
		{

		index = total - count; 		
		w25q128_soft_spi_swapByte(WriteByte[index]);//WriteByte[i]
		count--;	//���һ��byte д�룬������һ	
		addr_24bit++ ; //���һ����ַ ��д�룬��ַָ����һ��	
		
			if (addr_24bit %  256 == 0 ) //��ַ������ ��һ��page  ��������ǰ pageд����
			{
				break;
			}
		}
		 w25q128_soft_spi_CS(1) ;   //pageдֹͣ�ź�	
				
			
	}

		
}			           





u8 W25Q128_SOFT_Page_Program(u32 addr_24bit , u8 *WriteByte,u16 count)  // count :1-256��   //02h  A23-A16 A15-A8 A7-A0 D7-D0 D7-D0(3) 
{ 
	 u8 ret;
	
	 W25Q128_soft_Wait_BUSY();
	 w25q128_soft_Sector_Erase_4KB(addr_24bit);
	
	 W25Q128_soft_Wait_BUSY();
	 W25Q128_soft_Write_Enable();//ʹ�� WEL = 1;
	
	 W25Q128_soft_Wait_BUSY();
     w25q128_soft_spi_CS(0);   //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	w25q128_soft_spi_swapByte(Page_Program); 
	w25q128_soft_spi_swapByte( (addr_24bit >>16)&0XFF );//A23-A16 
	w25q128_soft_spi_swapByte( (addr_24bit >>8)&0XFF );	//A15-A8
	w25q128_soft_spi_swapByte(addr_24bit & 0XFF );// A7-A0
	
	     for(u16 i = 0;i<count ;i++)   //up to 256 bytes of data input.
		{	
			w25q128_soft_spi_swapByte(WriteByte[i]);		
		}
		ret = 0;
	
     w25q128_soft_spi_CS(1) ;   //ֹͣ�ź�

	return ret;
}

u8 w25q128_soft_Read_Data(u32 addr_24bit,u8 *ReadByte,u16 count)
{
	W25Q128_soft_Wait_BUSY() ;
	
	w25q128_soft_spi_CS(0);   //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	w25q128_soft_spi_swapByte(Read_Data);
	
	//A23-A16 
	w25q128_soft_spi_swapByte( (addr_24bit >>16) );
	//A15-A8
	w25q128_soft_spi_swapByte( (addr_24bit >>8)&0XFF );
	// A7-A0
	w25q128_soft_spi_swapByte(addr_24bit & 0XFF );
	
	
		
	
	//��ȡ���ݣ�����Ϊ�� count 
		for(u16 i = 0;i<count ;i++)  
		{
			
		ReadByte[i] = w25q128_soft_spi_swapByte(DUMMY_BYTE);
				
		}

	
 w25q128_soft_spi_CS(1) ;   //ֹͣ�ź�
	
		
		
	return 0;
}



