//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : Evk123
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : 0.69寸OLED 接口演示例程(STM32F103ZE系列IIC)
//              说明: 
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接5V或3.3v电源
//              SCL   接PA5（SCL）
//              SDA   接PA7（SDA）            
//              ----------------------------------------------------------------
//Copyright(C) 中景园电子2014/3/16
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
//-----------------OLED IIC端口定义----------------  					   

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_6)//SCL
#define OLED_SCLK_Set() GPIO_SetBits(GPIOB,GPIO_Pin_6)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_7)//SDA
#define OLED_SDIN_Set() GPIO_SetBits(GPIOB,GPIO_Pin_7)

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制用函数
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
	 



