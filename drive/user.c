#include "includes.h"
#include "bmp.h"

//内部变量
typedef void(*update_func_cbk)(void);
typedef float(*adc_func_cbk)(void);

short dsbDat = 0;
OLED_STR_T ole_str = {"    19:19:00    ","   2019-03-30   ","R:00 M:G    B:LE"};

//刷新屏幕函数
update_func_cbk update_func[UPDATE_FUNC_ITEM_NUM] = {
 Update_Map,
 Upload_RtcTime,
 Usart_Tx_Data
};

//ADC获取定义
adc_func_cbk adc_func[ADC_FUNC_ITEM_NUM]=
{
	ADC1_Process_Pot,
	ADC1_Process_Temp
};

//外部变量
extern BLE_STATUS bleStatus;
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern SNAKE_PARAM  Snake_Param;
extern ds_set_time_t ds_set_time;
extern ds_set_time_t ds_alm_time; 
extern U8 DS1302_time[];


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
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_TIM16,ENABLE);	//打开USART时钟
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
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;    //USART1_TX USART1_RX
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; 
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_Level_3;
  GPIO_Init(GPIOA, &GPIO_InitStruct);	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); 
	
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
    	
   USART_InitStructure.USART_BaudRate = 115200;     //波特率9600
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
   
   DMA_InitStructure.DMA_BufferSize = 16; // 缓存大小
   DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;    // 内存到内存关闭
   DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;   // 普通模式
   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // 内存到外设
   DMA_InitStructure.DMA_Priority = DMA_Priority_High; // DMA通道优先级
   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;// 内存地址递增
   DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)&USART1->TDR;   // 外设地址   
   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// 外设地址不变
   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据长度
   DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&uart_tx_buf;   // 定义内存基地址
   DMA_InitStructure.DMA_PeripheralDataSize =DMA_PeripheralDataSize_Byte;//外设数据长度
   
   DMA_Init(DMA1_Channel2,&DMA_InitStructure);
   DMA_ClearITPendingBit(DMA1_IT_TC2); // 清除一次DMA中断标志
   DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);// 使能DMA传输完成中断
   
   DMA_Cmd(DMA1_Channel2,ENABLE);
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
	
	TIM_DeInit(TIM16);
  TIM_TimeBaseStructure.TIM_Period=(1000-1);	//自动重装载值 计算方法：（重装载值/时钟预分频系数）/系统时钟
  TIM_TimeBaseStructure.TIM_Prescaler= (48-1);//时钟预分频数
  TIM_TimeBaseStructure.TIM_ClockDivision=0x0;//采样分频
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_ClearFlag(TIM16, TIM_FLAG_Update); //清除溢出中断标志
  TIM_ITConfig(TIM16,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM16, ENABLE);
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
		else if(SysParam.SysMode == MODE_TIMESET)//时间设置模式
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
		else if(SysParam.SysMode == MODE_ALARM)//闹钟模式
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
		else if(SysParam.SysMode == MODE_SECLECT)//选择模式
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
		else if(SysParam.SysMode == MODE_CPU_DATA) //CPU温度检测
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
		else if(SysParam.SysMode == MODE_TEMP)//室温检测
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
		else if(SysParam.SysMode == MODE_GAME)//游戏模式
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
		else if(SysParam.SysMode == MODE_LOWP)//低功耗模式
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
			//显示时间日期
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
	
	//定时更新时间
	update_func[UPDATE_FUNC_RTCTIME]();
}
