#include "st7796.h"
#include "spi.h"
#include "stm32f10x.h"
#include "stdio.h"
#include "lcd_font.h"
#include "st7796_ins.h"
#include "st7796_app.h"
#include "stm32f10x_fsmc.h"

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


void LCD_WR_REG(u16 regval);
void LCD_WR_DATA(u16 data);
u16  LCD_RD_DATA(void);

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
 // 反斜杠后面不要有空格
#define ST7796_RESET()  do 											\
{																	\
	GPIO_WriteBit(LCD_RST_PORT,LCD_RST_PIN,Bit_RESET); 	            \
	vTaskDelay(pdMS_TO_TICKS(200));									\
	GPIO_WriteBit(LCD_RST_PORT,LCD_RST_PIN,Bit_SET);				\
} while (0);

/**
	//BL背光
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); 
    GPIO_InitStructure.GPIO_Pin = LCD_BL_PIN;           //PB0 推挽输出 背光
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_BL_PORT, &GPIO_InitStructure);

	//RST复位
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
    GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;           
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);
 */

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
	 					    

void st7796_backlight_init(void)
{
	//BL背光,pwm,TIM4_CH1,remap

	GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); 
    GPIO_InitStructure.GPIO_Pin = LCD_BL_PIN;           // 推挽输出 背光
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    // 复用 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_BL_PORT, &GPIO_InitStructure);

	//Remap pin
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);

	//定时器4设置
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode =TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72-1; //1M hz
	TIM_TimeBaseInitStruct.TIM_Period = 100-1;//arr=100-1    10K= 1000 000/100

	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStruct);

		//配置比较输出CH2
		TIM_OCInitTypeDef  TIM_OCInitStruct;
		TIM_OCInitStruct.TIM_OCMode =  TIM_OCMode_PWM1;//向上计数大于比较值 输出有效电平,,,, // TIM_OCIdleState 高级定时器使用
		TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平
		TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Disable;//-------------先关闭输出通道,手动开启
		TIM_OCInitStruct.TIM_Pulse = 0;//ccr=0
		
		TIM_OC1Init( TIM4,   &TIM_OCInitStruct);
		
		//配置预装载
		TIM_ARRPreloadConfig(TIM4,ENABLE); //使能自动预装载寄存器
		TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);//CH1预装载使能
									
		//开启关闭定时器
		TIM_Cmd(TIM4,DISABLE);
}

void st7796_backlight_duty(u8 duty)
{
	if(duty>100)
		return;
		  
	//设置通道1的比较值
	TIM_SetCompare1(TIM4,duty); // 	ccr = duty
	//开启输出比较通道1
	TIM_CCxCmd(TIM4,TIM_Channel_1,TIM_CCx_Enable);

	//开启定时器
	if(duty==0){
		TIM_Cmd(TIM4,DISABLE);
	}else{
		TIM_Cmd(TIM4,ENABLE);
	}
}

/**
 * @brief  初始化FSMC,16并口
 * 
 */
void st7796_bsp_init(void)
{

	//设置gpio复用
	GPIO_InitTypeDef GPIO_InitStructure;

		//BL背光
		//RST复位
		//CS  => FSMC_NE1_PD7 =>FSMC_NE1
  	 	// RS =>FSMC_RS_PD11 =>FSMC_A16
   		// D0~D15 =>FSMC_D15~FSMC_D0
    	//WR =>FSMC_NWE =>PD5 
   		 //RD =>FSMC_NOE =>PD4

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); 

	//BL背光
    st7796_backlight_init();
	
	//RST复位
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 
    GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;           
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);

	//CS片选,复用,//CS  => FSMC_NE1_PD7 =>FSMC_NE1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 

 	// RS =>FSMC_RS_PD11 =>FSMC_A16
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 

    //WR,复用 =>FSMC_NWE =>PD5 
    //RD,复用 =>FSMC_NOE =>PD4	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE); 	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 


	//FSMC_D0~FSMC_D15
	//PD0 PD1 ;PD8 PD9 PD10 ;PD14 PD15  
	//PE7 PE8 PE9 PE10 PE11 PE12 PE13 PE14 PE15
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE,ENABLE); 	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_14|GPIO_Pin_15;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;         //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 

	//配置 FSMC_BCRx(片选控制)、 FSMC_BTRx(读)、 FSMC_BWTRx(写) 
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming,timingRead; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming,timingWrite;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);   //使能FSMC时钟

    readWriteTiming.FSMC_AddressSetupTime = 0x01;   //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;    //地址保持时间（ADDHLD）模式A未用到
    readWriteTiming.FSMC_DataSetupTime = 0x0f;      //数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;    //模式A 


    writeTiming.FSMC_AddressSetupTime = 0x01;   //0x00//地址建立时间（ADDSET）为1个HCLK  00:死,01:ok,02:死,03:no,05以上:死
    writeTiming.FSMC_AddressHoldTime = 0x00;    //地址保持时间（ADDHLD）模式A未用到
    writeTiming.FSMC_DataSetupTime = 0x08;      //0x03数据保存时间为4个HCLK 03:死屏,05:死屏,08:ok,0f:ok
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;    //模式A 

    // // 写 时间配置 主要是 AddressSetupTime 与 DataSetupTime ，可修改更小。写就是LCD显示。
    // // 读 时间配置 主要是 AddressSetupTime 与 DataSetupTime 

		//FSMC_BCRx(片选控制)
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;                      //这里我们使用NE1
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;    //不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;                //FSMC_MemoryType_SRAM;  SRAM
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;      //存储器数据宽度为16bit
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;   //FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;     //存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; //FSMC_ExtendedMode_Enable        //读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;        //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;                //写时序
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); //初始化FSMC配置

	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  //使能BANK1

}

