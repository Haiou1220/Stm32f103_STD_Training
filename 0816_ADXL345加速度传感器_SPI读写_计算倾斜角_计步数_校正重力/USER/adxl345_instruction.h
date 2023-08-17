#ifndef __ADXL345_INSTRUCTION_H__
#define __ADXL345_INSTRUCTION_H__


#define	ADXL345_DEVID			0x00	//	����ID��0xE5�Ĺ̶�����ID���� ��λֵ11100101 //�Ĵ���0x00����0xE5�Ĺ̶�����ID���룬������У��SPI�Ƿ�ͨ�ųɹ���
//#define	����		0x01 to 0x1C	//	��������Ҫ����
#define	ADXL345_THRESH_TAP		0x1D	//	�û���ֵ
#define	ADXL345_OFSX			0x1E	//	X��ƫ��
#define	ADXL345_OFSY			0x1F	//	Y��ƫ��
#define	ADXL345_OFSZ			0x20	//	Z��ƫ��
#define	ADXL345_DUR				0x21	//	�û�����ʱ��
#define	ADXL345_Latent			0x22	//	�û��ӳ�
#define	ADXL345_Window			0x23	//	�û�����
#define	ADXL345_THRESH_ACT		0x24	//	���ֵ
#define	ADXL345_THRESH_INACT	0x25	//	��ֹ��ֵ
#define	ADXL345_TIME_INACT		0x26	//	��ֹʱ��
#define	ADXL345_ACT_INACT_CTL	0x27	//	��ʹ�ܿ��ƻ�;�ֹ���
#define	ADXL345_THRESH_FF		0x28	//	����������ֵ
#define	ADXL345_TIME_FF			0x29	//	��������ʱ��
#define	ADXL345_TAP_AXES		0x2A	//	����/˫�������
#define	ADXL345_ACT_TAP_STATUS	0x2B	//	����/˫��Դ
#define	ADXL345_BW_RATE			0x2C	//	�������ʼ�����ģʽ���� ��λֵ00001010 ���ʴ���:1010 ,����:50HZ ,��������ٶȣ�100HZ
#define	ADXL345_POWER_CTL		0x2D	//	ʡ�����Կ���-----------
#define	ADXL345_INT_ENABLE		0x2E	//	�ж�ʹ�ܿ���--
#define	ADXL345_INT_MAP			0x2F	//	�ж�ӳ�����--
#define	ADXL345_INT_SOURCE		0x30	//	�ж�Դ
#define	ADXL345_DATA_FORMAT		0x31	//	���ݸ�ʽ����------ (D7)SELF_TEST (D6)SPI (D5)INT_INVERT (D4)0 (D3)FULL_RES (D2)Justify (D1-D0)Rang
#define	ADXL345_DATAX0			0x32	//	X������0 ���ֽ�
#define	ADXL345_DATAX1			0x33	//	X������1 ���ֽ�
#define	ADXL345_DATAY0			0x34	//	Y������0
#define	ADXL345_DATAY1			0x35	//	Y������1
#define	ADXL345_DATAZ0			0x36	//	Z������0
#define	ADXL345_DATAZ1			0x37	//	Z������1
#define	ADXL345_FIFO_CTL		0x38	//	FIFO����
#define	ADXL345_FIFO_STATUS		0x39	//	FIFO״̬
//////////////////////////////////////////////////////////////////////////////
#define DATA_FORMAT__Rang_16g  0x03  //��16 g
#define DATA_FORMAT__FULL_RES  0x08 //ȫ�ֱ���ģʽ
#define ADXL345_POWER_CTL__measurement     0x08 //����ʹ��
#define	ADXL345_INT_ENABLE__data_read  0x80		//����׼�����ж�ʹ��		
#define	ADXL345_INT_ENABLE__watermark ox02  //watermark �ж�ʹ��	
#define	ADXL345_INT_ENABLE__overrun  0x01  //overrun �ж�ʹ��	
//
#define	ADXL345_INT_SOURCE__overrun		0x01 // overrun�ж�״̬ 	

#define ADXL345_Axis_scale  3.9

#define ADXL345_FIFO_CTL__passby_mode  0X00  //��· ģʽ
#define ADXL345_FIFO_CTL__fifo_mode 0X40  //fifo ģʽ
#define ADXL345_FIFO_CTL__sample_num_16 0X0f  //fifo ģʽ ,16 ����
#endif

