#ifndef _DS18B20_H
#define _DS18B20_H

#define DSBDQ_IN   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)

void DSB_IO_IN(void);
void DSB_IO_OUT(void);
void DSB_SetIO(U8 status);
U8 DSB_Init(void);
void DSB_Reset(void);
U8 DSB_Check(void);
U8 DSB_ReadBit(void);
U8 DSB_ReadByte(void);
void DSB_WriteByte(U8 dat);
void DSB_Start(void);
short DSB_GetTemp(void);


#endif
