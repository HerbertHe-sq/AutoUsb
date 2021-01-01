#include "includes.h"

// *****************************************************************************
// �������ƣ�Delay_1us
// �����������ӕr
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�DSB_IO_IN
// �������������ö˿�����
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�DSB_IO_OUT
// �������������ö˿����
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�DSB_SetIO
// �������������ö˿�״̬
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void DSB_SetIO(U8 status)
{
	if(status)GPIO_SetBits(GPIOA,GPIO_Pin_15);
	else      GPIO_ResetBits(GPIOA,GPIO_Pin_15);
}

// *****************************************************************************
// �������ƣ�DSB_Init
// ������������ʼ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
U8 DSB_Init(void)
{
	DSB_SetIO(1);    //���1   
  DSB_Reset();
	return DSB_Check();
}

// *****************************************************************************
// �������ƣ�DSB_Reset
// ����������ģ�鸴λ
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void DSB_Reset(void)
{
	DSB_IO_OUT(); //SET PA0 OUTPUT
	DSB_SetIO(0); //����DQ
	Delay_1us(750);    //����750us
	DSB_SetIO(1); //DQ=1 
	Delay_1us(15);     //15US
}

// *****************************************************************************
// �������ƣ�DSB_Check
// ������������⵽DS18B20�Ĵ���
// �������: /
// �������: /
// ���ز���: 1:δ��⵽DS18B20�Ĵ��� 0:����
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
// �������ƣ�DSB_ReadBit
// ������������DS18B20��ȡһ��λ
// �������: /
// �������: /
// ���ز���: 1/0
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
// �������ƣ�DSB_ReadByte
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�DSB_WriteByte
// ����������дһ���ֽڵ�DS18B20
// �������: dat��Ҫд����ֽ�
// �������: /
// ���ز���: /
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

//��ʼ�¶�ת��
void DSB_Start(void)// ds1820 start convert
{   						               
    DSB_Reset();	   
    DSB_Check();	 
    DSB_WriteByte(0xcc);// skip rom
    DSB_WriteByte(0x44);// convert
} 

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
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
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	
		
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(short)((float)tem*0.625);//ת��     
    if(temp)return tem; //�����¶�ֵ
    else return -tem;    
}

