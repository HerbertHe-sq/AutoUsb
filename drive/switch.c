#include "includes.h"

//内部变量
U8 data[DEVICE_DATA_LEN]={0x00,0x00,0x00,0x00};
U8 SW_BTScanDat[SW_SEL_LINE]; //存储扫描数据
U8 BtnState[SW_SUM];          //按键状态表
U8 SW_Sel_Table[SW_SEL_LINE] = {0x0E,0x0D,0x0B};//选线表
U8 SetTime[8],AlmTime[8];
U8 mode_Fg = 1,sel_Fg = 0,tim_SetCnt = 1,usb_SwFg = 0;//模式标志位
U8 dis_cnt = 0;//关闭屏幕
const U8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	       //12个月份的天数


//外部变量
extern U8 swFlag,beepFlag;
extern SYSTEMSTRUCT SysParam;
extern U8 staBar[],dailyStr[],timeStr[];
extern U8 DS1302_time[];

/* ********************************************************************************************************
*	函 数 名: Delay
*	功能说明: 延时
*	形    参: 无
*	返 回 值: 无
*   备    注: 无
********************************************************************************************************* */
void SW_Delay(U8 de_Count)	 
{  
    U8 de_i;
    for(de_i=0;de_i<de_Count;de_i++);   
}

// *****************************************************************************
// 函数名称：Switch_Scan
// 功能描述：按键扫描
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
U8 Switch_Scan(void)
{
	U8 up_flag = 0,sl_i = 0;
	U16 s1_8x1 = 0x0000;
	
	static U8 s1_CountBT[SW_SEL_LINE];	           
	static U16 s1_PreBT[SW_SEL_LINE];
	if(!swFlag)
	{
		swFlag = 2;//相隔1ms扫描一次
		for(sl_i = 0;sl_i<SW_SEL_LINE;sl_i++)
		{
			//选线
			s1_8x1 = GPIOA->ODR & 0xfff0;            //清空选线
			GPIOA->ODR = s1_8x1 | SW_Sel_Table[sl_i];//轮询选线
			
			//延时
			SW_Delay(60);
			
			//读线
			s1_8x1 = GPIOB->IDR & 0x0003;
			
			if(s1_PreBT[sl_i]==s1_8x1)
			{
				if(s1_CountBT[sl_i]<0x0f)s1_CountBT[sl_i]++;
				if(s1_CountBT[sl_i]==5)
				{
					up_flag=1;
					SW_BTScanDat[sl_i] = ~(s1_8x1);
				}
			}
			else
			{
				s1_CountBT[sl_i] = 0;
				s1_PreBT[sl_i] = s1_8x1;
			}
		}		
		
	}
	return up_flag;
}

// *****************************************************************************
// 函数名称：Switch_Sfit
// 功能描述：按键处理函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Switch_Sfit(U8 flag)
{
  U8 temp_s1,s2_i,s2_j;
	U8 s2_change,s2_08x2,s2_08x3;
	static U8 s2_pre[SW_SEL_LINE];
		
	temp_s1 = flag;
	if(temp_s1 & 0x01)
	{
		for(s2_i = 0;s2_i<SW_SEL_LINE;s2_i++)
		{
			s2_change = s2_pre[s2_i] ^ SW_BTScanDat[s2_i];
			if(s2_change)
			{
				for(s2_j = 0;s2_j<2;s2_j++)
				{
					s2_08x2 = 1<<s2_j;
					//获取ID
					s2_08x3 = s2_i*2 + s2_j;
					//获取键值
					BtnState[s2_08x3] = SW_BTScanDat[s2_i] & s2_08x2;
					//赋值
					s2_pre[s2_i] ^= s2_08x2;
					
					//处理
				  Key_Process(s2_08x3);
				}
			}
		}
	}
}



// *****************************************************************************
// 函数名称：Key_Process
// 功能描述：按键程序函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Key_Process(U8 key_id)
{
	if(BtnState[key_id]!=0)
	{
		beepFlag = 100;
		
		switch(key_id)
		{
      case 0:Key_LeftEvt(); break;
			case 1:Key_UpEvt(); break;
			case 2:Key_EnterEvt();break;
			case 3:Key_RightEvt();break;
			case 4:Key_DownEvt(); break;
			case 5:Key_CacelEvt();break;	
			default:break;			
		}		
	}
}


