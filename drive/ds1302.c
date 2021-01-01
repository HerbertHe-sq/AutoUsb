#include "includes.h"


U8 DS1302_time[8];

/************************************************
�������ƣ�void DS1302_Init(void)
��������: DS1302�˿ڵĳ�ʼ��
��ڲ�������
����ֵ��  ��
˵��  ��  BA9->RST,BA11->SCLK
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
	DS1302_Write(0x80,0x00);       //��������
	DS1302_Write(0x90,0xa8);       //������
	DS1302_Write(0x8e,0x80);       //ʹ��д��������ֹ�����д��
	
}

/************************************************
�������ƣ�void IO_DS1302In(void)
��������: DS1302��I/O����Ϊ����
��ڲ�������
����ֵ��  ��
˵��  ��  BA10->I/O
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
�������ƣ�void IO_DS1302Out(void)
��������: DS1302��I/O����Ϊ���
��ڲ�������
����ֵ��  ��
˵��  ��  BA10->I/O
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
�������ƣ�void DS1302_WrByte(U8 dat)
��������: дһ���ֽڵ�DS1302
��ڲ�����dat
����ֵ��  ��
˵��  ��  
************************************************/
void DS1302_WrByte(U8 dat)
{
  U8 i = 0;
	IO_DS1302Out();//�������
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
�������ƣ�U8 DS1302_RdByte(void)
��������: ��DS1302�ж���һ���ֽ�
��ڲ�������
����ֵ��  dat
˵��  ��  
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
�������ƣ�void DS1302_Write(U8 add,U8 dat)
��������: ��DS1302д��һ����ַ������
��ڲ�������ַadd,����dat
����ֵ��  ��
˵��  ��  
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
�������ƣ�U8 DS1302_Read(U8 add)
��������: ��DS1302��Ӧ��ַ��������
��ڲ�������ַadd
����ֵ��  ����������shuju
˵��  ��  
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
�������ƣ�void DS1302_SetTime(U8 year, U8 month,U8 date,U8 hour,U8 min,U8 sec,U8 week)
��������: �����ں�ʱ����Ϣд��DS1302
��ڲ�������year,��month,��data,ʱhour,��min,��sec,��week
����ֵ��  ����������shuju
˵��  ��  
************************************************/
void DS1302_SetTime(U8 year, U8 month,U8 date,U8 hour,U8 min,U8 sec,U8 week)
 {
   DS1302_Write(DS_WRPT,0x00);//���д������д��ʱ����Ϣ
   DS1302_Write(DS_SEC,sec);//д����ʱ��
   DS1302_Write(DS_MIN,min);//д���ʱ��
   DS1302_Write(DS_HOUR,hour);//д��ʱʱ��
   DS1302_Write(DS_DATE,date);//д����
   DS1302_Write(DS_MON,month);//д����
   DS1302_Write(DS_WEEK,week);//д����
   DS1302_Write(DS_YEAR,year);//д����
   DS1302_Write(DS_WRPT,0x80);//ʹ��д��������ֹ�����д��
 }

/************************************************
�������ƣ�void DS1302_GetTime(void)
��������: ��DS130�������ں�ʱ����Ϣ
��ڲ�������
����ֵ��  ��
˵��  ��  
************************************************/
void DS1302_GetTime(void)
{ 
  U8 i;
	
	for(i=0;i<7;i++)
	{
		DS1302_time[i]=DS1302_Read(0x81+2*i);  //����DS1302��ʱ����Ϣ�����浽������
		DS1302_time[i] = ((DS1302_time[i]>>4)&0x0f)*10 + (DS1302_time[i]&0x0f);
		
	}
}
