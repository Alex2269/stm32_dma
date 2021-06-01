/* USER CODE BEGIN PFP */
void DMA1_Channel1_TransferComplete(void);
/* USER CODE END PFP */

  /* USER CODE BEGIN DMA1_Channel5_IRQn 0 */
  LL_DMA_ClearFlag_TC5(DMA1);
  /* USER CODE END DMA1_Channel5_IRQn 0 */