// *****************************************************************************
// 函数名称：Key_DownEvt
// 功能描述：按键程序函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Key_DownEvt(void)
{
	//U8 i = 0;
	if(sel_Fg==1)
	{
		mode_Fg--;
		if(mode_Fg<MODE_ID_GEN)mode_Fg = MODE_ID_BTAD;
		switch(mode_Fg)
		{
			case MODE_ID_GEN:
				staBar[7] = 'G';
				staBar[8] = 'E';
				staBar[9] = 'N';
				staBar[10] = ' ';
			  break;
			case MODE_ID_TIMESET:
				staBar[7] = 'S';
				staBar[8] = 'E';
				staBar[9] = 'T';
				staBar[10] = ' ';				
			  break;
			case MODE_ID_ALMSET:				
				staBar[7] = 'A';
				staBar[8] = 'L';
				staBar[9] = 'M';
				staBar[10] = ' ';
			  break;
			case MODE_ID_CPU:
				staBar[7] = 'C';
				staBar[8] = 'P';
				staBar[9] = 'U';
				staBar[10] = ' ';
				break;
			case MODE_ID_TEMP:
				staBar[7] = 'T';
				staBar[8] = 'E';
				staBar[9] = 'M';
				staBar[10] = 'P';
				break;
			case MODE_ID_GAME:
			  staBar[7] = 'G';
				staBar[8] = 'A';
				staBar[9] = 'M';
				staBar[10] = 'E';	
			  break;
			case MODE_ID_LOWP:
				staBar[7] = 'L';
				staBar[8] = 'O';
				staBar[9] = 'W';
				staBar[10] = 'P';
        break;			
			case MODE_ID_BTRS:
			  staBar[7] = 'B';
				staBar[8] = 'T';
				staBar[9] = 'R';
				staBar[10] = 'S';		
			  break;
      case MODE_ID_BTVE:
			  staBar[7] = 'B';
				staBar[8] = 'T';
				staBar[9] = 'V';
				staBar[10] = 'E';		
			  break;
      case MODE_ID_BTAD:
			  staBar[7] = 'B';
				staBar[8] = 'T';
				staBar[9] = 'A';
				staBar[10] = 'D';		
			  break;			
		}
	}
	else
	{
		if(SysParam.SysMode == MODE_TIMESET)
		{
			switch(tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					SetTime[DS_TIME_HOUR]++;
					if(SetTime[DS_TIME_HOUR]>23)SetTime[DS_TIME_HOUR] = 0;
					break;
				case TIME_SET_ID_MIN:
					SetTime[DS_TIME_MIN]++;
					if(SetTime[DS_TIME_MIN]>59)SetTime[DS_TIME_MIN] = 0;
					break;
				case TIME_SET_ID_SEC:
					SetTime[DS_TIME_SEC]++;
					if(SetTime[DS_TIME_SEC]>59)SetTime[DS_TIME_SEC] = 0;
					break;
				case TIME_SET_ID_YEAR:
					SetTime[DS_TIME_YEAR]++;
					if(SetTime[DS_TIME_YEAR]>2099)SetTime[DS_TIME_YEAR] = 2099;
					break;
				case TIME_SET_ID_MON:
					SetTime[DS_TIME_MON]++;
					if(SetTime[DS_TIME_MON]>12)SetTime[DS_TIME_MON] = 1;
					break;
				case TIME_SET_ID_DAY:
					SetTime[DS_TIME_DAY]++;
					if(SetTime[DS_TIME_DAY]>mon_table[SetTime[DS_TIME_MON]])SetTime[DS_TIME_DAY] = 1;
					break;
			}

		}
		else if(SysParam.SysMode == MODE_ALARM)
		{
			switch(tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					AlmTime[DS_TIME_HOUR]++;
					if(AlmTime[DS_TIME_HOUR]>23)AlmTime[DS_TIME_HOUR] = 0;
					break;
				case TIME_SET_ID_MIN:
					AlmTime[DS_TIME_MIN]++;
					if(AlmTime[DS_TIME_MIN]>59)AlmTime[DS_TIME_MIN] = 0;
					break;
				case TIME_SET_ID_SEC:
					AlmTime[DS_TIME_SEC]++;
					if(AlmTime[DS_TIME_SEC]>59)AlmTime[DS_TIME_SEC] = 0;
					break;
				case TIME_SET_ID_YEAR:
					AlmTime[DS_TIME_YEAR]++;
					if(AlmTime[DS_TIME_YEAR]>2099)AlmTime[DS_TIME_YEAR] = 2099;
					break;
				case TIME_SET_ID_MON:
					AlmTime[DS_TIME_MON]++;
					if(AlmTime[DS_TIME_MON]>12)AlmTime[DS_TIME_MON] = 1;
					break;
				case TIME_SET_ID_DAY:
					AlmTime[DS_TIME_DAY]++;
					if(AlmTime[DS_TIME_DAY]>mon_table[AlmTime[DS_TIME_MON]])AlmTime[DS_TIME_DAY] = 1;
					break;
			}
		}
		else if(SysParam.SysMode == MODE_GAME)
		{
			Key_Down_Move(2);  //Down-2
		}
		else{
		
			//读取EEP
			;//FLASH_ReadNByte(data,0x00,DEVICE_DATA_LEN);
		}
	}
}