/**
 * @brief 
 * 
 */
void st7796_init(void)
{
vTaskDelay(pdMS_TO_TICKS(120));
ST7796_RESET();
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
//LCD_WR_REGX(0x29); //Display ON
LCD_WR_REGX(DISPON); //Display ON 
st7796_backlight_duty(5);
}

uint8_t st7796_read_cmdRead8Bit (uint8_t cmd)
{
	uint8_t ret;
	LCD_WR_REGX(cmd);
	LCD_RD_DATA();// 空读
	ret =  LCD_RD_DATA();
	return ret;
}

uint16_t st7796_read_cmdRead16Bit (uint8_t cmd)
{	  

	return 0;
}





//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(u16 regval)
{
    LCD->LCD_REG=regval;        //写入要写的寄存器序号
}

//写LCD数据
//data:要写入的值
void LCD_WR_DATA(u16 data)
{
    LCD->LCD_RAM=data;
}

//读LCD数据
//返回值:读到的值
u16 LCD_RD_DATA(void)
{
    vu16 ram;                   //防止被优化
    ram=LCD->LCD_RAM;
    return ram;	 
}


void st7796_write_cmd(uint16_t regval)
{	  	
	 LCD_WR_REG(regval);
}

void st7796_write_data(uint16_t data)
{
	 LCD_WR_DATA(data);//要写入的值
}







void st7796_WriteGRAM_Prepare(void)
{
	//taskENTER_CRITICAL();

	st7796_write_cmd(RAMWR);

	//taskEXIT_CRITICAL();
}

/**
 * @brief 写GRAM
 * 
 * @param RGB_Code  颜色值
 */
void st7796_WriteGRAM(u16 RGB_Code)
{	
	LCD->LCD_RAM = RGB_Code;    //写十六位GRAM	 
}   

//设置光标位置
//Xpos:横坐标
//Ypos:纵坐标
void st7796_SetCursor(u16 Xpos, u16 Ypos)
{	 	  
	  	//taskENTER_CRITICAL();
		LCD_WR_REGX(CASET);
		LCD_WR_DATAX(Xpos>>8);LCD_WR_DATAX(Xpos&0XFF); 			 
		LCD_WR_REGX(RASET);
		LCD_WR_DATAX(Ypos>>8);LCD_WR_DATAX(Ypos&0XFF); 		
		//taskEXIT_CRITICAL(); 
} 	



void st7796_DrawPoint(u16 x,u16 y,u16 color) //原子操作,资源保护
{		
	//ST7796_MUTEX_TAKE();
	//taskENTER_CRITICAL();
	st7796_SetCursor(x,y);		//设置光标位置 
	st7796_WriteGRAM_Prepare();	//开始写入GRAM
	st7796_WriteGRAM(color); 

	//taskEXIT_CRITICAL();
	//ST7796_MUTEX_GIVE();

	

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
	
	//ST7796_MUTEX_TAKE();
	
	totalpoint*=lcddev.height; 			//得到总点数
	st7796_SetCursor(0x00,0x0000);	//设置光标位置 
	st7796_WriteGRAM_Prepare();     		//开始写入GRAM	  	
	
	for(index=0;index<totalpoint;index++) st7796_WriteGRAM(color);

	//ST7796_MUTEX_GIVE();
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
        st7796_ShowChar(x,y,*p,size,1);//0
        x+=size/2;
        p++;
    }
}

//读取个某点的颜色值	 
//x,y:坐标
//返回值:此点的颜色????????????
u16 st7796_ReadPoint_color(u16 x,u16 y)
{	
	//u16 ret;
	u16 r;

	if(x>=lcddev.width||y>=lcddev.height)return 0;	//超过了范围,直接返回	

	ST7796_MUTEX_TAKE();
	   
	st7796_SetCursor(x,y);	

	LCD_WR_REG(RAMRD);//发送读GRAM指令

	r= LCD_RD_DATA();          //假读




	ST7796_MUTEX_GIVE();

	return r;
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
void st7796_ScreenFill(u16 sx, u16 sy, u16 ex, u16 ey, u16 color) //ok
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





