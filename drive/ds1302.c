#include "includes.h"


U8 DS1302_time[8];

/************************************************
函数名称：void DS1302_Init(void)
函数功能: DS1302端口的初始化
入口参数：无
返回值：  无
说明  ：  BA9->RST,BA11->SCLK
************************************************/
void DS1302_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;    
   
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_5;			//SCLK,RST
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	DS_SCLK(0);  
																 //I/O
	DS1302_Write(0x80,0x00);       //秒钟清零
	DS1302_Write(0x90,0xa8);       //充电控制
	DS1302_Write(0x8e,0x80);       //使能写保护，防止误操作写入
	
}

/************************************************
函数名称：void IO_DS1302In(void)
函数功能: DS1302的I/O口设为输入
入口参数：无
返回值：  无
说明  ：  BA10->I/O
************************************************/
void IO_DS1302In(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 	 

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/************************************************
函数名称：void IO_DS1302Out(void)
函数功能: DS1302的I/O口设为输出
入口参数：无
返回值：  无
说明  ：  BA10->I/O
************************************************/
void IO_DS1302Out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct; 
	
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;			
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/************************************************
函数名称：void DS1302_WrByte(U8 dat)
函数功能: 写一个字节到DS1302
入口参数：dat
返回值：  无
说明  ：  
************************************************/
void DS1302_WrByte(U8 dat)
{
  U8 i = 0;
	IO_DS1302Out();//设置输出
	delay_us(1);
	for(i = 0;i<8;i++)
	{
		DS_SCLK(0);
		if(dat & 0x01)DS_WDATA(1);
		else DS_WDATA(0);
		delay_us(1);
		DS_SCLK(1);
		delay_us(1);
		dat>>=1;
	}
}

/************************************************
函数名称：U8 DS1302_RdByte(void)
函数功能: 从DS1302中读出一个字节
入口参数：无
返回值：  dat
说明  ：  
************************************************/
U8 DS1302_RdByte(void)
{
  U8 i, dat=0;
 
	IO_DS1302In();
	delay_us(1);
  for(i=0;i<8;i++)
  {
    dat>>=1; 
    DS_SCLK(1);
    delay_us(1);
    DS_SCLK(0);  
    delay_us(1);
    if(DS_READ==1)dat|=0x80;    
  }
  return dat;
}

/************************************************
函数名称：void DS1302_Write(U8 add,U8 dat)
函数功能: 向DS1302写入一个地址和数据
入口参数：地址add,数据dat
返回值：  无
说明  ：  
************************************************/
void DS1302_Write(U8 add,U8 dat)
{
  DS_RST(0);    
  DS_SCLK(0);  
  DS_RST(1);    
  DS1302_WrByte(add);
  DS1302_WrByte(dat); 
  DS_SCLK(1);  
  DS_RST(0); 
}

/************************************************
函数名称：U8 DS1302_Read(U8 add)
函数功能: 从DS1302相应地址读出数据
入口参数：地址add
返回值：  读出的数据shuju
说明  ：  
************************************************/
U8 DS1302_Read(U8 add)
{
   U8 bit;  
   DS_RST(0);   
   DS_SCLK(0);  
   DS_RST(1);
	
   DS1302_WrByte(add);  
   bit=DS1302_RdByte();
	
   DS_SCLK(1);   
   DS_RST(0);    
   return bit;  
}

/************************************************
函数名称：void DS1302_SetTime(U8 year, U8 month,U8 date,U8 hour,U8 min,U8 sec,U8 week)
函数功能: 将日期和时间信息写进DS1302
入口参数：年year,月month,日data,时hour,分min,秒sec,周week
返回值：  读出的数据shuju
说明  ：  
************************************************/
void DS1302_SetTime(U8 year, U8 month,U8 date,U8 hour,U8 min,U8 sec,U8 week)
 {
   DS1302_Write(DS_WRPT,0x00);//解除写保护，写入时间信息
   DS1302_Write(DS_SEC,sec);//写入秒时间
   DS1302_Write(DS_MIN,min);//写入分时间
   DS1302_Write(DS_HOUR,hour);//写入时时间
   DS1302_Write(DS_DATE,date);//写入日
   DS1302_Write(DS_MON,month);//写入月
   DS1302_Write(DS_WEEK,week);//写入周
   DS1302_Write(DS_YEAR,year);//写入年
   DS1302_Write(DS_WRPT,0x80);//使能写保护，防止误操作写入
 }

/************************************************
函数名称：void DS1302_GetTime(void)
函数功能: 从DS130读出日期和时间信息
入口参数：无
返回值：  无
说明  ：  
************************************************/
void DS1302_GetTime(void)
{ 
  U8 i;
	
	for(i=0;i<7;i++)
	{
		DS1302_time[i]=DS1302_Read(0x81+2*i);  //读出DS1302的时间信息，保存到数组中
		DS1302_time[i] = ((DS1302_time[i]>>4)&0x0f)*10 + (DS1302_time[i]&0x0f);
		
	}
}
