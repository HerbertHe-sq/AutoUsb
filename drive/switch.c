#include "includes.h"

//内部变量
//U8 data[DEVICE_DATA_LEN]={0x00,0x00,0x00,0x00};
U8 SW_BTScanDat[SW_SEL_LINE]; //存储扫描数据
U8 BtnState[SW_SUM];          //按键状态表
U8 SW_Sel_Table[SW_SEL_LINE] = {0x0E,0x0D,0x0B};//选线表
const U8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	       //12个月份的天数

ds_set_time_t ds_set_time={0U,{0U}};
ds_set_time_t ds_alm_time={0U,{0U}};
select_flag_t sw_flag={1,0};

key_func_game_t key_game_cbk = Key_Down_Move;   //游戏按钮
key_func_cbk Key_Funcbk[KEY_FUNC_ITEM_NUM] = {
	Key_LeftEvt, 
  Key_UpEvt,
	Key_EnterEvt, 
  Key_RightEvt,
  Key_DownEvt,
  Key_CacelEvt
};

//外部变量
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern SYSTEMSTRUCT SysParam;
extern OLED_STR_T ole_str;
extern U8 DS1302_time[];

/* ********************************************************************************************************
*	函 数 名: Delay
*	功能说明: 延时
*	形    参: 无
*	返 回 值: 无
* 备    注: 无
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
	if(!sysCtrlFlag.swFlag)
	{
		sysCtrlFlag.swFlag = 2;//相隔1ms扫描一次
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
		sysCtrlFlag.beepFlag = 100;		
		if(key_id<KEY_FUNC_ITEM_NUM)
		{
			Key_Funcbk[key_id]();
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
	if(sw_flag.sel_Fg==1)
	{
		sw_flag.mode_Fg--;
		if(sw_flag.mode_Fg<MODE_ID_GEN)sw_flag.mode_Fg = MODE_ID_BTAD;
		switch(sw_flag.mode_Fg)
		{
			case MODE_ID_GEN:
				//strcpy((char*)&ole_str.oled_str_sbar[7],"GEN ");
				ole_str.oled_str_sbar[7] = 'G';
				ole_str.oled_str_sbar[8] = 'E';
				ole_str.oled_str_sbar[9] = 'N';
				ole_str.oled_str_sbar[10] = ' ';
			  break;
			case MODE_ID_TIMESET:
				ole_str.oled_str_sbar[7] = 'S';
				ole_str.oled_str_sbar[8] = 'E';
				ole_str.oled_str_sbar[9] = 'T';
				ole_str.oled_str_sbar[10] = ' ';				
			  break;
			case MODE_ID_ALMSET:				
				ole_str.oled_str_sbar[7] = 'A';
				ole_str.oled_str_sbar[8] = 'L';
				ole_str.oled_str_sbar[9] = 'M';
				ole_str.oled_str_sbar[10] = ' ';
			  break;
			case MODE_ID_CPU:
				ole_str.oled_str_sbar[7] = 'C';
				ole_str.oled_str_sbar[8] = 'P';
				ole_str.oled_str_sbar[9] = 'U';
				ole_str.oled_str_sbar[10] = ' ';
				break;
			case MODE_ID_TEMP:
				ole_str.oled_str_sbar[7] = 'T';
				ole_str.oled_str_sbar[8] = 'E';
				ole_str.oled_str_sbar[9] = 'M';
				ole_str.oled_str_sbar[10] = 'P';
				break;
			case MODE_ID_GAME:
			  ole_str.oled_str_sbar[7] = 'G';
				ole_str.oled_str_sbar[8] = 'A';
				ole_str.oled_str_sbar[9] = 'M';
				ole_str.oled_str_sbar[10] = 'E';	
			  break;
			case MODE_ID_LOWP:
				ole_str.oled_str_sbar[7] = 'L';
				ole_str.oled_str_sbar[8] = 'O';
				ole_str.oled_str_sbar[9] = 'W';
				ole_str.oled_str_sbar[10] = 'P';
        break;			
			case MODE_ID_BTRS:
			  ole_str.oled_str_sbar[7] = 'B';
				ole_str.oled_str_sbar[8] = 'T';
				ole_str.oled_str_sbar[9] = 'R';
				ole_str.oled_str_sbar[10] = 'S';		
			  break;
      case MODE_ID_BTVE:
			  ole_str.oled_str_sbar[7] = 'B';
				ole_str.oled_str_sbar[8] = 'T';
				ole_str.oled_str_sbar[9] = 'V';
				ole_str.oled_str_sbar[10] = 'E';		
			  break;
      case MODE_ID_BTAD:
			  ole_str.oled_str_sbar[7] = 'B';
				ole_str.oled_str_sbar[8] = 'T';
				ole_str.oled_str_sbar[9] = 'A';
				ole_str.oled_str_sbar[10] = 'D';		
			  break;			
		}
	}
	else
	{
		if(SysParam.SysMode == MODE_TIMESET)
		{
			switch(sysCtrlFlag.tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					ds_set_time.ds_time[DS_TIME_HOUR]++;
					if(ds_set_time.ds_time[DS_TIME_HOUR]>23)ds_set_time.ds_time[DS_TIME_HOUR] = 0;
					break;
				case TIME_SET_ID_MIN:
					ds_set_time.ds_time[DS_TIME_MIN]++;
					if(ds_set_time.ds_time[DS_TIME_MIN]>59)ds_set_time.ds_time[DS_TIME_MIN] = 0;
					break;
				case TIME_SET_ID_SEC:
					ds_set_time.ds_time[DS_TIME_SEC]++;
					if(ds_set_time.ds_time[DS_TIME_SEC]>59)ds_set_time.ds_time[DS_TIME_SEC] = 0;
					break;
				case TIME_SET_ID_YEAR:
					ds_set_time.Year++;
					if(ds_set_time.Year>2099)ds_set_time.Year = 2099;
					break;
				case TIME_SET_ID_MON:
					ds_set_time.ds_time[DS_TIME_MON]++;
					if(ds_set_time.ds_time[DS_TIME_MON]>12)ds_set_time.ds_time[DS_TIME_MON] = 1;
					break;
				case TIME_SET_ID_DAY:
					ds_set_time.ds_time[DS_TIME_DAY]++;
					if(ds_set_time.ds_time[DS_TIME_DAY]>mon_table[ds_set_time.ds_time[DS_TIME_MON]])ds_set_time.ds_time[DS_TIME_DAY] = 1;
					break;
			}

		}
		else if(SysParam.SysMode == MODE_ALARM)
		{
			switch(sysCtrlFlag.tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					ds_alm_time.ds_time[DS_TIME_HOUR]++;
					if(ds_alm_time.ds_time[DS_TIME_HOUR]>23)ds_alm_time.ds_time[DS_TIME_HOUR] = 0;
					break;
				case TIME_SET_ID_MIN:
					ds_alm_time.ds_time[DS_TIME_MIN]++;
					if(ds_alm_time.ds_time[DS_TIME_MIN]>59)ds_alm_time.ds_time[DS_TIME_MIN] = 0;
					break;
				case TIME_SET_ID_SEC:
					ds_alm_time.ds_time[DS_TIME_SEC]++;
					if(ds_alm_time.ds_time[DS_TIME_SEC]>59)ds_alm_time.ds_time[DS_TIME_SEC] = 0;
					break;
				case TIME_SET_ID_YEAR:
					ds_alm_time.Year++;
					if(ds_alm_time.Year>2099)ds_alm_time.Year = 2099;
					break;
				case TIME_SET_ID_MON:
					ds_alm_time.ds_time[DS_TIME_MON]++;
					if(ds_alm_time.ds_time[DS_TIME_MON]>12)ds_alm_time.ds_time[DS_TIME_MON] = 1;
					break;
				case TIME_SET_ID_DAY:
					ds_alm_time.ds_time[DS_TIME_DAY]++;
					if(ds_alm_time.ds_time[DS_TIME_DAY]>mon_table[ds_alm_time.ds_time[DS_TIME_MON]])ds_alm_time.ds_time[DS_TIME_DAY] = 1;
					break;
			}
		}
		else if(SysParam.SysMode == MODE_GAME)
		{
			key_game_cbk(2);  //Down-2
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
	if(sw_flag.sel_Fg==1)
	{
		sw_flag.mode_Fg++;
		if(sw_flag.mode_Fg>MODE_ID_BTAD)sw_flag.mode_Fg = MODE_ID_GEN;
		switch(sw_flag.mode_Fg)
		{
			case MODE_ID_GEN:
				ole_str.oled_str_sbar[7] = 'G';
				ole_str.oled_str_sbar[8] = 'E';
				ole_str.oled_str_sbar[9] = 'N';
				ole_str.oled_str_sbar[10] = ' ';
			  break;
			case MODE_ID_TIMESET:
				ole_str.oled_str_sbar[7] = 'S';
				ole_str.oled_str_sbar[8] = 'E';
				ole_str.oled_str_sbar[9] = 'T';
				ole_str.oled_str_sbar[10] = ' ';				
			  break;
			case MODE_ID_ALMSET:				
				ole_str.oled_str_sbar[7] = 'A';
				ole_str.oled_str_sbar[8] = 'L';
				ole_str.oled_str_sbar[9] = 'M';
				ole_str.oled_str_sbar[10] = ' ';
			  break;
			case MODE_ID_CPU:
				ole_str.oled_str_sbar[7] = 'C';
				ole_str.oled_str_sbar[8] = 'P';
				ole_str.oled_str_sbar[9] = 'U';
				ole_str.oled_str_sbar[10] = ' ';
				break;
			case MODE_ID_TEMP:
				ole_str.oled_str_sbar[7] = 'T';
				ole_str.oled_str_sbar[8] = 'E';
				ole_str.oled_str_sbar[9] = 'M';
				ole_str.oled_str_sbar[10] = 'P';
				break;
			case MODE_ID_GAME:
			  ole_str.oled_str_sbar[7] = 'G';
				ole_str.oled_str_sbar[8] = 'A';
				ole_str.oled_str_sbar[9] = 'M';
				ole_str.oled_str_sbar[10] = 'E';	
			  break;
			case MODE_ID_LOWP:
				ole_str.oled_str_sbar[7] = 'L';
				ole_str.oled_str_sbar[8] = 'O';
				ole_str.oled_str_sbar[9] = 'W';
				ole_str.oled_str_sbar[10] = 'P';
        break;			
			case MODE_ID_BTRS:
			  ole_str.oled_str_sbar[7] = 'B';
				ole_str.oled_str_sbar[8] = 'T';
				ole_str.oled_str_sbar[9] = 'R';
				ole_str.oled_str_sbar[10] = 'S';		
			  break;
      case MODE_ID_BTVE:
			  ole_str.oled_str_sbar[7] = 'B';
				ole_str.oled_str_sbar[8] = 'T';
				ole_str.oled_str_sbar[9] = 'V';
				ole_str.oled_str_sbar[10] = 'E';		
			  break;
      case MODE_ID_BTAD:
			  ole_str.oled_str_sbar[7] = 'B';
				ole_str.oled_str_sbar[8] = 'T';
				ole_str.oled_str_sbar[9] = 'A';
				ole_str.oled_str_sbar[10] = 'D';		
			  break;
		}
	}
	else
	{
		if(SysParam.SysMode == MODE_TIMESET)
		{
			switch(sysCtrlFlag.tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					ds_set_time.ds_time[DS_TIME_HOUR]--;
					if(ds_set_time.ds_time[DS_TIME_HOUR]==0xff)ds_set_time.ds_time[DS_TIME_HOUR] = 23;
					break;
				case TIME_SET_ID_MIN:
					ds_set_time.ds_time[DS_TIME_MIN]--;
					if(ds_set_time.ds_time[DS_TIME_MIN]==0xff)ds_set_time.ds_time[DS_TIME_MIN] = 59;
					break;
				case TIME_SET_ID_SEC:
					ds_set_time.ds_time[DS_TIME_SEC]--;
					if(ds_set_time.ds_time[DS_TIME_SEC]==0xff)ds_set_time.ds_time[DS_TIME_SEC] = 59;
					break;
				case TIME_SET_ID_YEAR:
					ds_set_time.Year--;
					if(ds_set_time.Year<DS_YEAR_START)ds_set_time.Year = DS_YEAR_START;
					break;
				case TIME_SET_ID_MON:
					ds_set_time.ds_time[DS_TIME_MON]--;
					if(ds_set_time.ds_time[DS_TIME_MON]<1)ds_set_time.ds_time[DS_TIME_MON] = 12;
					break;
				case TIME_SET_ID_DAY:	
					ds_set_time.ds_time[DS_TIME_DAY]--;
					if(ds_set_time.ds_time[DS_TIME_DAY]<1)ds_set_time.ds_time[DS_TIME_DAY] = mon_table[ds_set_time.ds_time[DS_TIME_MON]-1];
					break;
			}			
		}	
		else if(SysParam.SysMode == MODE_ALARM)
		{
			switch(sysCtrlFlag.tim_SetCnt)
			{
				case TIME_SET_ID_HOUR:
					ds_alm_time.ds_time[DS_TIME_HOUR]--;
					if(ds_alm_time.ds_time[DS_TIME_HOUR]==0xff)ds_alm_time.ds_time[DS_TIME_HOUR] = 23;
					break;
				case TIME_SET_ID_MIN:
					ds_alm_time.ds_time[DS_TIME_MIN]--;
					if(ds_alm_time.ds_time[DS_TIME_MIN]==0xff)ds_alm_time.ds_time[DS_TIME_MIN] = 59;
					break;
				case TIME_SET_ID_SEC:
					ds_alm_time.ds_time[DS_TIME_SEC]--;
					if(ds_alm_time.ds_time[DS_TIME_SEC]==0xff)ds_alm_time.ds_time[DS_TIME_SEC] = 59;
					break;
				case TIME_SET_ID_YEAR:
					ds_alm_time.Year--;
					if(ds_alm_time.Year<2000)ds_alm_time.Year = 2000;
					break;
				case TIME_SET_ID_MON:
					ds_alm_time.ds_time[DS_TIME_MON]--;
					if(ds_alm_time.ds_time[DS_TIME_MON]<1)ds_alm_time.ds_time[DS_TIME_MON] = 12;
					break;
				case TIME_SET_ID_DAY:	
					ds_alm_time.ds_time[DS_TIME_DAY]--;
					if(ds_alm_time.ds_time[DS_TIME_DAY]<1)ds_alm_time.ds_time[DS_TIME_DAY] = mon_table[ds_alm_time.ds_time[DS_TIME_MON]-1];
					break;
			}
		}
		else if(SysParam.SysMode == MODE_GAME)
		{
			key_game_cbk(1);  //Up-1
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
	if(sw_flag.sel_Fg!=1)
	{
		sysCtrlFlag.tim_SetCnt--;
		if(sysCtrlFlag.tim_SetCnt<TIME_SET_ID_HOUR)sysCtrlFlag.tim_SetCnt = TIME_SET_ID_DAY;		
	}
	if(SysParam.SysMode==MODE_NORMALLY||SysParam.SysMode==MODE_CPU_DATA)
	{
		sysCtrlFlag.usb_SwFg++;
		SetPowerSta(sysCtrlFlag.usb_SwFg);
		AT24C02_WriteOneByte(SW_EEPADDR,sysCtrlFlag.usb_SwFg%2==0?USB_SW_OFF:USB_SW_ON);
	}
	else if(SysParam.SysMode == MODE_GAME)
	{
		key_game_cbk(3);  //Left-3
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
	if(sw_flag.sel_Fg==1)
	{
	  SysParam.SysMode = sw_flag.mode_Fg-1;
		sw_flag.sel_Fg = 0;
		sysCtrlFlag.tim_SetCnt = TIME_SET_ID_HOUR;
		if(SysParam.SysMode==MODE_TIMESET)
		{
			ds_set_time.Year = DS1302_time[DS_TIME_YEAR]+2000;
			memcpy(ds_set_time.ds_time,DS1302_time,sizeof(ds_set_time.ds_time));
			ole_str.oled_str_sbar[5] = 'M';
		}
		else if(SysParam.SysMode==MODE_ALARM)
		{
		  ds_alm_time.Year = DS1302_time[DS_TIME_YEAR]+2000;
			memcpy(ds_alm_time.ds_time,DS1302_time,sizeof(ds_alm_time.ds_time));
			ole_str.oled_str_sbar[5] = 'M';
		}
		else if(SysParam.SysMode==MODE_NORMALLY)ole_str.oled_str_sbar[5] = 'M';
		else if(SysParam.SysMode==MODE_CPU_DATA)ole_str.oled_str_sbar[5] = 'M';
		else if(SysParam.SysMode==MODE_TEMP)ole_str.oled_str_sbar[5] = 'M';
		else if(SysParam.SysMode==MODE_GAME)
		{			
			GPIO_LED_Disable();
			ole_str.oled_str_sbar[5] = 'M';	
			Sanke_Init();
		}
		else if(SysParam.SysMode==MODE_LOWP)  //低功耗模式
		{			
			PWR_Sleep_Mode(0);
			ole_str.oled_str_sbar[5] = 'M';
		}
		else if(SysParam.SysMode==MODE_BTRS)HC_Reset();//蓝牙重启
		else if(SysParam.SysMode==MODE_BTVE)
		{
			ole_str.oled_str_sbar[5] = 'M';
			HC_AT_Command(AT_COMMAND_VERSION);//蓝牙版本显示
		}
		else if(SysParam.SysMode==MODE_BTAD)
		{
			ole_str.oled_str_sbar[5] = 'M';
			HC_AT_Command(AT_COMMAND_ADDR);//蓝牙地址显示
		}
		else{;}
	}
	else
	{
		if(SysParam.SysMode == MODE_TIMESET)
		{
			sysCtrlFlag.tim_SetCnt++;
			if(sysCtrlFlag.tim_SetCnt>TIME_SET_ID_DAY)sysCtrlFlag.tim_SetCnt = TIME_SET_ID_HOUR;
		}
		else if(SysParam.SysMode == MODE_ALARM)
		{
			sysCtrlFlag.tim_SetCnt++;
			if(sysCtrlFlag.tim_SetCnt>TIME_SET_ID_SEC)sysCtrlFlag.tim_SetCnt = TIME_SET_ID_HOUR;
		}
		else if(SysParam.SysMode == MODE_GAME)
		{
			key_game_cbk(4);  //right-4
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
	if(sw_flag.sel_Fg!=1)
	{
		if(SysParam.SysMode == MODE_NORMALLY || SysParam.SysMode == MODE_CPU_DATA)
		{
			sw_flag.sel_Fg = 1;
			sw_flag.mode_Fg = 1;
			ole_str.oled_str_sbar[5] = 'S';
			SysParam.SysMode = MODE_SECLECT;
			sysCtrlFlag.pwFlag = 0;
		}
		else if(SysParam.SysMode == MODE_TIMESET)
		{
			temp = (ds_set_time.Year-2000)&0xff;
			DS1302_SetTime((((temp/10)<<4)|(temp%10)),
			(((ds_set_time.ds_time[DS_TIME_MON]/10))<<4|(ds_set_time.ds_time[DS_TIME_MON]%10)),
			(((ds_set_time.ds_time[DS_TIME_DAY]/10))<<4|(ds_set_time.ds_time[DS_TIME_DAY]%10)),
			(((ds_set_time.ds_time[DS_TIME_HOUR]/10))<<4|(ds_set_time.ds_time[DS_TIME_HOUR]%10)),
			(((ds_set_time.ds_time[DS_TIME_MIN]/10))<<4|(ds_set_time.ds_time[DS_TIME_MIN]%10)),
			0,0);
			
			sysCtrlFlag.beepFlag = 200;
			SysParam.SysMode = MODE_NORMALLY;
		}
		else if(SysParam.SysMode == MODE_ALARM)
		{
			Save_ALMTime();
			sysCtrlFlag.beepFlag = 200;
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
	U8 temp[8]={0};
	if(sw_flag.sel_Fg==1)
	{
		SysParam.SysMode = MODE_NORMALLY;
		sw_flag.sel_Fg = 0;
		ole_str.oled_str_sbar[5] = 'M';
	}
	else
	{
		if(SysParam.SysMode != MODE_NORMALLY)
		{			
			if(SysParam.SysMode==MODE_GAME)
			{
				Dispose_Game();
				GPIO_LED_Enable();
				//OLED_Clear();
			}
			else if(SysParam.SysMode==MODE_LOWP)
			{
				PWR_Sleep_Mode(1);			
			}
			else if(SysParam.SysMode == MODE_ALARM)
			{
				AT24C02_Read(ALM_EEPADDR,temp,8);
	      ArrayCopy(temp,SysParam.SysALMTime,8,0);
			}
			else{;}	
      
			SysParam.SysMode = MODE_NORMALLY;
			sysCtrlFlag.pwFlag = 0;				
			ole_str.oled_str_time[0] = ' ';
			ole_str.oled_str_time[1] = ' ';
			ole_str.oled_str_time[2] = ' ';
			ole_str.oled_str_time[3] = ' ';
			ole_str.oled_str_time[12] = ' ';
			ole_str.oled_str_time[13] = ' ';
			ole_str.oled_str_time[14] = ' ';
			ole_str.oled_str_time[15] = ' ';
			ole_str.oled_str_daily[0] = ' ';
			ole_str.oled_str_daily[1] = ' ';
			ole_str.oled_str_daily[2] = ' ';
			ole_str.oled_str_daily[13] = ' ';
			ole_str.oled_str_daily[14] = ' ';
			ole_str.oled_str_daily[15] = ' ';		  
		}
		else
		{
			//关闭屏幕			
			if(sysCtrlFlag.screenFlag%2==0) OLED_Display_Off();			
			else             OLED_Display_On();
			sysCtrlFlag.screenFlag++;
			
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
	U8 temp[8] = {0};
	//保存闹钟时间
	ds_alm_time.ds_time[DS_TIME_YEAR] = (ds_alm_time.Year-2000)&0xff;
	AT24C02_Write(ALM_EEPADDR,ds_alm_time.ds_time,8);
	AT24C02_Read(ALM_EEPADDR,temp,8);
	ArrayCopy(temp,SysParam.SysALMTime,8,0);
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
		if(DS1302_time[DS_TIME_HOUR]==SysParam.SysALMTime[DS_TIME_HOUR] 
		&& DS1302_time[DS_TIME_MIN]==SysParam.SysALMTime[DS_TIME_MIN] 
		&& DS1302_time[DS_TIME_SEC]<=20)
		{
			sysCtrlFlag.beepFlag = 255;
			sysCtrlFlag.usb_SwFg = USB_SW_OFF;
			SetPowerSta(sysCtrlFlag.usb_SwFg);
			if(temp_sw_fg!=sysCtrlFlag.usb_SwFg)
			{
				AT24C02_WriteOneByte(SW_EEPADDR,sysCtrlFlag.usb_SwFg%2==0?USB_SW_OFF:USB_SW_ON);
				temp_sw_fg = sysCtrlFlag.usb_SwFg;
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
		  ole_str.oled_str_sbar[2]='0';
			ole_str.oled_str_sbar[3]='0';
	}
	else
	{
			POWER_ON;		
		  ole_str.oled_str_sbar[2]='0';
			ole_str.oled_str_sbar[3]='1';
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
		GPIO_LED_Disable();
		OLED_Display_Off();   //关闭屏幕		
		SetPowerSta(0);
		sysCtrlFlag.screenFlag = 1;
 
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
		
		GPIO_LED_Enable();
		sysCtrlFlag.screenFlag = 0;
		sw_flag.sel_Fg = 0;	
	}
}





