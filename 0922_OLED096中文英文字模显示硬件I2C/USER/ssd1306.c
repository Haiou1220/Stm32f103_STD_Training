/*

 OLED 0.96寸 驱动 //无  DMA 
 OLED 0.96寸 驱动 //无  DMA ，PCtoLCD2002 英文取模：逐列式+顺向，中文取模：行列式 逆向(低位在前)，
 
  //12*12 ASCII字符集点阵,宋体,阴码+逐列式+顺向+C51格式
SSD1306的写入方向，决定字体取模方式
 
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

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
u8 OLED_GRAM[128][8];	 //128行 的u8[8]






//更新显存到LCD,byte		 
void OLED_Refresh_Gram(void)
{
	  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
		
	}   
}



//向SSD1306写入一个字节。
//dat:要写入的数据/命令
//cmd:数据/命令标志 0,表示命令;1,表示数据;
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
	
	
	//是否忙线
	{
		FlagStatus status;
		FlagTimeout = FlagTimeoutReset;
		do
		{
			status = I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY);  //0:空闲  1：忙碌
			if((FlagTimeout--) == 0) return I2CError();
		}while(status == SET);
	}
	
	
	
	
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	
	}
	//发送id地址 
	 FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{	
		
		 if((FlagTimeout--) == 0) return I2CError();
	}
	
	 
	 
		//写字节 CTRL_BYTE 
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   CTRL_BYTE_CMD );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
				 
			}
 
			//写字节 cmd
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   command );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
				
			}
 
			

	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //成功
						
}

u8 ssd1306_WriteCommandValeByte(uint8_t command,uint8_t ValeByte)
{
  
	
	u8 id = SSD1306_I2C_ADDR;
 
 
	 u32 FlagTimeoutReset = Num_DownToZero_1s;
	 u32 FlagTimeout ;
	 
	//是否忙线
	{
		FlagStatus status;
		FlagTimeout = FlagTimeoutReset;
		do
		{
			status = I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY);  //0:空闲  1：忙碌
			if((FlagTimeout--) == 0) return I2CError();
		}while(status == SET);
	}
	
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();

	}
	//发送id地址 
	 FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{	
		 if((FlagTimeout--) == 0) return I2CError();
	}
	
	
		    //写字节 CTRL_BYTE_CMD
				FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   CTRL_BYTE_CMD );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
			}
	
		   //写字节 ValeByte
			FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,   ValeByte );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
				
			}

	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //成功
						
}




u8 ssd1306_WriteData(uint8_t* pdata, uint16_t size)
{
 
 
	uint8_t id = SSD1306_I2C_ADDR;
	uint8_t* pbuf = pdata;
	uint8_t num_byte_to_write = size;
	

	 u32 FlagTimeoutReset = Num_DownToZero_1s;
	 u32 FlagTimeout ;
	 
	//是否忙线
	{
		FlagStatus status;
		FlagTimeout = FlagTimeoutReset;
		do
		{
			status = I2C_GetFlagStatus(  I2C1, I2C_FLAG_BUSY);  //0:空闲  1：忙碌
			if((FlagTimeout--) == 0) return I2CError();
		}while(status == SET);
	}
	
	//产生开始信号
	FlagTimeout = FlagTimeoutReset;
	I2C_GenerateSTART(  I2C1,  ENABLE);
	while(!I2C_CheckEvent( I2C1,  I2C_EVENT_MASTER_MODE_SELECT  )) ////  等待ev5
	{
		if((FlagTimeout--) == 0) return I2CError();
	}
	//发送id地址 
	 FlagTimeout = FlagTimeoutReset;
	I2C_Send7bitAddress(  I2C1,  id,  I2C_Direction_Transmitter );// 0，写方向
	while(!I2C_CheckEvent(  I2C1,  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  ))//等待ev6
	{	
		
		 if((FlagTimeout--) == 0) return I2CError();
	}
	
	
	//写control byte
			FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,    CTRL_BYTE_DATA );	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				 if((FlagTimeout--) == 0) return I2CError();
				 
			}
			
			

	
	for(int i = 0;i < num_byte_to_write;i++ )
	{
		//写字节
		    FlagTimeout = FlagTimeoutReset;
			I2C_SendData(  I2C1,  pbuf[i]);	
			while(!I2C_CheckEvent(	I2C1,  I2C_EVENT_MASTER_BYTE_TRANSMITTED  )	 ) //等待应答
			{
				if((FlagTimeout--) == 0) return I2CError();
				 
			}
	}

	//停止信号
	I2C_GenerateSTOP(  I2C1,  ENABLE);

	return 	0 ; //成功
						

}

void ssd1306_Reset(void) {
    /* for I2C - do nothing */
}



//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}	

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}


//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}


//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
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
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	

 //m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	
				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
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
//	//#define SetDisplayOFF (AEh) //显示关闭
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
//	ssd1306_WriteCommand(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
//	ssd1306_WriteCommand(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
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

				  
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); //设置时钟分频因子,震荡频率
	OLED_WR_Byte(80,OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
	OLED_WR_Byte(0xA8,OLED_CMD); //设置驱动路数
	OLED_WR_Byte(0X3F,OLED_CMD); //默认0X3F(1/64) 
	OLED_WR_Byte(0xD3,OLED_CMD); //设置显示偏移
	OLED_WR_Byte(0X00,OLED_CMD); //默认为0

	OLED_WR_Byte(0x40,OLED_CMD); //设置显示开始行 [5:0],行数.
													    
	OLED_WR_Byte(0x8D,OLED_CMD); //电荷泵设置
	OLED_WR_Byte(0x14,OLED_CMD); //bit2，开启/关闭
	OLED_WR_Byte(0x20,OLED_CMD); //设置内存地址模式
	OLED_WR_Byte(0x02,OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0xA1,OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
	OLED_WR_Byte(0xC0,OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); //设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); //[5:4]配置
		 
	OLED_WR_Byte(0x81,OLED_CMD); //对比度设置
	OLED_WR_Byte(0xEF,OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
	OLED_WR_Byte(0xD9,OLED_CMD); //设置预充电周期
	OLED_WR_Byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	OLED_WR_Byte(0xDB,OLED_CMD); //设置VCOMH 电压倍率
	OLED_WR_Byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	OLED_WR_Byte(0xA4,OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
	OLED_WR_Byte(0xA6,OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
	OLED_WR_Byte(0xAF,OLED_CMD); //开启显示	 
	OLED_Clear();

}




//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//取模方式，列行式
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
						{temp=zh_font_1616[chr1][i];}//调用16*16字体
				else if(size1==24)
					 	 {temp=zh_font_2424[chr1][i];}//调用24*24字体
				else if(size1==32)       
					return;	//{temp=Hzk3[chr1][i];}//调用32*32字体
				else if(size1==64)
					return;	//{temp=Hzk4[chr1][i];}//调用64*64字体
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



 









