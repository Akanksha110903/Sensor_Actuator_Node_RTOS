#include "monitor_helper.h"
#include <stdio.h>
#include "rtos_handles.h"
#include "monitor_helper.h"

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_utils.h"

#include "FreeRTOS.h"
#include "task.h"

/* ================= GLOBAL STATE ================= */


SemaphoreHandle_t systemStateMutex;
system_state_t g_systemState = {0};



/* ================= MUTEX ================= */

void Monitor_CreateMutex(void)
{
    systemStateMutex = xSemaphoreCreateMutex();
}


/* ================= LED ================= */

void Monitor_LED_Init(void)
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

    LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_12, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(GPIOD, LL_GPIO_PIN_12, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_12, LL_GPIO_SPEED_FREQ_LOW);
}


void Monitor_ToggleHeartbeat(void)
{
    LL_GPIO_TogglePin(GPIOD, LL_GPIO_PIN_12);
}


/* ================= WATCHDOG ================= */

void Monitor_WatchdogInit(void)
{
    LL_IWDG_Enable(IWDG);

    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_64);

    LL_IWDG_SetReloadCounter(IWDG, 2000);

    while (LL_IWDG_IsReady(IWDG) != 1);

    LL_IWDG_ReloadCounter(IWDG);
}


void Monitor_WatchdogRefresh(void)
{
    LL_IWDG_ReloadCounter(IWDG);
}


/* ================= SYSTEM STATE UPDATE ================= */

void Monitor_UpdateSystemState(void)
{
    uint32_t ticks = xTaskGetTickCount();

    if(systemStateMutex != NULL)
    {
        xSemaphoreTake(systemStateMutex, portMAX_DELAY);

        g_systemState.uptime_sec = ticks / configTICK_RATE_HZ;
        g_systemState.free_heap = xPortGetFreeHeapSize();
        g_systemState.task_count = uxTaskGetNumberOfTasks();

        xSemaphoreGive(systemStateMutex);
    }
}


/* ================= PRINT INFO ================= */

// void Monitor_PrintSystemInfo(void)
// {
//     if(systemStateMutex != NULL)
//     {
//         xSemaphoreTake(systemStateMutex, portMAX_DELAY);

//         printf("\n====== SYSTEM INFO ======\n");

//         printf("Uptime: %lu sec\n", system_state.uptime_sec);

//         printf("Free Heap: %lu bytes\n", system_state.free_heap);

//         printf("Task Count: %u\n", system_state.task_count);

//         printf("Build: %s\n", build_info);

//         printf("=========================\n");

//         xSemaphoreGive(systemStateMutex);
//     }
// }