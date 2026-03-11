#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "stm32f4xx_ll_gpio.h"
#include "rtos_handles.h"
#include "FreeRTOS.h"
#include "task.h"
#include "config.h"

#define CMD_BUF_SIZE 30

extern volatile char cmdBuf[CMD_BUF_SIZE];
extern volatile uint8_t cmdIndex;


void CLI_ParseCommand(char *cmd);
void print_menu(void);
void menu(void);
