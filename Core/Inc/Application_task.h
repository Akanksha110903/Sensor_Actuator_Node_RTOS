#include "FreeRTOS.h"
#include "task.h"
#include "rtos_handles.h"
#include "spi_dma.h"
#include "adc.h"
#include "tim.h"
#include "stm32f4xx_ll_tim.h"
#include "cli.h"
#include "usart.h"


extern volatile uint8_t pwm_percentage;

// Function prototypes
void Communication_task(void *pvParameters);
void ADC_task(void *pvParameters);
void Accelerometer_task(void *pvParameters);
void Control_task(void *pvParameters);
void CLI_task(void *pvParameters);
void Print_task(void *pvParameters);
void Monitor_task(void *pvParameters);

//function prototypes