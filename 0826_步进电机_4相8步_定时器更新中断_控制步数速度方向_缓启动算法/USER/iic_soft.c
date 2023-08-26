

#include "iic_soft.h"
#include "stm32f10x.h"
#include "global.h"
#include "systick.h"
#include "stdio.h"

#define SCL_PIN   GPIO_Pin_12     // SCL  pb12
#define SDA_PIN  GPIO_Pin_13   //SDA pb13
#define IIC_PORT  GPIOB //
#define IIC_APB2Periph  RCC_APB2Periph_GPIOB //
#define IIC_DELAY  iic_delay() 


void iic_soft_initialize(void)  // //SDA����ΪOD�����������Ͳ��ÿ���IO������,���ģʽ��Ҳ��������
{	
	//gpio ʱ��
	RCC_APB2PeriphClockCmd(IIC_APB2Periph,ENABLE);
	//gpio ���� //SCL  //pb12 
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = SCL_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB,  &GPIO_InitStruct);
    //gpio ���� //sda  //pb13
	GPIO_InitStruct.GPIO_Pin = SDA_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOB,  &GPIO_InitStruct);
}

void iic_delay_init(void)
{
	 sysTick_init();//��ʼ��systick
}

void iic_delay(void)
{
  sysTickDelay_us_Block(2); 
}

void IIC_SDA(u8 state) 
{
	if((BitAction)state == Bit_SET){
		
	   GPIO_WriteBit( IIC_PORT,   SDA_PIN,  Bit_SET);
		
	}else if((BitAction)state == Bit_RESET) {
		
	   GPIO_WriteBit( IIC_PORT,   SDA_PIN,  Bit_RESET);
	}

}

void IIC_SCL(u8 state)
{
	if((BitAction)state == Bit_SET){
		
	   GPIO_WriteBit( IIC_PORT,   SCL_PIN,  Bit_SET);
		
	}else if((BitAction)state == Bit_RESET) {
		
	   GPIO_WriteBit( IIC_PORT,   SCL_PIN,  Bit_RESET);
	}
}






//SCL�ߵ�ƽ�ڼ䣬�ǲɼ������ڼ䣬Ҫ���ƽ����
// ���SCL�ߵ�ƽ�ڼ䣬��ƽ�����ı䣬����Ϊ��ʼ�źź�ֹͣ�ź�
// ��׼������ �� ����

void iic_start(void) //���߿���״̬��l���߶�Ϊ�ߵ�ƽ����sda �ߵ�ƽ��͵�ƽ 
{
	IIC_SDA(1);  
	IIC_SCL(1); //��ʼ�ɼ�����
	IIC_DELAY;
	
	IIC_SDA(0); //�½���
	IIC_DELAY;
	
	IIC_SCL(0); // ��������
	IIC_DELAY;
	

}

void iic_stop(void) // SCL �ߵ�ƽ��sda �͵�ƽ  ��  �ߵ�ƽ
{
	IIC_SDA(0);
	IIC_SCL(1); //��ʼ�ɼ�����
	IIC_DELAY;
	
	IIC_SDA(1); //������
	IIC_DELAY;
	
	//���ͷ�SDA ,sda = 1 
}

u8 iic_wait_ack(void) // ����1����ʧ�ܣ�����0�����ɹ����� 0=ack / 1= nack
{
	IIC_SDA(1);  //�ͷ�SDA��
	IIC_DELAY;

	IIC_SCL(1); //��ʼ�ɼ�����
	IIC_DELAY;
	
	if( GPIO_ReadInputDataBit( IIC_PORT,SDA_PIN ) ){ //nack ����Ӧ �����Ự
	
		iic_stop();
		return 1;
	}
	else{ // ack
		
		IIC_SCL(0);  //��������
		IIC_DELAY;

		return 0;
	}
		
	
}

void iic_ack(void) //����ack
{
	IIC_SDA(0); //׼������
	IIC_SCL(0); 
	IIC_DELAY;
	
	IIC_SCL(1); //��ʼ����
	IIC_DELAY;
	
	IIC_SCL(0); //��������
	IIC_DELAY;
}
void iic_nack(void)
{
	IIC_SDA(1);
	IIC_SCL(0);
	IIC_DELAY;
	
	IIC_SCL(1); //��ʼ�ɼ�����
	IIC_DELAY;
	
	IIC_SCL(0); //��������
	IIC_DELAY;
	
}

void iic_send_byte(u8 byte)	//����һ��byte
{
	
	for(u8 i = 0;i < 8;i++) //0-7 //ѭ�� ��λ�ȷ� 1000 0000 �� 0100 0000
	{
		IIC_SDA( (byte & 0X80) >> 7 ) ; //ȡĿǰ���λ    //׼������
		IIC_DELAY;
		
		IIC_SCL(1); //��ʼ�ɼ�
		IIC_DELAY;	

		IIC_SCL(0); //��������
		IIC_DELAY;
		
		byte <<= 1; 
	}
	//��������ͷ�sda
	IIC_SDA(1);

}

void iic_send_ID_R(u8 id )	//����ID
{	
	//����id //����+1 д��+0
		iic_send_byte( id | 0X01 );	 
		IIC_DELAY;

}
void iic_send_ID_W(u8 id )	//����ID
{	
		//����+1 д��+0
          //д����
		  iic_send_byte(  id);	 
			IIC_DELAY;
}


	



u8 iic_read_byte(u8 ack) //par: 0 = ack ,1 = nack
{	
	u8 ret = 0;
	
	IIC_SCL(0); //׼����ʼ���壬ǯλ
	
	for(u8 i = 0;i < 8;i++) //0-7 //ѭ�� ��λ�Ȼ�ȡ  
	{
		IIC_SCL(1); //��ʼ�ɼ�
		IIC_DELAY;	
		if( GPIO_ReadInputDataBit(IIC_PORT,SDA_PIN) ) //     
		{
			ret |= (0x80 >> i);    //1000 0000 ,  0100 0000 , 0000 0001
			
			
		}
		
		IIC_SCL(0); //��������
		IIC_DELAY;	
	}
	//�������
	if( !ack ) // ack
	{
		  iic_ack(); //����ack
	}else //nack
	{
		  iic_nack();
	}

	return ret;
}





