#include "stm32f10x.h"
#include "w25q128.h"
#include "global.h"
#include "w25q128_instructions.h"
#include "systick.h"
#include "spi_soft.h"
#include "spi_hard.h"


u8 w25q128_soft_get_respond(void);

//BUSY ���ϵ�Ҫ���ߵ�ƽ ,busy �ڴ���״̬ ��Ϊbusy = 1;

u8 W25Q128_soft_init(void)  // ���spi ��ʼ�� ��W25Q128:ģʽ0
{
	u8 RET;
	RET = spi_soft_init(0);  //MODE; 0 1 2 3 W25Q128:ģʽ0
	if( RET ) ErrorInfo( error in spi_soft_init );
	if( !w25q128_soft_get_respond() )
	{	
		ErrorInfo( w25q128 Soft NOT respond );
		RET = 1;//ʧ��
	}
	
   return  RET;

}


u8  W25Q128_soft_Get_JEDEC_ID( u8 *MANUFACTURER_ID,u16 *Device_ID) 
{
	if( !w25q128_soft_get_respond() )
	{	
		ErrorInfo( w25q128 Soft NOT respond );
		return 1;//ʧ��
	}
	
	
	
    spi_soft_CS(0) ;    //�����ź�

	spi_soft_swapByte(JEDEC_ID);
	*MANUFACTURER_ID = spi_soft_swapByte(0XFF);
	*Device_ID = spi_soft_swapByte(0XFF);
	*Device_ID <<= 8;
	*Device_ID |= spi_soft_swapByte(0XFF);

	
    spi_soft_CS(1) ;   //ֹͣ�ź�

	return 0;
}

u8 W25Q128_soft_Read_Status_Register_1(void ) // xx SEC TB BP2 BP1 BP0 WEL BUSY 
{
	u8 Status_byte;
	
	 spi_soft_CS(0) ;    //�����ź�

	spi_soft_swapByte(Read_Status_Register_1);
	
	 Status_byte = spi_soft_swapByte(DUMMY_BYTE);
	
	spi_soft_CS(1) ;   //ֹͣ�ź�

	return Status_byte;
}


u8 W25Q128_soft_Write_Enable(void)
{
	spi_soft_CS(0) ;    //�����ź�
	spi_soft_swapByte(Write_Enable);
    spi_soft_CS(1) ;   //ֹͣ�ź�
	
return 0;
	
}
 

u8 W25Q128_soft_GetStatus_WEL_BIT(void)  // 1:��λ  0������
{
	//����WEL_BIT 
	if( ( W25Q128_soft_Read_Status_Register_1() & 0x02)  )  // xx SEC TB BP2 BP1 BP0 WEL BUSY
	{
		return 1; //����WEL_BIT = 1; 
	}	
	return 0;
}

	
	
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
			sysTickDelay_ms( 20); 
			busy_count++;
			if(busy_count>= 50) //1000 MS
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
//	spi_soft_CS(0) ;    //�����ź�
//	do
//	{
//	spi_soft_swapByte(Read_Status_Register_1);
//		Status_byte = spi_soft_swapByte(DUMMY_BYTE);
// 		busy = ( Status_byte & 0x01); //busy
//	}while(busy); 
//		spi_soft_CS(1) ;   //ֹͣ�ź�
// return 0;
//}

u8 w25q128_soft_Sector_Erase_4KB( u32 addr_24bit)
{
	 W25Q128_soft_Wait_BUSY() ;
	
	 W25Q128_soft_Write_Enable();//ʹ��
	
	spi_soft_CS(0) ;    //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	spi_soft_swapByte(Sector_Erase_4KB);
	//A23-A16 
	spi_soft_swapByte( (addr_24bit >>16) );
	//A15-A8
	spi_soft_swapByte( (addr_24bit >>8)&0XFF );
	// A7-A0
	spi_soft_swapByte(addr_24bit & 0XFF );
	spi_soft_CS(1) ;   //ֹͣ�ź�

	return 0;
	
}



