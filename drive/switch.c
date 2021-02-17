#include "includes.h"

//�ڲ�����
//U8 data[DEVICE_DATA_LEN]={0x00,0x00,0x00,0x00};
U8 SW_BTScanDat[SW_SEL_LINE]; //�洢ɨ������
U8 BtnState[SW_SUM];          //����״̬��
U8 SW_Sel_Table[SW_SEL_LINE] = {0x0E,0x0D,0x0B};//ѡ�߱�
const U8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	       //12���·ݵ�����

ds_set_time_t ds_set_time={0U,{0U}};
ds_set_time_t ds_alm_time={0U,{0U}};
select_flag_t sw_flag={1,0};

key_func_game_t key_game_cbk = Key_Down_Move;   //��Ϸ��ť
key_func_cbk Key_Funcbk[KEY_FUNC_ITEM_NUM] = {
	Key_LeftEvt, 
  Key_UpEvt,
	Key_EnterEvt, 
  Key_RightEvt,
  Key_DownEvt,
  Key_CacelEvt
};

//�ⲿ����
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern SYSTEMSTRUCT SysParam;
extern OLED_STR_T ole_str;
extern U8 DS1302_time[];

/* ********************************************************************************************************
*	�� �� ��: Delay
*	����˵��: ��ʱ
*	��    ��: ��
*	�� �� ֵ: ��
* ��    ע: ��
********************************************************************************************************* */
void SW_Delay(U8 de_Count)	 
{  
    U8 de_i;
    for(de_i=0;de_i<de_Count;de_i++);   
}

// *****************************************************************************
// �������ƣ�Switch_Scan
// ��������������ɨ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
U8 Switch_Scan(void)
{
	U8 up_flag = 0,sl_i = 0;
	U16 s1_8x1 = 0x0000;
	
	static U8 s1_CountBT[SW_SEL_LINE];	           
	static U16 s1_PreBT[SW_SEL_LINE];
	if(!sysCtrlFlag.swFlag)
	{
		sysCtrlFlag.swFlag = 2;//���1msɨ��һ��
		for(sl_i = 0;sl_i<SW_SEL_LINE;sl_i++)
		{
			//ѡ��
			s1_8x1 = GPIOA->ODR & 0xfff0;            //���ѡ��
			GPIOA->ODR = s1_8x1 | SW_Sel_Table[sl_i];//��ѯѡ��
			
			//��ʱ
			SW_Delay(60);
			
			//����
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
// �������ƣ�Switch_Sfit
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
					//��ȡID
					s2_08x3 = s2_i*2 + s2_j;
					//��ȡ��ֵ
					BtnState[s2_08x3] = SW_BTScanDat[s2_i] & s2_08x2;
					//��ֵ
					s2_pre[s2_i] ^= s2_08x2;
					
					//����
				  Key_Process(s2_08x3);
				}
			}
		}
	}
}



// *****************************************************************************
// �������ƣ�Key_Process
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�Key_DownEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
		
			//��ȡEEP
			;//FLASH_ReadNByte(data,0x00,DEVICE_DATA_LEN);
		}
	}
}

// *****************************************************************************
// �������ƣ�Key_UpEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�Key_LeftEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�Key_RightEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
		else if(SysParam.SysMode==MODE_LOWP)  //�͹���ģʽ
		{			
			PWR_Sleep_Mode(0);
			ole_str.oled_str_sbar[5] = 'M';
		}
		else if(SysParam.SysMode==MODE_BTRS)HC_Reset();//��������
		else if(SysParam.SysMode==MODE_BTVE)
		{
			ole_str.oled_str_sbar[5] = 'M';
			HC_AT_Command(AT_COMMAND_VERSION);//�����汾��ʾ
		}
		else if(SysParam.SysMode==MODE_BTAD)
		{
			ole_str.oled_str_sbar[5] = 'M';
			HC_AT_Command(AT_COMMAND_ADDR);//������ַ��ʾ
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
// �������ƣ�Key_EnterEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
			//���½�����Ϸ
			Game_Reset();
		}
		else{;}
	}
	
}

// *****************************************************************************
// �������ƣ�Key_CacelEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
			//�ر���Ļ			
			if(sysCtrlFlag.screenFlag%2==0) OLED_Display_Off();			
			else             OLED_Display_On();
			sysCtrlFlag.screenFlag++;
			
		}
	}
}

// *****************************************************************************
// �������ƣ�ArrayCopy(U8 *source_buf,U8 *des_buf,U16 len,U16 start)
// �������������鸴�ƺ���
// �������: source_buf���������飬des_bufĿ�����飬len���ȣ�start��ʼλ��
// �������: /
// ���ز���: /
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
// �������ƣ�Save_ALMTime
// ������������������ʱ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Save_ALMTime(void)
{
	U8 temp[8] = {0};
	//��������ʱ��
	ds_alm_time.ds_time[DS_TIME_YEAR] = (ds_alm_time.Year-2000)&0xff;
	AT24C02_Write(ALM_EEPADDR,ds_alm_time.ds_time,8);
	AT24C02_Read(ALM_EEPADDR,temp,8);
	ArrayCopy(temp,SysParam.SysALMTime,8,0);
}

// *****************************************************************************
// �������ƣ�ALM_Proc
// ��������������ʱ��Աȳ���
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�SetPowerSta
// �������������ü̵���ֵ
// �������: /
// �������: /
// ���ز���: /
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
��������:void PWM_IRQHandler(void)
��������:��TIM3�жϴ���
��ڲ�������
����ֵ����
˵��:  ���ںͷ�Ƶ�Ⱦ�����ʱ��ʱ��
************************************************/
void PWM_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET) //����ж�
	{
		//timerSecCount++;
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ		
	}	
}

/************************************************
��������:void PWR_Sleep_Mode(void)
��������:����˯��ģʽ
��ڲ�������
����ֵ����
˵��:  ���ںͷ�Ƶ�Ⱦ�����ʱ��ʱ��
************************************************/
void PWR_Sleep_Mode(U8 mcu_status)
{
	if(mcu_status==0)
	{
		//˯��ģʽ
		ADC_Cmd(ADC1, DISABLE);	        						//ʧ��ADC
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);	
	  ADC_DeInit(ADC1);
		
		DMA_Cmd(DMA1_Channel1,DISABLE);    //ʹ��DMAͨ��1
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,DISABLE);
		DMA_DeInit(DMA1_Channel1);//����DMA1ͨ��1		
 
	  USART_DeInit(USART1);
	  USART_Cmd(USART1,DISABLE);                              //ʧ�ܴ��� 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE );	
		
		
		TIM_Cmd(TIM3, DISABLE);
		TIM_Cmd(TIM2, DISABLE);	
		TIM_DeInit(TIM3);
		TIM_CtrlPWMOutputs(TIM3, DISABLE);	                    //PWM����P
		
		GPIO_Write(GPIOA,(GPIOA->ODR|0x00B0));
		GPIO_LED_Disable();
		OLED_Display_Off();   //�ر���Ļ		
		SetPowerSta(0);
		sysCtrlFlag.screenFlag = 1;
 
//	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);//����Դ����ʱ��
//	  PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);//����ͣ��ģʽ		
	}
	else
	{
		//�ָ���������		
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