// *****************************************************************************
// 函数名称：Key_UpEvt
// 功能描述：按键程序函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Key_UpEvt(void)
{
	if(sel_Fg==1)
	{
		mode_Fg++;
		if(mode_Fg>MODE_ID_BTAD)mode_Fg = MODE_ID_GEN;
		switch(mode_Fg)
		{
			case MODE_ID_GEN:
				staBar[7] = 'G';
				staBar[8] = 'E';
				staBar[9] = 'N';
				staBar[10] = ' ';
			  break;
			case MODE_ID_TIMESET:
				staBar[7] = 'S';
				staBar[8] = 'E';
				staBar[9] = 'T';
				staBar[10] = ' ';				
			  break;
			case MODE_ID_ALMSET:				
				staBar[7] = 'A';
				staBar[8] = 'L';
				staBar[9] = 'M';
				staBar[10] = ' ';
			  break;
			case MODE_ID_CPU:
				staBar[7] = 'C';
				staBar[8] = 'P';
				staBar[9] = 'U';
				staBar[10] = ' ';
				break;
			case MODE_ID_TEMP:
				staBar[7] = 'T';
				staBar[8] = 'E';
				staBar[9] = 'M';
				staBar[10] = 'P';
				break;
			case MODE_ID_GAME:
			  staBar[7] = 'G';
				staBar[8] = 'A';
				staBar[9] = 'M';
				staBar[10] = 'E';	
			  break;
			case MODE_ID_LOWP:
				staBar[7] = 'L';
				staBar[8] = 'O';
				staBar[9] = 'W';
				staBar[10] = 'P';
        break;			
			case MODE_ID_BTRS:
			  staBar[7] = 'B';
				staBar[8] = 'T';
				staBar[9] = 'R';
				staBar[10] = 'S';		
			  break;
      case MODE_ID_BTVE:
			  staBar[7] = 'B';
				staBar[8] = 'T';
				staBar[9] = 'V';
				staBar[10] = 'E';		
			  break;
      case MODE_ID_BTAD:
			  staBar[7] = 'B';
				staBar[8] = 'T';
				staBar[9] = 'A';
				staBar[10] = 'D';		
			  break;
		}
	}
	else
	{
		if(SysParam.SysMode == MODE_TIMESET)
		{
			switch(tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					SetTime[DS_TIME_HOUR]--;
					if(SetTime[DS_TIME_HOUR]==0xff)SetTime[DS_TIME_HOUR] = 23;
					break;
				case TIME_SET_ID_MIN:
					SetTime[DS_TIME_MIN]--;
					if(SetTime[DS_TIME_MIN]==0xff)SetTime[DS_TIME_MIN] = 59;
					break;
				case TIME_SET_ID_SEC:
					SetTime[DS_TIME_SEC]--;
					if(SetTime[DS_TIME_SEC]==0xff)SetTime[DS_TIME_SEC] = 59;
					break;
				case TIME_SET_ID_YEAR:
					SetTime[DS_TIME_YEAR]--;
					if(SetTime[DS_TIME_YEAR]<DS_YEAR_START)SetTime[DS_TIME_YEAR] = DS_YEAR_START;
					break;
				case TIME_SET_ID_MON:
					SetTime[DS_TIME_MON]--;
					if(SetTime[DS_TIME_MON]<1)SetTime[DS_TIME_MON] = 12;
					break;
				case TIME_SET_ID_DAY:	
					SetTime[DS_TIME_DAY]--;
					if(SetTime[DS_TIME_DAY]<1)SetTime[DS_TIME_DAY] = mon_table[SetTime[DS_TIME_MON]-1];
					break;
			}			
		}	
		else if(SysParam.SysMode == MODE_ALARM)
		{
			switch(tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					AlmTime[TIME_SET_ID_HOUR]--;
					if(AlmTime[TIME_SET_ID_HOUR]==0xff)AlmTime[TIME_SET_ID_HOUR] = 23;
					break;
				case TIME_SET_ID_MIN:
					AlmTime[DS_TIME_MIN]--;
					if(AlmTime[DS_TIME_MIN]==0xff)AlmTime[DS_TIME_MIN] = 59;
					break;
				case TIME_SET_ID_SEC:
					AlmTime[DS_TIME_SEC]--;
					if(AlmTime[DS_TIME_SEC]==0xff)AlmTime[DS_TIME_SEC] = 59;
					break;
				case TIME_SET_ID_YEAR:
					AlmTime[DS_TIME_YEAR]--;
					if(AlmTime[DS_TIME_YEAR]<2000)AlmTime[DS_TIME_YEAR] = 2000;
					break;
				case TIME_SET_ID_MON:
					AlmTime[DS_TIME_MON]--;
					if(AlmTime[DS_TIME_MON]<1)AlmTime[DS_TIME_MON] = 12;
					break;
				case TIME_SET_ID_DAY:	
					AlmTime[DS_TIME_DAY]--;
					if(AlmTime[DS_TIME_DAY]<1)AlmTime[DS_TIME_DAY] = mon_table[AlmTime[DS_TIME_MON]-1];
					break;
			}
		}
		else if(SysParam.SysMode == MODE_GAME)
		{
			Key_Down_Move(1);  //Up-1
		}
		else{
			;
		  //Flash_WriteCmd(EEPROM_START_ARRR,(U16*)data,4);
		}		
	}
}

