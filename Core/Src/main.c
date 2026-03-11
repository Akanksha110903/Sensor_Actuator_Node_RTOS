#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Application_task.h"
#include "adc.h"
#include "spi_dma.h"
#include "acce.h"
#include "tim.h"
#include "rtos_handles.h"
#include "clock_config.h"
#include "usart.h"
#include "modbus.h"
#include "monitor_helper.h"





// Private define
TaskHandle_t communication_task_handle;
TaskHandle_t adc_task_handle;
TaskHandle_t accelerometer_task_handle;
TaskHandle_t control_task_handle;
TaskHandle_t cli_task_handle;
TaskHandle_t print_task_handle;
TaskHandle_t monitor_task_handle;
sensor_state_t g_sensorData;
SemaphoreHandle_t sensorMutex;
QueueHandle_t printDataQueue;
BaseType_t status;

//Private define





int main(void)
{
  // System initialization
  // SystemClock_Config();
  // PeriphCommonClock_Config();
  sys_init();                //system clock and peripheral clock configuration
  USART3_UART_Init();          //USART initialization for cli task and print task
  MX_USART2_UART_Init();
  TIM3_Init();                //timer initialization for pwm signal generation for led control  
  ADC_Init();                 //adc init for temperature and potentiometer conversion
  TIM_Init(); 
  do_state_init();                //timer initialization for timer triggerd adc
  SPI1_LL_Init();             //spi initilization for accelerometer
  LIS3DSH_Init();             //accelerometer initialization
  SPI1_LL_StartStreaming();   //dma streaming for accelrometer data in DMA
   
  
    Monitor_LED_Init();

    Monitor_CreateMutex();

    Monitor_WatchdogInit();



  //Mutex creation
    sensorMutex = xSemaphoreCreateMutex();
    configASSERT(sensorMutex != NULL);
    printDataQueue = xQueueCreate(10, 100);//queue creation
    configASSERT(printDataQueue != NULL);
    status = xTaskCreate(Communication_task, "Communication",512, NULL, 4, &communication_task_handle);
    status = xTaskCreate(ADC_task, "ADC", 250, NULL, 3, &adc_task_handle);
    status = xTaskCreate(Accelerometer_task, "Accelerometer", 250, NULL, 3, &accelerometer_task_handle);
    status = xTaskCreate(Control_task, "Control", 250, NULL, 3, &control_task_handle);
    status = xTaskCreate(CLI_task, "CLI", 250, NULL, 2, &cli_task_handle);
    status = xTaskCreate(Print_task, "Print", 250, NULL, 2, &print_task_handle);
    status = xTaskCreate(Monitor_task, "Monitor", 250, NULL, 1, &monitor_task_handle);

    vTaskStartScheduler(); // Start the FreeRTOS scheduler


}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    while(1);
}

void vApplicationMallocFailedHook(void)
{
    while(1);
}



/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */



