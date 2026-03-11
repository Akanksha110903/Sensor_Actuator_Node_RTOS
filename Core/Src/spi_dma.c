#include "spi_dma.h"
#include "FreeRTOS.h"
#include "task.h"

#define LIS3DSH_CS_PORT    GPIOE
#define LIS3DSH_CS_PIN     LL_GPIO_PIN_3
#define OUT_X_L            0x28

/* Change streams */


int16_t raw_x, raw_y, raw_z;

/* Circular buffers */
static uint8_t tx_buffer[14];
static uint8_t rx_buffer[14];

extern TaskHandle_t accelero_handle;

void SPI1_LL_Init(void)
{
    /* Enable clocks */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    /* SPI pins */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ALTERNATE);

    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_5, LL_GPIO_AF_5);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_6, LL_GPIO_AF_5);
    LL_GPIO_SetAFPin_0_7(GPIOA, LL_GPIO_PIN_7, LL_GPIO_AF_5);

    /* CS pin */
    LL_GPIO_SetPinMode(LIS3DSH_CS_PORT, LIS3DSH_CS_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetOutputPin(LIS3DSH_CS_PORT, LIS3DSH_CS_PIN);

    /* SPI config */
    LL_SPI_SetTransferDirection(SPI1, LL_SPI_FULL_DUPLEX);
    LL_SPI_SetMode(SPI1, LL_SPI_MODE_MASTER);
    LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
    LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
    LL_SPI_SetBaudRatePrescaler(SPI1, LL_SPI_BAUDRATEPRESCALER_DIV16);
    LL_SPI_SetTransferBitOrder(SPI1, LL_SPI_MSB_FIRST);
    LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
    LL_SPI_SetNSSMode(SPI1, LL_SPI_NSS_SOFT);
    LL_SPI_Enable(SPI1);

    /* Prepare TX buffer */
    for(int i=0;i<14;i++)
        tx_buffer[i] = 0xFF;

    tx_buffer[0] = 0xC0 | OUT_X_L;
    tx_buffer[7] = 0xC0 | OUT_X_L;

    /* ================= RX DMA CONFIG ================= */
    LL_DMA_SetChannelSelection(DMA2, SPI1_DMA_RX_STREAM, SPI1_DMA_CHANNEL);
    LL_DMA_SetDataTransferDirection(DMA2, SPI1_DMA_RX_STREAM,
                                    LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_DMA_SetPeriphIncMode(DMA2, SPI1_DMA_RX_STREAM, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA2, SPI1_DMA_RX_STREAM, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA2, SPI1_DMA_RX_STREAM, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA2, SPI1_DMA_RX_STREAM, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_SetMode(DMA2, SPI1_DMA_RX_STREAM, LL_DMA_MODE_CIRCULAR);

    LL_DMA_SetMemoryAddress(DMA2, SPI1_DMA_RX_STREAM, (uint32_t)rx_buffer);
    LL_DMA_SetPeriphAddress(DMA2, SPI1_DMA_RX_STREAM, (uint32_t)&SPI1->DR);
    LL_DMA_SetDataLength(DMA2, SPI1_DMA_RX_STREAM, 14);

    /* ================= TX DMA CONFIG ================= */
    LL_DMA_SetChannelSelection(DMA2, SPI1_DMA_TX_STREAM, SPI1_DMA_CHANNEL);
    LL_DMA_SetDataTransferDirection(DMA2, SPI1_DMA_TX_STREAM,
                                    LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetPeriphIncMode(DMA2, SPI1_DMA_TX_STREAM, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA2, SPI1_DMA_TX_STREAM, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA2, SPI1_DMA_TX_STREAM, LL_DMA_PDATAALIGN_BYTE);
    LL_DMA_SetMemorySize(DMA2, SPI1_DMA_TX_STREAM, LL_DMA_MDATAALIGN_BYTE);
    LL_DMA_SetMode(DMA2, SPI1_DMA_TX_STREAM, LL_DMA_MODE_CIRCULAR);

    LL_DMA_SetMemoryAddress(DMA2, SPI1_DMA_TX_STREAM, (uint32_t)tx_buffer);
    LL_DMA_SetPeriphAddress(DMA2, SPI1_DMA_TX_STREAM, (uint32_t)&SPI1->DR);
    LL_DMA_SetDataLength(DMA2, SPI1_DMA_TX_STREAM, 14);

    /* Enable interrupts for Stream2 */
    LL_DMA_EnableIT_HT(DMA2, SPI1_DMA_RX_STREAM);
    LL_DMA_EnableIT_TC(DMA2, SPI1_DMA_RX_STREAM);

    NVIC_SetPriority(DMA2_Stream2_IRQn, 6);
    NVIC_EnableIRQ(DMA2_Stream2_IRQn);

    LL_SPI_EnableDMAReq_RX(SPI1);
    LL_SPI_EnableDMAReq_TX(SPI1);
}
void SPI1_LL_CS_Low(void) { LL_GPIO_ResetOutputPin(LIS3DSH_CS_PORT, LIS3DSH_CS_PIN); }
void SPI1_LL_CS_High(void) { LL_GPIO_SetOutputPin(LIS3DSH_CS_PORT, LIS3DSH_CS_PIN); }

void SPI1_LL_StartStreaming(void)
{
    SPI1_LL_CS_Low();
    LL_DMA_EnableStream(DMA2, SPI1_DMA_RX_STREAM);
    LL_DMA_EnableStream(DMA2, SPI1_DMA_TX_STREAM);
}

/* ================= DMA2 STREAM2 ISR ================= */
void DMA2_Stream2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Half Transfer */
    if(LL_DMA_IsActiveFlag_HT2(DMA2))
    {
        LL_DMA_ClearFlag_HT2(DMA2);

        raw_x = (int16_t)((rx_buffer[2] << 8) | rx_buffer[1]);
        raw_y = (int16_t)((rx_buffer[4] << 8) | rx_buffer[3]);
        raw_z = (int16_t)((rx_buffer[6] << 8) | rx_buffer[5]);

        vTaskNotifyGiveFromISR(accelerometer_task_handle, &xHigherPriorityTaskWoken);
    }

    /* Full Transfer */
    if(LL_DMA_IsActiveFlag_TC2(DMA2))
    {
        LL_DMA_ClearFlag_TC2(DMA2);

        raw_x = (int16_t)((rx_buffer[9] << 8) | rx_buffer[8]);
        raw_y = (int16_t)((rx_buffer[11] << 8) | rx_buffer[10]);
        raw_z = (int16_t)((rx_buffer[13] << 8) | rx_buffer[12]);

        vTaskNotifyGiveFromISR(accelerometer_task_handle, &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
