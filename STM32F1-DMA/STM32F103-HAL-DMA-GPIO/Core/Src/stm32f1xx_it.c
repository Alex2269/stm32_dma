/* USER CODE BEGIN 0 */
extern TIM_HandleTypeDef htim1;

/* USER CODE END 0 */

  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */
  __HAL_TIM_DISABLE_DMA(&htim1, TIM_DMA_UPDATE); // Disable TIM1 for synchronous restart with DMA
  /* USER CODE END DMA1_Channel5_IRQn 0 */
