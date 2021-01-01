#include "includes.h"

U16 ADC_ConvertedValue[CHANNEL*CNT];  //数据存储空间
U16 AD_CH1[CNT],AD_CH2[CNT];  //存放ADC1通道的AD转换数据
U16 AD_Value1,AD_Value2;	//存放ADC1转换AD平均值

// *****************************************************************************
// 函数名称：Read_IVot_Init
// 功能描述：读取内部电压初始化
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void Read_IVot_Init(void)
{
	ADCTemp_Config();
	DMA1_Config();
}

// *****************************************************************************
// 函数名称：ADCTemp_Config
// 功能描述：内部电压初始化
// 输入参数: /
// 输出参数: /
// 返回参量: /
// *****************************************************************************
void ADCTemp_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	ADC_DeInit(ADC1); //ADC恢复默认设置
  ADC_StructInit(&ADC_InitStructure); //初始化ADC结构
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //12位精度
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //规定模式装换工作在连续模式
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //数据对其为右对齐
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward; // ADC_ScanDirection_Backward; //ADC的扫描方向
  ADC_Init(ADC1, &ADC_InitStructure);

	ADC_TempSensorCmd(ENABLE);   //开启内部温度传感器
	ADC_VrefintCmd(ENABLE);             //使能内部参考电压 
	
	ADC_ChannelConfig(ADC1, ADC_Channel_16, ADC_SampleTime_71_5Cycles);//内部温度传感器
  ADC_ChannelConfig(ADC1, ADC_Channel_17, ADC_SampleTime_71_5Cycles);//内部参考电压 
	
	ADC_GetCalibrationFactor(ADC1);                        // ADC 校准
  ADC_Cmd(ADC1, ENABLE);                                 // 使能ADC1     
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));        // 等待ADCEN 标志 
}

/*******************************************
*函数名称：void DMA1_Config(void)
*函数功能：配置DMA1
*输入参数：无
*返回值： 无
*函数说明：
********************************************/
void DMA1_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_DeInit(DMA1_Channel1);//设置DMA1通道1
  DMA_InitStructure.DMA_PeripheralBaseAddr=ADC1_DR_Address;//DMA外设基地址（源地址）
  DMA_InitStructure.DMA_MemoryBaseAddr=(U32)&ADC_ConvertedValue;//内存地址（目标地址）	 ////////////////////////////////
  DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA传输方向，数据来自外设ADC
  DMA_InitStructure.DMA_BufferSize=CHANNEL*CNT;//设置DMA在传输时缓冲区的长度（word）
  DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设寄存器地址不变
  DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//设置DMA内存递增模式	 ??????
  DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//外设数据字长
  DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//内存数据字长
  DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;//设置传输模式连续不断的循环模式
  DMA_InitStructure.DMA_Priority=DMA_Priority_High;//设置DMA的优先级别
  DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMA通道没有设置内存到内存的传输

  DMA_Init(DMA1_Channel1,&DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1,ENABLE);    //使能DMA通道1
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	  //允许DMA传输完中断
	
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular); /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
	ADC_StartOfConversion(ADC1);                         // ADC1 定期变换
}


/*******************************************
*函数名称：void DMA_Interrupt(void)
*函数功能：ADC1转换
*输入参数：无
*返回值： 无
*函数说明：
********************************************/
void DMA_Interrupt(void)
{
  U8 i;
  U32 sum1=0,sum2=0;//存放AD转换值的求和
  if(DMA_GetITStatus(DMA1_IT_TC1)) //通道1传输完成中断
  {
    DMA_ClearITPendingBit(DMA1_IT_GL1);
	  for(i=0;i<CNT;i++)
		{
		  AD_CH1[i]=ADC_ConvertedValue[2*i];
		  AD_CH2[i]=ADC_ConvertedValue[2*i+1];
		}
    for(i=0;i<CNT;i++)
    {
      sum1=sum1+AD_CH1[i];
		  sum2=sum2+AD_CH2[i]; 
    }
    AD_Value1=sum1/CNT;	  //存放ADC1通道数据平均值
    AD_Value2=sum2/CNT;   //存放ADC2通道数据平均值
    sum1=0;
    sum2=0; 
  }
}

/*******************************************
*函数名称：void ADC1_Process_Pot(u16 ADValue)
*函数功能：ADC1转换
*输入参数：ADValue--AD转换值
*返回值： 无
*函数说明：
********************************************/
float ADC1_Process_Pot(void)
{
	float cpu_vol = 0.0f;
  cpu_vol = (AD_Value2*3.2)/4095;		//得到电压值,单位10mV
	return cpu_vol;
}

/*******************************************
*函数名称：void ADC1_Process_Temp(u16 ADValue)
*函数功能：ADC1转换
*输入参数：ADValue--AD转换值
*返回值： 无
*函数说明：
********************************************/
float ADC1_Process_Temp(void)
{
  float ADTemp = 0,cpu_temp=0.0f,cpu_vol = 0.0f;
	cpu_vol = ADC1_Process_Pot();
  ADTemp = (AD_Value1*3.2)/4095;		//得到电压值
	if(cpu_vol<=1.43)
	{
    cpu_temp = ((1.43-ADTemp)/0.0043)+25;  //温度计算公式
	}
	else
	{
		cpu_temp = ((ADTemp-cpu_vol)/0.0043)+25;  //温度计算公式
	}
	return cpu_temp;
}

