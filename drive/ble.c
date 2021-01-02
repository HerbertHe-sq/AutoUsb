#include "includes.h"


//内部变量：
U8 Usart_Rx_Buf[USART_BUF_LEN],Usart_Rec_Buf[USART_BUF_LEN];   //串口接收数组
U8 Rx_Sta = 0,heartCnt = 0,tx_cnt = 0,rx_cnt = 0;
//外部变量：
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern U8 wr_Code[],DS1302_time[];
extern short dsbDat;



/************************************************
函数名称:USART_Rec
函数功能: 串口接收函数
入口参数：无
返回值：无
说明:  
************************************************/
void USART_Rec(void)
{
	U8 dat = 0x00;
	static U8 rx_i = 0;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {     
  	USART_ClearFlag(USART1,USART_FLAG_RXNE);        //清除中断
    USART_ClearITPendingBit(USART1,USART_IT_RXNE); 
		
		dat = ((USART1->RDR) & 0xff); //USART_ReceiveData(USART1);
    if(dat==0xf0 || dat==0x4f)rx_i = 0;					
		
		Usart_Rx_Buf[rx_i] = dat;
    rx_i++;	
    if(dat==0xf7||dat==0x4b)
		{
			Rx_Sta = rx_i;
			rx_i = 0;
		}
    if(SysParam.SysMode==MODE_BTVE && rx_i>=21)//针对BLE版本接收
		{
			Rx_Sta = rx_i;
			rx_i = 0;
		}
		else if(SysParam.SysMode==MODE_BTAD && rx_i>=19)
		{
			Rx_Sta = rx_i;
			rx_i = 0;
		}
		else{;}
			
		rx_cnt++;//更新指示灯
  } 
}


/************************************************
函数名称:USART_Unpack
函数功能: 串口解析函数
入口参数：无
返回值：无
说明:  
************************************************/
U8 USART_Unpack(void)
{
	U8 ua_i = 0,flag = 0,index = 0;
	U8 *p_buf;
	
	p_buf = Usart_Rec_Buf;

	if(Rx_Sta)
	{
		index = Rx_Sta;
		Rx_Sta = 0;
		for(ua_i = 0;ua_i<index;ua_i++)
		{
			*p_buf++=Usart_Rx_Buf[ua_i];
			Usart_Rx_Buf[ua_i] = 0;
		}
		flag = 1;
	} 	
	return flag;
}

/************************************************
函数名称:USART_Deal
函数功能: 串口处理函数
入口参数：无
返回值：无
说明:  
************************************************/
void USART_Deal(U8 flag)
{
	if(flag)
	{
    if(Usart_Rec_Buf[0]==0xf0 && Usart_Rec_Buf[1]==0x5a && Usart_Rec_Buf[2]==0xa5)
		{
			if(Usart_Rec_Buf[5]==0xf7)
			{
				switch(Usart_Rec_Buf[3])
				{
					case BLE_FUNC_ITEM_LED1:
						if(Usart_Rec_Buf[4]==0x01)	LED1_ON;
						else LED1_OFF;
					break;	
					case BLE_FUNC_ITEM_LED2:
						if(Usart_Rec_Buf[4]==0x01)	LED2_ON;	
						else LED2_OFF;	
					
					break;
					case BLE_FUNC_ITEM_LED3:
            if(Usart_Rec_Buf[4]==0x01)	LED3_ON;	
						else LED3_OFF;
					break;
					case BLE_FUNC_ITEM_BEEP:
						if(Usart_Rec_Buf[4]==0x01)	BEEP_ON;	
						else BEEP_OFF;						
					break;					
					case BLE_FUNC_ITEM_USB:
							if(Usart_Rec_Buf[4]==0x00)sysCtrlFlag.usb_SwFg = 0x00;						
							else                      sysCtrlFlag.usb_SwFg = 0x01;
					    SetPowerSta(sysCtrlFlag.usb_SwFg);
					    AT24C02_WriteOneByte(SW_EEPADDR,sysCtrlFlag.usb_SwFg%2==0?0x00:0x01);				
					break;
					default:break;
					
				}
			}
			else if(Usart_Rec_Buf[6]==0xf7)
			{
				if(Usart_Rec_Buf[3]==BLE_FUNC_ITEM_DRST)//soft reset
				{
					ST_Device_Reset();
				}
				else if(Usart_Rec_Buf[3]==BLE_FUNC_ITEM_HEART)
				{
					if(Usart_Rec_Buf[4]==0x7f && Usart_Rec_Buf[5]==0x01)
					{
						heartCnt = 0;
					}
				}
				else{;}
			}
			else if(Usart_Rec_Buf[12]==0xf7)
			{
				if(Usart_Rec_Buf[3]==BLE_FUNC_ITEM_MODRTC && Usart_Rec_Buf[4]==0x40)// modify rtc time msg
				{
					//修改时间
					Modify_RtcTime(Usart_Rec_Buf);		
				}
			}
			else{;}
		}
		else
		{
		  //AT指令接收
			BT_Usart_Rec(Usart_Rec_Buf);
		}	
	}
}


