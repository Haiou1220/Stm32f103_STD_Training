/*****************************************
* 基于AVR单片机GY-29模块通信程序 		 *
* 功    能：IIC通信读取数据并显示        *
* 时钟频率：内部1M 						 *
* 设    计：广运电子					 *
* 修改日期：2011年4月20日				 *
* 编译环境：ICC-AVR7.14					 *
* 实验环境：ATmega16+1602    			 *
* 使用端口：PC0,PC1,PC6,PC7,PA4~PA7 	 *
* 参    考：莫锦攀实验程序24c02读取实验  *
*****************************************/
#include <iom16v.h>
#include "I2C.h"
#include "1602.h"
#include "delay.h"
void conversion(unsigned int i);
void ADXL345_init(void); 
unsigned char display[4]={0,0,0,'g'};//显示数据

/*********************************************
数据转换,十六进制数据转换成10进制
输入十六进制范围：0x0000-0x270f（0-9999）
结果分成个十百千位，以ascii存入显示区
**********************************************/
void conversion(unsigned int i)  
{  
  // 	display[0]=i/10000+0x30 ;
  //  i=i%10000;    //取余运算
	display[0]=i/1000+0x30 ;
    i=i%1000;    //取余运算
    display[1]=i/100+0x30 ;
    i=i%100;    //取余运算
    display[2]=i/10+0x30 ;
 //  i=i%10;     //取余运算
 //   display[3]=i+0x30;  
}
//*******************************
//显示x
void display_x(void)
{   float temp;
    int x;

     x=I2C_Read(0x33);
	 x=(x<<8)+I2C_Read(0x32);
	 
	if(x<0){
	 x=-x;
     LCD_write_char(2,0,'-');       //显示负符号位
	}
	else 
	LCD_write_char(2,0,' ');        //显示空格
    temp=(float)x*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
    conversion(temp);          //转换出显示需要的数据
	LCD_write_char(0,0,'X');   //第0行，第0列 显示X
    LCD_write_char(1,0,':'); 
    LCD_write_char(3,0,display[0]); 
	LCD_write_char(4,0,'.'); 
    LCD_write_char(5,0,display[1]); 
    LCD_write_char(6,0,display[2]); 
	LCD_write_char(7,0,'g'); 

}
//*******************************
//显示y
void display_y(void)
{   float temp;
    int y;
     y=I2C_Read(0x35);
	 y=(y<<8)+I2C_Read(0x34);
	 
	if(y<0){
	 y=-y;
     LCD_write_char(2,1,'-');       //显示负符号位
	}
	else 
	LCD_write_char(2,1,' ');        //显示空格
    temp=(float)y*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
    conversion(temp);          //转换出显示需要的数据
	LCD_write_char(0,1,'y');   //第0行，第0列 显示X
    LCD_write_char(1,1,':'); 
    LCD_write_char(3,1,display[0]); 
	LCD_write_char(4,1,'.'); 
    LCD_write_char(5,1,display[1]); 
    LCD_write_char(6,1,display[2]); 
	LCD_write_char(7,1,'g'); 
}

//*******************************
//显示z
void display_z(void)
{   float temp;
    int z;

     z=I2C_Read(0x37);
	 z=(z<<8)+I2C_Read(0x36);
	 
	if(z<0){
	 z=-z;
     LCD_write_char(10,1,'-');       //显示负符号位
	}
	else 
	LCD_write_char(10,1,' ');        //显示空格
    temp=(float)z*3.9;  //计算数据和显示,查考ADXL345快速入门第4页
    conversion(temp);          //转换出显示需要的数据
	LCD_write_char(10,0,'z');   //第0行，第0列 显示X
    LCD_write_char(11,0,':'); 
    LCD_write_char(11,1,display[0]); 
	LCD_write_char(12,1,'.'); 
    LCD_write_char(13,1,display[1]); 
    LCD_write_char(14,1,display[2]); 
	LCD_write_char(15,1,'g'); 

}

//***********************************************
void ADXL345_init(void)             //adxl345初始化
{
	 I2C_Write(0x31,0x0B);   //测量范围,正负16g，13位模式
	 I2C_Write(0x2C,0x08);   //速率设定为12.5 参考pdf13页
	 I2C_Write(0x2D,0x08);   //选择电源模式   参考pdf24页
	 I2C_Write(0x2E,0x80);   //使能 DATA_READY 中断
	 I2C_Write(0x1E,0x00);   //X 偏移量 根据测试传感器的状态写入pdf29页
	 I2C_Write(0x1F,0x00);   //Y 偏移量 根据测试传感器的状态写入pdf29页
	 I2C_Write(0x20,0x05);   //Z 偏移量 根据测试传感器的状态写入pdf29页
}

/*******************************
主程序
*******************************/
void main(void)
{	
	unsigned char i;
		
	 delay_nms(20);          //lcd上电延时
	 LCD_init();             //lcd初始化
	 ADXL345_init(); 
     
	while(1){               //循环  
	
	display_x();      //显示x
	display_y();      //显示y
	display_z();      //显示z

	delay_nms(100); 
    }
}

