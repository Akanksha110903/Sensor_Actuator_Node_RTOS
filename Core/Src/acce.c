#include "acce.h"

#define WHO_AM_I_REG  0x0F
#define CTRL_REG4     0x20

static uint8_t tx[2], rx[2];
static void write_reg(uint8_t reg, uint8_t val);

static void write_reg(uint8_t reg, uint8_t val)
{
    tx[0] = reg;
    tx[1] = val;

    SPI1_LL_CS_Low();
    while(!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, tx[0]);
    while(!LL_SPI_IsActiveFlag_RXNE(SPI1));
    LL_SPI_ReceiveData8(SPI1);

    while(!LL_SPI_IsActiveFlag_TXE(SPI1));
    LL_SPI_TransmitData8(SPI1, tx[1]);
    while(!LL_SPI_IsActiveFlag_RXNE(SPI1));
    LL_SPI_ReceiveData8(SPI1);

    SPI1_LL_CS_High();
}

void LIS3DSH_Init(void)
{
    write_reg(CTRL_REG4, 0x67);  // 100 Hz XYZ enable
}
