#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"


#define RX_BUF_SIZE 128
extern volatile uint8_t rxBuf[RX_BUF_SIZE];
extern volatile uint16_t rxWrite;


void USART3_UART_Init(void);
void USART3_SendChar(char c);
void USART3_SendString(const char *str);
void MX_USART2_UART_Init(void);

