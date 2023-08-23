#ifndef __W25Q128_INSTRUCTIONS_H__
#define __W25Q128_INSTRUCTIONS_H__




#define DUMMY_BYTE 0XFF
#define Write_Enable 0X06  //
#define JEDEC_ID   0X9f  // 9Fh (MF7-MF0) (ID15-ID8) (ID7-ID0)

#define Page_Program 0X02  //02h  A23-A16 A15-A8 A7-A0 D7-D0 D7-D0(3)

#define Sector_Erase_4KB   0X20   //20h  A23-A16  A15-A8  A7-A0

#define Read_Status_Register_1 0X05  //05h (S7-S0) ÷ÿ∏¥∂¡

#define Read_Data 0X03 //0X03  A23-A16 A15-A8 A7-A0 (D7-D0)


#endif