// *****************************************************************************
// 函数名称：Key_LeftEvt
// 功能描述：按键程序函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Key_LeftEvt(void)
{
	if(sel_Fg!=1)
	{
		tim_SetCnt--;
		if(tim_SetCnt<TIME_SET_ID_HOUR)tim_SetCnt = TIME_SET_ID_DAY;		
	}
	if(SysParam.SysMode==MODE_NORMALLY||SysParam.SysMode==MODE_CPU_DATA)
	{
		usb_SwFg++;
		SetPowerSta(usb_SwFg);
		AT24C02_WriteOneByte(SW_EEPADDR,usb_SwFg%2==0?0x00:0x01);
	}
	else if(SysParam.SysMode == MODE_GAME)
	{
		Key_Down_Move(3);  //Left-3
	}
	else{;}
}

// *****************************************************************************
// 函数名称：Key_RightEvt
// 功能描述：按键程序函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Key_RightEvt(void)
{
	if(sel_Fg==1)
	{
	  SysParam.SysMode = mode_Fg-1;
		sel_Fg = 0;
		tim_SetCnt = TIME_SET_ID_HOUR;
		if(SysParam.SysMode==MODE_TIMESET)
		{
		  ArrayCopy(DS1302_time,SetTime,8,0);
			staBar[5] = 'M';
		}
		else if(SysParam.SysMode==MODE_ALARM)
		{
		  ArrayCopy(DS1302_time,AlmTime,8,0);
			staBar[5] = 'M';
		}
		else if(SysParam.SysMode==MODE_NORMALLY)staBar[5] = 'M';
		else if(SysParam.SysMode==MODE_CPU_DATA)staBar[5] = 'M';
		else if(SysParam.SysMode==MODE_TEMP)staBar[5] = 'M';
		else if(SysParam.SysMode==MODE_GAME)
		{			
			staBar[5] = 'M';	
			Sanke_Init();
		}
		else if(SysParam.SysMode==MODE_LOWP)  //低功耗模式
		{
			staBar[5] = 'M';
			PWR_Sleep_Mode(0);
		}
		else if(SysParam.SysMode==MODE_BTRS)HC_Reset();//蓝牙重启
		else if(SysParam.SysMode==MODE_BTVE)
		{
			staBar[5] = 'M';
			HC_AT_Command(AT_COMMAND_VERSION);//蓝牙版本显示
		}
		else if(SysParam.SysMode==MODE_BTAD)
		{
			staBar[5] = 'M';
			HC_AT_Command(AT_COMMAND_ADDR);//蓝牙地址显示
		}
		else{;}
	}
	else
	{
		if(SysParam.SysMode == MODE_TIMESET || SysParam.SysMode == MODE_ALARM)
		{
			tim_SetCnt++;
			if(tim_SetCnt>TIME_SET_ID_YEAR)tim_SetCnt = TIME_SET_ID_HOUR;
		}
		else if(SysParam.SysMode == MODE_GAME)
		{
			Key_Down_Move(4);  //right-4
		}
		else{;}
	}
}

