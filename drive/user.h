#ifndef _USER_H_
#define _USER_H_

#define LED1_OFF        GPIO_SetBits(GPIOA,GPIO_Pin_4)	
#define LED1_ON         GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define LED2_OFF        GPIO_SetBits(GPIOA,GPIO_Pin_5)	
#define LED2_ON         GPIO_ResetBits(GPIOA,GPIO_Pin_5)
#define POWER_ON        GPIO_SetBits(GPIOA,GPIO_Pin_8)	
#define POWER_OFF       GPIO_ResetBits(GPIOA,GPIO_Pin_8)	
#define BEEP_ON         GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define BEEP_OFF        GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define READ_POWER_PIN  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8)


void RCC_Configuration(void);
void GPIO_Configuration(void);
void GPIO_LED3_Config(void);
void SysTick05ms(void);
void EXIT_Config(void);
void TIM_Config(void);
void USART_Config(void);
void NVIC_Configuration(void);
void IWDG_Init(void);
void Device_Scan(void);
void OLED_Start_Mov(void);
void OLED_Display(void);
	
#endif