/************************************************
函数名称:USART_SendDat
函数功能: 串口发送函数
入口参数：无
返回值：无
说明:  
************************************************/
void USART_SendDat(U8 *buf,U8 len)
{
	U8 temp_i;
	for(temp_i = 0;temp_i<len;temp_i++)
	{
		USART_SendData(USART1,buf[temp_i]);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	}
	tx_cnt++;//更新指示灯
}

// *****************************************************************************
// 函数名称：HC_Reset
// 功能描述：蓝牙复位函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
U8 HC_Reset(void)
{
	//USART_SendDat("AT+MODE=?\r\n",11);
	USART_SendDat((U8 *)"AT+RESET",8);
	return 1;
}

// *****************************************************************************
// 函数名称：HC_AT_Command
// 功能描述：蓝牙指令查询
// 输入参数: com_index 命令序号
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void HC_AT_Command(U8 com_index)
{
	switch(com_index)
	{
		case 0:USART_SendDat((U8 *)"AT",2);break;
		case 1:USART_SendDat((U8 *)"AT+VERSION",10);break;
		case 2:USART_SendDat((U8 *)"AT+ADDR=?",9);break;
	}
}

// *****************************************************************************
// 函数名称：ST_Device_Reset
// 功能描述：设备软件复位
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void ST_Device_Reset(void)
{
	U8 send_buf[]={0xF0,0x5A ,0xA5 ,0x06 ,0x00 ,0x01 ,0xF7};
	USART_SendDat(send_buf,7);//发送数据
	NVIC_SystemReset();//系统复位
}

// *****************************************************************************
// 函数名称：Modify_RtcTime
// 功能描述：更新RTC时间
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Modify_RtcTime(U8 *buf)
{
	U8 send_buf[]={0xF0,0x5A ,0xA5 ,0x09 ,0x00 ,0x01 ,0xF7};
	U16 year = buf[5]*256+buf[6]-2000;	
	DS1302_SetTime((((year/10)<<4)|(year%10)),
			(((buf[7]/10))<<4|(buf[7]%10)),
			(((buf[8]/10))<<4|(buf[8]%10)),
			(((buf[9]/10))<<4|(buf[9]%10)),
			(((buf[10]/10))<<4|(buf[10]%10)),
			0,0);
	
	USART_SendDat(send_buf,7);//发送数据
}


// *****************************************************************************
// 函数名称：Upload_RtcTime
// 功能描述：上传RTC时间
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Upload_RtcTime(void)
{
	U8 send_buf[16];
	
	static U8 _sec_t = 0;
	U8 *p_tim;
	p_tim = send_buf;
	if(DS1302_time[0]!=_sec_t)
	{
		_sec_t = DS1302_time[0];
		*p_tim++=0xf0;
		*p_tim++=0x5A;
		*p_tim++=0xA5;
		*p_tim++=BLE_FUNC_ITEM_UPRTC;
		*p_tim++=0x40;
		
		*p_tim++=((DS1302_time[DS_TIME_YEAR]+DS_YEAR_START)/256);//Year
		*p_tim++=(DS1302_time[DS_TIME_YEAR]+DS_YEAR_START)%256;
		
		*p_tim++=DS1302_time[DS_TIME_MON];//month
		*p_tim++=DS1302_time[DS_TIME_DAY];//day
		
		*p_tim++=DS1302_time[DS_TIME_HOUR];//hour
		*p_tim++=DS1302_time[DS_TIME_MIN];//min
		*p_tim++=DS1302_time[DS_TIME_SEC];//sec
		
		*p_tim++=(((U32)dsbDat)/100%10);//TEMP
		*p_tim++=(((U32)dsbDat)/10%10);
		*p_tim++=(((U32)dsbDat)%10);
		
		*p_tim=0xf7;
		
		USART_SendDat(send_buf,16);		
	}
}

/************************************************
函数名称:void TIME_IRQHandler(void)
函数功能:对TIM2中断处理
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void TIME_IRQHandler(void)
{
	U8 send_buf[]={0xF0,0x5A ,0xA5 ,0x07 ,0x01 ,0x7f ,0xF7};
	static U8 time_cnt = 0;
	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET) //溢出中断
	{
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);  //清除中断标志位
						
		//秒数更新
		if(time_cnt%10==0)
		{
			USART_SendDat(send_buf,7);
			heartCnt++;
		}
		time_cnt++;	
		
    //蛇身运行
		Snake_RunStep();		
	}	
}

