#include "includes.h"
#include "bmp.h"

//�ڲ�����
typedef void(*update_func_cbk)(void);
typedef float(*adc_func_cbk)(void);

short dsbDat = 0;
OLED_STR_T ole_str = {"    19:19:00    ","   2019-03-30   ","R:00 M:G    B:LE"};

//ˢ����Ļ����
update_func_cbk update_func[UPDATE_FUNC_ITEM_NUM] = {
 Update_Map,
 Upload_RtcTime,
 Usart_Tx_Data
};

//ADC��ȡ����
adc_func_cbk adc_func[ADC_FUNC_ITEM_NUM]=
{
	ADC1_Process_Pot,
	ADC1_Process_Temp
};

//�ⲿ����
extern BLE_STATUS bleStatus;
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern SNAKE_PARAM  Snake_Param;
extern ds_set_time_t ds_set_time;
extern ds_set_time_t ds_alm_time; 
extern U8 DS1302_time[];


// *****************************************************************************
// �������ƣ�RCC_Configuration
// ����������ʱ�ӳ�ʼ������
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void RCC_Configuration(void)
{
	 //SystemInit();
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB,ENABLE);	//��GPIOAʱ��
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_TIM16,ENABLE);	//��USARTʱ��
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM2,ENABLE);    //�򿪶�ʱ��ʱ��
}

// *****************************************************************************
// �������ƣ�GPIO_Configuration
// ����������GPIO��ʼ������
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct); 		
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;  //SW-SEL
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;  //SW-RD
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_1);	 
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;    //USART1_TX USART1_RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); 
	
	//��ʼ��LED
	GPIO_Write(GPIOA,(GPIOA->ODR|0x00B0));

}

void GPIO_LED3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// *****************************************************************************
// �������ƣ�GPIO_LED_Enable
// ����������GPIO LED ʹ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void GPIO_LED_Enable(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct); 

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);	
}

// *****************************************************************************
// �������ƣ�GPIO_LED_Disable
// ����������GPIO LED ʧ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void GPIO_LED_Disable(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct); 

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}


// *****************************************************************************
// �������ƣ�SysTick05ms
// ��������������ϵͳ��ʱ�ж�
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void SysTick05ms(void)
{
  SysTick_Config(SystemCoreClock / 6000);  //48Mʱ�� 16M�ٶ�
}

// *****************************************************************************
// �������ƣ�USART_Config
// �������������ô���
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void USART_Config(void)
{
   USART_InitTypeDef USART_InitStructure;
	
   USART_DeInit(USART1);
    	
   USART_InitStructure.USART_BaudRate = 115200;     //������9600
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART1, &USART_InitStructure);
  
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //IT
	 USART_ClearFlag(USART1,USART_FLAG_TC);
   USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
   USART_Cmd(USART1,ENABLE);//USART1 
}

void DMA1_Init(void)
{
   DMA_InitTypeDef DMA_InitStructure;
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
   
   DMA_InitStructure.DMA_BufferSize = 16; // �����С
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;    // �ڴ浽�ڴ�ر�
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;   // ��ͨģʽ
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // �ڴ浽����
   DMA_InitStructure.DMA_Priority = DMA_Priority_High; // DMAͨ�����ȼ�
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;// �ڴ��ַ����
   DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&USART1->TDR;   // �����ַ   
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// �����ַ����
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // �ڴ����ݳ���
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&uart_tx_buf;   // �����ڴ����ַ
   DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//�������ݳ���
   
   DMA_Init(DMA1_Channel2,&DMA_InitStructure);
   DMA_ClearITPendingBit(DMA1_IT_TC2); // ���һ��DMA�жϱ�־
   DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);// ʹ��DMA��������ж�
   
   DMA_Cmd(DMA1_Channel2,ENABLE);
}


