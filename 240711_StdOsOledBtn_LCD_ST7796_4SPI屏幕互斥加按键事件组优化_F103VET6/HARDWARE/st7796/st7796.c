#include "st7796.h"
#include "spi.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "lcd_font.h"
#include "st7796_ins.h"
#include "st7796_app.h"

/*
ST7796 模块 特性简述
SCL时钟空闲为low,上升沿采样,下降沿输出,CPOL=0,CPHA=0
The driver samples the SDA (input data) at rising edge of SCL,
but shifts SDA (output data) at the falling edge of SCL.
写周期:66ns => Hz 0.01515*10^9 = 15M
读周期:150ns => Hz 0.00666*10^9 = 6.6M
72/SPI_BaudRatePrescaler_16 = 4.5M < 6.6M  < 72/SPI_BaudRatePrescaler_8 = 9M 
*/
//PA4 -NSS //PA5-SCK  //PA6-MI  //PA7-MO  //PA3 - DC  //PA2 - reset
#define ST7796_RESET_PIN_RCC_APB2Periph    RCC_APB2Periph_GPIOA
#define ST7796_RESET_PIN                   GPIO_Pin_2
#define ST7796_RESET_PORT                  GPIOA 

#define ST7796_CS_PIN_RCC_APB2Periph    RCC_APB2Periph_GPIOA
#define ST7796_CS_PIN                   GPIO_Pin_4
#define ST7796_CS_PORT                  GPIOA 
#define ST7796_SPI_PIN_RCC_APB2Periph    RCC_APB2Periph_GPIOA
#define ST7796_SPI_PINX_PORT             GPIOA
#define ST7796_SPI_SCK_PIN              GPIO_Pin_5
#define ST7796_SPI_SCK_PORT             GPIOA
#define ST7796_SPI_MI_PIN               GPIO_Pin_6
#define ST7796_SPI_MI_PORT              GPIOA
#define ST7796_SPI_MO_PIN               GPIO_Pin_7
#define ST7796_SPI_MO_PORT              GPIOA
#define ST7796_DC_PIN_RCC_APB2Periph    RCC_APB2Periph_GPIOA
#define ST7796_DC_PIN                   GPIO_Pin_3
#define ST7796_DC_PORT                  GPIOA
#define ST7796_SPIX_RCC_APB2Periph    RCC_APB2Periph_SPI1

#define DUMMY_BYTE 0xFF
#define LCD_WR_REGX(x)  st7796_write_cmd(x)
#define	LCD_WR_DATAX(x)	st7796_write_data(x)




//画笔颜色
#define RGB_WHITE         	 0xFFFF
#define RGB_BLACK         	 0x0000	  
#define RGB_BLUE         	 0x001F  //
#define RGB_BRED             0XF81F
#define RGB_GRED 			 0XFFE0
#define RGB_GBLUE			 0X07FF
#define RGB_RED           	 0xF800  //
#define RGB_MAGENTA       	 0xF81F
#define RGB_GREEN         	 0x07E0		//
#define RGB_CYAN          	 0x7FFF
#define RGB_YELLOW        	 0xFFE0
#define RGB_BROWN 			 0XBC40 //棕色
#define RGB_BRRED 			 0XFC07 //棕红色
#define RGB_GRAY  			 0X8430 //灰色
//GUI颜色

#define RGB_DARKBLUE      	 0X01CF	//深蓝色
#define RGB_LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define RGB_GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define RGB_LIGHTGREEN     	 0X841F //浅绿色 
#define RGB_LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define RGB_LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define RGB_LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)




//LCD的画笔颜色和背景色	   
u16 PAINT_COLOR=RGB_RED;	//画笔颜色
u16 BACK_COLOR=0xFFFF;  //背景色 

 //reset :Signal is active low,HW RESET NEED TO Wait 100ms	  
#define ST7796_RESET do 											\
{																	\
	GPIO_WriteBit(ST7796_RESET_PORT,ST7796_RESET_PIN,Bit_RESET); 	\
	vTaskDelay(pdMS_TO_TICKS(100));									\
	GPIO_WriteBit(ST7796_RESET_PORT,ST7796_RESET_PIN,Bit_SET); 		\
} while (0);



//LCD重要参数集
typedef struct  
{										    
	u16 width;			//LCD 宽度
	u16 height;			//LCD 高度
	u16 id;				//LCD ID
	u8  dir;			//横屏还是竖屏控制：0，竖屏；1，横屏。	
}_lcd_dev; 	 

//管理LCD重要参数
//默认为竖屏
static _lcd_dev lcddev;
	 					    

