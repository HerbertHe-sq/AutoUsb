/******************************************************************************
* �� �� ��: includes.h
* ��    ��: Herbert   
* ��ʼ����: 190707 
* �汾��־:

******************************************************************************/
#ifndef _INCLUDES_H_
#define _INCLUDES_H_

#include "stm32f0xx.h"
#include "stm32f0xx_adc.h"
#include "string.h"

//#include "sys.h"
#include "mydefine.h"

#include "user.h"
#include "delay.h"
#include "switch.h"
#include "ble.h"
#include "eepiic.h"
#include "24cl02.h"
#include "oled.h"
#include "ds1302.h"
#include "flash.h"
#include "itemp.h"
#include "game.h"
#include "dev_func.h"
#include "ds18b20.h"


#define IWDG_ENV

#define MODE_NORMALLY  0           //��ͨģʽ
#define MODE_TIMESET   1           //ʱ������ģʽ
#define MODE_ALARM     2           //��ʱģʽ
#define MODE_CPU_DATA  3           //CPU���ݼ��
#define MODE_TEMP      4           //�¶ȼ��
#define MODE_GAME      5           //��Ϸģʽ
#define MODE_LOWP      6           //�͹���ģʽ
#define MODE_BTRS      7           //��������ģʽ
#define MODE_BTVE      8           //�����汾��ʾģʽ
#define MODE_BTAD      9           //������ַ��ʾģʽ
#define MODE_SECLECT   10          //ѡ��ģʽ



#define ALM_EEPADDR 0x00           //���ӵ�ַ
#define SW_EEPADDR  0x08           //USB����
#define SNAKE_SPEED 0x09           //̰�����ٶȵ����ַ

typedef struct{
	
	U8 SysMode;
  U8 SysALMTime[8];	

}SYSTEMSTRUCT;

typedef struct{
	U8 swFlag;      //����
	U8 beepFlag;    //������
	U8 upTimeFg;    //����ʱ���־λ
	U8 sLedFlag;    //��������־λ  
  U8 tim_SetCnt;  //ѡ�����ñ�־λ
  U8 usb_SwFg;	  //USB���ر�־λ
	U8 screenFlag;  //��Ļ���ر�־λ
  U16 pwFlag;     //����������
	U16 timSFlag;   //ˢ��ʱ��
	U16 timLedFlag; //������
	U16 adcFlag;    //ADCת����־λ
}SYSTEM_CTRL_FLAG;

typedef struct{
	U8 oled_str_time[16];   //ʱ����
	U8 oled_str_daily[16];  //������
	U8 oled_str_sbar[16];   //״̬��
}OLED_STR_T;


extern OLED_STR_T ole_str;

#endif

