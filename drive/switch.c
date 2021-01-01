#include "includes.h"

//�ڲ�����
U8 data[DEVICE_DATA_LEN]={0x00,0x00,0x00,0x00};
U8 SW_BTScanDat[SW_SEL_LINE]; //�洢ɨ������
U8 BtnState[SW_SUM];          //����״̬��
U8 SW_Sel_Table[SW_SEL_LINE] = {0x0E,0x0D,0x0B};//ѡ�߱�
U8 SetTime[8],AlmTime[8];
U8 mode_Fg = 1,sel_Fg = 0,tim_SetCnt = 1,usb_SwFg = 0;//ģʽ��־λ
U8 dis_cnt = 0;//�ر���Ļ
const U8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};	       //12���·ݵ�����


//�ⲿ����
extern U8 swFlag,beepFlag;
extern SYSTEMSTRUCT SysParam;
extern U8 staBar[],dailyStr[],timeStr[];
extern U8 DS1302_time[];

/* ********************************************************************************************************
*	�� �� ��: Delay
*	����˵��: ��ʱ
*	��    ��: ��
*	�� �� ֵ: ��
*   ��    ע: ��
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
	if(!swFlag)
	{
		swFlag = 2;//���1msɨ��һ��
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
// �������ƣ�Key_DownEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�Key_LeftEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
// �������ƣ�Key_RightEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
		else if(SysParam.SysMode==MODE_LOWP)  //�͹���ģʽ
		{
			staBar[5] = 'M';
			PWR_Sleep_Mode(0);
		}
		else if(SysParam.SysMode==MODE_BTRS)HC_Reset();//��������
		else if(SysParam.SysMode==MODE_BTVE)
		{
			staBar[5] = 'M';
			HC_AT_Command(AT_COMMAND_VERSION);//�����汾��ʾ
		}
		else if(SysParam.SysMode==MODE_BTAD)
		{
			staBar[5] = 'M';
			HC_AT_Command(AT_COMMAND_ADDR);//������ַ��ʾ
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
// �������ƣ�Key_EnterEvt
// ��������������������
// �������: /
// �������: /
// ���ز���: /
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
			//�ر���Ļ			
			if(dis_cnt%2==0) OLED_Display_Off();			
			else             OLED_Display_On();
			dis_cnt++;
			
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
	//��������ʱ��
	AT24C02_Write(ALM_EEPADDR,AlmTime,8);
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
		
		OLED_Display_Off();   //�ر���Ļ		
		SetPowerSta(0);
		dis_cnt = 1;
 
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
		dis_cnt = 0;
		sel_Fg = 0;	
	}
}





