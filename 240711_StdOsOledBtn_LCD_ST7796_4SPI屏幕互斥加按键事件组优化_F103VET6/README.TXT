//LCD ST7796 spi 驱动,硬件spi方式
//4线spi: DC SCL MOSI MISO  + CS;

//ST7796 :RGAM:320 * 480 * 16bit
//设置col地址,设置row 地址,写GRAM. 

//128*64oled屏: GRAM为 128*64 * 1 bit,128*8bit 为一页,共8页,
//写入的行y(0-63)需要换算成页地址(0-7),通过设置列地址(col)与page页地址,再写入GRAM
//ST7796与128*64oled屏 驱动的区别,字体取模方式可一样,但是ST7796绘制GRAM时候ST7796可以写入每一个像素点的颜色值.

//4寸 液晶屏在调试时候,有足够多的屏幕空间显示调试数据,调试方便

//优化方式:改用DMA 或者 并口 驱动方式

//程序再优化:st7796_write_cmd,st7796_write_data对SPI1的资源竞争问题,解决方法使用freertos互斥锁解决资源竞争问题
//key_task 按键事件回调函数的优化:使用发送信号量到对应的task处理事件,不要阻塞按键事件回调函数的返回

//*** error:file:..\HARDWARE\spi\spi.c,line:33 SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) 超时,需要互斥锁解决资源竞争

********************优化1*******************
完成key_task 任务对按键事件的优化,之前的按键扫描中回调了 按键处理函数,按键处理函数有可能会影响阻塞扫描的间隔,
优化化后,按键扫描的间隔不影响,在扫描中的回调函数中只做了设置按键事件组,
按键的处理逻辑在task中进行处理,使用事件组对每一个按键事件进行处理,处理完成相应的事件标志位就清零

********************优化2*******************
lcd屏幕使用互斥访问保护资源,对部分接口进行原子操作的封装,使用宏定义方便修改与移植                             



https://freertoskernel.asicfans.com/
 /*等待所有任务到达各自的同步点。*/
        xEventGroupSync(/* 用于同步的事件组。 */
                        xEventGroup,
            
                        /* 该任务设置的位，表示它已到达同步点。 */
                        uxThisTasksSyncBit,
            
                        /*需要等待的比特，每个参与同步的任务都有一个位*/
                        (mainFIRST_TASK_BIT | mainSECOND_TASK_BIT | mainTHIRD_TASK_BIT),
            
                        /*无限等待所有三个任务到达同步点*/
                        portMAX_DELAY);

        /*取递归互斥锁。 */
        if( xSemaphoreTakeRecursive( xRecursiveMutex, xMaxBlock20ms ) == pdPASS )
        {
            /*成功获取递归互斥锁。任务现在可以访问互斥锁保护的资源。此时，递归调用计数(即对xSemaphoreTakeRecursive()的嵌套调用的数量)为1，因为递归互斥对象只被取过一次。* /
            
 			/*当它已经持有递归的互斥对象时，任务再次接受互斥对象。在实际的应用程序中这只可能发生在该任务调用的子函数中，因为没有实际的理由要多次使用同一个互斥锁。\
            调用任务已经是互斥锁的持有者，所以对xSemaphorerakeRecursive()的第二次调用只是将递归调用的计数增加到2。*/
 		   	xSemaphoreTakeRecursive( xRecursiveMutex, xMaxBlock20ms );
            
 			/* ... */
            /*任务在访问了互斥锁保护的资源后返回互斥锁。此时，递归调用计数为2，因此对xSemaphoreGiveRecursive()的第一次调用不返回互斥锁。相反它只是将递归调用计数减回1。*/
 			xSemaphoreGiveRecursive( xRecursiveMutex );
            
			/*再次调用xSemaphoreGiveRecursive()时，返回的调用计数减少为0。所以这次返回的是递归互斥锁。*/
 			xSemaphoreGiveRecursive( xRecursiveMutex );
            
			/*现在开始每次调用 xSemaphoreTakeRecursive()都会执行获取互斥锁的动作，因为任务不再是互斥锁的持有者。*/
		}                    