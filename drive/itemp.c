#include "includes.h"

U16 ADC_ConvertedValue[CHANNEL*CNT];  //���ݴ洢�ռ�
U16 AD_CH1[CNT],AD_CH2[CNT];  //���ADC1ͨ����ADת������
U16 AD_Value1,AD_Value2;	//���ADC1ת��ADƽ��ֵ

// *****************************************************************************
// �������ƣ�Read_IVot_Init
// ������������ȡ�ڲ���ѹ��ʼ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void Read_IVot_Init(void)
{
	ADCTemp_Config();
	DMA1_Config();
}

// *****************************************************************************
// �������ƣ�ADCTemp_Config
// �����������ڲ���ѹ��ʼ��
// �������: /
// �������: /
// ���ز���: /
// *****************************************************************************
void ADCTemp_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	ADC_DeInit(ADC1); //ADC�ָ�Ĭ������
  ADC_StructInit(&ADC_InitStructure); //��ʼ��ADC�ṹ
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; //12λ����
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //�涨ģʽװ������������ģʽ
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //���ݶ���Ϊ�Ҷ���
  ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward; // ADC_ScanDirection_Backward; //ADC��ɨ�跽��
  ADC_Init(ADC1, &ADC_InitStructure);

	ADC_TempSensorCmd(ENABLE);   //�����ڲ��¶ȴ�����
	ADC_VrefintCmd(ENABLE);             //ʹ���ڲ��ο���ѹ 
	
	ADC_ChannelConfig(ADC1, ADC_Channel_16, ADC_SampleTime_71_5Cycles);//�ڲ��¶ȴ�����
  ADC_ChannelConfig(ADC1, ADC_Channel_17, ADC_SampleTime_71_5Cycles);//�ڲ��ο���ѹ 
	
	ADC_GetCalibrationFactor(ADC1);                        // ADC У׼
  ADC_Cmd(ADC1, ENABLE);                                 // ʹ��ADC1     
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));        // �ȴ�ADCEN ��־ 
}

/*******************************************
*�������ƣ�void DMA1_Config(void)
*�������ܣ�����DMA1
*�����������
*����ֵ�� ��
*����˵����
********************************************/
void DMA1_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_DeInit(DMA1_Channel1);//����DMA1ͨ��1
  DMA_InitStructure.DMA_PeripheralBaseAddr=ADC1_DR_Address;//DMA�������ַ��Դ��ַ��
  DMA_InitStructure.DMA_MemoryBaseAddr=(U32)&ADC_ConvertedValue;//�ڴ��ַ��Ŀ���ַ��	 ////////////////////////////////
  DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;//DMA���䷽��������������ADC
  DMA_InitStructure.DMA_BufferSize=CHANNEL*CNT;//����DMA�ڴ���ʱ�������ĳ��ȣ�word��
  DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//����Ĵ�����ַ����
  DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//����DMA�ڴ����ģʽ	 ??????
  DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//���������ֳ�
  DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;//�ڴ������ֳ�
  DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;//���ô���ģʽ�������ϵ�ѭ��ģʽ
  DMA_InitStructure.DMA_Priority=DMA_Priority_High;//����DMA�����ȼ���
  DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;//DMAͨ��û�������ڴ浽�ڴ�Ĵ���

  DMA_Init(DMA1_Channel1,&DMA_InitStructure);

  DMA_Cmd(DMA1_Channel1,ENABLE);    //ʹ��DMAͨ��1
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);	  //����DMA�������ж�
	
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular); /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
	ADC_StartOfConversion(ADC1);                         // ADC1 ���ڱ任
}


/*******************************************
*�������ƣ�void DMA_Interrupt(void)
*�������ܣ�ADC1ת��
*�����������
*����ֵ�� ��
*����˵����
********************************************/
void DMA_Interrupt(void)
{
  U8 i;
  U32 sum1=0,sum2=0;//���ADת��ֵ�����
  if(DMA_GetITStatus(DMA1_IT_TC1)) //ͨ��1��������ж�
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
    AD_Value1=sum1/CNT;	  //���ADC1ͨ������ƽ��ֵ
    AD_Value2=sum2/CNT;   //���ADC2ͨ������ƽ��ֵ
    sum1=0;
    sum2=0; 
  }
}

/*******************************************
*�������ƣ�void ADC1_Process_Pot(u16 ADValue)
*�������ܣ�ADC1ת��
*���������ADValue--ADת��ֵ
*����ֵ�� ��
*����˵����
********************************************/
float ADC1_Process_Pot(void)
{
	float cpu_vol = 0.0f;
  cpu_vol = (AD_Value2*3.2)/4095;		//�õ���ѹֵ,��λ10mV
	return cpu_vol;
}

/*******************************************
*�������ƣ�void ADC1_Process_Temp(u16 ADValue)
*�������ܣ�ADC1ת��
*���������ADValue--ADת��ֵ
*����ֵ�� ��
*����˵����
********************************************/
float ADC1_Process_Temp(void)
{
  float ADTemp = 0,cpu_temp=0.0f,cpu_vol = 0.0f;
	cpu_vol = ADC1_Process_Pot();
  ADTemp = (AD_Value1*3.2)/4095;		//�õ���ѹֵ
	if(cpu_vol<=1.43)
	{
    cpu_temp = ((1.43-ADTemp)/0.0043)+25;  //�¶ȼ��㹫ʽ
	}
	else
	{
		cpu_temp = ((ADTemp-cpu_vol)/0.0043)+25;  //�¶ȼ��㹫ʽ
	}
	return cpu_temp;
}

