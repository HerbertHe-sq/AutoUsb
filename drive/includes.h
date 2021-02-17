/******************************************************************************
* 文 件 名: includes.h
* 作    者: Herbert   
* 开始日期: 190707 
* 版本日志:

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

#define MODE_NORMALLY  0           //普通模式
#define MODE_TIMESET   1           //时间设置模式
#define MODE_ALARM     2           //定时模式
#define MODE_CPU_DATA  3           //CPU数据监测
#define MODE_TEMP      4           //温度检测
#define MODE_GAME      5           //游戏模式
#define MODE_LOWP      6           //低功耗模式
#define MODE_BTRS      7           //蓝牙重启模式
#define MODE_BTVE      8           //蓝牙版本显示模式
#define MODE_BTAD      9           //蓝牙地址显示模式
#define MODE_SECLECT   10          //选择模式



#define ALM_EEPADDR 0x00           //闹钟地址
#define SW_EEPADDR  0x08           //USB记忆
#define SNAKE_SPEED 0x09           //贪吃蛇速度掉电地址

typedef struct{
	
	U8 SysMode;
  U8 SysALMTime[8];	

}SYSTEMSTRUCT;

typedef struct{
	U8 swFlag;      //开关
	U8 beepFlag;    //蜂鸣器
	U8 upTimeFg;    //更新时间标志位
	U8 sLedFlag;    //蜂鸣器标志位  
  U8 tim_SetCnt;  //选择设置标志位
  U8 usb_SwFg;	  //USB开关标志位
	U8 screenFlag;  //屏幕开关标志位
  U16 pwFlag;     //电量进度条
	U16 timSFlag;   //刷屏时间
	U16 timLedFlag; //呼吸灯
	U16 adcFlag;    //ADC转换标志位
}SYSTEM_CTRL_FLAG;

typedef struct{
	U8 oled_str_time[16];   //时间栏
	U8 oled_str_daily[16];  //日期栏
	U8 oled_str_sbar[16];   //状态栏
}OLED_STR_T;


extern OLED_STR_T ole_str;

#endif