void st7796_bsp_init(void)
{
    // spi1硬件初始化,模式0  NSS 软件模式 

	//开启外设时钟 SPIX  GPIOX
	RCC_APB2PeriphClockCmd(ST7796_SPIX_RCC_APB2Periph,ENABLE);
	RCC_APB2PeriphClockCmd(ST7796_SPI_PIN_RCC_APB2Periph,ENABLE);
	RCC_APB2PeriphClockCmd(ST7796_CS_PIN_RCC_APB2Periph,ENABLE);
    RCC_APB2PeriphClockCmd(ST7796_DC_PIN_RCC_APB2Periph,ENABLE);
    RCC_APB2PeriphClockCmd(ST7796_RESET_PIN_RCC_APB2Periph,ENABLE);

	//配置gpio ,复用
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = ST7796_CS_PIN; //软件模式下，SPI1_NCC可作为通用I/O
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; //
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(ST7796_CS_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ST7796_SPI_SCK_PIN|ST7796_SPI_MO_PIN;  //MO SCK AF_OUTPUT
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz; //50Mhz读取失败
	GPIO_Init(ST7796_SPI_PINX_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = ST7796_SPI_MI_PIN;	            //MI  INPUT
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(ST7796_SPI_PINX_PORT, &GPIO_InitStruct);
	    
	GPIO_InitStruct.GPIO_Pin = ST7796_DC_PIN;	            //DC  OUTPUT
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(ST7796_DC_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = ST7796_RESET_PIN;	            //RESET  OUTPUT
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(ST7796_RESET_PORT, &GPIO_InitStruct);

	//配置spi1
	SPI_InitTypeDef  SPI_InitStruct;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge; //模式0 
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low; //模式0 
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master; //主机
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
     //72/SPI_BaudRatePrescaler_2 = 36M  最大sck频率
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;  //不使用硬件NSS
	SPI_InitStruct.SPI_CRCPolynomial = 7;//不使用,填默认值7

   	SPI_Init( SPI1,&SPI_InitStruct);
	
	//配置中断  NVIC_Init(&NVIC_InitStruct);SPI_I2S_ITConfig  不需要中断
	
	//启用SPI
	  SPI_Cmd(SPI1, ENABLE);
	  	
	  //CS上电默认为高电平
	  GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_SET); 
}

/**
 * @brief 
 * 
 */
void st7796_init(void)
{
ST7796_RESET;
//*************LCD Driver Initial **********//
//delay_ms(120); // Delay 120ms
vTaskDelay(pdMS_TO_TICKS(120));
LCD_WR_REGX(0x11); // Sleep Out
//delay_ms(120); // Delay 120ms
vTaskDelay(pdMS_TO_TICKS(120));

LCD_WR_REGX(0xf0) ;
LCD_WR_DATAX(0xc3) ;
LCD_WR_REGX(0xf0) ;
LCD_WR_DATAX(0x96) ;
/*
MY[D7]=1 ,Bottom to Top; MY=0,Top to Bottom
MX[D6]=1,Right to Left,; MX=0,Left to Right
MV[D5]=1, Row/column exchange;MV=0,Normal

ML[D4]=0,LCD refresh Top to Bottom;ML=1,LCD refresh Bottom to Top
MH[D2]=0,LCD refresh Left to Right;MH=1,LCD refresh Right to Left

RGB[D3]=0,RGB Order; RGB=1,BGR Order
*/
LCD_WR_REGX(0x36); //MADCTL:Memory Data Access Control
LCD_WR_DATAX(0x48); //MY=0 MX=1 MV=0 ML=0; RGB=1 MH=0;
LCD_WR_REGX(0x3A);//Interface Pixel Format
LCD_WR_DATAX(0x55);//'101’ =16bit/pixel ,RGB-5-6-5-bit input
LCD_WR_REGX(0xB4);
LCD_WR_DATAX(0x01);
LCD_WR_REGX(0xB7) ;
LCD_WR_DATAX(0xC6) ;
LCD_WR_REGX(0xe8);
LCD_WR_DATAX(0x40);
LCD_WR_DATAX(0x8a);
LCD_WR_DATAX(0x00);
LCD_WR_DATAX(0x00);
LCD_WR_DATAX(0x29);
LCD_WR_DATAX(0x19);
LCD_WR_DATAX(0xa5);
LCD_WR_DATAX(0x33);
LCD_WR_REGX(0xc1);
LCD_WR_DATAX(0x06);
LCD_WR_REGX(0xc2);
LCD_WR_DATAX(0xa7);
LCD_WR_REGX(0xc5);
LCD_WR_DATAX(0x18);
LCD_WR_REGX(0xe0); //Positive Voltage Gamma Control
LCD_WR_DATAX(0xf0);
LCD_WR_DATAX(0x09);
LCD_WR_DATAX(0x0b);
LCD_WR_DATAX(0x06);
LCD_WR_DATAX(0x04);
LCD_WR_DATAX(0x15);
LCD_WR_DATAX(0x2f);
LCD_WR_DATAX(0x54);
LCD_WR_DATAX(0x42);
LCD_WR_DATAX(0x3c);
LCD_WR_DATAX(0x17);
LCD_WR_DATAX(0x14);
LCD_WR_DATAX(0x18);
LCD_WR_DATAX(0x1b);
LCD_WR_REGX(0xe1); //Negative Voltage Gamma Control
LCD_WR_DATAX(0xf0);
LCD_WR_DATAX(0x09);
LCD_WR_DATAX(0x0b);
LCD_WR_DATAX(0x06);
LCD_WR_DATAX(0x04);
LCD_WR_DATAX(0x03);
LCD_WR_DATAX(0x2d);
LCD_WR_DATAX(0x43);
LCD_WR_DATAX(0x42);
LCD_WR_DATAX(0x3b);
LCD_WR_DATAX(0x16);
LCD_WR_DATAX(0x14);
LCD_WR_DATAX(0x17);
LCD_WR_DATAX(0x1b);
LCD_WR_REGX(0xf0);
LCD_WR_DATAX(0x3c);
LCD_WR_REGX(0xf0);
LCD_WR_DATAX(0x69);
vTaskDelay(pdMS_TO_TICKS(120)); //delay_ms(120); //Delay 120ms
LCD_WR_REGX(0x21);
LCD_WR_REGX(0x29); //Display ON
}

uint8_t st7796_read_cmdRead8Bit (uint8_t cmd)
{
	uint8_t byte;
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_RESET);  //CS拉低,开始信号
	GPIO_WriteBit(ST7796_DC_PORT,ST7796_DC_PIN,Bit_RESET);  //dc拉低,cmd
	spi1_SwapByte(cmd);

	spi1_SwapByte(DUMMY_BYTE); //第一次空读
	byte = spi1_SwapByte(DUMMY_BYTE);

	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_SET);  //CS拉高,结束信号

	return byte;
}