// *****************************************************************************
// 函数名称：Key_EnterEvt
// 功能描述：按键程序函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Key_EnterEvt(void)
{
	U8 temp = 0;
	if(sel_Fg!=1)
	{
		if(SysParam.SysMode == MODE_NORMALLY || SysParam.SysMode == MODE_CPU_DATA)
		{
			sel_Fg = 1;
			mode_Fg = 1;
			staBar[5] = 'S';
			SysParam.SysMode = MODE_SECLECT;
		}
		else if(SysParam.SysMode == MODE_TIMESET)
		{
			temp = SetTime[DS_TIME_YEAR];
			DS1302_SetTime((((temp/10)<<4)|(temp%10)),
			(((SetTime[DS_TIME_MON]/10))<<4|(SetTime[DS_TIME_MON]%10)),
			(((SetTime[DS_TIME_DAY]/10))<<4|(SetTime[DS_TIME_DAY]%10)),
			(((SetTime[DS_TIME_HOUR]/10))<<4|(SetTime[DS_TIME_HOUR]%10)),
			(((SetTime[DS_TIME_MIN]/10))<<4|(SetTime[DS_TIME_MIN]%10)),
			0,0);
			
			beepFlag = 200;
			SysParam.SysMode = MODE_NORMALLY;
		}
		else if(SysParam.SysMode == MODE_ALARM)
		{
			Save_ALMTime();
			beepFlag = 200;
			SysParam.SysMode = MODE_NORMALLY;
		}
		else if(SysParam.SysMode == MODE_GAME)
		{
			//重新进入游戏
			Game_Reset();
		}
		else{;}
	}
	
}

// *****************************************************************************
// 函数名称：Key_CacelEvt
// 功能描述：按键程序函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Key_CacelEvt(void)
{
	
	if(sel_Fg==1)
	{
		SysParam.SysMode = MODE_NORMALLY;
		sel_Fg = 0;
		staBar[5] = 'M';
	}
	else
	{
		if(SysParam.SysMode != MODE_NORMALLY)
		{
			if(SysParam.SysMode==MODE_GAME)
			{
				Dispose_Game();
			}
			else if(SysParam.SysMode==MODE_LOWP)
			{
				PWR_Sleep_Mode(1);			
			}
			else{;}
			OLED_Clear();
			SysParam.SysMode = MODE_NORMALLY;
			timeStr[0] = ' ';
			timeStr[1] = ' ';
			timeStr[2] = ' ';
			timeStr[3] = ' ';
			timeStr[12] = ' ';
			timeStr[13] = ' ';
			timeStr[14] = ' ';
			timeStr[15] = ' ';
			dailyStr[0] = ' ';
			dailyStr[1] = ' ';
			dailyStr[2] = ' ';
			dailyStr[13] = ' ';
			dailyStr[14] = ' ';
			dailyStr[15] = ' ';
		}
		else
		{
			//关闭屏幕			
			if(dis_cnt%2==0) OLED_Display_Off();			
			else             OLED_Display_On();
			dis_cnt++;
			
		}
	}
}

