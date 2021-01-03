#include "includes.h"
#include "bmp.h"

//内部变量
U8 dailyStr[] = "   2019-03-30   ",dailySet[] = "   2019-03-30   ",wr_Code[] = "    00-00-00    ";
U8 timeStr[] = "    19:19:00    ",timeSet[] = "    19:19:00    ";//,wr_Space[] = "                ",wr_Set[]="      N0.01     ";
U8 staBar[] = "R:00 M:G    B:LE";//red_w_Code[3];
short dsbDat = 0;

//外部变量
extern BLE_STATUS bleStatus;
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern SNAKE_PARAM  Snake_Param;
extern U8 DS1302_time[],SetTime[],AlmTime[];


// *****************************************************************************
// 函数名称：RCC_Configuration
// 功能描述：时钟初始化函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void RCC_Configuration(void)
{
	 //SystemInit();
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB,ENABLE);	//打开GPIOA时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	//打开USART时钟
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM2,ENABLE);    //打开定时器时钟
}

// *****************************************************************************
// 函数名称：GPIO_Configuration
// 功能描述：GPIO初始化函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
	
	//初始化LED
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
// 函数名称：GPIO_LED_Enable
// 功能描述：GPIO LED 使能
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：GPIO_LED_Disable
// 功能描述：GPIO LED 失能
// 输入参数: /
// 输出参数: /
// 返回参量: /
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
// 函数名称：SysTick05ms
// 功能描述：设置系统定时中断
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void SysTick05ms(void)
{
  SysTick_Config(SystemCoreClock / 6000);  //48M时钟 16M速度
}

// *****************************************************************************
// 函数名称：USART_Config
// 功能描述：设置串口
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void USART_Config(void)
{
   USART_InitTypeDef USART_InitStructure;
	
   USART_DeInit(USART1);
    	
   USART_InitStructure.USART_BaudRate = 9600;     //波特率9600
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
函数名称:void TIM_Config(void)
函数功能:对TIM3进行初始化
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void TIM_Config(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;	

  TIM_DeInit(TIM3);
  TIM_TimeBaseStructure.TIM_Period=(500-1);	//自动重装载值 计算方法：（重装载值/时钟预分频系数）/系统时钟
  TIM_TimeBaseStructure.TIM_Prescaler= (48-1);//时钟预分频数
  TIM_TimeBaseStructure.TIM_ClockDivision=0x0;//采样分频
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update); //清除溢出中断标志
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);


	/*====== 初始化TIM4 Channel2 PWM模式 ======*/  
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //TIM_OCPolarity_Low 输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC2
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器	

  TIM_Cmd(TIM3, ENABLE);
	
  	
	
	TIM_DeInit(TIM2);
  TIM_TimeBaseStructure.TIM_Period=(100-1);	//自动重装载值 计算方法：（重装载值/时钟预分频系数）/系统时钟
  TIM_TimeBaseStructure.TIM_Prescaler= (48000-1);//时钟预分频数
  TIM_TimeBaseStructure.TIM_ClockDivision=0x0;//采样分频
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM2, TIM_FLAG_Update); //清除溢出中断标志
  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM2, ENABLE);
}

/************************************************
函数名称:void EXIT_Config(void)
函数功能:外部中断
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void EXIT_Config(void)
{				
		GPIO_InitTypeDef GPIO_InitStruct; //GPIO 定义
  	EXTI_InitTypeDef EXTI_InitStruct; //外部中断定义
	  NVIC_InitTypeDef NVIC_InitStruct; //嵌套中断定义
	  
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
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP; // 上拉输入
		GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* EXTI line(PB3 PB4 PB5) mode config */
		EXTI_ClearITPendingBit(EXTI_Line0);	
    EXTI_ClearITPendingBit(EXTI_Line1);			
		
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
		
		EXTI_InitStruct.EXTI_Line = EXTI_Line0|EXTI_Line1;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling ;//EXTI_Trigger_Falling;//上 下降沿中断
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct); 				
}


/************************************************
函数名称:void NVIC_Configuration(void)
函数功能:中断向量表
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void NVIC_Configuration(void)     //配置中断优先级
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
函数名称:void IWDG_Init(void)
函数功能:看门狗初始化
入口参数：无
返回值：无
说明:  
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
函数名称:Device_Scan
函数功能: 设备扫描
入口参数：无
返回值：无
说明:  
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
函数名称:OLED_Start_Mov
函数功能: OLED开机动画
入口参数：无
返回值：无
说明:  
************************************************/
void OLED_Start_Mov(void)
{
	U8 i = 0;
	//清屏
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
函数名称:OLED_Display
函数功能: 显示设置
入口参数：无
返回值：无
说明:  
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

		if(SysParam.SysMode == MODE_NORMALLY)//一般模式显示
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
		else if(SysParam.SysMode == MODE_TIMESET)//时间设置模式
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
		else if(SysParam.SysMode == MODE_ALARM)//闹钟模式
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
		else if(SysParam.SysMode == MODE_SECLECT)//选择模式
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
		else if(SysParam.SysMode == MODE_CPU_DATA) //CPU温度检测
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
		else if(SysParam.SysMode == MODE_TEMP)//室温检测
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
		else if(SysParam.SysMode == MODE_GAME)//游戏模式
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
		else if(SysParam.SysMode == MODE_LOWP)//低功耗模式
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
			//显示时间日期
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
	
	//定时更新时间
	Upload_RtcTime();
}
