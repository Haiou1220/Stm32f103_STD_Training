
#include "lm75b.h"  //包含自己，定义与声明在同一个文件中了，这样 编译器可以做类型检查(函数、全局变量、自定义数据类型)
#include "stm32f10x.h"
#include "global.h"
#include "stdio.h"
#include "iic_soft.h"


#define LM75B_TEMP_REG 0X00
#define LM75B_CONF_REG 0X01
#define LM75B_OVER_TEMP_REG 0X03
#define LM75B_HYST_TEMP_REG 0X02

#define LM75B_ID 0X90

/*
LM75B传感器：
寄存器 0x00:temp只读;    0x01:conf  0x02:滞后温度设置  0x03：关停温度设置

滞后温度设置 和 关停温度设置的 位：高八位 + 低八位的最高1位 ：d8 d7d6d5d4d3d2d1d0  0 0011 0010 0x32 = 50度

conf寄存器：B[4:3] OS fault queue [1:0]
			B[2]= OS极限 0 = 有效低,od输出
			B[1] = 比较中断选择，0 = 比较

temp 寄存器：d10d9d8d7d6d5d4d3 d2d1d0 x x x x x 
			D10D9D8D7D6D5D4D3  d2d1d0
			
	XXXXXD10D9D8D7D6D5D4D3 000
			
			如果d10 = 0，正温度， 计算 = (d10-d0 )*0.125
			如果d10 = 1,负温度，计算 = - （(d10-d0 )的补码）*0.125   
						111 1111 1111 的 补码 = 取反+1 = 00 0000 0001     = 1
						
T_over 和 T_hys :存储单位温度为0.5，如果写入温度25，要求存储为：25/0.5 = 50
*/
u8 lm75b_read_temp( float * temper,u16* temp_reg)     
{	
	u16   temp = 0;
	u8 id = LM75B_ID;
	
	//第一步：dummy 写操作 第二步：读操作
	
	  iic_start() ;
	//发送id //读：+1 写：+0
	 iic_send_ID_W( id ); // id写
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+w   ) ;
		return 1;
	}	
	
	 iic_send_byte( LM75B_TEMP_REG ); // reg
	if(  iic_wait_ack() )
	{
		ErrorInfo(error:send addr byte  ) ;
		return 1;
	}		
	
	  iic_start() ;
	iic_send_ID_R( id ); // id读
	
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+r  : ) ;
		return 1;
	}	
	
	 u8 MSB = iic_read_byte(0); //par: 0 = ack ,1 = nack
	 u8 LSB = iic_read_byte(1); //par: 0 = ack ,1 = nack
	

	iic_stop();
	
	 
	
	//
	if(!(MSB & 0x80)) //正温度
	{
		*temp_reg = temp =   ((u16)MSB)<<3 |  (LSB >>5 );
		
		 *temper = temp*0.125;

	}else{ //负
	
	
	}	
	
	 	 
	return 	0;	
}

 

u8 lm75b_read_conf( u8* conf_reg )    
{
	u8 id = LM75B_ID;
	
	//第一步：dummy 写操作 第二步：读操作
	
	  iic_start() ;
	//发送id //读：+1 写：+0
	 iic_send_ID_W( id ); // id写
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+w   ) ;
		return 1;
	}	
	
	 iic_send_byte( LM75B_CONF_REG ); // reg
	if(  iic_wait_ack() )
	{
		ErrorInfo(error:send addr byte  ) ;
		return 1;
	}		
	
	  iic_start() ;
	iic_send_ID_R( id ); // id读
	
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+r  : ) ;
		return 1;
	}	
	
	*conf_reg = iic_read_byte(1); //par: 0 = ack ,1 = nack

	iic_stop();
	 	 
	return 	0;	

}