u8 W25Q128_SOFT_Sector_Write_4K(u32 addr_24bit , u8 *WriteByte,u16 count)
{
	u16 total = count, index;
	
	if( !w25q128_soft_get_respond() )
	{	
		ErrorInfo( w25q128 Soft NOT respond );
		return  1;//ʧ��
	}
	
	W25Q128_soft_Wait_BUSY();//�ȴ�����
	
	
	w25q128_soft_Sector_Erase_4KB(addr_24bit);//�������� //��Ҫ����count������ �Ƿ���Ҫ�����������
	
	while(count )// ִ�� Buf д��
	{
		 W25Q128_soft_Wait_BUSY();//�ȴ�����
		 W25Q128_soft_Write_Enable();//д��ʹ�� ��WEL = 1;
		 spi_soft_CS(0);   //pageд����
		
		spi_soft_swapByte(Page_Program); //02h  A23-A16 A15-A8 A7-A0 ��D7-D0 D7-D0(3) 
		spi_soft_swapByte( (addr_24bit >>16)&0XFF );////A23-A16 
		spi_soft_swapByte( (addr_24bit >>8)&0XFF );//A15-A8
		spi_soft_swapByte(addr_24bit & 0XFF );// A7-A0
		
		while(count)//ִ�� page д����,д���߽������������count������
		{

		index = total - count; 		
		spi_soft_swapByte(WriteByte[index]);//WriteByte[i]
		count--;	//���һ��byte д�룬������һ	
		addr_24bit++ ; //���һ����ַ ��д�룬��ַָ����һ��	
		
			if (addr_24bit %  256 == 0 ) //��ַ������ ��һ��page  ��������ǰ pageд����
			{
				break;
			}
		}
		 spi_soft_CS(1) ;   //pageдֹͣ�ź�	
				
			
	}
		return  0;
		
}			           



u8 W25Q128_SOFT_Page_Program(u32 addr_24bit , u8 *WriteByte,u16 count)  // count :1-256��   //02h  A23-A16 A15-A8 A7-A0 D7-D0 D7-D0(3) 
{ 
	 u8 ret;
	if( !w25q128_soft_get_respond() )
	{	
		ErrorInfo( w25q128 Soft NOT respond );
		return  1;//ʧ��
	}
	
	 W25Q128_soft_Wait_BUSY();
	 w25q128_soft_Sector_Erase_4KB(addr_24bit);
	
	 W25Q128_soft_Wait_BUSY();
	 W25Q128_soft_Write_Enable();//ʹ�� WEL = 1;
	
	 W25Q128_soft_Wait_BUSY();
     spi_soft_CS(0);   //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	spi_soft_swapByte(Page_Program); 
	spi_soft_swapByte( (addr_24bit >>16)&0XFF );//A23-A16 
	spi_soft_swapByte( (addr_24bit >>8)&0XFF );	//A15-A8
	spi_soft_swapByte(addr_24bit & 0XFF );// A7-A0
	
	     for(u16 i = 0;i<count ;i++)   //up to 256 bytes of data input.
		{	
			spi_soft_swapByte(WriteByte[i]);		
		}
		ret = 0;
	
     spi_soft_CS(1) ;   //ֹͣ�ź�

	return ret;
}

u8 w25q128_soft_Read_Data(u32 addr_24bit,u8 *ReadByte,u16 count)
{
	 
	if( !w25q128_soft_get_respond() )
	{	
		ErrorInfo( w25q128 Soft NOT respond );
		return  1;//ʧ��
	}
	
	
	W25Q128_soft_Wait_BUSY() ;
	
	spi_soft_CS(0);   //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	spi_soft_swapByte(Read_Data);
	
	//A23-A16 
	spi_soft_swapByte( (addr_24bit >>16) );
	//A15-A8
	spi_soft_swapByte( (addr_24bit >>8)&0XFF );
	// A7-A0
	spi_soft_swapByte(addr_24bit & 0XFF );
	
	
		
	
	//��ȡ���ݣ�����Ϊ�� count 
		for(u16 i = 0;i<count ;i++)  
		{
			
		ReadByte[i] = spi_soft_swapByte(DUMMY_BYTE);
				
		}
	spi_soft_CS(1) ;   //ֹͣ�ź�

	return 0;
}


