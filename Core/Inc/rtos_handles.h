#ifndef RTOS_HANDLES_H
#define RTOS_HANDLES_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"


typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    float temperature;
    float voltage;

    uint16_t led_duty;      // reg 5
    uint16_t do_state;
    uint16_t di_state; 
    

} sensor_state_t;

typedef struct
{
    uint32_t uptime_sec;
    uint32_t free_heap;
    uint16_t task_count;

} system_state_t;


extern TaskHandle_t communication_task_handle;
extern TaskHandle_t adc_task_handle;
extern TaskHandle_t accelerometer_task_handle;
extern TaskHandle_t control_task_handle;
extern TaskHandle_t cli_task_handle;
extern TaskHandle_t print_task_handle;
extern TaskHandle_t monitor_task_handle;
extern sensor_state_t g_sensorData;
extern SemaphoreHandle_t sensorMutex;
extern QueueHandle_t printDataQueue;
extern system_state_t g_systemState;
extern SemaphoreHandle_t systemStateMutex;


#endif /* RTOS_HANDLES_H */