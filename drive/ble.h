#ifndef __BLE_H
#define __BLE_H


#define USART_BUF_LEN  50
#define AT_COMMAND_TEST         0
#define AT_COMMAND_VERSION      1
#define AT_COMMAND_ADDR         2

typedef enum{
	BLE_FUNC_ITEM_LED1 = 0x01,
	BLE_FUNC_ITEM_LED2,
	BLE_FUNC_ITEM_LED3,
	BLE_FUNC_ITEM_BEEP,
	BLE_FUNC_ITEM_UPRTC,
	BLE_FUNC_ITEM_DRST,
	BLE_FUNC_ITEM_HEART,
	BLE_FUNC_ITEM_USB,
	BLE_FUNC_ITEM_MODRTC,
	BLE_FUNC_ITEM_NUM
}ble_func_id_t;

typedef struct{
	U8 Rx_Sta;
	U8 heartCnt;
	U8 tx_cnt;
	U8 rx_cnt;
}BLE_STATUS;


void USART_Rec(void);
U8 USART_Unpack(void);
void USART_Deal(U8 flag);
void USART_SendDat(U8 *buf,U8 len);
U8 HC_Reset(void);
void HC_AT_Command(U8 com_index);
void ST_Device_Reset(void);
void Upload_RtcTime(void);
void Modify_RtcTime(U8 *buf);
void TIME_IRQHandler(void);


#endif