uint16_t st7796_read_cmdRead16Bit (uint8_t cmd)
{	  
	uint16_t word = 0;
	uint8_t byte_h,byte_l;
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_RESET);  //CS拉低,开始信号
	GPIO_WriteBit(ST7796_DC_PORT,ST7796_DC_PIN,Bit_RESET);  //dc拉低,cmd
	spi1_SwapByte(cmd);

	spi1_SwapByte(DUMMY_BYTE);//第一次空读
	byte_h = spi1_SwapByte(DUMMY_BYTE);
	byte_l = spi1_SwapByte(DUMMY_BYTE);
	
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_SET);  //CS拉高,结束信号

	word = (byte_h << 8)|byte_l;
	return word;
}



void st7796_write_cmd(uint8_t cmd_byte)
{	  
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_RESET);  //CS拉低,开始信号
	GPIO_WriteBit(ST7796_DC_PORT,ST7796_DC_PIN,Bit_RESET);  //dc拉低,cmd
	spi1_SwapByte(cmd_byte);
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_SET);  //CS拉高,结束信号
}

void st7796_write_data(uint8_t data_byte)
{
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_RESET);  //CS拉低,开始信号
	GPIO_WriteBit(ST7796_DC_PORT,ST7796_DC_PIN,Bit_SET);  //dc拉高,data
	spi1_SwapByte(data_byte);
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_SET);  //CS拉高,结束信号
}

void st7796_WriteGRAM_Prepare(void)
{
	st7796_write_cmd(RAMWR);
}

/**
 * @brief 写GRAM
 * 
 * @param RGB_Code  颜色值
 */
void st7796_WriteGRAM(u16 RGB_Code)
{	
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_RESET);  //CS拉低,开始信号
	GPIO_WriteBit(ST7796_DC_PORT,ST7796_DC_PIN,Bit_SET);  //dc拉高,data
	spi1_SwapByte((RGB_Code>>8)&0XFF);
	spi1_SwapByte(RGB_Code&0XFF);
	GPIO_WriteBit(ST7796_CS_PORT,ST7796_CS_PIN,Bit_SET);  //CS拉高,结束信号	  		 
}   

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void st7796_SetCursor(u16 Xpos, u16 Ypos)
{	 	    
		LCD_WR_REGX(CASET);
		LCD_WR_DATAX(Xpos>>8);LCD_WR_DATAX(Xpos&0XFF); 			 
		LCD_WR_REGX(RASET);
		LCD_WR_DATAX(Ypos>>8);LCD_WR_DATAX(Ypos&0XFF); 		 
} 	



