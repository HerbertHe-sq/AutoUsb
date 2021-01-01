#ifndef __EEPIIC_H
#define __EEPIIC_H
#include "stm32f0xx.h"

//IO方向设置
//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=8<<28;}  //是使用寄存器操作，参考微控制器参考手册V1.0
//#define SDA_OUT1() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}  //P116


//IO操作函数	 
#define IIC_SCLH    GPIO_SetBits(GPIOA,GPIO_Pin_11) //SCL
#define IIC_SCLL    GPIO_ResetBits(GPIOA,GPIO_Pin_11)
#define IIC_SDAH    GPIO_SetBits(GPIOA,GPIO_Pin_12) //SDA	
#define IIC_SDAL     GPIO_ResetBits(GPIOA,GPIO_Pin_12)
#define READ_SDA   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12)  //输入SDA 

void SDA_IN(void);         //配置IO口的方向
void SDA_OUT(void);

void IIC_Init(void);        //IIC的初始化设置
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);   //写一个字节的函数
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	            //读一个字节的函数



#endif
