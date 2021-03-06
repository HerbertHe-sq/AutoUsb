/**
  ******************************************************************************
  * @file    stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "includes.h"

extern void Sysclk_Interrupt(void);

/** @addtogroup STM32F0-Discovery_Demo
  * @{
  */

/** @addtogroup STM32F0XX_IT
  * @brief Interrupts driver modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	Sysclk_Interrupt();
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/**
  * @brief  This function USART1 IRQ.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
	USART_Rec();
}

void TIM2_IRQHandler(void)
{ 
  TIME_IRQHandler();
}

void TIM3_IRQHandler(void)
{ 
  PWM_IRQHandler();
}

void TIM16_IRQHandler(void)
{ 
	TIM_OUT_Handler(); 
}

/**
  *@brief  DMA1_Channel1中断服务函数
  *@param  无
  *@retval 无
  */
void DMA1_Channel2_3_IRQHandler(void)  
{  
	/*判断DMA传输完成中断*/
	if(DMA_GetITStatus(DMA1_IT_TC2)!= RESET)                        
	{
	  /*清除DMA中断标志位*/   
	  DMA_ClearITPendingBit(DMA1_IT_TC2); 
	}	                    
}


//void EXTI0_1_IRQHandler(void)
//{
// if(EXTI_GetITStatus(EXTI_Line0) != RESET || EXTI_GetITStatus(EXTI_Line1) != RESET)
// {
//	 /* Clear the EXTI line 0 pending bit */
//	 EXTI_ClearITPendingBit(EXTI_Line0);
//	 EXTI_ClearITPendingBit(EXTI_Line1);
//	 PWR_Sleep_Mode(1);
//	 NVIC_SystemReset();//复位重启
// }
//}
/*******************************************
*函数名称：DMA1_Channel1_IRQHandler
*函数功能：DMA转换完成中断
*输入参数：无
*返回值： 无
*函数说明：
********************************************/
//void DMA1_Channel1_IRQHandler(void)
//{
//	DMA_Interrupt();
//}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
