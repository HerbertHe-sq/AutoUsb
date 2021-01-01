#include "includes.h"


/*******************************************************
*函数名称：u8 AT24CXX_ReadOneByte(u16 ReadAddr)
*函数功能：在AT24CXX指定地址读出一个数据
*输入参数：ReadAddr:开始读数的地址
*返回值： temp--读取的数据 
*说明： 
*******************************************************/
uint8_t AT24C02_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();                 //产生一个停止条件	    
	return temp;
}

/*******************************************************
*函数名称：void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
*函数功能：在AT24CXX指定地址写入一个数据
*输入参数：WriteAddr  :写入数据的目的地址	   DataToWrite:要写入的数据
*返回值： 无
*说明： 
*******************************************************/
void AT24C02_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/*******************************************************
*函数名称：void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
*函数功能：在AT24CXX里面的指定地址开始写入长度为Len的数据
*输入参数：WriteAddr  :开始写入数据的目的地址	   DataToWrite:数据数组首地址   Len :要写入数据的长度，字节为单位
*返回值： 无
*说明： 该函数用于写入16bit或者32bit的数据.8位也可以
*******************************************************/
void AT24C02_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{  	
	uint8_t t;
	for(t=0;t<Len;t++)
	{
		AT24C02_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/*******************************************************
*函数名称：u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
*函数功能：在AT24CXX里面的指定地址开始读出长度为Len的数据
*输入参数：ReadAddr  :开始读出的地址	Len  :要读出数据的长度，字节为单位
*返回值： 读出的数据
*说明： 该函数用于读出16bit或者32bit的数据.8位也可以
*******************************************************/
uint32_t AT24C02_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{  	
	uint8_t t;
	uint32_t temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24C02_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/*******************************************************
*函数名称：u8 AT24CXX_Check(void)
*函数功能：检查AT24CXX是否正常
*输入参数：ReadAddr  :开始读出的地址	Len  :要读出数据的长度，字节为单位
*返回值： 1:检测失败，0:检测成功
*说明： 这里用了24XX的最后一个地址(255)来存储标志字. 如果用其他24C系列,这个地址要修改
*******************************************************/
uint8_t AT24C02_Check(void)
{
	uint8_t temp;
	temp=AT24C02_ReadOneByte(255);     //避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else                               //排除第一次初始化的情况
	{
		AT24C02_WriteOneByte(255,0X55);
	    temp=AT24C02_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/*******************************************************
*函数名称：void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
*函数功能：在AT24CXX里面的指定地址开始读出指定个数的数据
*输入参数：ReadAddr  :开始读出的地址,对24c02为0~255  pBuffer  :数据数组首地址   NumToRead:要读出数据的个数
*返回值： 无
*说明： 
*******************************************************/
void AT24C02_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24C02_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
} 

/*******************************************************
*函数名称：void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
*函数功能：在AT24CXX里面的指定地址开始写入指定个数的数据
*输入参数：WriteAddr :开始写入的地址 对24c02为0~255  pBuffer  :数据数组首地址   NumToRead:要写入数据的个数
*返回值： 无
*说明： 
*******************************************************/ 
void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		AT24C02_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
