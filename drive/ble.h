#ifndef __BLE_H
#define __BLE_H


#define USART_BUF_LEN  50
#define AT_COMMAND_TEST         0
#define AT_COMMAND_VERSION      1
#define AT_COMMAND_ADDR         2


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
