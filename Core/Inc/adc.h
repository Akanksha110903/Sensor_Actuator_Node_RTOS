#include <stdint.h>
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"



extern  volatile uint16_t adc_buf[2];
void GPIO_Init_For_ADC(void);
void DMA_Init_For_ADC(void);
void ADC_Init(void);
float ADC_To_Volt(void);
float ADC_To_Temperature(void);

