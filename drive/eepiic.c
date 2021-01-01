#include "includes.h"



/*****************************************************
由于神舟四号时钟比普通开发板时钟快两倍，所以延时是原来的
两倍
*****************************************************/
/*******************************************************
*函数名称：void IIC_Init(void)
*函数功能：对模拟I2C的GPIO的时钟和端口初始化
*输入参数：无
*返回值：无
*说明： PA11管脚设置为SCL，PA12管脚设置为SDA
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

void SDA_IN(void)      //配置SDA线是输入方向
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}  //是使用寄存器操作，参考微控制器参考手册V1.0
void SDA_OUT(void)     //配置SDA线输出方向
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	 
 }	

//产生IIC起始信号
/*******************************************************
*函数名称：void IIC_Start(void)
*函数功能：产生I2C的起始信号
*输入参数：无
*返回值：无
*说明： 在SCL为高时，SDA由高变低，即可产生起始信号
*******************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDAH;	  	  
	IIC_SCLH;
	delay_us(4);
 	IIC_SDAL;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCLL;//钳住I2C总线，准备发送或接收数据 
}	  

/*******************************************************
*函数名称：void IIC_Stop(void)
*函数功能：产生I2C的停止信号
*输入参数：无
*返回值：无
*说明： 在SCL为高时，SDA由低变高，即可产生停止信号
*******************************************************/
void IIC_Stop(void)
{
	SDA_OUT();      //sda线输出
	IIC_SCLL;
	IIC_SDAL;      //STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCLH; 
	delay_us(4);
	IIC_SDAH;      //发送I2C总线结束信号
	delay_us(4);							   	
}

/*******************************************************
*函数名称：u8 IIC_Wait_Ack(void)
*函数功能：等待应答信号到来
*输入参数：无
*返回值： 1，接收应答失败    0，接收应答成功 
*说明： 主机释放总线，并拉高，如果检测到总线被拉低，说明有应答信号
*******************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();           //SDA设置为输入  
	IIC_SDAH;		    //拉高总线
	delay_us(1);	   
	IIC_SCLH;
	delay_us(1);	 
	while(READ_SDA)		  //读取总线的值
	{
		ucErrTime++;
		if(ucErrTime>250)	  //如果一直为1，则没有应答信号
		{
			IIC_Stop();
			return 1;		  //没有应答，返回值1
		}
	}
	IIC_SCLL;//时钟输出0 	   
	return 0;  
} 

/*******************************************************
*函数名称：void IIC_Ack(void)
*函数功能：产生应答信号
*输入参数：无
*返回值： 无 
*说明： 主机产生应答信号
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
*函数名称：void IIC_NAck(void)
*函数功能：不产生应答信号
*输入参数：无
*返回值： 无 
*说明： 主机不产生应答信号
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
*函数名称：void IIC_Send_Byte(u8 txd)
*函数功能：I2C发送一个字节
*输入参数：无
*返回值： 无 
*说明： 主机不产生应答信号
*******************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    IIC_SCLL;            //拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		if(txd&0x80)	  //提取数据的最高位，先传输最高位
			IIC_SDAH;
		else
			IIC_SDAL;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCLH;
		delay_us(2); 
		IIC_SCLL;	
		delay_us(2);
    }	 
} 

/*******************************************************
*函数名称：u8 IIC_Read_Byte(unsigned char ack)
*函数功能：I2C读取一个字节
*输入参数：ack--应答信号，ack=1时，发送ACK，ack=0，发送nACK
*返回值： receive--读取的字节 
*说明： 
*******************************************************/	    
uint8_t IIC_Read_Byte(unsigned char ack)
{
	uint8_t i,receive=0;
	SDA_IN();              //SDA设置为输入
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
        IIC_NAck();     //发送nACK
    else
        IIC_Ack();      //发送ACK   
    return receive;
}


