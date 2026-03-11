#ifndef MONITOR_HELPER_H
#define MONITOR_HELPER_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_iwdg.h"
#include <stdint.h>
#include "stm32f4xx_ll_rcc.h"



/* ================= INIT FUNCTIONS ================= */

void Monitor_LED_Init(void);
void Monitor_WatchdogInit(void);
void Monitor_CreateMutex(void);


/* ================= MONITOR FUNCTIONS ================= */

void Monitor_ToggleHeartbeat(void);
void Monitor_WatchdogRefresh(void);

void Monitor_UpdateSystemState(void);

/* ================= INFO PRINT ================= */

// void Monitor_PrintSystemInfo(void);

#endif