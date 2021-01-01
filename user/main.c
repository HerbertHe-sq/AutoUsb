#include "includes.h"

//内部变量
U8 swFlag = 0,beepFlag = 0,upTimeFg = 0,sLedFlag = 1;//蜂鸣器标志位    //更新时间标志位
U16 pwFlag = 0,timSFlag,timLedFlag=10,adcFlag = 0;
SYSTEMSTRUCT SysParam;

//外部变量
extern U8 usb_SwFg;

// *****************************************************************************
// 函数名称：Sysclk_Interrupt
// 功能描述：系统定时中断服务子函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Sysclk_Interrupt(void)
{  
  if(swFlag)swFlag--;
	if(timSFlag)timSFlag--;
	if(beepFlag)beepFlag--;
	if(upTimeFg)upTimeFg--;
	if(timLedFlag)timLedFlag--;
	if(adcFlag)adcFlag--;
	if(sLedFlag)sLedFlag--;
	pwFlag++;
}

// *****************************************************************************
// 函数名称：Sys_Init
// 功能描述：系统初始化函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Sys_Init(void)
{
	//  System clock configuration
	RCC_Configuration();   
    
	// General Purpose I/O default configration
	GPIO_Configuration();	
	
	//USART default configration
	USART_Config();
	
	//TIM
	TIM_Config();
	
	//ADC
	Read_IVot_Init();
	
	// Nested Vector Interrupt Controller configuration
	NVIC_Configuration();

  //IIC
	IIC_Init();	
	
	//RTC
	DS1302_Init();
	
	//OLED初始化  
	OLED_Init();
	
	//Systick Init
	SysTick05ms();
	
	//Watch Dog
  IWDG_Init();        
}


// *****************************************************************************
// 函数名称：User_Init
// 功能描述：用户功能函数初始化配置
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void UserInit(void)
{	
	U8 temp[8];
	
	//执行清屏指令
	OLED_Clear();
	
	//读取标志位
	AT24C02_Read(ALM_EEPADDR,temp,8);
	ArrayCopy(temp,SysParam.SysALMTime,8,0);
	
	//读取继电器标志位
	usb_SwFg = AT24C02_ReadOneByte(SW_EEPADDR);
	SetPowerSta(usb_SwFg);
	
	//读取游戏速度
	Game_SetSpeed(AT24C02_ReadOneByte(SNAKE_SPEED));
	
	//检查DS18B20是否存在
	DSB_Init();
	
	//获取RTC时间
	DS1302_GetTime();
	
	//开机动画
	OLED_Start_Mov();
  OLED_Clear();
	
	//执行呼吸灯代表程序正在运行
	GPIO_LED3_Config();
}

// *****************************************************************************
// 函数名称：main
// 功能描述：主函数
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
int main(void)
{
	
	Sys_Init();   //系统初始化
	UserInit();   //用户初始化
	
	while(1)
	{
		//独立看门狗，防止程序跑飞
		IWDG_ReloadCounter(); 

    //设备扫描
    Device_Scan();
		
    //屏幕显示
		OLED_Display();		
	}
}

