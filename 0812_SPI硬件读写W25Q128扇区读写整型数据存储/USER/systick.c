
#include "systick.h"
#include "global.h"


 u8 FlagDelay_SysTickInt = 0;//systick��ʱ��־λ
     
	void sysTick_init() //��ʼ��systick
	{
		// ����ʱ��Դ
		SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk ; //0=8��Ƶ
		SysTick->VAL = 0;
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;  //systickȡ���ж�
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//ֹͣ����
	}
	
	void  sysTickDelay_ms(uint32_t delay_count) //9M ->9*1000 000  
	{	
		u32 temp;
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//ʧ��
		delay_count *= 9000; //1000MS -> 9*1000 000 
		if (delay_count > 0x00FFFFFF )  { ErrorInfo("SysTick LOAD too large");  } //0-23bit(24λ)
		SysTick->VAL = 0;
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
		SysTick->LOAD = delay_count; //0-23bit(24λ),
		//ʹ��
		SysTick->CTRL |= SysTick_CTRL_ENABLE;//systickʹ��
		
		do{ //��ѯ��ɱ�־���ȴ���ʱ���
			temp = SysTick->CTRL;
		}while( (temp&SysTick_CTRL_COUNTFLAG_Msk) == 0 );
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//ʧ��
	}
	
	
	
	u8  sysTickDelay_us_Block(uint32_t delay_count) //9M ->:   (9*1000 000 -> 1000 ms ,1ms= 9000)   (9*1000 000 -> 1000 000 us ,1us = 9)
	{	
		u32 temp;
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//ʧ��
		delay_count *= 9; //1000MS -> 9*1000 000 
		if (delay_count > 0x00FFFFFF )  { ErrorInfo("SysTick LOAD too large"); return 1;} //0-23bit(24λ)
		SysTick->VAL = 0;
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
		SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk; //���ж�
		SysTick->LOAD = delay_count; //0-23bit(24λ),
		//ʹ��
		SysTick->CTRL |= SysTick_CTRL_ENABLE;//systickʹ��
		
		do{ //��ѯ��ɱ�־���ȴ���ʱ���
			temp = SysTick->CTRL;
		}while( (temp&SysTick_CTRL_COUNTFLAG_Msk) == 0 );
		
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//ʧ��
		return 0; //�ɹ�
				
	}

	void  sysTickDelay_us_NonBlock(uint32_t delay_count) //9M ->:   (9*1000 000 / 1000 ms = 9000)   (9*1000 000 -> 1000 000 us ,1us = 9)
	{	
		
		
		delay_count *= 9; //1000MS -> 9*1000 000 
		//�жϲ����Ϸ���
		if (delay_count > 0x00FFFFFF )  { ErrorInfo("SysTick LOAD too large");  } //0-23bit(24λ) 16,777,215/9 000 000 =
		FlagDelay_SysTickInt = 0; //���flag
		 //���systickʧ��,��ֵ��ʼ����
		if( !(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk)  )
		{
			SysTick->VAL = 0;
			SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk; //��flag
			SysTick->CTRL |=  SysTick_CTRL_TICKINT_Msk; //���ж�
			SysTick->LOAD = delay_count; //0-23bit(24λ)
			SysTick->CTRL |= SysTick_CTRL_ENABLE;//systickʹ��		
		}else{ ////���systick ��ʹ�� �����Բ�����
		
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



	


