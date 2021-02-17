#include "includes.h"


//外部变量：
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern BLE_STATUS bleStatus;


// *****************************************************************************
// 函数名称：BT_Usart_Rec
// 功能描述：蓝牙反馈
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void BT_Usart_Rec(U8 *Usart_Rec_Buf)
{
	U8 temp_i = 0;
	switch(SysParam.SysMode)
	{
		case MODE_BTRS:
			if(Usart_Rec_Buf[0]==0x4f && Usart_Rec_Buf[1]==0x4b)//OK
			{		  		
					OLED_Print((U8 *)"  BT RESET OK! ");
					ole_str.oled_str_sbar[5] = 'M'; 
					SysParam.SysMode = MODE_NORMALLY;
			}
		  break;
		case MODE_BTVE:
			if(Usart_Rec_Buf[0]==0x48)
			{
				for(temp_i = 3;temp_i<13;temp_i++)
				{
					ole_str.oled_str_time[temp_i] = Usart_Rec_Buf[temp_i-3];
					ole_str.oled_str_daily[temp_i] = Usart_Rec_Buf[temp_i+8];
				}
			}
			break;
		case MODE_BTAD:
      strcpy((char *)ole_str.oled_str_time,"   BT-ADDRESS   ");
		
			ole_str.oled_str_daily[0] = ' ';
			ole_str.oled_str_daily[1] = Usart_Rec_Buf[0];
			ole_str.oled_str_daily[2] = Usart_Rec_Buf[1];
			ole_str.oled_str_daily[3] = Usart_Rec_Buf[3];
			ole_str.oled_str_daily[4] = Usart_Rec_Buf[4];
			ole_str.oled_str_daily[5] =  ':';
			ole_str.oled_str_daily[6] = Usart_Rec_Buf[6];
			ole_str.oled_str_daily[7] = Usart_Rec_Buf[7];
			ole_str.oled_str_daily[8] = Usart_Rec_Buf[9];
			ole_str.oled_str_daily[9] = Usart_Rec_Buf[10];
			ole_str.oled_str_daily[10] =  ':';
			ole_str.oled_str_daily[11] = Usart_Rec_Buf[12];
			ole_str.oled_str_daily[12] = Usart_Rec_Buf[13];
			ole_str.oled_str_daily[13] = Usart_Rec_Buf[15];
			ole_str.oled_str_daily[14] = Usart_Rec_Buf[16];
			ole_str.oled_str_daily[15] = ' ';
			break;
  }
}

// *****************************************************************************
// 函数名称：OLED_Print
// 功能描述：游戏打印
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void OLED_Print(U8 *tip_str)
{
	OLED_Clear_Row(2,6);
	OLED_ShowString(0,2,tip_str,16,16);
}

// *****************************************************************************
// 函数名称：Signal_LED
// 功能描述：信号灯控制
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Signal_LED(void)
{
	if(!sysCtrlFlag.sLedFlag)
	{
		sysCtrlFlag.sLedFlag = 255;
		if(bleStatus.tx_cnt%2==1)LED1_ON;
		else LED1_OFF;
		
		if(bleStatus.rx_cnt%2==1)LED2_ON;
		else LED2_OFF;
	}
}


