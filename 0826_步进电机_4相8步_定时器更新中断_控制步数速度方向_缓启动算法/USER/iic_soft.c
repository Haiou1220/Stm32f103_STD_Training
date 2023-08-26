

#include "iic_soft.h"
#include "stm32f10x.h"
#include "global.h"
#include "systick.h"
#include "stdio.h"

#define SCL_PIN   GPIO_Pin_12     // SCL  pb12
#define SDA_PIN  GPIO_Pin_13   //SDA pb13
#define IIC_PORT  GPIOB //
#define IIC_APB2Periph  RCC_APB2Periph_GPIOB //
#define IIC_DELAY  iic_delay() 


void iic_soft_initialize(void)  // //SDA配置为OD上拉，这样就不用考虑IO方向了,输出模式下也可以输入
{	
	//gpio 时钟
	RCC_APB2PeriphClockCmd(IIC_APB2Periph,ENABLE);
	//gpio 配置 //SCL  //pb12 
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = SCL_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB,  &GPIO_InitStruct);
    //gpio 配置 //sda  //pb13
	GPIO_InitStruct.GPIO_Pin = SDA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB,  &GPIO_InitStruct);
}

void iic_delay_init(void)
{
	 sysTick_init();//初始化systick
}

void iic_delay(void)
{
  sysTickDelay_us_Block(2); 
}

void IIC_SDA(u8 state) 
{
	if((BitAction)state == Bit_SET){
		
	   GPIO_WriteBit( IIC_PORT,   SDA_PIN,  Bit_SET);
		
	}else if((BitAction)state == Bit_RESET) {
		
	   GPIO_WriteBit( IIC_PORT,   SDA_PIN,  Bit_RESET);
	}

}

void IIC_SCL(u8 state)
{
	if((BitAction)state == Bit_SET){
		
	   GPIO_WriteBit( IIC_PORT,   SCL_PIN,  Bit_SET);
		
	}else if((BitAction)state == Bit_RESET) {
		
	   GPIO_WriteBit( IIC_PORT,   SCL_PIN,  Bit_RESET);
	}
}






//SCL高电平期间，是采集数据期间，要求电平不变
// 如果SCL高电平期间，电平发生改变，定义为开始信号和停止信号
// 先准备数据 再 采样

void iic_start(void) //两线空闲状态（l两者都为高电平），sda 高电平变低电平 
{
	IIC_SDA(1);  
	IIC_SCL(1); //开始采集监听
	IIC_DELAY;
	
	IIC_SDA(0); //下降沿
	IIC_DELAY;
	
	IIC_SCL(0); // 结束脉冲
	IIC_DELAY;
	

}

void iic_stop(void) // SCL 高电平，sda 低电平  变  高电平
{
	IIC_SDA(0);
	IIC_SCL(1); //开始采集监听
	IIC_DELAY;
	
	IIC_SDA(1); //上升沿
	IIC_DELAY;
	
	//已释放SDA ,sda = 1 
}

u8 iic_wait_ack(void) // 返回1函数失败，返回0函数成功，， 0=ack / 1= nack
{
	IIC_SDA(1);  //释放SDA线
	IIC_DELAY;

	IIC_SCL(1); //开始采集监听
	IIC_DELAY;
	
	if( GPIO_ReadInputDataBit( IIC_PORT,SDA_PIN ) ){ //nack 无响应 结束会话
	
		iic_stop();
		return 1;
	}
	else{ // ack
		
		IIC_SCL(0);  //结束脉冲
		IIC_DELAY;

		return 0;
	}
		
	
}

void iic_ack(void) //产生ack
{
	IIC_SDA(0); //准备数据
	IIC_SCL(0); 
	IIC_DELAY;
	
	IIC_SCL(1); //开始脉冲
	IIC_DELAY;
	
	IIC_SCL(0); //结束脉冲
	IIC_DELAY;
}
void iic_nack(void)
{
	IIC_SDA(1);
	IIC_SCL(0);
	IIC_DELAY;
	
	IIC_SCL(1); //开始采集监听
	IIC_DELAY;
	
	IIC_SCL(0); //结束脉冲
	IIC_DELAY;
	
}

void iic_send_byte(u8 byte)	//发送一个byte
{
	
	for(u8 i = 0;i < 8;i++) //0-7 //循环 高位先发 1000 0000 ， 0100 0000
	{
		IIC_SDA( (byte & 0X80) >> 7 ) ; //取目前最高位    //准备数据
		IIC_DELAY;
		
		IIC_SCL(1); //开始采集
		IIC_DELAY;	

		IIC_SCL(0); //结束脉冲
		IIC_DELAY;
		
		byte <<= 1; 
	}
	//发送完成释放sda
	IIC_SDA(1);

}

void iic_send_ID_R(u8 id )	//发送ID
{	
	//发送id //读：+1 写：+0
		iic_send_byte( id | 0X01 );	 
		IIC_DELAY;

}
void iic_send_ID_W(u8 id )	//发送ID
{	
		//读：+1 写：+0
          //写操作
		  iic_send_byte(  id);	 
			IIC_DELAY;
}


	



u8 iic_read_byte(u8 ack) //par: 0 = ack ,1 = nack
{	
	u8 ret = 0;
	
	IIC_SCL(0); //准备开始脉冲，钳位
	
	for(u8 i = 0;i < 8;i++) //0-7 //循环 高位先获取  
	{
		IIC_SCL(1); //开始采集
		IIC_DELAY;	
		if( GPIO_ReadInputDataBit(IIC_PORT,SDA_PIN) ) //     
		{
			ret |= (0x80 >> i);    //1000 0000 ,  0100 0000 , 0000 0001
			
			
		}
		
		IIC_SCL(0); //结束脉冲
		IIC_DELAY;	
	}
	//接收完成
	if( !ack ) // ack
	{
		  iic_ack(); //产生ack
	}else //nack
	{
		  iic_nack();
	}

	return ret;
}





