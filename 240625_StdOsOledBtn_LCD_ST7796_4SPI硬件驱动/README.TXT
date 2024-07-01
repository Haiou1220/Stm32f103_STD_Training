LCD ST7796 spi 驱动,硬件spi方式
4线spi: DC SCL MOSI MISO  + CS;

ST7796 :RGAM:320 * 480 * 16bit
设置col地址,设置row 地址,写GRAM. 

128*64oled屏: GRAM为 128*64 * 1 bit,128*8bit 为一页,共8页,
写入的行y(0-63)需要换算成页地址(0-7),通过设置列地址(col)与page页地址,再写入GRAM
ST7796与128*64oled屏 驱动的区别,字体取模方式可一样,但是ST7796绘制GRAM时候ST7796可以写入每一个像素点的颜色值.

4寸 液晶屏在调试时候,有足够多的屏幕空间显示调试数据,调试方便

优化方式:改用DMA 或者 并口 驱动方式

程序再优化:st7796_write_cmd,st7796_write_data对SPI1的资源竞争问题,解决方法使用freertos互斥锁解决资源竞争问题
key_task 按键事件回调函数的优化:使用发送信号量到对应的task处理事件,不要阻塞按键事件回调函数的返回

*** error:file:..\HARDWARE\spi\spi.c,line:33 SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) 超时,需要互斥锁解决资源竞争