#include "includes.h"


/*******************************************************
*�������ƣ�u8 AT24CXX_ReadOneByte(u16 ReadAddr)
*�������ܣ���AT24CXXָ����ַ����һ������
*���������ReadAddr:��ʼ�����ĵ�ַ
*����ֵ�� temp--��ȡ������ 
*˵���� 
*******************************************************/
uint8_t AT24C02_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();                 //����һ��ֹͣ����	    
	return temp;
}

/*******************************************************
*�������ƣ�void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
*�������ܣ���AT24CXXָ����ַд��һ������
*���������WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ	   DataToWrite:Ҫд�������
*����ֵ�� ��
*˵���� 
*******************************************************/
void AT24C02_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
 	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/*******************************************************
*�������ƣ�void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
*�������ܣ���AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
*���������WriteAddr  :��ʼд�����ݵ�Ŀ�ĵ�ַ	   DataToWrite:���������׵�ַ   Len :Ҫд�����ݵĳ��ȣ��ֽ�Ϊ��λ
*����ֵ�� ��
*˵���� �ú�������д��16bit����32bit������.8λҲ����
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
*�������ƣ�u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
*�������ܣ���AT24CXX�����ָ����ַ��ʼ��������ΪLen������
*���������ReadAddr  :��ʼ�����ĵ�ַ	Len  :Ҫ�������ݵĳ��ȣ��ֽ�Ϊ��λ
*����ֵ�� ����������
*˵���� �ú������ڶ���16bit����32bit������.8λҲ����
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
*�������ƣ�u8 AT24CXX_Check(void)
*�������ܣ����AT24CXX�Ƿ�����
*���������ReadAddr  :��ʼ�����ĵ�ַ	Len  :Ҫ�������ݵĳ��ȣ��ֽ�Ϊ��λ
*����ֵ�� 1:���ʧ�ܣ�0:���ɹ�
*˵���� ��������24XX�����һ����ַ(255)���洢��־��. ���������24Cϵ��,�����ַҪ�޸�
*******************************************************/
uint8_t AT24C02_Check(void)
{
	uint8_t temp;
	temp=AT24C02_ReadOneByte(255);     //����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else                               //�ų���һ�γ�ʼ�������
	{
		AT24C02_WriteOneByte(255,0X55);
	    temp=AT24C02_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/*******************************************************
*�������ƣ�void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
*�������ܣ���AT24CXX�����ָ����ַ��ʼ����ָ������������
*���������ReadAddr  :��ʼ�����ĵ�ַ,��24c02Ϊ0~255  pBuffer  :���������׵�ַ   NumToRead:Ҫ�������ݵĸ���
*����ֵ�� ��
*˵���� 
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
*�������ƣ�void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
*�������ܣ���AT24CXX�����ָ����ַ��ʼд��ָ������������
*���������WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255  pBuffer  :���������׵�ַ   NumToRead:Ҫд�����ݵĸ���
*����ֵ�� ��
*˵���� 
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
