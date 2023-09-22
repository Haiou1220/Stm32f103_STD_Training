#ifndef __ssd1306_H__
#define __ssd1306_H__
#include "stdint.h"
#include "fonts.h"
#include "stm32f10x.h"

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//typedef enum {
//    Black = 0x00, // Black color, no pixel
//    White = 0x01  // Pixel is set. Color depends on OLED
//} SSD1306_COLOR;


//  Struct to store transformations
//
//typedef struct {
//  uint16_t      CurrentX;
//  uint16_t      CurrentY;
//  uint8_t       Inverted;
//  SSD1306_COLOR Color;
//  uint8_t       Initialized;
//} SSD1306_t;

//10.1.1 Set Lower Column Start Address for Page Addressing Mode (00h~0Fh)
//10.1.2 Set Higher Column Start Address for Page Addressing Mode (10h~1Fh)
#define  SetMemoryAddressingMode (20h) //�ڴ��ַģʽ
#define SetColumnAddress (21h)  
#define SetPageAddress (22h) 
//10.1.6 Set Display Start Line (40h~7Fh)

#define SetContrastControlforBANK0 (0X81) //�Աȶ�����

#define SetSegmentRemapOff (A0h)    
#define SetSegmentRemapOn (A1h) //���ض��忪��
 
#define EntireDisplayON (A4h)  //ȫ����ʾ ����
#define EntireDisplayOff (A5h)  //ȫ����ʾ



#define SetNormalInverseDisplayN (A6h) //��ʾ��ʽ��������Ϊ����
#define SetNormalInverseDisplayI (A7h) //��ʾ��ʽ��������Ϊ ����



#define SetMultiplexRatio (A8h)  //����·��

#define SetDisplayON (AFh)
#define SetDisplayOFF (AEh) //��ʾ�ر�



//10.1.13 Set Page Start Address for Page Addressing Mode (B0h~B7h)
//10.1.14 Set COM Output Scan Direction (C0h/C8h)

#define SetDisplayOffset (D3h) //��ʾƫ��
#define SetDisplayClockDivideRatioOscillatorFrequency (D5h) //��Ƶ��

#define SetPrechargePeriod (D9h) //Ԥ�������
#define SetCOMPinsHardwareConfiguration (DAh)   
#define SetVCOMHDeselectLevel (DBh)
#define NOP (E3h)


//10.1.21 Status register Read
//10.2 Graphic Acceleration Command
//10.2.1 Horizontal Scroll Setup (26h/27h)
//10.2.2 Continuous Vertical and Horizontal Scroll Setup (29h/2Ah)
//10.2.3 Deactivate Scroll (2Eh)
//10.2.4 Activate Scroll (2Fh)
//10.2.5 Set Vertical Scroll Area(A3h)


void ssd1306_init(void);



uint8_t ssd1306_WriteCommand(uint8_t command);
uint8_t ssd1306_WriteData(uint8_t* pdata, uint16_t size);
uint8_t ssd1306_WriteCommandValeByte(uint8_t command,uint8_t ValeByte);



void OLED_WR_Byte (uint8_t dat,uint8_t cmd);

//����OLED��ʾ    
void OLED_Display_On(void);
//�ر�OLED��ʾ     
void OLED_Display_Off(void);
void OLED_Refresh_Gram(void);
void OLED_Clear(void)  ;
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size,uint8_t mode);

void OLED_ShowString(uint8_t x,uint8_t y,const uint8_t *p,uint8_t size);
	
	
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
 
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);

 
 
void OLED_ShowChinese_det(u8 x,u8 y,u8 num,u8 size1);

#endif





