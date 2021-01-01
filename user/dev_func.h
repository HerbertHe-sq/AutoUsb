#ifndef _DEV_FUNC_H_
#define _DEV_FUNC_H_

typedef enum{
	MODE_ID_GEN = 1,
	MODE_ID_TIMESET = 2,
	MODE_ID_ALMSET = 3,
	MODE_ID_CPU = 4,
	MODE_ID_TEMP = 5,
	MODE_ID_GAME = 6,
	MODE_ID_LOWP = 7,
	MODE_ID_BTRS = 8,
	MODE_ID_BTVE = 9,
	MODE_ID_BTAD = 10	
}mode_type_id_t;

typedef enum{
	TIME_SET_ID_HOUR = 1,
	TIME_SET_ID_MIN = 2,
	TIME_SET_ID_SEC = 3,
	TIME_SET_ID_YEAR = 4,
	TIME_SET_ID_MON = 5,
	TIME_SET_ID_DAY = 6
}time_set_id_t;

void BT_Usart_Rec(U8 *rec_buf);
void OLED_Print(U8 *tip_str);
void Signal_LED(void);

#endif
