#include "includes.h"
#include "bmp.h"

//�ڲ�����
U8 dailyStr[] = "   2019-03-30   ",dailySet[] = "   2019-03-30   ",wr_Code[] = "    00-00-00    ";
U8 timeStr[] = "    19:19:00    ",timeSet[] = "    19:19:00    ";//,wr_Space[] = "                ",wr_Set[]="      N0.01     ";
U8 staBar[] = "R:00 M:G    B:LE";//red_w_Code[3];
short dsbDat = 0;

//�ⲿ����
extern BLE_STATUS bleStatus;
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern SNAKE_PARAM  Snake_Param;
extern U8 DS1302_time[],SetTime[],AlmTime[];


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
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//��USARTʱ��
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
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);  
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;    //USART1_TX USART1_RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
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
    	
   USART_InitStructure.USART_BaudRate = 9600;     //������9600
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART1, &USART_InitStructure);
  
   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //IT
   USART_Cmd(USART1,ENABLE);//USART1
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
	OLED_ShowString(0,2,(U8 *)"  AUTO USB V1.0  ",16);	
  OLED_ShowString(0,4,(U8 *)"   Zero Studio   ",16);	

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
			dailyStr[3] = ((DS1302_time[DS_TIME_YEAR]+2000)/1000)+48;
			dailyStr[4] = ((DS1302_time[DS_TIME_YEAR]+2000)/100%10)+48;
			dailyStr[5] = (((DS1302_time[DS_TIME_YEAR]+2000)/10)%10)+48;
			dailyStr[6] = ((DS1302_time[DS_TIME_YEAR]+2000)%10)+48;
			dailyStr[7] = '-';
			dailyStr[8] = (DS1302_time[DS_TIME_MON]/10)+48;
			dailyStr[9] = (DS1302_time[DS_TIME_MON]%10)+48;
			dailyStr[10] = '-';
			dailyStr[11] = (DS1302_time[DS_TIME_DAY]/10)+48;
			dailyStr[12] = (DS1302_time[DS_TIME_DAY]%10)+48;
			
			timeStr[4] = (DS1302_time[DS_TIME_HOUR]/10)+48;
			timeStr[5] = (DS1302_time[DS_TIME_HOUR]%10)+48;
			timeStr[6] = ':';
			timeStr[7] = (DS1302_time[DS_TIME_MIN]/10)+48;
			timeStr[8] = (DS1302_time[DS_TIME_MIN]%10)+48;
			timeStr[9] = ':';
			timeStr[10] = (DS1302_time[DS_TIME_SEC]/10)+48;
			timeStr[11] = (DS1302_time[DS_TIME_SEC]%10)+48;
			
			staBar[0]='R';
			staBar[2]='0';
			staBar[3]=READ_POWER_PIN+48;
			
			staBar[7] = 'G';
			staBar[8] = 'E';
			staBar[9] = 'N';
			staBar[10] = ' ';
			
			staBar[12] = 'B';		
			if(bleStatus.heartCnt>4)
			{
				staBar[14] = 'N';
				staBar[15] = 'C';
				LED2_OFF;
				bleStatus.rx_cnt = 0;
			}
			else 
			{
				staBar[14] = 'L';
				staBar[15] = 'E';
			}
			
			sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_TIMESET)//ʱ������ģʽ
		{
			cnt++;
			if(cnt>20)cnt = 0;
			if(cnt<10)
			{
				dailyStr[3] = ((SetTime[DS_TIME_YEAR]+2000)/1000)+48;
				dailyStr[4] = ((SetTime[DS_TIME_YEAR]+2000)/100%10)+48;
				dailyStr[5] = (((SetTime[DS_TIME_YEAR]+2000)/10)%10)+48;
				dailyStr[6] = ((SetTime[DS_TIME_YEAR]+2000)%10)+48;
				dailyStr[7] = '-';
				dailyStr[8] = (SetTime[DS_TIME_MON]/10)+48;
				dailyStr[9] = (SetTime[DS_TIME_MON]%10)+48;
				dailyStr[10] = '-';
				dailyStr[11] = (SetTime[DS_TIME_DAY]/10)+48;
				dailyStr[12] = (SetTime[DS_TIME_DAY]%10)+48;
				
				timeStr[4] = (SetTime[DS_TIME_HOUR]/10)+48;
				timeStr[5] = (SetTime[DS_TIME_HOUR]%10)+48;
				timeStr[6] = ':';
				timeStr[7] = (SetTime[DS_TIME_MIN]/10)+48;
				timeStr[8] = (SetTime[DS_TIME_MIN]%10)+48;
				timeStr[9] = ':';
				timeStr[10] = (SetTime[DS_TIME_SEC]/10)+48;
				timeStr[11] = (SetTime[DS_TIME_SEC]%10)+48;
				
				staBar[7] = 'S';
				staBar[8] = 'E';
				staBar[9] = 'T';
				staBar[10] = ' ';
			}
			else
			{
				if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_HOUR)      timeStr[5] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MIN) timeStr[8] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_SEC) timeStr[11] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_YEAR) dailyStr[6] = '_';				
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MON) dailyStr[9] = '_';
				else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_DAY) dailyStr[12] = '_';				
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
				dailyStr[3] = ((AlmTime[DS_TIME_YEAR]+2000)/1000)+48;
				dailyStr[4] = ((AlmTime[DS_TIME_YEAR]+2000)/100%10)+48;
				dailyStr[5] = (((AlmTime[DS_TIME_YEAR]+2000)/10)%10)+48;
				dailyStr[6] = ((AlmTime[DS_TIME_YEAR]+2000)%10)+48;
				dailyStr[7] = '-';
				dailyStr[8] = (AlmTime[DS_TIME_MON]/10)+48;
				dailyStr[9] = (AlmTime[DS_TIME_MON]%10)+48;
				dailyStr[10] = '-';
				dailyStr[11] = (AlmTime[DS_TIME_DAY]/10)+48;
				dailyStr[12] = (AlmTime[DS_TIME_DAY]%10)+48;
				
				timeStr[4] = (AlmTime[DS_TIME_HOUR]/10)+48;
				timeStr[5] = (AlmTime[DS_TIME_HOUR]%10)+48;
				timeStr[6] = ':';
				timeStr[7] = (AlmTime[DS_TIME_MIN]/10)+48;
				timeStr[8] = (AlmTime[DS_TIME_MIN]%10)+48;
				timeStr[9] = ':';
				timeStr[10] = (AlmTime[DS_TIME_SEC]/10)+48;
				timeStr[11] = (AlmTime[DS_TIME_SEC]%10)+48;
				
				staBar[7] = 'A';
				staBar[8] = 'L';
				staBar[9] = 'M';
				staBar[10] = ' ';
			}
			else
			{			
					if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_HOUR)      timeStr[5] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MIN) timeStr[8] = '_';				
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_SEC) timeStr[11] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_YEAR) dailyStr[6] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_MON) dailyStr[9] = '_';					
					else if(sysCtrlFlag.tim_SetCnt==TIME_SET_ID_DAY) dailyStr[12] = '_';					
					else{;}	
			}
			
			sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_SECLECT)//ѡ��ģʽ
		{
			dailyStr[3] = ((DS1302_time[DS_TIME_YEAR]+2000)/1000)+48;
			dailyStr[4] = ((DS1302_time[DS_TIME_YEAR]+2000)/100%10)+48;
			dailyStr[5] = (((DS1302_time[DS_TIME_YEAR]+2000)/10)%10)+48;
			dailyStr[6] = ((DS1302_time[DS_TIME_YEAR]+2000)%10)+48;
			dailyStr[7] = '-';
			dailyStr[8] = (DS1302_time[DS_TIME_MON]/10)+48;
			dailyStr[9] = (DS1302_time[DS_TIME_MON]%10)+48;
			dailyStr[10] = '-';
			dailyStr[11] = (DS1302_time[DS_TIME_DAY]/10)+48;
			dailyStr[12] = (DS1302_time[DS_TIME_DAY]%10)+48;
			
			timeStr[4] = (DS1302_time[DS_TIME_HOUR]/10)+48;
			timeStr[5] = (DS1302_time[DS_TIME_HOUR]%10)+48;
			timeStr[6] = ':';
			timeStr[7] = (DS1302_time[DS_TIME_MIN]/10)+48;
			timeStr[8] = (DS1302_time[DS_TIME_MIN]%10)+48;
			timeStr[9] = ':';
			timeStr[10] = (DS1302_time[DS_TIME_SEC]/10)+48;
			timeStr[11] = (DS1302_time[DS_TIME_SEC]%10)+48;		
			
			sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_CPU_DATA) //CPU�¶ȼ��
		{		
      cpu_vol = ADC1_Process_Pot()*1000;				
		  cpu_temp = ADC1_Process_Temp()*100;       
			
			dailyStr[3] = 'R';
			dailyStr[4] = 'E';
			dailyStr[5] = 'F';
			dailyStr[6] = ':';
			dailyStr[7] = (((U32)cpu_vol)/1000)+48;
			dailyStr[8] = '.';
			dailyStr[9] = (((U32)cpu_vol)/100%10)+48;
			dailyStr[10] = (((U32)cpu_vol)/10%10)+48;
			dailyStr[11] = (((U32)cpu_vol)%10)+48;
			dailyStr[12] = 'V';
			
			timeStr[4] = 'T';
			timeStr[5] = ':';
			timeStr[6] = (((U32)cpu_temp)/1000)+48;
			timeStr[7] = (((U32)cpu_temp)/100%10)+48;
			timeStr[8] = '.';
			timeStr[9] = (((U32)cpu_temp)/10%10)+48;
			timeStr[10] = (((U32)cpu_temp)%10)+48;
			timeStr[11] = 'C';
			
	    staBar[7] = 'C';
			staBar[8] = 'P';
			staBar[9] = 'U';
			staBar[10] = ' ';

      sup_fg = 1;			
		}
		else if(SysParam.SysMode == MODE_TEMP)//���¼��
		{			
			dailyStr[0] = 'R';
			dailyStr[1] = 'o';
			dailyStr[2] = 'o';
			dailyStr[3] = 'm';
			dailyStr[4] = ' ';
			dailyStr[5] = 'T';
			dailyStr[6] = 'e';
			dailyStr[7] = 'm';
			dailyStr[8] = 'p';
			dailyStr[9] = 'e';
			dailyStr[10] = 'r';
			dailyStr[11] = 'a';
			dailyStr[12] = 't';
			dailyStr[13] = 'u';
			dailyStr[14] = 'r';
			dailyStr[15] = 'e';
			
			timeStr[4] = 'T';
			timeStr[5] = ':';
			if(dsbDat<0)timeStr[6]='-';
			else        timeStr[6]=' ';
			timeStr[7] = (((U32)dsbDat)/100%10)+48;
			timeStr[8] = (((U32)dsbDat)/10%10)+48;
			timeStr[9] = '.';
			timeStr[10] = (((U32)dsbDat)%10)+48;
			timeStr[11] = 'C';
			
	    staBar[7] = 'T';
			staBar[8] = 'E';
			staBar[9] = 'M';
			staBar[10] = 'P';

      sup_fg = 1;
		}
		else if(SysParam.SysMode == MODE_GAME)//��Ϸģʽ
		{
			staBar[0] = 'V';
			staBar[2] = (Snake_Param.Snake_Speed/10)+48;
			staBar[3] = (Snake_Param.Snake_Speed%10)+48; 
			
			staBar[7] = 'G';
			staBar[8] = 'A';
			staBar[9] = 'M';
			staBar[10] = 'E';
			
			staBar[12] = 'S';
			staBar[14] = (Snake_Param.Game_Score/10)+48;
			staBar[15] = (Snake_Param.Game_Score%10)+48;
			
			sup_fg = 1;	
		}
		else if(SysParam.SysMode == MODE_LOWP)//�͹���ģʽ
		{
			staBar[7] = 'L';
			staBar[8] = 'O';
			staBar[9] = 'W';
			staBar[10] = 'P';
		}
		else if(SysParam.SysMode == MODE_BTVE || SysParam.SysMode == MODE_BTAD)sup_fg = 1;	
		else{;}				
			
		if(sup_fg)
		{
			//��ʾʱ������
			if(SysParam.SysMode != MODE_GAME)
			{
			   OLED_ShowString(0,2,timeStr,16);
			   OLED_ShowString(0,5,dailyStr,4);
			}
			else
			{
				Update_Map();
			}
			OLED_ShowString(0,7,staBar,4);	
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
	Upload_RtcTime();
}
