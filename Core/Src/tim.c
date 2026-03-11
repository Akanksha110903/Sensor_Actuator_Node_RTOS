#include "tim.h"


void TIM_Init(void)
{
    /* Enable clock for TIM2 */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    /* Timer base configuration
       Assume APB1 timer clock = 84 MHz (typical on F407)
       Example: want 1 kHz trigger rate
       Prescaler = 8400-1  -> 10 kHz timer clock
       ARR = 10-1          -> 1 kHz update event
    */
    LL_TIM_SetPrescaler(TIM2, 8400 - 1);
    LL_TIM_SetAutoReload(TIM2, 10 - 1);
    LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableARRPreload(TIM2);

    /* Set TRGO on update event */
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);

    /* Start timer */
    LL_TIM_EnableCounter(TIM2);
}


void TIM3_Init(void)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

    LL_GPIO_SetPinMode(GPIOC,LL_GPIO_PIN_6,LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_0_7(GPIOC,LL_GPIO_PIN_6,LL_GPIO_AF_2);


    LL_TIM_SetPrescaler(TIM3,3);
    LL_TIM_SetAutoReload(TIM3,ARR);
    LL_TIM_SetCounterMode(TIM3,LL_TIM_COUNTERMODE_UP);
    LL_TIM_EnableARRPreload(TIM3);
    LL_TIM_OC_SetMode(TIM3,LL_TIM_CHANNEL_CH1,LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetCompareCH1(TIM3,((ARR*g_sensorData.led_duty)/100));
    LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH1);
    LL_TIM_GenerateEvent_UPDATE(TIM3);
    LL_TIM_EnableCounter(TIM3);
}
