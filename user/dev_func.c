#include "includes.h"


//�ⲿ������
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern U8 staBar[],dailyStr[],timeStr[];
extern U8 tx_cnt,rx_cnt;

// *****************************************************************************
// �������ƣ�BT_Usart_Rec
// ������������������
// �������: /
// �������: /
// ���ز���: /
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
					staBar[5] = 'M'; 
					SysParam.SysMode = MODE_NORMALLY;
			}
		  break;
		case MODE_BTVE:
			if(Usart_Rec_Buf[0]==0x48)
			{
				for(temp_i = 3;temp_i<13;temp_i++)
				{
					timeStr[temp_i] = Usart_Rec_Buf[temp_i-3];
					dailyStr[temp_i] = Usart_Rec_Buf[temp_i+8];
				}
			}
			break;
		case MODE_BTAD:			
		  timeStr[0] = ' ';
			timeStr[1] = ' ';
			timeStr[2] = ' ';
			timeStr[3] = 'B';
			timeStr[4] = 'T';
			timeStr[5] = '-';
			timeStr[6] = 'A';
			timeStr[7] = 'D';
			timeStr[8] = 'D';
			timeStr[9] = 'R';
			timeStr[10] = 'E';
			timeStr[11] = 'S';
			timeStr[12] = 'S';
			timeStr[13] = ' ';
			timeStr[14] = ' ';
			timeStr[15] = ' ';
		
			dailyStr[0] = ' ';
			dailyStr[1] = Usart_Rec_Buf[0];
			dailyStr[2] = Usart_Rec_Buf[1];
			dailyStr[3] = Usart_Rec_Buf[3];
			dailyStr[4] = Usart_Rec_Buf[4];
			dailyStr[5] =  ':';
			dailyStr[6] = Usart_Rec_Buf[6];
			dailyStr[7] = Usart_Rec_Buf[7];
			dailyStr[8] = Usart_Rec_Buf[9];
			dailyStr[9] = Usart_Rec_Buf[10];
			dailyStr[10] =  ':';
			dailyStr[11] = Usart_Rec_Buf[12];
			dailyStr[12] = Usart_Rec_Buf[13];
			dailyStr[13] = Usart_Rec_Buf[15];
			dailyStr[14] = Usart_Rec_Buf[16];
			dailyStr[15] = ' ';
			break;
  }
}

// *****************************************************************************
// �������ƣ�OLED_Print
// ������������Ϸ��ӡ
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void OLED_Print(U8 *tip_str)
{
	OLED_Clear_Row(2,6);
	OLED_ShowString(0,2,tip_str,16);
}

// *****************************************************************************
// �������ƣ�Signal_LED
// �����������źŵƿ���
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Signal_LED(void)
{
	if(!sysCtrlFlag.sLedFlag)
	{
		sysCtrlFlag.sLedFlag = 255;
		if(tx_cnt%2==1)LED1_ON;
		else LED1_OFF;
		
		if(rx_cnt%2==1)LED2_ON;
		else LED2_OFF;
	}
}


