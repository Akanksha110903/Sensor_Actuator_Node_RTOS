#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_gpio.h"
#include "rtos_handles.h"

#define ARR 20999 // Auto-reload value for 2 Hz (assuming 84 MHz timer clock with prescaler of 8400)


void TIM_Init(void);
void TIM3_Init(void);
