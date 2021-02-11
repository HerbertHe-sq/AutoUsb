#ifndef _ITEMP_H_
#define _ITEMP_H_

#include "mydefine.h"

#define ADC1_DR_Address ((U32)0x40012440) //…Ë÷√ADCµƒµÿ÷∑
#define CHANNEL   2
#define CNT       1

typedef enum
{
	ADC_FUNC_ITEM_VOT=0,
	ADC_FUNC_ITEM_TEMP,
	ADC_FUNC_ITEM_NUM
}adc_func_t;


void Read_IVot_Init(void);
void ADCTemp_Config(void);
U16 T_Get_Adc(void);  
void DMA1_Config(void);
void DMA_Interrupt(void);
float ADC1_Process_Temp(void);
float ADC1_Process_Pot(void);


#endif
