#include "Application_task.h"
#include "modbus.h"
#include "monitor_helper.h"

uint32_t notifyCount;
volatile uint8_t pwm_percentage;//variable to hold the percentage value for pwm signal duty cycle which will be set by cli task or modbus task and control task will update the duty cycle of pwm signal according to this percentage value



void Communication_task(void *pvParameters)
{
    while (1)
    {
        // Communication task code
        if(process_frame_flag)
        {
        xSemaphoreTake(sensorMutex, portMAX_DELAY);//mutextake
        Modbus_Process();
        xSemaphoreGive(sensorMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ADC_task(void *pvParameters)
{
    print_menu();
    while (1)
    {
        // ADC task code
        xSemaphoreTake(sensorMutex, portMAX_DELAY);//mutextake
        
        //update the value in sendsor buffer
        g_sensorData.temperature = ADC_To_Temperature();
        g_sensorData.voltage = ADC_To_Volt();
        
        xSemaphoreGive(sensorMutex);//mutex give
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void Accelerometer_task(void *pvParameters)
{
    while (1)
    {
        // Accelerometer task code
        notifyCount =ulTaskNotifyTake(pdTRUE,portMAX_DELAY);//wait to get DMA interrupt notification
        xSemaphoreTake(sensorMutex, portMAX_DELAY);//mutextake

		//update the value in sendsor buffer
		g_sensorData.accel_x = raw_x;
		g_sensorData.accel_y = raw_y;
		g_sensorData.accel_z = raw_z;

		xSemaphoreGive(sensorMutex);//mutex give

		vTaskDelay(pdMS_TO_TICKS(10));
    }
}



void Control_task(void *pvParameters)
{
    while (1)
    {
        // Control task code
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);//wait to get notification from cli task or modbus task
        uint8_t percentage = g_sensorData.led_duty; 
        if(percentage > 100)
        {
            percentage = 100; // Cap the percentage at 100%
        }
        uint32_t CRR = (ARR*percentage)/100; //update the duty cycle of pwm signal according to the percentage value set by cli or modbus task
        LL_TIM_OC_SetCompareCH1(TIM3,CRR);
        if(g_sensorData.do_state)
        {
            LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9); // Set PC9 high using LL function
        }
        else
        {
            LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_9); // Set PC9 low using LL function
        }
    }
}

void CLI_task(void *pvParameters)
{
    while (1)
    {
        // CLI task code
        ulTaskNotifyTake(pdTRUE,portMAX_DELAY);//wait to get notification from USART3 interrupt handler
        CLI_ParseCommand((char *)cmdBuf);//parse the command received from USART3 interrupt handler
        
    }
}

void Print_task(void *pvParameters)
{ 
    char rxBuf[100];
    while (1)
    {
        // Print task code
        xQueueReceive(printDataQueue, rxBuf, portMAX_DELAY);//wait to receive data from other tasks to print
        USART3_SendString(rxBuf);//send the data to usart for printing
    }
}

void Monitor_task(void *pvParameters)
{
    TickType_t lastWake;

    lastWake = xTaskGetTickCount();

    while(1)
    {
        /* Update system state */
        Monitor_UpdateSystemState();

        /* Toggle heartbeat LED */
        Monitor_ToggleHeartbeat();

        /* Refresh watchdog */
        Monitor_WatchdogRefresh();

        /* Run every 1 second */
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(10));
    }

}