/************************************************
��������:void TIM_Config(void)
��������:��TIM3���г�ʼ��
��ڲ�������
����ֵ����
˵��:  ���ںͷ�Ƶ�Ⱦ�����ʱ��ʱ��
************************************************/
void TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;	

  TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period=(500-1);	//�Զ���װ��ֵ ���㷽��������װ��ֵ/ʱ��Ԥ��Ƶϵ����/ϵͳʱ��
  TIM_TimeBaseStructure.TIM_Prescaler= (48-1);//ʱ��Ԥ��Ƶ��
  TIM_TimeBaseStructure.TIM_ClockDivision=0x0;//������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); //�������жϱ�־
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);


	/*====== ��ʼ��TIM4 Channel2 PWMģʽ ======*/  
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //TIM_OCPolarity_Low �������:TIM����Ƚϼ��Ե�
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC2
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM4��CCR2�ϵ�Ԥװ�ؼĴ���	

  TIM_Cmd(TIM3, ENABLE);
	
  	
	
	TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period=(100-1);	//�Զ���װ��ֵ ���㷽��������װ��ֵ/ʱ��Ԥ��Ƶϵ����/ϵͳʱ��
  TIM_TimeBaseStructure.TIM_Prescaler= (48000-1);//ʱ��Ԥ��Ƶ��
  TIM_TimeBaseStructure.TIM_ClockDivision=0x0;//������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM2, TIM_FLAG_Update); //�������жϱ�־
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2, ENABLE);
	
	TIM_DeInit(TIM16);
  TIM_TimeBaseStructure.TIM_Period=(1000-1);	//�Զ���װ��ֵ ���㷽��������װ��ֵ/ʱ��Ԥ��Ƶϵ����/ϵͳʱ��
  TIM_TimeBaseStructure.TIM_Prescaler= (48-1);//ʱ��Ԥ��Ƶ��
  TIM_TimeBaseStructure.TIM_ClockDivision=0x0;//������Ƶ
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM16, TIM_FLAG_Update); //�������жϱ�־
  TIM_ITConfig(TIM16,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM16, ENABLE);
}

/************************************************
��������:void EXIT_Config(void)
��������:�ⲿ�ж�
��ڲ�������
����ֵ����
˵��:  ���ںͷ�Ƶ�Ⱦ�����ʱ��ʱ��
************************************************/
void EXIT_Config(void)
{				
		GPIO_InitTypeDef GPIO_InitStruct; //GPIO ����
  	EXTI_InitTypeDef EXTI_InitStruct; //�ⲿ�ж϶���
	  NVIC_InitTypeDef NVIC_InitStruct; //Ƕ���ж϶���
	  
	  /* config the extiline(PB3 PB4 PB5) clock and AFIO clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	  
    /* Configyre P[A|B|C|D|E]3  NIVC  */
		NVIC_InitStruct.NVIC_IRQChannel = EXTI0_1_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);
			
	  	/* EXTI line gpio config(PB3 PB4 PB5) */	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1; 
	  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP; // ��������
		GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* EXTI line(PB3 PB4 PB5) mode config */
		EXTI_ClearITPendingBit(EXTI_Line0);	
    EXTI_ClearITPendingBit(EXTI_Line1);			
		
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
		
		EXTI_InitStruct.EXTI_Line = EXTI_Line0|EXTI_Line1;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling ;//EXTI_Trigger_Falling;//�� �½����ж�
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct); 				
}


/************************************************
��������:void NVIC_Configuration(void)
��������:�ж�������
��ڲ�������
����ֵ����
˵��:  ���ںͷ�Ƶ�Ⱦ�����ʱ��ʱ��
************************************************/
void NVIC_Configuration(void)     //�����ж����ȼ�
{
  NVIC_InitTypeDef NVIC_InitStructure; 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStructure);		
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd =ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_Init(&NVIC_InitStructure);	
}

/************************************************
��������:void IWDG_Init(void)
��������:���Ź���ʼ��
��ڲ�������
����ֵ����
˵��:  
************************************************/
void IWDG_Init(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_8);
	IWDG_SetReload(4000);        //800ms
	IWDG_ReloadCounter();
	IWDG_Enable();
}

/************************************************
��������:Device_Scan
��������: �豸ɨ��
��ڲ�������
����ֵ����
˵��:  
************************************************/
void Device_Scan(void)
{
	U8 key_flag = 0;
		
	//SW Scan
	key_flag = Switch_Scan();
	if(key_flag)Switch_Sfit(key_flag);
	
	//USART TX
	update_func[UPDATE_FUNC_UART_TX]();
	
	//USART Recvice
	key_flag = USART_Unpack();
	if(key_flag)USART_Deal(key_flag);
	
	//ADC Convert
	if(!sysCtrlFlag.adcFlag)
	{
		sysCtrlFlag.adcFlag = 2000;
		DMA_Interrupt();	
    dsbDat = DSB_GetTemp();		
	}
	
	//Get Time
	if(!sysCtrlFlag.upTimeFg)
	{
		sysCtrlFlag.upTimeFg = 100;
	  DS1302_GetTime();	
		ALM_Proc();
	}
	
	//Beep
	if(sysCtrlFlag.beepFlag)BEEP_ON;
	else BEEP_OFF;
	
	//Signal LED
	Signal_LED();
}


