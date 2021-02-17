#include "includes.h"

//�ڲ�����
SYSTEMSTRUCT SysParam;
SYSTEM_CTRL_FLAG sysCtrlFlag={0,0,0,1,1,0,0,0,0,10,0};

//�ⲿ����

// *****************************************************************************
// �������ƣ�Sysclk_Interrupt
// ����������ϵͳ��ʱ�жϷ����Ӻ���
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Sysclk_Interrupt(void)
{  
  if(sysCtrlFlag.swFlag)sysCtrlFlag.swFlag--;
	if(sysCtrlFlag.timSFlag)sysCtrlFlag.timSFlag--;
	if(sysCtrlFlag.beepFlag)sysCtrlFlag.beepFlag--;
	if(sysCtrlFlag.upTimeFg)sysCtrlFlag.upTimeFg--;
	if(sysCtrlFlag.timLedFlag)sysCtrlFlag.timLedFlag--;
	if(sysCtrlFlag.adcFlag)sysCtrlFlag.adcFlag--;
	if(sysCtrlFlag.sLedFlag)sysCtrlFlag.sLedFlag--;
	sysCtrlFlag.pwFlag++;
}

// *****************************************************************************
// �������ƣ�Sys_Init
// ����������ϵͳ��ʼ������
// �������: /
// �������: /
// ���ز���: /
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
	
	//DMA
	DMA1_Init();
	
	// Nested Vector Interrupt Controller configuration
	NVIC_Configuration();

  //IIC
	IIC_Init();	
	
	//RTC
	DS1302_Init();
	
	//OLED��ʼ��  
	OLED_Init();
	
	//Systick Init
	SysTick05ms();
	
	//Watch Dog
#ifdef IWDG_ENV	
  IWDG_Init(); 
#endif	
}


// *****************************************************************************
// �������ƣ�User_Init
// �����������û����ܺ�����ʼ������
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void UserInit(void)
{	
	U8 temp[8];
	
	//ִ������ָ��
	OLED_Clear();
	
	//��ȡ��־λ
	AT24C02_Read(ALM_EEPADDR,temp,8);
	ArrayCopy(temp,SysParam.SysALMTime,8,0);
	
	//��ȡ�̵�����־λ
	sysCtrlFlag.usb_SwFg = AT24C02_ReadOneByte(SW_EEPADDR);
	SetPowerSta(sysCtrlFlag.usb_SwFg);
	
	//��ȡ��Ϸ�ٶ�
	Game_SetSpeed(AT24C02_ReadOneByte(SNAKE_SPEED));
	
	//���DS18B20�Ƿ����
	DSB_Init();
	
	//��ȡRTCʱ��
	DS1302_GetTime();
	
	//��������
	OLED_Start_Mov();
  OLED_Clear();
	
	//ִ�к����ƴ��������������
	GPIO_LED3_Config();
	
	//���ö�����״̬
	sysCtrlFlag.pwFlag = 0;
}

// *****************************************************************************
// �������ƣ�main
// ����������������
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
int main(void)
{
	
	Sys_Init();   //ϵͳ��ʼ��
	UserInit();   //�û���ʼ��
	
	while(1)
	{
		//�������Ź�����ֹ�����ܷ�
#ifdef IWDG_ENV	
		IWDG_ReloadCounter(); 
#endif		

    //�豸ɨ��
    Device_Scan();
		
    //��Ļ��ʾ
		OLED_Display();			
	}
}

