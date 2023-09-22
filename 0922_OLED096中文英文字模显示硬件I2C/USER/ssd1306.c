/*

 OLED 0.96�� ���� //��  DMA 
 OLED 0.96�� ���� //��  DMA ��PCtoLCD2002 Ӣ��ȡģ������ʽ+˳������ȡģ������ʽ ����(��λ��ǰ)��
 
  //12*12 ASCII�ַ�������,����,����+����ʽ+˳��+C51��ʽ
SSD1306��д�뷽�򣬾�������ȡģ��ʽ
 
*/
#include  "ssd1306.h"
#include "stm32f10x.h"
#include "iic_hard.h"
#include "systick.h"
#include "fonts.h"


#define  Num_DownToZero_1s   (100000 *2)   
#define SSD1306_I2C_ADDR 0X78


#define CTRL_BYTE_CMD  0X00
#define CTRL_BYTE_DATA  0x40

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 //128�� ��u8[8]






//�����Դ浽LCD,byte		 
void OLED_Refresh_Gram(void)
{
	  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
		
	}   
}



//��SSD1306д��һ���ֽڡ�
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;
void OLED_WR_Byte (uint8_t dat,uint8_t cmd)
{
	if(cmd == OLED_CMD)
	{
	  ssd1306_WriteCommand(  dat);
	}else if(cmd == OLED_DATA)
	{

     ssd1306_WriteData(  &dat,  1);
	}
	
}


u8 ssd1306_WriteCommand(uint8_t command)
{
  
	
	u8 id = SSD1306_I2C_ADDR;

 
	 u32 FlagTimeoutReset = Num_DownToZero_1s;
	 u32 FlagTimeout ;
	
	
	//�Ƿ�æ��
	{
		FlagStatus status;
		FlagTimeout = FlagTimeoutReset;
		do
		{
			status = I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY);  //0:����  1��æµ
			if((FlagTimeout--) == 0) return I2CError();
		}while(status == SET);
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
	
	 
	 
		//д�ֽ� CTRL_BYTE 
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   CTRL_BYTE_CMD );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
				 
			}
 
			//д�ֽ� cmd
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   command );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
				
			}
 
			

	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //�ɹ�
						
}

u8 ssd1306_WriteCommandValeByte(uint8_t command,uint8_t ValeByte)
{
  
	
	u8 id = SSD1306_I2C_ADDR;
 
 
	 u32 FlagTimeoutReset = Num_DownToZero_1s;
	 u32 FlagTimeout ;
	 
	//�Ƿ�æ��
	{
		FlagStatus status;
		FlagTimeout = FlagTimeoutReset;
		do
		{
			status = I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY);  //0:����  1��æµ
			if((FlagTimeout--) == 0) return I2CError();
		}while(status == SET);
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
	
	
		    //д�ֽ� CTRL_BYTE_CMD
				FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   CTRL_BYTE_CMD );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
	
		   //д�ֽ� ValeByte
			FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   ValeByte );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
				
			}

	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //�ɹ�
						
}




u8 ssd1306_WriteData(uint8_t* pdata, uint16_t size)
{
 
 
	uint8_t id = SSD1306_I2C_ADDR;
	uint8_t* pbuf = pdata;
	uint8_t num_byte_to_write = size;
	

	 u32 FlagTimeoutReset = Num_DownToZero_1s;
	 u32 FlagTimeout ;
	 
	//�Ƿ�æ��
	{
		FlagStatus status;
		FlagTimeout = FlagTimeoutReset;
		do
		{
			status = I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY);  //0:����  1��æµ
			if((FlagTimeout--) == 0) return I2CError();
		}while(status == SET);
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
	
	
	//дcontrol byte
			FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,    CTRL_BYTE_DATA );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				 if((FlagTimeout--) == 0) return I2CError();
				 
			}
			
			

	
	for(int i = 0;i < num_byte_to_write;i++ )
	{
		//д�ֽ�
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  pbuf[i]);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //�ȴ�Ӧ��
			{
				if((FlagTimeout--) == 0) return I2CError();
				 
			}
	}

	//ֹͣ�ź�
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //�ɹ�
						

}

void ssd1306_Reset(void) {
    /* for I2C - do nothing */
}



//����OLED��ʾ    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}


//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}


