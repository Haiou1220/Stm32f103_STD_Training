
项目:key button 驱动

注意:使能AFIO时钟的事项
复用功能不需要开启AFIO时钟 ,但是 端口重映射需要开启AFIO时钟.
 只有当使用事件控制寄存器、复用重映射和调试寄存器以及外部中断寄存器的时候，
 才需要提前开启AFIO的时钟！并不是使用到引脚复用功能就必须开启AFIO时钟。

功能:自定义print 实现临界值中断保护
 当你需要自己写一个自定义 printf 函数时候才需要 vprintf 函数,vprintf 函数一般和 va_start / va_end 配套使用；

 FreeRTOS任务优先级与硬件系统的中断优先级的关系:
    在FreeRTOS系统中，中断的优先级总是高于任何任务的优先级。也就是说，当系统发生中断时，CPU会立即执行相应的中断服务程序，
    无论当前正在执行的任务优先级有多高。
    这是因为中断通常用于处理紧急的硬件事件，需要得到及时的响应和处理，而FreeRTOS任务则更多地用于执行应用程序的逻辑。

中断函数里面的注意事项:vTaskNotifyGiveFromISR 与 xTaskNotifyFromISR 需要注意:
      /* 如果 xHigherPriorityTaskWoken 的值为 pdRTUE
         则执行一次上下文切换*/
         portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

启用 USE_FULL_ASSERT 检查到上一版本的 my_system_init()=>FLASH_HalfCycleAccessCmd 参数错误,目前已修正

定时器使用:
   xTimerChangePeriod(key_timer_handle,period,portMAX_DELAY);
    xTimerReset(key_timer_handle,portMAX_DELAY); 
    pvTimerGetTimerID(TimerHandle),可以在回调函数中,通过ID确定是那个TimerHandle调用

    
//key_app函数 实现 :短按 长按 
//   方法:利用硬件中断,中断里面 滤波后发送 二值信号量表示有效按下的动作   
//   获取按下动作的信号量,再用软件定时器 定时1秒 ,判断电平是否还是按下,如果是按下 长按键值加一,继续软件定时1秒重复这个步骤.
//   如果定时1秒,电平不是按下,退出判断循环.主循环继续等待新的按下信号量 .