// *****************************************************************************
// 函数名称：ArrayCopy(U8 *source_buf,U8 *des_buf,U16 len,U16 start)
// 功能描述：数组复制函数
// 输入参数: source_buf被复制数组，des_buf目标数组，len长度，start起始位置
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void ArrayCopy(U8 *source_buf,U8 *des_buf,U16 len,U16 start)
{
	U8 i = 0;
	U8 *p_buf;
	p_buf = &source_buf[start];
	for(i = 0;i<len;i++)
	{
		*des_buf++ = *p_buf++;
	}
}

// *****************************************************************************
// 函数名称：Save_ALMTime
// 功能描述：保存闹钟时间
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Save_ALMTime(void)
{
	//保存闹钟时间
	AT24C02_Write(ALM_EEPADDR,AlmTime,8);
}

// *****************************************************************************
// 函数名称：ALM_Proc
// 功能描述：闹钟时间对比程序
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void ALM_Proc(void)
{
	
	static U8 temp_sw_fg = 0;
	if(SysParam.SysMode==MODE_NORMALLY)
	{
		if(DS1302_time[DS_TIME_HOUR]==AlmTime[DS_TIME_HOUR] && DS1302_time[DS_TIME_MIN]==AlmTime[DS_TIME_MIN] && DS1302_time[DS_TIME_SEC]<=20)
		{
			beepFlag = 255;
			usb_SwFg = 1;
			SetPowerSta(usb_SwFg);
			if(temp_sw_fg!=usb_SwFg)
			{
				AT24C02_WriteOneByte(SW_EEPADDR,usb_SwFg%2==0?0x00:0x01);
				temp_sw_fg = usb_SwFg;
			}
		}
  }
}

// *****************************************************************************
// 函数名称：SetPowerSta
// 功能描述：设置继电器值
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void SetPowerSta(U8 flag)
{
	if(flag%2==0)
	{
			POWER_OFF;
		  staBar[2]='0';
			staBar[3]='0';
	}
	else
	{
			POWER_ON;		
		  staBar[2]='0';
			staBar[3]='1';
	}
}

/************************************************
函数名称:void PWM_IRQHandler(void)
函数功能:对TIM3中断处理
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void PWM_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET) //溢出中断
	{
		//timerSecCount++;
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位		
	}	
}

/************************************************
函数名称:void PWR_Sleep_Mode(void)
函数功能:进入睡眠模式
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void PWR_Sleep_Mode(U8 mcu_status)
{
	if(mcu_status==0)
	{
		//睡眠模式
		ADC_Cmd(ADC1, DISABLE);	        						//失能ADC
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);	
	  ADC_DeInit(ADC1);
		
		DMA_Cmd(DMA1_Channel1,DISABLE);    //使能DMA通道1
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,DISABLE);
		DMA_DeInit(DMA1_Channel1);//设置DMA1通道1		
 
	  USART_DeInit(USART1);
	  USART_Cmd(USART1,DISABLE);                              //失能串口 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE );	
		
		
		TIM_Cmd(TIM3, DISABLE);
		TIM_Cmd(TIM2, DISABLE);	
		TIM_DeInit(TIM3);
		TIM_CtrlPWMOutputs(TIM3, DISABLE);	                    //PWM输出關
		
		GPIO_Write(GPIOA,(GPIOA->ODR|0x00B0));
		
		OLED_Display_Off();   //关闭屏幕		
		SetPowerSta(0);
		dis_cnt = 1;
 
//	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);//开电源管理时钟
//	  PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//进入停机模式		
	}
	else
	{
		//恢复正常功耗		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);
		//  System clock configuration
		RCC_Configuration();   		
		
		//USART default configration
		USART_Config();
		
		//TIM
		TIM_Config();
		
		//ADC
		Read_IVot_Init();
		
		// Nested Vector Interrupt Controller configuration
		NVIC_Configuration();
		
		OLED_Display_On();
		dis_cnt = 0;
		sel_Fg = 0;	
	}
}





