#ifndef __W25Q128_H__
#define __W25Q128_H__


 //128Mbit = 128M/8 byte = 16M BYTE
 //16*1024*1024 BYTE = 16,777,216 BYTE
  //2^24 = 16,777,216 ,所以addr_bit_width=24
  
  
  
  
#include "stm32f10x.h"
#define W25Q128_PAGE_OFFSET_ADDR  (0x000100)

#define  W25Q128_Sector_4k_OFFSET_ADDR  (0x001000)   //W25Q128 扇区有 4k 个Byte  ,共4k 个扇区，16M byte容量



 








u8 W25Q128_soft_init(s8 mode)  ;//MODE; 0 1 2 3

u8  W25Q128_soft_Get_JEDEC_ID( u8 *MANUFACTURER_ID,u16 *Device_ID) ;


u8 w25q128_soft_Read_Data(u32 addr_24bit,u8 *ReadByte,u16 count);
u8 W25Q128_SOFT_Page_Program(u32 addr_24bit , u8 *WriteByte,u16 count); //02h  A23-A16 A15-A8 A7-A0 D7-D0 D7-D0(3) ;

u8 W25Q128_soft_Read_Status_Register_1(void );

u8 W25Q128_soft_Write_Enable(void);

u8 W25Q128_soft_Wait_BUSY(void);
	
u8 w25q128_soft_Sector_Erase_4KB( u32 addr_24bit);
void W25Q128_SOFT_Sector_Write_4K(u32 addr_24bit , u8 *WriteByte,u16 count);
	
	
	//硬件方式 接口////
u8  W25Q128_hard_Get_JEDEC_ID( u8 *MANUFACTURER_ID,u16 *Device_ID) ;
u8 w25q128_hard_Read_Data(u32 addr_24bit,u8 *ReadByte,u16 count);
void W25Q128_hard_Sector_Write_4K(u32 addr_24bit , u8 *WriteByte,u16 count);
#endif


