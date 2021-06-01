void DMA1_Channel2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_IRQn 0 */
  #define DMAx DMA1
  #define Channel 2
  DMAx->IFCR |= 2<<(4*(Channel-1)); // Channel transfer complete flag
  // GPIO_Pin_Set(GPIOB, 4); // chip select pin PB4
  GPIOB->ODR = 0b1111111111111011;
  CLEAR_BIT(TIM2->CCER, TIM_CCER_CC1E); ; // stop cynchro pin on PA0, the PWM on CH1 pin
  // LL_TIM_OC_DisablePreload(TIM2, LL_TIM_CHANNEL_CH2);
  /* USER CODE END DMA1_Channel2_IRQn 0 */

  /* USER CODE BEGIN DMA1_Channel2_IRQn 1 */

  /* USER CODE END DMA1_Channel2_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
  LL_TIM_ClearFlag_CC1(TIM2);
  /* USER CODE END TIM2_IRQn 0 */
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}
