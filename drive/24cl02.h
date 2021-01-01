#ifndef __24CL02_H
#define __24CL02_H
#include "stm32f0xx.h"


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	  8191
#define AT24C128	16383
#define AT24C256	32767 
#define AT24C512	65535
//STM32F107VC开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02



void AT24C02_Init(void);
uint8_t AT24C02_ReadOneByte(uint16_t ReadAddr);
void AT24C02_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite);
void AT24C02_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len);
uint32_t AT24C02_ReadLenByte(uint16_t ReadAddr,uint8_t Len);
uint8_t AT24C02_Check(void);
void AT24C02_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);
void AT24C02_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);


#endif