u8 lm75b_set_temper_over(s8 temper)
{	
	u8 id = LM75B_ID;
	u16 temper_reg1;
	if(temper >= 0) //设置温度大于等于0
	{
			
		temper_reg1 = (u16)((float)temper/0.5 );  //25/0.5 = 50;
		u8 MSB = (u8)(temper_reg1 >> 1) ;          //25
		u8 LSB = (u8)(temper_reg1 << 7) ;
												//0
		//写入
		iic_start() ;
		//发送id //读：+1 写：+0
		 iic_send_ID_W( id ); // id写
		if(  iic_wait_ack() ) 
		{
			ErrorInfo(error:id+w   ) ;
			return 1;
		}	
		
		 iic_send_byte( LM75B_OVER_TEMP_REG ); // reg
		if(  iic_wait_ack() )
		{
			ErrorInfo(error:send addr byte  ) ;
			return 1;
		}	
		
		//高字节 +低字节
		 iic_send_byte( MSB ); // reg
		if(  iic_wait_ack() )
		{
			ErrorInfo(error:send data byte  ) ;
			return 1;
		}	
			
		iic_send_byte( LSB ); // reg
		if(  iic_wait_ack() )
		{
			ErrorInfo(error:send data byte  ) ;
			return 1;
		}
	
		
		iic_stop();
		
		
	}
	return 0;
}

	
u8 lm75b_set_temper_hyster(s8 temper)
{	
	u8 id = LM75B_ID;
	u16 temper_reg1;
	if(temper >= 0) //设置温度大于等于0
	{
		
		temper_reg1 = (u16)((float)temper/0.5 );  //25/0.5 = 50;
		u8 MSB = (u8)(temper_reg1 >> 1) ;          //25
		u8 LSB = (u8)(temper_reg1 << 7) ;
												//0
		//写入
		iic_start() ;
		//发送id //读：+1 写：+0
		 iic_send_ID_W( id ); // id写
		if(  iic_wait_ack() ) 
		{
			ErrorInfo(error:id+w   ) ;
			return 1;
		}	
		
		 iic_send_byte( LM75B_HYST_TEMP_REG ); // reg
		if(  iic_wait_ack() )
		{
			ErrorInfo(error:send addr byte  ) ;
			return 1;
		}	
		
		//高字节 +低字节
		 iic_send_byte( MSB ); // reg
		if(  iic_wait_ack() )
		{
			ErrorInfo(error:send data byte  ) ;
			return 1;
		}	
			
		iic_send_byte( LSB ); // reg
		if(  iic_wait_ack() )
		{
			ErrorInfo(error:send data byte  ) ;
			return 1;
		}
	
		
		iic_stop();
		
		
		
		
	}

	
	
	return 0;
	
}

u8 lm75b_read_Tover_reg( u16* Tover_reg_9b )    
{
	u8 id = LM75B_ID;
	u8 MSB,LSB;
	
	//第一步：dummy 写操作 第二步：读操作
	
	  iic_start() ;
	//发送id //读：+1 写：+0
	 iic_send_ID_W( id ); // id写
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+w   ) ;
		return 1;
	}	
	
	 iic_send_byte( LM75B_OVER_TEMP_REG ); // reg
	if(  iic_wait_ack() )
	{
		ErrorInfo(error:send addr byte  ) ;
		return 1;
	}		
	
	  iic_start() ;
	iic_send_ID_R( id ); // id读
	
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+r  : ) ;
		return 1;
	}	
	
	MSB = iic_read_byte(0); //par: 0 = ack ,1 = nack
	LSB = iic_read_byte(1); //par: 0 = ack ,1 = nack
	
	iic_stop();
	
	*Tover_reg_9b = (((u16)MSB)<<1 ) | (  LSB<<7 );
	 	 
	return 	0;	

}

u8 lm75b_read_Thys_reg_9b( u16* Thys_reg_9b )    
{
	u8 id = LM75B_ID;
	u8 MSB,LSB;
	
	//第一步：dummy 写操作 第二步：读操作
	
	  iic_start() ;
	//发送id //读：+1 写：+0
	 iic_send_ID_W( id ); // id写
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+w   ) ;
		return 1;
	}	
	
	 iic_send_byte( LM75B_HYST_TEMP_REG ); // reg
	if(  iic_wait_ack() )
	{
		ErrorInfo(error:send addr byte  ) ;
		return 1;
	}		
	
	  iic_start() ;
	iic_send_ID_R( id ); // id读
	
	if(  iic_wait_ack() ) 
	{
		ErrorInfo(error:id+r  : ) ;
		return 1;
	}	
	
	MSB = iic_read_byte(0); //par: 0 = ack ,1 = nack
	LSB = iic_read_byte(1); //par: 0 = ack ,1 = nack
	
	iic_stop();
	
	*Thys_reg_9b = (((u16)MSB)<<1 ) | (  LSB<<7 );
	 	 
	return 	0;	

}
