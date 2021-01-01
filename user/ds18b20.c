#include "includes.h"

// *****************************************************************************
// 函数名称：Delay_1us
// 功能描述：延時
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Delay_1us(unsigned int Del_1us)
{
	unsigned char j;
	while(Del_1us--)
	{	
		for(j=0;j<2;j++);
	}
}

// *****************************************************************************
// 函数名称：DSB_IO_IN
// 功能描述：设置端口输入
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void DSB_IO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15; 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// *****************************************************************************
// 函数名称：DSB_IO_OUT
// 功能描述：设置端口输出
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void DSB_IO_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15; 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// *****************************************************************************
// 函数名称：DSB_SetIO
// 功能描述：设置端口状态
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void DSB_SetIO(U8 status)
{
	if(status)GPIO_SetBits(GPIOA,GPIO_Pin_15);
	else      GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}

// *****************************************************************************
// 函数名称：DSB_Init
// 功能描述：初始化
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
U8 DSB_Init(void)
{
	DSB_SetIO(1);    //输出1   
  DSB_Reset();
	return DSB_Check();
}

// *****************************************************************************
// 函数名称：DSB_Reset
// 功能描述：模块复位
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void DSB_Reset(void)
{
	DSB_IO_OUT(); //SET PA0 OUTPUT
	DSB_SetIO(0); //拉低DQ
	Delay_1us(750);    //拉低750us
	DSB_SetIO(1); //DQ=1 
	Delay_1us(15);     //15US
}

// *****************************************************************************
// 函数名称：DSB_Check
// 功能描述：检测到DS18B20的存在
// 输入参数: /
// 输出参数: /
// 返回参量: 1:未检测到DS18B20的存在 0:存在
// *****************************************************************************
U8 DSB_Check(void) 	   
{   
    U8 retry=0;
    DSB_IO_IN();//SET PA15 INPUT	 
    while (DSBDQ_IN&&retry<200)
    {
        retry++;
        Delay_1us(1);
    };	 
    if(retry>=200)return 1;
    else retry=0;
    while (!DSBDQ_IN&&retry<240)
    {
        retry++;
        Delay_1us(1);
    };
    if(retry>=240)return 1;	    
    return 0;
}

// *****************************************************************************
// 函数名称：DSB_ReadBit
// 功能描述：从DS18B20读取一个位
// 输入参数: /
// 输出参数: /
// 返回参量: 1/0
// *****************************************************************************
U8 DSB_ReadBit(void) 			 // read one bit
{
    U8 data;
    DSB_IO_OUT();//SET PA15 OUTPUT
    DSB_SetIO(0); 
    Delay_1us(2);
    DSB_SetIO(1);  
    DSB_IO_IN();//SET PA15 INPUT
    Delay_1us(12);
    if(DSBDQ_IN)data=1;
    else data=0;	 
    Delay_1us(50);           
    return data;
}

// *****************************************************************************
// 函数名称：DSB_ReadByte
// 功能描述：读到的数据
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
U8 DSB_ReadByte(void)    // read one byte
{        
    U8 i,j,dat;
    dat=0;
    for (i=1;i<=8;i++) 
    {
        j=DSB_ReadBit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}

// *****************************************************************************
// 函数名称：DSB_WriteByte
// 功能描述：写一个字节到DS18B20
// 输入参数: dat：要写入的字节
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void DSB_WriteByte(U8 dat)     
{             
    U8 j;
    U8 testb;
    DSB_IO_OUT();//SET PA15 OUTPUT;
    for (j=1;j<=8;j++) 
    {
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DSB_SetIO(0);// Write 1
            Delay_1us(2);                            
            DSB_SetIO(1);
            Delay_1us(60);             
        }
        else 
        {
            DSB_SetIO(0);// Write 0
            Delay_1us(60);             
            DSB_SetIO(1);
            Delay_1us(2);                          
        }
    }
}

//开始温度转换
void DSB_Start(void)// ds1820 start convert
{   						               
    DSB_Reset();	   
    DSB_Check();	 
    DSB_WriteByte(0xcc);// skip rom
    DSB_WriteByte(0x44);// convert
} 

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DSB_GetTemp(void)
{
    U8 temp,TL,TH;
    short tem;
    DSB_Start();                    // ds1820 start convert
    DSB_Reset();
    DSB_Check();	 
    DSB_WriteByte(0xcc);// skip rom
    DSB_WriteByte(0xbe);// convert	    
    TL=DSB_ReadByte(); // LSB   
    TH=DSB_ReadByte(); // MSB  
    
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	
		
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(short)((float)tem*0.625);//转换     
    if(temp)return tem; //返回温度值
    else return -tem;    
}

