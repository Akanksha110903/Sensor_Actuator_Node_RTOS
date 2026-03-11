#include "adc.h"


/* ADC buffer: [0] = Temp sensor, [1] = Potentiometer */
volatile uint16_t adc_buf[2];

 void GPIO_Init_For_ADC(void)
{

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

    /* PA0 in analog mode */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);
}

 void DMA_Init_For_ADC(void)
{
    /* Enable DMA2 clock */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    /* ADC1 is typically on DMA2 Stream0 Channel0 on F407 */
    LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_0, LL_DMA_CHANNEL_0);

    LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_0, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_0, LL_DMA_PRIORITY_HIGH);

    LL_DMA_SetMode(DMA2, LL_DMA_STREAM_0, LL_DMA_MODE_CIRCULAR);

    LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_0, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_0, LL_DMA_MEMORY_INCREMENT);

    LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_0, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_0, LL_DMA_MDATAALIGN_HALFWORD);

    /* Addresses */
    LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_0, (uint32_t)&ADC1->DR);
    LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_0, (uint32_t)adc_buf);

    /* Number of conversions = 2 channels */
    LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_0, 2);

    /* Enable the DMA stream */
    LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_0);
}

void ADC_Init(void)
{
    /* Enable clocks */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    /* GPIO for external ADC channel */
    GPIO_Init_For_ADC();

    /* DMA for ADC */
    DMA_Init_For_ADC();

    /* ADC common configuration */
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    /* Enable internal channels: Temp sensor & Vrefint */
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
                                   LL_ADC_PATH_INTERNAL_TEMPSENSOR | LL_ADC_PATH_INTERNAL_VREFINT);

    /* ADC basic configuration */
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
    LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_ENABLE);

    /* IMPORTANT: Disable continuous mode (we use timer trigger) */
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    /* External trigger from TIM2 TRGO */
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_EXT_TIM2_TRGO);
    //LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

    /* DMA transfer */
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

    /* Regular sequence length = 2 conversions */
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);

    /* Rank 1: Temperature sensor */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_112CYCLES);

    /* Rank 2: External channel PA0 = ADC1_IN0 */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_0);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_112CYCLES);

    /* Enable ADC */
    LL_ADC_Enable(ADC1);

    /* Small delay or wait for ADC ready flag (optional but recommended) */
    while (!LL_ADC_IsEnabled(ADC1))
    {
        /* wait */
    }

    /* Enable ADC DMA requests */
    LL_ADC_REG_StartConversionExtTrig(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

}


float ADC_To_Volt(void)
{
	return ((float)adc_buf[1]/4095)*(3.3f);
}



float ADC_To_Temperature(void)
{
    float vref = 3.3f;
    float v_sense = (adc_buf[0] * vref) / 4095.0f;

    // Formula from STM32 datasheet
    return ((v_sense - 0.76f) / 0.0025f) + 25.0f;
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}