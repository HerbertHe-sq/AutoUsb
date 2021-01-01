#include "includes.h"

//�ڲ�����
U8 swFlag = 0,beepFlag = 0,upTimeFg = 0,sLedFlag = 1;//��������־λ    //����ʱ���־λ
U16 pwFlag = 0,timSFlag,timLedFlag=10,adcFlag = 0;
SYSTEMSTRUCT SysParam;

//�ⲿ����
extern U8 usb_SwFg;

// *****************************************************************************
// �������ƣ�Sysclk_Interrupt
// ����������ϵͳ��ʱ�жϷ����Ӻ���
// �������: /
// �������: /
// ���ز���: /
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
  IWDG_Init();        
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
	usb_SwFg = AT24C02_ReadOneByte(SW_EEPADDR);
	SetPowerSta(usb_SwFg);
	
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
		IWDG_ReloadCounter(); 

    //�豸ɨ��
    Device_Scan();
		
    //��Ļ��ʾ
		OLED_Display();		
	}
}

