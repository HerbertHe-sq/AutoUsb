#ifndef __EEPIIC_H
#define __EEPIIC_H
#include "stm32f0xx.h"

//IO��������
//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}  //��ʹ�üĴ����������ο�΢�������ο��ֲ�V1.0
//#define SDA_OUT1() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}  //P116


//IO��������	 
#define IIC_SCLH    GPIO_SetBits(GPIOA,GPIO_Pin_11) //SCL
#define IIC_SCLL    GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define IIC_SDAH    GPIO_SetBits(GPIOA,GPIO_Pin_12) //SDA	
#define IIC_SDAL     GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define READ_SDA   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)  //����SDA 

void SDA_IN(void);         //����IO�ڵķ���
void SDA_OUT(void);

void IIC_Init(void);        //IIC�ĳ�ʼ������
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);   //дһ���ֽڵĺ���
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	            //��һ���ֽڵĺ���



#endif