u8 w25q128_soft_get_respond(void) //ret:1 �����л�Ӧ ,,������û�г�ʼ������ʼʱ�򣬻ᷢ���������������л�Ӧ  ���취�ǣ�  ����������豸�� �Ƿ���ȷ  �������ͱ���
{
	
	 	//MANUFACTURER_ID:0X00EF,Device_ID:0X4018
	u8 MANUFACTURER_ID,RET;
	
    spi_soft_CS(0) ;    //�����ź�
	spi_soft_swapByte(JEDEC_ID);
	MANUFACTURER_ID = spi_soft_swapByte(0XFF);
    spi_soft_CS(1) ;   //ֹͣ�ź�
	
	if(0XEF == MANUFACTURER_ID )
	{
		RET = 1;
	}else
	{
		RET = 0;
	}
		
	return RET;
	
	
}



////////////////////////Ӳ����ʽ  �ӿں���//////////////////////////////////////////////////////////////////////

u8 w25q128_hard_get_respond(void) //ret:1 �����л�Ӧ ,,������û�г�ʼ������ʼʱ�򣬻ᷢ���������������л�Ӧ  ���취�ǣ�  ����������豸�� �Ƿ���ȷ  �������ͱ���
{	//MANUFACTURER_ID:0X00EF,Device_ID:0X4018
	u8 MANUFACTURER_ID,RET;
	
    spi_hard_CS(0) ;    //�����ź�
	spi_hard_swapByte(JEDEC_ID);
	MANUFACTURER_ID = spi_hard_swapByte(0XFF);
    spi_hard_CS(1) ;   //ֹͣ�ź�
	
	if(0XEF == MANUFACTURER_ID )
	{
		RET = 1;
	}else
	{
		RET = 0;
	}
		
	return RET;
}



void W25Q128_hard_init(void)
{
	 spi1_hard_init( 0); //   W25Q128 ģʽ0   
}

u8 W25Q128_hard_Read_Status_Register_1(void ) // xx SEC TB BP2 BP1 BP0 WEL BUSY 
{
	u8 Status_byte;
	
	 spi_hard_CS(0) ;    //�����ź�

	spi_hard_swapByte(Read_Status_Register_1);
	
	 Status_byte = spi_hard_swapByte(DUMMY_BYTE);
	
	spi_hard_CS(1) ;   //ֹͣ�ź�

	return Status_byte;
}



u8 W25Q128_hard_Wait_BUSY(void)  
{ 
	 u8 busy;
	u8  busy_count = 0;
	
	do
	{
		busy = ( W25Q128_hard_Read_Status_Register_1() & 0x01); //busy
		if(busy)
		{
			sysTickDelay_ms( 50); 
			busy_count++;
			if(busy_count>= 20) //20*50 = 1000S
			{
				ErrorInfo(  W25Q128_hard_Wait_BUSY out time more then 1s  );//5000ms ,1000ms
				return 1;
			}
		}else
		{
			break;
		}
			
	}while(busy); 
	

 return 0;
}



u8  W25Q128_hard_Get_JEDEC_ID( u8 *MANUFACTURER_ID,u16 *Device_ID) 
{
	if( !w25q128_hard_get_respond() )
	{	
		ErrorInfo( w25q128_hard NOT respond );
		return 1;//ʧ��
	}
	
    spi_hard_CS(0) ;    //�����ź�
    
	
	
	
	spi_hard_swapByte(JEDEC_ID);
	*MANUFACTURER_ID = spi_hard_swapByte(0XFF);
	*Device_ID = spi_hard_swapByte(0XFF);
	*Device_ID <<= 8;
	*Device_ID |= spi_hard_swapByte(0XFF);

	
    spi_hard_CS(1) ;   //ֹͣ�ź�
	

return 0;
}

