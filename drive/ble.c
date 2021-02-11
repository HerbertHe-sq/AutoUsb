#include "includes.h"


//内部变量：
U8 Usart_Rec_Buf[USART_BUF_LEN];   //串口接收数组
U8 uart_tx_buf[USART_BUF_LEN] = {0};
BLE_STATUS bleStatus={0,0,0,0};
uart_rx_msg_t uart_rx_msg = {0};

//外部变量：
extern SYSTEMSTRUCT SysParam;
extern SYSTEM_CTRL_FLAG sysCtrlFlag;
extern U8 wr_Code[],DS1302_time[];
extern short dsbDat;

typedef void(*snake_func_cbk)(void);

snake_func_cbk snake_func[GAME_FUNC_ITEM_NUM]={Snake_RunStep};

/************************************************
函数名称:USART_Rec
函数功能: 串口接收函数
入口参数：无
返回值：无
说明:  
************************************************/
void USART_Rec(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {         		
		if(uart_rx_msg.rx_state==UART_STATE_ITEM_WAIT_ACK)
		{
			uart_rx_msg.rx_index = 0;
			uart_rx_msg.time_cnt = 0;
			uart_rx_msg.rx_state = UART_STATE_ITEM_RECEIVING;
			TIM1_Start();			
		}
		uart_rx_msg.rx_buf[uart_rx_msg.rx_index++] = ((USART1->RDR) & 0xff);
		uart_rx_msg.time_cnt = 0;	
			
		bleStatus.rx_cnt++;//更新指示灯
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
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
	U8 ua_i = 0,flag = 0;
	U8 *p_buf;
	
	p_buf = Usart_Rec_Buf;
	if(uart_rx_msg.rx_state == UART_STATE_ITEM_UNPACK)
	{
		for(ua_i = 0;ua_i<uart_rx_msg.rx_index;ua_i++)
		{
			*p_buf++=uart_rx_msg.rx_buf[ua_i];
			uart_rx_msg.rx_buf[ua_i] = 0;
		}
		//memset(uart_rx_msg.rx_buf,0,sizeof(uart_rx_msg.rx_buf));
		uart_rx_msg.rx_state = UART_STATE_ITEM_WAIT_ACK;
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
							if(Usart_Rec_Buf[4]==0x00)sysCtrlFlag.usb_SwFg = USB_SW_OFF;						
							else                      sysCtrlFlag.usb_SwFg = USB_SW_ON;
					    SetPowerSta(sysCtrlFlag.usb_SwFg);
					    AT24C02_WriteOneByte(SW_EEPADDR,sysCtrlFlag.usb_SwFg%2==0?USB_SW_OFF:USB_SW_ON);				
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
						bleStatus.heartCnt = 0;
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
void USART_DMASendDat(U8 *buf,U8 len)
{
	U8 temp_i;
	if(UASRT1_IDLE) //判断总线空闲
	{
		for(temp_i = 0;temp_i<len;temp_i++)
		{
			uart_tx_buf[temp_i] = buf[temp_i];
		}
		DMA_Cmd(DMA1_Channel2,DISABLE);            // 发送完成先关掉DMA通道
		DMA_SetCurrDataCounter(DMA1_Channel2,len); // 设置需要发送的长度
		DMA_Cmd(DMA1_Channel2,ENABLE);             // 再打开DMA通道
		
		bleStatus.tx_cnt++;                        //更新指示灯
  }
}

void USART_SendDat(U8 *buf,U8 len)
{
	U8 temp_i;
	if(UASRT1_IDLE) //判断总线空闲
	{
		for(temp_i = 0;temp_i<len;temp_i++)
		{
			USART1->TDR = (buf[temp_i]&0x1ff);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
		}		
		bleStatus.tx_cnt++;                        //更新指示灯
  }
}

// *****************************************************************************
// 函数名称：Usart_Tx_Data
// 功能描述：串口数据发送
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Usart_Tx_Data(void)
{
	if(UASRT1_IDLE) //判断总线空闲
	{
		;
	}
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
	
	USART_DMASendDat(send_buf,7);//发送数据
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
		
		USART_DMASendDat(send_buf,16);		
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
		if(time_cnt%2==0)
		{
			USART_DMASendDat(send_buf,7);
			bleStatus.heartCnt++;
		}
		time_cnt++;	
		
    //蛇身运行
		snake_func[GAME_FUNC_ITEM_RUN]();	
	}	
}

/************************************************
函数名称:TIM_OUT_Handler
函数功能:超时计算
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void TIM_OUT_Handler(void)
{
  if(TIM_GetITStatus(TIM16,TIM_IT_Update) != RESET) //溢出中断
	{
		TIM_ClearITPendingBit(TIM16,TIM_IT_Update);  //清除中断标志位	
    uart_rx_msg.time_cnt++;
    if(uart_rx_msg.time_cnt>10 || uart_rx_msg.rx_index>USART_BUF_LEN)
		{
			TIM1_Stop();				
      uart_rx_msg.rx_state = UART_STATE_ITEM_UNPACK;
		}			
	}	
}

/************************************************
函数名称:TIM1_Start
函数功能:定时器启动
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void TIM1_Start(void)
{
	TIM_ITConfig(TIM16,TIM_IT_Update,ENABLE);
}

/************************************************
函数名称:TIM1_Stop
函数功能:定时器停止
入口参数：无
返回值：无
说明:  周期和分频比决定定时的时常
************************************************/
void TIM1_Stop(void)
{
	TIM_ITConfig(TIM16,TIM_IT_Update,DISABLE);
}

