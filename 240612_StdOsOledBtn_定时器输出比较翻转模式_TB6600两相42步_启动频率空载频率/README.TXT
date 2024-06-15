
项目:步进电机 定时器比较输出翻转模式,一个定时器产生4个通道的不同频率的脉冲,最多可以驱动4个步进电机
步骤:设置最大的ARR重载值,开启比较输出中断,在CNT==CCR时候,进入oc中断,在oc中断修改下一个CCR的值.
中断注意事项:中断优先级要最大,主优先级0,次优先级0,以免脉冲被打断不连续.
如果不分频,计数频率为72M,最大的oc输出频率(myCCR+cnt=1)为72M/(2*ccr)=36M,ps:乘以2是因为两次翻转为一个周期
                        最小的oc输出频率(myCCR+cnt=0xffff)为72M/(2*0XFFFF)= 549 hz

步进电机参数:步进电机 最大启动频率: 1500 pps（pulses per second，每秒脉冲数）
                    最大空载:1900 pps
输出通道:最大频率1.5k HZ = 0.66ms = 660 us
                空载:1.9k Hz = 0.52ms = 520us


cnt =  TIM_GetCounter(TIMx);
 TIM_SetCompare1(TIMx,cnt+CCR_VAL)


关于SysTick的配置:
在freertos中,port.c文件完成了配置systick的工作,源码已经帮我们完成了,在启动调度器vTaskStartScheduler函数里面路径如下,
vTaskStartScheduler=>xPortStartScheduler=>vPortSetupTimerInterrupt的函数里面,
vPortSetupTimerInterrupt 函数:
		portNVIC_SYSTICK_LOAD_REG = ( configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
		portNVIC_SYSTICK_CTRL_REG = ( portNVIC_SYSTICK_CLK_BIT | portNVIC_SYSTICK_INT_BIT | portNVIC_SYSTICK_ENABLE_BIT );
        使能了systick;开启异常中断;选用内核时钟FCLK;设置了重载值为72 000 - 1,意味着1ms,发送下溢中断(心跳时钟，一般默认心跳时钟为1m);
#define xPortSysTickHandler  SysTick_Handler 
在sysTick异常中断中,调用了xPortSysTickHandler,让xTickCount 加1