void st7796_DrawPoint(u16 x,u16 y,u16 color) //原子操作,资源保护
{		
	ST7796_MUTEX_TAKE();

	st7796_SetCursor(x,y);		//设置光标位置 
	st7796_WriteGRAM_Prepare();	//开始写入GRAM
	st7796_WriteGRAM(color); 

	ST7796_MUTEX_GIVE();
}	 

/**
 * @brief 清屏函数
 * 
 * @param color :要清屏的填充色
 */
void st7796_ScreenClear(u16 color)
{
	u32 index=0;      
	u32 totalpoint=lcddev.width;
	if(!totalpoint) return;
	
	ST7796_MUTEX_TAKE();
	
	totalpoint*=lcddev.height; 			//得到总点数
	st7796_SetCursor(0x00,0x0000);	//设置光标位置 
	st7796_WriteGRAM_Prepare();     		//开始写入GRAM	  	  
	for(index=0;index<totalpoint;index++) st7796_WriteGRAM(color);

	ST7796_MUTEX_GIVE();
}


/**
 * @brief 显示屏参数初始化
 * 
 * @param dir :方向选择 	0-0度旋转，1-180度旋转，2-270度旋转，3-90度旋转
 */
void st7796_Display_device_init(u8 dir)
{
	/*
MY[D7]=1 ,Bottom to Top; MY=0,Top to Bottom
MX[D6]=1,Right to Left,; MX=0,Left to Right
MV[D5]=1, Row/column exchange;MV=0,Normal

ML[D4]=0,LCD refresh Top to Bottom;ML=1,LCD refresh Bottom to Top
MH[D2]=0,LCD refresh Left to Right;MH=1,LCD refresh Right to Left

RGB[D3]=0,RGB Order; RGB=1,BGR Order
*/
	if(dir==0||dir==1)			//竖屏
	{
			lcddev.dir=0;	//竖屏
			lcddev.width=320;
			lcddev.height=480;
				
		if(dir==0)        //0-0度旋转
		{
			LCD_WR_REGX(0x36); 
			LCD_WR_DATAX((1<<3)|(0<<7)|(1<<6)|(0<<5));
		}else							//1-180度旋转
		{
			LCD_WR_REGX(0x36); 
			LCD_WR_DATAX((1<<3)|(1<<7)|(0<<6)|(0<<5));		
		}
		
	}else if(dir==2||dir==3)
	{
		
			lcddev.dir=1;	//横屏
			lcddev.width=480;
			lcddev.height=320;		

				if(dir==2)				//2-270度旋转
				{
					LCD_WR_REGX(0x36); 
					LCD_WR_DATAX((1<<3)|(1<<7)|(1<<6)|(1<<5));

				}else							//3-90度旋转
				{
					LCD_WR_REGX(0x36); 
					LCD_WR_DATAX((1<<3)|(0<<7)|(0<<6)|(1<<5));
				}		
	}	
		//设置显示区域	
		LCD_WR_REGX(CASET); //x
		LCD_WR_DATAX(0);LCD_WR_DATAX(0);//x start
		LCD_WR_DATAX((lcddev.width-1)>>8);LCD_WR_DATAX((lcddev.width-1)&0XFF);//x end
		LCD_WR_REGX(RASET); //y
		LCD_WR_DATAX(0);LCD_WR_DATAX(0);
		LCD_WR_DATAX((lcddev.height-1)>>8);LCD_WR_DATAX((lcddev.height-1)&0XFF);  		
}	


//在指定位置显示一个字符
//x,y:起始坐标
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16/24
//mode:叠加方式(1)还是非叠加方式(0)
void st7796_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数	
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80) //(x,y)需要填
			{
				st7796_DrawPoint(x,y,PAINT_COLOR);
			}else //(x,y)不需要填
			{
				if(mode==0)st7796_DrawPoint(x,y,BACK_COLOR); //如果mode=1,反色
			}
			
			temp<<=1;
			y++;
			if(y>=lcddev.height)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=lcddev.width)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   

//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}	

//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void st7796_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size)//?????????????????????
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				st7796_ShowChar(x+(size/2)*t,y,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	st7796_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
	}
} 


