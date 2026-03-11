#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_dma.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_handles.h"


#define SPI1_DMA_RX_STREAM LL_DMA_STREAM_2
#define SPI1_DMA_TX_STREAM LL_DMA_STREAM_5
#define SPI1_DMA_CHANNEL   LL_DMA_CHANNEL_3

void SPI1_LL_Init(void);
void SPI1_LL_StartStreaming(void);
void SPI1_LL_CS_Low(void);
void SPI1_LL_CS_High(void);


extern int16_t raw_x, raw_y, raw_z;

