#ifndef _SWITCH_H_
#define _SWITCH_H_

#include "mydefine.h"

#define SW_SEL_LINE 3
#define SW_SUM 6

typedef enum{
	KEY_FUNC_LEFT = 0,
	KEY_FUNC_UP,
	KEY_FUNC_ENTER,
	KEY_FUNC_RIGHT,
	KEY_FUNC_DOWN,
	KEY_FUNC_CANCEL
}key_func_id_t;

typedef enum{
	USB_SW_OFF = 0,
	USB_SW_ON
}usb_sw_state_t;

typedef struct{
	U8 mode_Fg;
	U8 sel_Fg;//模式标志位
}SEITCH_FLAG;

void SW_Delay(U8 de_Count);
U8 Switch_Scan(void);
void Switch_Sfit(U8 flag);
void Key_Process(U8 key_id);
void Key_DownEvt(void);
void Key_UpEvt(void);
void Key_LeftEvt(void);
void Key_RightEvt(void);
void Key_EnterEvt(void);
void Key_CacelEvt(void);
void ArrayCopy(U8 *source_buf,U8 *des_buf,U16 len,U16 start);
void Save_ALMTime(void);
void ALM_Proc(void);
void SetPowerSta(U8 flag);
void PWM_IRQHandler(void);
void PWR_Sleep_Mode(U8 mcu_status);

#endif
