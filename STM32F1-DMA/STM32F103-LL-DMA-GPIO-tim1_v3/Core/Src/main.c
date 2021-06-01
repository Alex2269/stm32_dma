/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t data[] = {0x0101, 0x0202, 0x0303, 0x0404, 0x0505, 0x0606, 0x0707, 0x0808, 0x0909, 0xa0a0, 0xb0b0, 0xc0c0, 0xd0d0, 0xe0e0, 0xf0f0, 0x1f1f, 0x2f2f}; // Data to transfer

// static uint16_t cmmd_mask = 0b1110000000000000 | (1<<RD) | (0<<WR) | (0<<DC) | (0<<CS) | (1<<RST);
// GPIOB->ODR = cmmd_mask | eightbit;
// static uint16_t data_mask = 0b1110000000000000 | (1<<RD) | (0<<WR) | (1<<DC) | (0<<CS) | (1<<RST);
// GPIOB->ODR = data_mask | eightbit;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
	int DataIdx;
	for(DataIdx = 0; DataIdx < len; DataIdx++)
	{
	   ITM_SendChar( *ptr++ );
	}
	return len;
}

void dma_clock_irq(uint32_t Channel)
{
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  /* DMA1 interrupt init */
  /* DMA1 Channel IRQn interrupt configuration */
  NVIC_SetPriority(Channel+10, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(Channel+10);
}

void dma_setting(DMA_TypeDef *DMAx, uint32_t Channel)
{
  LL_DMA_SetDataTransferDirection(DMAx, Channel, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
  LL_DMA_SetChannelPriorityLevel(DMAx, Channel, LL_DMA_PRIORITY_VERYHIGH);
  LL_DMA_SetMode(DMAx, Channel, LL_DMA_MODE_NORMAL);
  LL_DMA_SetPeriphIncMode(DMAx, Channel, LL_DMA_PERIPH_NOINCREMENT);
  LL_DMA_SetMemoryIncMode(DMAx, Channel, LL_DMA_MEMORY_INCREMENT);
  LL_DMA_SetPeriphSize(DMAx, Channel, LL_DMA_PDATAALIGN_HALFWORD);
  LL_DMA_SetMemorySize(DMAx, Channel, LL_DMA_MDATAALIGN_HALFWORD);
}

void timer_setting(TIM_TypeDef *TIMx)
{
  LL_APB2_GRP1_EnableClock(RCC_APB2ENR_TIM1EN);
  LL_TIM_DisableCounter(TIMx);
  LL_TIM_SetPrescaler(TIMx, 0);
  LL_TIM_EnableARRPreload(TIMx);
  // LL_TIM_EnableAllOutputs(TIMx);
  LL_TIM_SetAutoReload(TIMx, 72-1);
  LL_TIM_EnableCounter(TIMx);
}

void dma_transmit(DMA_TypeDef *DMAx, uint32_t SrcAddress, uint32_t DstAddress, uint32_t Channel, uint32_t DataLength)
{
  LL_DMA_DisableChannel(DMAx, Channel);

  DMAx->IFCR |= 2<<(4*(Channel-1)); // Channel transfer complete flag
  DMAx->IFCR |= 4<<(4*(Channel-1)); // Channel transfer error flag

  LL_DMA_SetDataLength(DMAx, Channel, DataLength);
  LL_DMA_ConfigAddresses(DMAx, Channel, (uint32_t)SrcAddress, (uint32_t)DstAddress, LL_DMA_GetDataTransferDirection(DMAx, Channel));

  LL_DMA_EnableIT_TC(DMAx, Channel); // Enable Transfer complete interrupt
  LL_DMA_EnableIT_TE(DMAx, Channel); // Enable Transfer error interrupt
  LL_DMA_EnableChannel(DMAx, Channel);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /** NOJTAG: JTAG-DP Disabled and SW-DP Enabled
  */
  LL_GPIO_AF_Remap_SWJ_NOJTAG();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  dma_clock_irq(5);
  dma_setting(DMA1, 5);
  timer_setting(TIM1);

  dma_transmit(DMA1, (uint32_t)data, ((uint32_t)&GPIOB->ODR), 5, 17);
  TIM1->DIER |= TIM_DIER_UDE; // LL_TIM_EnableDMAReq_UPDATE(TIM1);

  printf("Running\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    printf("DMA Transmit...\n");
    dma_transmit(DMA1, (uint32_t)data, ((uint32_t)&GPIOB->ODR), 5, 17);
    TIM1->DIER |= TIM_DIER_UDE; // LL_TIM_EnableDMAReq_UPDATE(TIM1);
    // for(uint16_t i =0; i<1e3L; i++) __asm volatile ("nop");

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(72000000);
  LL_SetSystemCoreClock(72000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  // printf("Error! %s:%d", file, line);
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