/************************************************
��������:OLED_Start_Mov
��������: OLED��������
��ڲ�������
����ֵ����
˵��:  
************************************************/
void OLED_Start_Mov(void)
{
	U8 i = 0;
	//����
  OLED_Clear();
	OLED_ShowString(0,2,(U8 *)"  AUTO USB V1.0  ",16,16);	
  OLED_ShowString(0,4,(U8 *)"   Zero Studio   ",16,16);	

  for(i = 0;i<15;i++)
	{
		if(i%3==0)
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_4);
			delay_ms(50);
			GPIO_SetBits(GPIOA,GPIO_Pin_4);
		}
		else if(i%3==1)
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_5);
			delay_ms(50);
			GPIO_SetBits(GPIOA,GPIO_Pin_5);
		}
		else if(i%3==2)
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			delay_ms(50);
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
		}
		else{;}
		delay_ms(30);
	}	
}

/************************************************
��������:OLED_Display
��������: ��ʾ����
��ڲ�������
����ֵ����
˵��:  
************************************************/
void OLED_Display(void)
{
	U8 update_fg = 0,sup_fg = 0,pw_i = 0;
	static float cpu_temp,cpu_vol;
	static U8 cnt = 0,pw_cnt = 0;
	static U16 pulse = 0,direct_cnt = 0;
	if(sysCtrlFlag.pwFlag<=25000)
	{
		if(sysCtrlFlag.pwFlag>(pw_cnt*5000) && sysCtrlFlag.pwFlag<((pw_cnt*5000)+200))
		{
			for(pw_i=0;pw_i<4;pw_i++)
			{
				if(pw_i<pw_cnt)
				{
					Power_BMP[124-(pw_i*3)] = 0xba;
					Power_BMP[123-(pw_i*3)] = 0xba;
				}
				else
				{
					Power_BMP[124-(pw_i*3)] = 0x82;
					Power_BMP[123-(pw_i*3)] = 0x82;
				}
			}
			pw_cnt++;
			if(pw_cnt>4)pw_cnt = 0;
			update_fg = 1;
		}
		if(update_fg)
		{
			update_fg = 0;
			OLED_DrawBMP(0,0,128,2,Power_BMP);
		}
  }
  else sysCtrlFlag.pwFlag = 0;
	
	if(!sysCtrlFlag.timSFlag)
	{
		sysCtrlFlag.timSFlag = 250;

		if(SysParam.SysMode == MODE_NORMALLY)//һ��ģʽ��ʾ
		{		
			ole_str.oled_str_daily[3] = ((DS1302_time[DS_TIME_YEAR]+2000)/1000)+48;
			ole_str.oled_str_daily[4] = ((DS1302_time[DS_TIME_YEAR]+2000)/100%10)+48;
			ole_str.oled_str_daily[5] = (((DS1302_time[DS_TIME_YEAR]+2000)/10)%10)+48;
			ole_str.oled_str_daily[6] = ((DS1302_time[DS_TIME_YEAR]+2000)%10)+48;
			ole_str.oled_str_daily[7] = '-';
			ole_str.oled_str_daily[8] = (DS1302_time[DS_TIME_MON]/10)+48;
			ole_str.oled_str_daily[9] = (DS1302_time[DS_TIME_MON]%10)+48;
			ole_str.oled_str_daily[10] = '-';
			ole_str.oled_str_daily[11] = (DS1302_time[DS_TIME_DAY]/10)+48;
			ole_str.oled_str_daily[12] = (DS1302_time[DS_TIME_DAY]%10)+48;
			
			ole_str.oled_str_time[4] = (DS1302_time[DS_TIME_HOUR]/10)+48;
			ole_str.oled_str_time[5] = (DS1302_time[DS_TIME_HOUR]%10)+48;
			ole_str.oled_str_time[6] = ':';
			ole_str.oled_str_time[7] = (DS1302_time[DS_TIME_MIN]/10)+48;
			ole_str.oled_str_time[8] = (DS1302_time[DS_TIME_MIN]%10)+48;
			ole_str.oled_str_time[9] = ':';
			ole_str.oled_str_time[10] = (DS1302_time[DS_TIME_SEC]/10)+48;
			ole_str.oled_str_time[11] = (DS1302_time[DS_TIME_SEC]%10)+48;
			

			
			ole_str.oled_str_sbar[0]='R';
			ole_str.oled_str_sbar[2]='0';
			ole_str.oled_str_sbar[3]=READ_POWER_PIN+48;
			
			ole_str.oled_str_sbar[7] = 'G';
			ole_str.oled_str_sbar[8] = 'E';
			ole_str.oled_str_sbar[9] = 'N';
			ole_str.oled_str_sbar[10] = ' ';
			
			ole_str.oled_str_sbar[12] = 'B';		
			if(bleStatus.heartCnt>4)
			{
				ole_str.oled_str_sbar[14] = 'N';
				ole_str.oled_str_sbar[15] = 'C';
				LED2_OFF;
				bleStatus.rx_cnt = 0;
			}
			else 
			{
				ole_str.oled_str_sbar[14] = 'L';
				ole_str.oled_str_sbar[15] = 'E';
			}						
			sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_TIMESET)//ʱ������ģʽ
		{
			cnt++;
			if(cnt>20)cnt = 0;
			if(cnt<10)
			{
				ole_str.oled_str_daily[3] = ((ds_set_time.Year)/1000)+48;
				ole_str.oled_str_daily[4] = ((ds_set_time.Year)/100%10)+48;
				ole_str.oled_str_daily[5] = (((ds_set_time.Year)/10)%10)+48;
				ole_str.oled_str_daily[6] = ((ds_set_time.Year)%10)+48;
				ole_str.oled_str_daily[7] = '-';
				ole_str.oled_str_daily[8] = (ds_set_time.ds_time[DS_TIME_MON]/10)+48;
				ole_str.oled_str_daily[9] = (ds_set_time.ds_time[DS_TIME_MON]%10)+48;
				ole_str.oled_str_daily[10] = '-';
				ole_str.oled_str_daily[11] = (ds_set_time.ds_time[DS_TIME_DAY]/10)+48;
				ole_str.oled_str_daily[12] = (ds_set_time.ds_time[DS_TIME_DAY]%10)+48;
				
				ole_str.oled_str_time[4] = (ds_set_time.ds_time[DS_TIME_HOUR]/10)+48;
				ole_str.oled_str_time[5] = (ds_set_time.ds_time[DS_TIME_HOUR]%10)+48;
				ole_str.oled_str_time[6] = ':';
				ole_str.oled_str_time[7] = (ds_set_time.ds_time[DS_TIME_MIN]/10)+48;
				ole_str.oled_str_time[8] = (ds_set_time.ds_time[DS_TIME_MIN]%10)+48;
				ole_str.oled_str_time[9] = ':';
				ole_str.oled_str_time[10] = (ds_set_time.ds_time[DS_TIME_SEC]/10)+48;
				ole_str.oled_str_time[11] = (ds_set_time.ds_time[DS_TIME_SEC]%10)+48;
				
				ole_str.oled_str_sbar[7] = 'S';
				ole_str.oled_str_sbar[8] = 'E';
				ole_str.oled_str_sbar[9] = 'T';
				ole_str.oled_str_sbar[10] = ' ';
			}
			else
			{
				if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_HOUR)     ole_str.oled_str_time[5] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MIN) ole_str.oled_str_time[8] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_SEC) ole_str.oled_str_time[11] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_YEAR) ole_str.oled_str_daily[6] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MON) ole_str.oled_str_daily[9] = '_';
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_DAY) ole_str.oled_str_daily[12] = '_';				
				else{;}				
			}
			
			sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_ALARM)//����ģʽ
		{
			cnt++;
			if(cnt>20)cnt = 0;
			if(cnt<10)
			{
				ole_str.oled_str_daily[3] = ((ds_alm_time.ds_time[DS_TIME_YEAR]+2000)/1000)+48;
				ole_str.oled_str_daily[4] = ((ds_alm_time.ds_time[DS_TIME_YEAR]+2000)/100%10)+48;
				ole_str.oled_str_daily[5] = (((ds_alm_time.ds_time[DS_TIME_YEAR]+2000)/10)%10)+48;
				ole_str.oled_str_daily[6] = ((ds_alm_time.ds_time[DS_TIME_YEAR]+2000)%10)+48;
				ole_str.oled_str_daily[7] = '-';
				ole_str.oled_str_daily[8] = (ds_alm_time.ds_time[DS_TIME_MON]/10)+48;
				ole_str.oled_str_daily[9] = (ds_alm_time.ds_time[DS_TIME_MON]%10)+48;
				ole_str.oled_str_daily[10] = '-';
				ole_str.oled_str_daily[11] = (ds_alm_time.ds_time[DS_TIME_DAY]/10)+48;
				ole_str.oled_str_daily[12] = (ds_alm_time.ds_time[DS_TIME_DAY]%10)+48;
				
				ole_str.oled_str_time[4] = (ds_alm_time.ds_time[DS_TIME_HOUR]/10)+48;
				ole_str.oled_str_time[5] = (ds_alm_time.ds_time[DS_TIME_HOUR]%10)+48;
				ole_str.oled_str_time[6] = ':';
				ole_str.oled_str_time[7] = (ds_alm_time.ds_time[DS_TIME_MIN]/10)+48;
				ole_str.oled_str_time[8] = (ds_alm_time.ds_time[DS_TIME_MIN]%10)+48;
				ole_str.oled_str_time[9] = ':';
				ole_str.oled_str_time[10] = (ds_alm_time.ds_time[DS_TIME_SEC]/10)+48;
				ole_str.oled_str_time[11] = (ds_alm_time.ds_time[DS_TIME_SEC]%10)+48;
				
				ole_str.oled_str_sbar[7] = 'A';
				ole_str.oled_str_sbar[8] = 'L';
				ole_str.oled_str_sbar[9] = 'M';
				ole_str.oled_str_sbar[10] = ' ';
			}
			else
			{			
					if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_HOUR)      ole_str.oled_str_time[5] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MIN)  ole_str.oled_str_time[8] = '_';				
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_SEC)  ole_str.oled_str_time[11] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_YEAR) ole_str.oled_str_daily[6] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MON)  ole_str.oled_str_daily[9] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_DAY)  ole_str.oled_str_daily[12] = '_';					
					else{;}	
			}
			
			sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_SECLECT)//ѡ��ģʽ
		{
			ole_str.oled_str_daily[3] = ((DS1302_time[DS_TIME_YEAR]+2000)/1000)+48;
			ole_str.oled_str_daily[4] = ((DS1302_time[DS_TIME_YEAR]+2000)/100%10)+48;
			ole_str.oled_str_daily[5] = (((DS1302_time[DS_TIME_YEAR]+2000)/10)%10)+48;
			ole_str.oled_str_daily[6] = ((DS1302_time[DS_TIME_YEAR]+2000)%10)+48;
			ole_str.oled_str_daily[7] = '-';
			ole_str.oled_str_daily[8] = (DS1302_time[DS_TIME_MON]/10)+48;
			ole_str.oled_str_daily[9] = (DS1302_time[DS_TIME_MON]%10)+48;
			ole_str.oled_str_daily[10] = '-';
			ole_str.oled_str_daily[11] = (DS1302_time[DS_TIME_DAY]/10)+48;
			ole_str.oled_str_daily[12] = (DS1302_time[DS_TIME_DAY]%10)+48;
			
			ole_str.oled_str_time[4] = (DS1302_time[DS_TIME_HOUR]/10)+48;
			ole_str.oled_str_time[5] = (DS1302_time[DS_TIME_HOUR]%10)+48;
			ole_str.oled_str_time[6] = ':';
			ole_str.oled_str_time[7] = (DS1302_time[DS_TIME_MIN]/10)+48;
			ole_str.oled_str_time[8] = (DS1302_time[DS_TIME_MIN]%10)+48;
			ole_str.oled_str_time[9] = ':';
			ole_str.oled_str_time[10] = (DS1302_time[DS_TIME_SEC]/10)+48;
			ole_str.oled_str_time[11] = (DS1302_time[DS_TIME_SEC]%10)+48;		
			
			sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_CPU_DATA) //CPU�¶ȼ��
		{		
      cpu_vol = adc_func[ADC_FUNC_ITEM_VOT]()*1000;				
		  cpu_temp = adc_func[ADC_FUNC_ITEM_TEMP]()*100;       
			
			ole_str.oled_str_daily[3] = 'R';
			ole_str.oled_str_daily[4] = 'E';
			ole_str.oled_str_daily[5] = 'F';
			ole_str.oled_str_daily[6] = ':';
			ole_str.oled_str_daily[7] = (((U32)cpu_vol)/1000)+48;
			ole_str.oled_str_daily[8] = '.';
			ole_str.oled_str_daily[9] = (((U32)cpu_vol)/100%10)+48;
			ole_str.oled_str_daily[10] = (((U32)cpu_vol)/10%10)+48;
			ole_str.oled_str_daily[11] = (((U32)cpu_vol)%10)+48;
			ole_str.oled_str_daily[12] = 'V';
			
			ole_str.oled_str_time[4] = 'T';
			ole_str.oled_str_time[5] = ':';
			ole_str.oled_str_time[6] = (((U32)cpu_temp)/1000)+48;
			ole_str.oled_str_time[7] = (((U32)cpu_temp)/100%10)+48;
			ole_str.oled_str_time[8] = '.';
			ole_str.oled_str_time[9] = (((U32)cpu_temp)/10%10)+48;
			ole_str.oled_str_time[10] = (((U32)cpu_temp)%10)+48;
			ole_str.oled_str_time[11] = 'C';
			
	    ole_str.oled_str_sbar[7] = 'C';
			ole_str.oled_str_sbar[8] = 'P';
			ole_str.oled_str_sbar[9] = 'U';
			ole_str.oled_str_sbar[10] = ' ';

      sup_fg = 1;			
		}
		else if(SysParam.SysMode == MODE_TEMP)//���¼��
		{		
      strcpy((char *)ole_str.oled_str_daily,"Room Temperature");			
			
			ole_str.oled_str_time[4] = 'T';
			ole_str.oled_str_time[5] = ':';
			if(dsbDat<0)ole_str.oled_str_time[6]='-';
			else        ole_str.oled_str_time[6]=' ';
			ole_str.oled_str_time[7] = (((U32)dsbDat)/100%10)+48;
			ole_str.oled_str_time[8] = (((U32)dsbDat)/10%10)+48;
			ole_str.oled_str_time[9] = '.';
			ole_str.oled_str_time[10] = (((U32)dsbDat)%10)+48;
			ole_str.oled_str_time[11] = 'C';
			
	    ole_str.oled_str_sbar[7] = 'T';
			ole_str.oled_str_sbar[8] = 'E';
			ole_str.oled_str_sbar[9] = 'M';
			ole_str.oled_str_sbar[10] = 'P';

      sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_GAME)//��Ϸģʽ
		{
			ole_str.oled_str_sbar[0] = 'V';
			ole_str.oled_str_sbar[2] = (Snake_Param.Snake_Speed/10)+48;
			ole_str.oled_str_sbar[3] = (Snake_Param.Snake_Speed%10)+48; 
			
			ole_str.oled_str_sbar[7] = 'G';
			ole_str.oled_str_sbar[8] = 'A';
			ole_str.oled_str_sbar[9] = 'M';
			ole_str.oled_str_sbar[10] = 'E';
			
			ole_str.oled_str_sbar[12] = 'S';
			ole_str.oled_str_sbar[14] = (Snake_Param.Game_Score/10)+48;
			ole_str.oled_str_sbar[15] = (Snake_Param.Game_Score%10)+48;
			
			sup_fg = 1;	
		}
		else if(SysParam.SysMode == MODE_LOWP)//�͹���ģʽ
		{
			ole_str.oled_str_sbar[7] = 'L';
			ole_str.oled_str_sbar[8] = 'O';
			ole_str.oled_str_sbar[9] = 'W';
			ole_str.oled_str_sbar[10] = 'P';
		}
		else if(SysParam.SysMode == MODE_BTVE || SysParam.SysMode == MODE_BTAD)sup_fg = 1;	
		else{;}				
			
		if(sup_fg)
		{
			//��ʾʱ������
			if(SysParam.SysMode != MODE_GAME)
			{
			   OLED_ShowString(0,2,ole_str.oled_str_time,16,sizeof(ole_str.oled_str_time));
			   OLED_ShowString(0,5,ole_str.oled_str_daily,4,sizeof(ole_str.oled_str_daily));
			}
			else
			{
				//Update Map
				update_func[UPDATE_FUNC_MAP]();
			}
			OLED_ShowString(0,7,ole_str.oled_str_sbar,4,sizeof(ole_str.oled_str_sbar));	
			sup_fg = 0;				
		}
	}
	if(!sysCtrlFlag.timLedFlag)
	{
		sysCtrlFlag.timLedFlag = 10;
		//PWM
    if(direct_cnt==0)	
		{
			pulse++;
			if(pulse>=500)
			{
				direct_cnt = 1;
				pulse = 500;
			}
		}	
		else
		{
			pulse--;
			if(pulse<=0)
			{
				direct_cnt = 0;
				pulse = 0;
			}
		}								
		TIM_SetCompare2(TIM3,pulse);	
	}
	
	//��ʱ����ʱ��
	update_func[UPDATE_FUNC_RTCTIME]();
}
