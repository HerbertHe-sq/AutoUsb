#ifndef __BLE_H
#define __BLE_H


#define USART_BUF_LEN  50
#define AT_COMMAND_TEST         0
#define AT_COMMAND_VERSION      1
#define AT_COMMAND_ADDR         2
#define UASRT1_IDLE USART_GetFlagStatus(USART1,USART_FLAG_IDLE)

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

typedef enum
{
	UART_STATE_ITEM_WAIT_ACK = 0,
	UART_STATE_ITEM_RECEIVING,
	UART_STATE_ITEM_UNPACK,
	UART_STATE_ITEM_FINISH,
	UART_STATE_ITEM_NUM
}uart_state_t;

typedef struct{
	U8 Rx_Sta;
	U8 heartCnt;
	U8 tx_cnt;
	U8 rx_cnt;
}BLE_STATUS;

typedef struct
{
	U8 time_cnt;
	U8 rx_index;
	U8 rx_state;
	U8 rx_buf[USART_BUF_LEN];
}uart_rx_msg_t;


void USART_Rec(void);
U8 USART_Unpack(void);
void USART_Deal(U8 flag);
void USART_DMASendDat(U8 *buf,U8 len);
void USART_SendDat(U8 *buf,U8 len);
void Usart_Tx_Data(void);
U8 HC_Reset(void);
void HC_AT_Command(U8 com_index);
void ST_Device_Reset(void);
void Upload_RtcTime(void);
void Modify_RtcTime(U8 *buf);
void TIME_IRQHandler(void);
void TIM_OUT_Handler(void);
void TIM1_Start(void);
void TIM1_Stop(void);

extern U8 uart_tx_buf[];


#endif
