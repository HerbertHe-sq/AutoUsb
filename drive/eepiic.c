#include "includes.h"



/*****************************************************
���������ĺ�ʱ�ӱ���ͨ������ʱ�ӿ�������������ʱ��ԭ����
����
*****************************************************/
/*******************************************************
*�������ƣ�void IIC_Init(void)
*�������ܣ���ģ��I2C��GPIO��ʱ�ӺͶ˿ڳ�ʼ��
*�����������
*����ֵ����
*˵���� PA11�ܽ�����ΪSCL��PA12�ܽ�����ΪSDA
*******************************************************/

void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  //	IIC_SDA=1,IIC_SCL=1;
	GPIO_SetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12);

}

void SDA_IN(void)      //����SDA�������뷽��
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}  //��ʹ�üĴ����������ο�΢�������ο��ֲ�V1.0
void SDA_OUT(void)     //����SDA���������
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	 
 }	

//����IIC��ʼ�ź�
/*******************************************************
*�������ƣ�void IIC_Start(void)
*�������ܣ�����I2C����ʼ�ź�
*�����������
*����ֵ����
*˵���� ��SCLΪ��ʱ��SDA�ɸ߱�ͣ����ɲ�����ʼ�ź�
*******************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDAH;	  	  
	IIC_SCLH;
	delay_us(4);
 	IIC_SDAL;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCLL;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  

/*******************************************************
*�������ƣ�void IIC_Stop(void)
*�������ܣ�����I2C��ֹͣ�ź�
*�����������
*����ֵ����
*˵���� ��SCLΪ��ʱ��SDA�ɵͱ�ߣ����ɲ���ֹͣ�ź�
*******************************************************/
void IIC_Stop(void)
{
	SDA_OUT();      //sda�����
	IIC_SCLL;
	IIC_SDAL;      //STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCLH; 
	delay_us(4);
	IIC_SDAH;      //����I2C���߽����ź�
	delay_us(4);							   	
}

/*******************************************************
*�������ƣ�u8 IIC_Wait_Ack(void)
*�������ܣ��ȴ�Ӧ���źŵ���
*�����������
*����ֵ�� 1������Ӧ��ʧ��    0������Ӧ��ɹ� 
*˵���� �����ͷ����ߣ������ߣ������⵽���߱����ͣ�˵����Ӧ���ź�
*******************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();           //SDA����Ϊ����  
	IIC_SDAH;		    //��������
	delay_us(1);	   
	IIC_SCLH;
	delay_us(1);	 
	while(READ_SDA)		  //��ȡ���ߵ�ֵ
	{
		ucErrTime++;
		if(ucErrTime>250)	  //���һֱΪ1����û��Ӧ���ź�
		{
			IIC_Stop();
			return 1;		  //û��Ӧ�𣬷���ֵ1
		}
	}
	IIC_SCLL;//ʱ�����0 	   
	return 0;  
} 

/*******************************************************
*�������ƣ�void IIC_Ack(void)
*�������ܣ�����Ӧ���ź�
*�����������
*����ֵ�� �� 
*˵���� ��������Ӧ���ź�
*******************************************************/
void IIC_Ack(void)
{
	IIC_SCLL;
	SDA_OUT();
	IIC_SDAL;
	delay_us(2);
	IIC_SCLH;
	delay_us(2);
	IIC_SCLL;
}


/*******************************************************
*�������ƣ�void IIC_NAck(void)
*�������ܣ�������Ӧ���ź�
*�����������
*����ֵ�� �� 
*˵���� ����������Ӧ���ź�
*******************************************************/	    
void IIC_NAck(void)
{
	IIC_SCLL;
	SDA_OUT();
	IIC_SDAH;
	delay_us(2);
	IIC_SCLH;
	delay_us(2);
	IIC_SCLL;
}

/*******************************************************
*�������ƣ�void IIC_Send_Byte(u8 txd)
*�������ܣ�I2C����һ���ֽ�
*�����������
*����ֵ�� �� 
*˵���� ����������Ӧ���ź�
*******************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCLL;            //����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
		if(txd&0x80)	  //��ȡ���ݵ����λ���ȴ������λ
			IIC_SDAH;
		else
			IIC_SDAL;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCLH;
		delay_us(2); 
		IIC_SCLL;	
		delay_us(2);
    }	 
} 

/*******************************************************
*�������ƣ�u8 IIC_Read_Byte(unsigned char ack)
*�������ܣ�I2C��ȡһ���ֽ�
*���������ack--Ӧ���źţ�ack=1ʱ������ACK��ack=0������nACK
*����ֵ�� receive--��ȡ���ֽ� 
*˵���� 
*******************************************************/	    
uint8_t IIC_Read_Byte(unsigned char ack)
{
	uint8_t i,receive=0;
	SDA_IN();              //SDA����Ϊ����
    for(i=0;i<8;i++ )
	  {
        IIC_SCLL; 
        delay_us(2);
		IIC_SCLH;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();     //����nACK
    else
        IIC_Ack();      //����ACK   
    return receive;
}


