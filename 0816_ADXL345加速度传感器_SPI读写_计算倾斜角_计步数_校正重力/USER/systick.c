
#include "systick.h"
#include "global.h"


 u8 FlagDelay_SysTickInt = 0;//systick定时标志位
     
	void sysTick_init() //初始化systick
	{
		// 配置时钟源
		SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk ; //0=8分频
		SysTick->VAL = 0;
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;  //systick取消中断
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//停止计数
	}
	
	void  sysTickDelay_ms(uint32_t delay_count) //9M ->9*1000 000  
	{	
		u32 temp;
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//失能
		delay_count *= 9000; //1000MS -> 9*1000 000 
		if (delay_count > 0x00FFFFFF )  { ErrorInfo("SysTick LOAD too large");  } //0-23bit(24位)
		SysTick->VAL = 0;
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
		SysTick->LOAD = delay_count; //0-23bit(24位),
		//使能
		SysTick->CTRL |= SysTick_CTRL_ENABLE;//systick使能
		
		do{ //查询完成标志，等待计时完成
			temp = SysTick->CTRL;
		}while( (temp&SysTick_CTRL_COUNTFLAG_Msk) == 0 );
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//失能
	}
	
	
	
	u8  sysTickDelay_us_Block(uint32_t delay_count) //9M ->:   (9*1000 000 -> 1000 ms ,1ms= 9000)   (9*1000 000 -> 1000 000 us ,1us = 9)
	{	
		u32 temp;
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//失能
		delay_count *= 9; //1000MS -> 9*1000 000 
		if (delay_count > 0x00FFFFFF )  { ErrorInfo("SysTick LOAD too large"); return 1;} //0-23bit(24位)
		SysTick->VAL = 0;
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; //关中断
		SysTick->LOAD = delay_count; //0-23bit(24位),
		//使能
		SysTick->CTRL |= SysTick_CTRL_ENABLE;//systick使能
		
		do{ //查询完成标志，等待计时完成
			temp = SysTick->CTRL;
		}while( (temp&SysTick_CTRL_COUNTFLAG_Msk) == 0 );
		
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//失能
		return 0; //成功
				
	}

	void  sysTickDelay_us_NonBlock(uint32_t delay_count) //9M ->:   (9*1000 000 / 1000 ms = 9000)   (9*1000 000 -> 1000 000 us ,1us = 9)
	{	
		
		
		delay_count *= 9; //1000MS -> 9*1000 000 
		//判断参数合法性
		if (delay_count > 0x00FFFFFF )  { ErrorInfo("SysTick LOAD too large");  } //0-23bit(24位) 16,777,215/9 000 000 =
		FlagDelay_SysTickInt = 0; //清除flag
		 //如果systick失能,赋值开始计数
		if( !(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk)  )
		{
			SysTick->VAL = 0;
			SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk; //清flag
			SysTick->CTRL |=  SysTick_CTRL_TICKINT_Msk; //开中断
			SysTick->LOAD = delay_count; //0-23bit(24位)
			SysTick->CTRL |= SysTick_CTRL_ENABLE;//systick使能		
		}else{ ////如果systick 已使能 ，忽略不处理
		
		}				
	}
	
	
u8 getFlagSysTickDelayOut(void)
{	
	if( FlagDelay_SysTickInt )
	{	
		FlagDelay_SysTickInt = 0;
		return 1;
	}
	return 0;
}



	