//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	

 //m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	
				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{     
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

void ssd1306_init(void)
{

  i2c1_init( ); //i2c1   pb6-scl pb7-sda    
	
  sysTickDelay_ms(200);

//	ssd1306_WriteCommand(0xAE);//--turn off oled panel
//	//#define SetDisplayOFF (AEh) //��ʾ�ر�
//	
//	
//	ssd1306_WriteCommand(0x00);//---set low column address
//	ssd1306_WriteCommand(0x10);//---set high column address
////10.1.1 Set Lower Column Start Address for Page Addressing Mode (00h~0Fh)
////10.1.2 Set Higher Column Start Address for Page Addressing Mode (10h~1Fh)
//	
//	
//	ssd1306_WriteCommand(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
//	ssd1306_WriteCommand(0x81);//--set contrast control register
//	ssd1306_WriteCommand(0xCF);// Set SEG Output Current Brightness
//	ssd1306_WriteCommand(0xA1);//--Set SEG/Column Mapping     0xa0���ҷ��� 0xa1����
//	ssd1306_WriteCommand(0xC8);//Set COM/Row Scan Direction   0xc0���·��� 0xc8����
//	ssd1306_WriteCommand(0xA6);//--set normal display
//	ssd1306_WriteCommand(0xA8);//--set multiplex ratio(1 to 64)
//	ssd1306_WriteCommand(0x3f);//--1/64 duty
//	ssd1306_WriteCommand(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
//	ssd1306_WriteCommand(0x00);//-not offset
//	ssd1306_WriteCommand(0xd5);//--set display clock divide ratio/oscillator frequency
//	ssd1306_WriteCommand(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
//	ssd1306_WriteCommand(0xD9);//--set pre-charge period
//	ssd1306_WriteCommand(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
//	ssd1306_WriteCommand(0xDA);//--set com pins hardware configuration
//	ssd1306_WriteCommand(0x12);
//	ssd1306_WriteCommand(0xDB);//--set vcomh
//	ssd1306_WriteCommand(0x30);//Set VCOM Deselect Level
//	ssd1306_WriteCommand(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
//	ssd1306_WriteCommand(0x02);//
//	ssd1306_WriteCommand(0x8D);//--set Charge Pump enable/disable
//	ssd1306_WriteCommand(0x14);//--set(0x10) disable
//	//OLED_Clear();
//	ssd1306_WriteCommand(0xAF);

				  
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],��Ƶ����;[7:4],��Ƶ��
	OLED_WR_Byte(0xA8,OLED_CMD); //��������·��
	OLED_WR_Byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //������ʾƫ��
	OLED_WR_Byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	OLED_WR_Byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //��ɱ�����
	OLED_WR_Byte(0x14,OLED_CMD); //bit2������/�ر�
	OLED_WR_Byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	OLED_WR_Byte(0xDA,OLED_CMD); //����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]����
		 
	OLED_WR_Byte(0x81,OLED_CMD); //�Աȶ�����
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	OLED_WR_Byte(0xD9,OLED_CMD); //����Ԥ�������
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	OLED_WR_Byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //������ʾ	 
	OLED_Clear();

}




//��ʾ����
//x,y:�������
//num:���ֶ�Ӧ�����
//ȡģ��ʽ������ʽ
void OLED_ShowChinese_det(u8 x,u8 y,u8 num,u8 size1)
{
	u8 i,m,n=0,temp,chr1;
	u8 x0=x,y0=y;
	u8 size3=size1/8;
	while(size3--)
	{
		chr1=num*size1/8+n;
		n++;
			for(i=0;i<size1;i++)
			{
				if(size1==16)
						{temp=zh_font_1616[chr1][i];}//����16*16����
				else if(size1==24)
					 	 {temp=zh_font_2424[chr1][i];}//����24*24����
				else if(size1==32)       
					return;	//{temp=Hzk3[chr1][i];}//����32*32����
				else if(size1==64)
					return;	//{temp=Hzk4[chr1][i];}//����64*64����
				else return;
							
						for(m=0;m<8;m++)
							{
								if(temp&0x01)
								{
								  OLED_DrawPoint(  x,  y,1);
								}					
								else  OLED_DrawPoint(  x,  y,0);
								temp>>=1;
								y++;
							}
							x++;
							if((x-x0)==size1)
							{x=x0;y0=y0+8;}
							y=y0;
			 }
	}
}



 