u8 w25q128_hard_Read_Data(u32 addr_24bit,u8 *ReadByte,u16 count)
{
	if( !w25q128_hard_get_respond() )
	{	
		ErrorInfo( w25q128_hard NOT respond );
		return 1;//ʧ��
	}
	
	
	W25Q128_hard_Wait_BUSY() ;
	
	spi_hard_CS(0);   //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	spi_hard_swapByte(Read_Data);
	
	//A23-A16 
	spi_hard_swapByte( (addr_24bit >>16) );
	//A15-A8
	spi_hard_swapByte( (addr_24bit >>8)&0XFF );
	// A7-A0
	spi_hard_swapByte(addr_24bit & 0XFF );
	
	
		
	
	//��ȡ���ݣ�����Ϊ�� count 
		for(u16 i = 0;i<count ;i++)  
		{
			
		ReadByte[i] = spi_hard_swapByte(DUMMY_BYTE);
				
		}
	spi_hard_CS(1) ;   //ֹͣ�ź�

	return 0;
}

u8 W25Q128_hard_Write_Enable(void)
{
	spi_hard_CS(0) ;    //�����ź�
	spi_hard_swapByte(Write_Enable);
    spi_hard_CS(1) ;   //ֹͣ�ź� //���ֹͣ�ź� ��Ҫ��Ӳ��ͬ�����ȴ���һ�� ���
	
return 0;
	
}
 


u8 w25q128_hard_Sector_Erase_4KB( u32 addr_24bit)
{
	 W25Q128_hard_Wait_BUSY() ;
	
	 W25Q128_hard_Write_Enable();//ʹ��
	
	spi_hard_CS(0) ;    //�����ź�
	
	addr_24bit &= 0XFFFFFF;
	spi_hard_swapByte(Sector_Erase_4KB);
	//A23-A16 
	spi_hard_swapByte( (addr_24bit >>16) );
	//A15-A8
	spi_hard_swapByte( (addr_24bit >>8)&0XFF );
	// A7-A0
	spi_hard_swapByte(addr_24bit & 0XFF );
	spi_hard_CS(1) ;   //ֹͣ�ź�

	return 0;
	
}


u8 W25Q128_hard_Sector_Write_4K(u32 addr_24bit , u8 *WriteByte,u16 count)
{
	u16 total = count, index;
	
	if( !w25q128_hard_get_respond() )
	{	
		ErrorInfo( w25q128_hard NOT respond );
		return  1;//ʧ��
	}
	
	W25Q128_hard_Wait_BUSY();//�ȴ�����
	w25q128_hard_Sector_Erase_4KB(addr_24bit);//�������� //��Ҫ����count������ �Ƿ���Ҫ�����������
	
	while(count )// ִ�� Buf д��
	{
		 W25Q128_hard_Wait_BUSY();//�ȴ�����
		 W25Q128_hard_Write_Enable();//д��ʹ�� ��WEL = 1;
		 spi_hard_CS(0);   //pageд����
		
		spi_hard_swapByte(Page_Program); //02h  A23-A16 A15-A8 A7-A0 ��D7-D0 D7-D0(3) 
		spi_hard_swapByte( (addr_24bit >>16)&0XFF );////A23-A16 
		spi_hard_swapByte( (addr_24bit >>8)&0XFF );//A15-A8
		spi_hard_swapByte(addr_24bit & 0XFF );// A7-A0
		
		while(count)//ִ�� page д����,д���߽������������count������
		{

		index = total - count; 		
		spi_hard_swapByte(WriteByte[index]);//WriteByte[i]
		count--;	//���һ��byte д�룬������һ	
		addr_24bit++ ; //���һ����ַ ��д�룬��ַָ����һ��	
		
			if (addr_24bit %  256 == 0 ) //��ַ������ ��һ��page  ��������ǰ pageд����
			{
				break;
			}
		}
		
		spi_hard_CS(1) ;   //pageдֹͣ�ź�	
				
			
	}

	return 0;	
}			           


