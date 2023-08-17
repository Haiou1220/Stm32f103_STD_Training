#ifndef __ADXL345_INSTRUCTION_H__
#define __ADXL345_INSTRUCTION_H__


#define	ADXL345_DEVID			0x00	//	器件ID：0xE5的固定器件ID代码 复位值11100101 //寄存器0x00保存0xE5的固定器件ID代码，可用于校验SPI是否通信成功。
//#define	保留		0x01 to 0x1C	//	保留，不要操作
#define	ADXL345_THRESH_TAP		0x1D	//	敲击阈值
#define	ADXL345_OFSX			0x1E	//	X轴偏移
#define	ADXL345_OFSY			0x1F	//	Y轴偏移
#define	ADXL345_OFSZ			0x20	//	Z轴偏移
#define	ADXL345_DUR				0x21	//	敲击持续时间
#define	ADXL345_Latent			0x22	//	敲击延迟
#define	ADXL345_Window			0x23	//	敲击窗口
#define	ADXL345_THRESH_ACT		0x24	//	活动阈值
#define	ADXL345_THRESH_INACT	0x25	//	静止阈值
#define	ADXL345_TIME_INACT		0x26	//	静止时间
#define	ADXL345_ACT_INACT_CTL	0x27	//	轴使能控制活动和静止检测
#define	ADXL345_THRESH_FF		0x28	//	自由落体阈值
#define	ADXL345_TIME_FF			0x29	//	自由落体时间
#define	ADXL345_TAP_AXES		0x2A	//	单击/双击轴控制
#define	ADXL345_ACT_TAP_STATUS	0x2B	//	单击/双击源
#define	ADXL345_BW_RATE			0x2C	//	数据速率及功率模式控制 复位值00001010 速率代码:1010 ,带宽:50HZ ,输出数据速度：100HZ
#define	ADXL345_POWER_CTL		0x2D	//	省电特性控制-----------
#define	ADXL345_INT_ENABLE		0x2E	//	中断使能控制--
#define	ADXL345_INT_MAP			0x2F	//	中断映射控制--
#define	ADXL345_INT_SOURCE		0x30	//	中断源
#define	ADXL345_DATA_FORMAT		0x31	//	数据格式控制------ (D7)SELF_TEST (D6)SPI (D5)INT_INVERT (D4)0 (D3)FULL_RES (D2)Justify (D1-D0)Rang
#define	ADXL345_DATAX0			0x32	//	X轴数据0 低字节
#define	ADXL345_DATAX1			0x33	//	X轴数据1 高字节
#define	ADXL345_DATAY0			0x34	//	Y轴数据0
#define	ADXL345_DATAY1			0x35	//	Y轴数据1
#define	ADXL345_DATAZ0			0x36	//	Z轴数据0
#define	ADXL345_DATAZ1			0x37	//	Z轴数据1
#define	ADXL345_FIFO_CTL		0x38	//	FIFO控制
#define	ADXL345_FIFO_STATUS		0x39	//	FIFO状态
//////////////////////////////////////////////////////////////////////////////
#define DATA_FORMAT__Rang_16g  0x03  //±16 g
#define DATA_FORMAT__FULL_RES  0x08 //全分辨率模式
#define ADXL345_POWER_CTL__measurement     0x08 //测量使能
#define	ADXL345_INT_ENABLE__data_read  0x80		//数据准备好中断使能		
#define	ADXL345_INT_ENABLE__watermark ox02  //watermark 中断使能	
#define	ADXL345_INT_ENABLE__overrun  0x01  //overrun 中断使能	
//
#define	ADXL345_INT_SOURCE__overrun		0x01 // overrun中断状态 	

#define ADXL345_Axis_scale  3.9

#define ADXL345_FIFO_CTL__passby_mode  0X00  //旁路 模式
#define ADXL345_FIFO_CTL__fifo_mode 0X40  //fifo 模式
#define ADXL345_FIFO_CTL__sample_num_16 0X0f  //fifo 模式 ,16 样本
#endif