void st7796_set_paint_color(u16 rgb_color)
{
	PAINT_COLOR = rgb_color;
}

inline u16 st7796_get_lcddev_width(void)
{
	return lcddev.width;
}
inline u16 st7796_get_lcddev_height(void)
{
	return lcddev.height;
}
			

void st7796_set_back_color(u16 rgb_color)
{
	BACK_COLOR = rgb_color;
}

//显示字符串
//x,y:起点坐标
//width,height:区域大小  
//size:字体大小
//*p:字符串起始地址		  
void st7796_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;//退出
        st7796_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }
}

//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色
u16 st7796_ReadPoint_color(u16 x,u16 y)
{	
	u16 ret;
	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回	

	ST7796_MUTEX_TAKE();
	   
	st7796_SetCursor(x,y);								
	ret = st7796_read_cmdRead16Bit (RAMRD);

	ST7796_MUTEX_GIVE();

	return ret;
}	

//Read IDn ,read_8_bit
u8 st7796_ReadIDn(u8 id_num)
{
	uint8_t cmd = 0;
	switch (id_num)
	{
	case 1:
		cmd = RDID1;
		break;
	case 2:
		cmd = RDID2;
		break;
	case 3:
		cmd = RDID3;
		break;

	default:
		break;
	}

	return (0 == cmd)?(0):(st7796_read_cmdRead8Bit(cmd));
}

//在指定区域内填充指定颜色
//区域大小:(xend-xsta+1)*(yend-ysta+1)
//xsta
//color:要填充的颜色
void st7796_ScreenFill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
	 
	u16 xnum = ex - sx + 1;

	if ((!lcddev.width) || (!lcddev.height))
		return; // 屏幕对象没有初始化
	if ((sx > lcddev.width - 1) || (ex > lcddev.width - 1))
		return;
	if ((sy > lcddev.height - 1) || (ey > lcddev.height - 1))
		return;	
	if( (sx >= ex)|| (sy >= ey)) return;


	for (u16 i = sy; i <= ey; i++)
	{
		ST7796_MUTEX_TAKE();

		st7796_SetCursor(sx, i);	// 设置光标位置
		st7796_WriteGRAM_Prepare(); // 开始写入GRAM
		for (u16 j = 0; j < xnum; j++){
			st7796_WriteGRAM(color); // 设置光标位置
		}

		ST7796_MUTEX_GIVE();
	}
}

//设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
//sx,sy:窗口起始坐标(左上角)
//width,height:窗口宽度和高度,必须大于0!!
//窗体大小:width*height. 
void st7796_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{    
	u16 twidth,theight;
	twidth=sx+width-1;
	theight=sy+height-1;

		LCD_WR_REGX(CASET); 
		LCD_WR_DATAX(sx>>8); 
		LCD_WR_DATAX(sx&0XFF);	 
		LCD_WR_DATAX(twidth>>8); 
		LCD_WR_DATAX(twidth&0XFF);  
		LCD_WR_REGX(RASET); 
		LCD_WR_DATAX(sy>>8); 
		LCD_WR_DATAX(sy&0XFF); 
		LCD_WR_DATAX(theight>>8); 
		LCD_WR_DATAX(theight&0XFF); 
}

//显示40*40图片
void st7796_DrawPic_bmp(u16 x,u16 y,const unsigned char *p) //显示40*40图片
{
  	int i; 
	unsigned char picH,picL; 
	st7796_Set_Window(x,y,40,40);
	st7796_WriteGRAM_Prepare();	
	
    for(i=0;i<40*40;i++)
	{	
	 	picL=*(p+i*2);	//数据低位在前
		picH=*(p+i*2+1);				
		st7796_WriteGRAM(picH<<8|picL);  						
	}	
	st7796_Set_Window(0,0,lcddev.width,lcddev.height);//恢复显示窗口为全屏	

}

//矩形填充颜色
void st7796_DrawRectangle_color(u16 x,u16 y,u16 width,u16 height,u16 color)
{	
	if(x+width-1 > lcddev.width-1) return;
	if(y+height-1 > lcddev.height-1) return;

	ST7796_MUTEX_TAKE();

	st7796_Set_Window(x,y,width,height);
	st7796_WriteGRAM_Prepare();	
	
    for(int i=0;i<width*height;i++)
	{			 			
		st7796_WriteGRAM(color<<8|color);  						
	}	
	st7796_Set_Window(0,0,lcddev.width,lcddev.height);//恢复显示窗口为全屏	

	ST7796_MUTEX_GIVE();
}





