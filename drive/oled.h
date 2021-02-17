//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : Evk123
//  ��������   : 2014-0101
//  ����޸�   : 
//  ��������   : 0.69��OLED �ӿ���ʾ����(STM32F103ZEϵ��IIC)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND   ��Դ��
//              VCC   ��5V��3.3v��Դ
//              SCL   ��PA5��SCL��
//              SDA   ��PA7��SDA��            
//              ----------------------------------------------------------------
//Copyright(C) �о�԰����2014/3/16
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
#ifndef __OLED_H
#define __OLED_H	

#include "mydefine.h"
#include "stdlib.h"


#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED IIC�˿ڶ���----------------  					   

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_6)//SCL
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_6)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_7)//SDA
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_7)

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����


//OLED�����ú���
void OLED_WR_Byte(unsigned dat,unsigned cmd);  
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear_Row(U8 start_row,U8 end_row);
void OLED_Clear(void);
void OLED_DrawPoint(U8 x,U8 y,U8 t);
void OLED_Fill(U8 x1,U8 y1,U8 x2,U8 y2,U8 dot);
void OLED_ShowChar(U8 x,U8 y,U8 chr,U8 Char_Size);
void OLED_ShowNum(U8 x,U8 y,U32 num,U8 len,U8 size);
void OLED_ShowString(U8 x,U8 y, U8 *p,U8 Char_Size,U8 chr_len);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(U8 x,U8 y,U8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void Delay_50ms(unsigned int Del_50ms);
void Delay_1ms(unsigned int Del_1ms);
void fill_picture(unsigned char fill_Data);
//void Picture();
void Oled_Write_IIC_DataIIC_Start(void);
void Oled_Write_IIC_DataIIC_Stop(void);
void Oled_Write_IIC_DataWrite_IIC_Command(unsigned char IIC_Command);
void Oled_Write_IIC_DataWrite_IIC_Data(unsigned char IIC_Data);
void Oled_Write_IIC_DataWrite_IIC_Data(unsigned char IIC_Byte);

void Oled_IIC_Wait_Ack(void);
#endif  
	 



