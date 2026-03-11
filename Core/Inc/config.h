#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

/* ================= MODBUS DEFAULTS ================= */
#define MODBUS_DEFAULT_ID      1
#define MODBUS_DEFAULT_BAUD    9600

/* ================= FIRMWARE INFO =================== */
#define FW_VERSION_MAJOR   1
#define FW_VERSION_MINOR   0
#define FIRMWARE_VERSION   (FW_VERSION_MAJOR * 10 + FW_VERSION_MINOR)

/* ================= REGISTER COUNTS ================= */
#define HOLDING_REG_COUNT  5
#define INPUT_REG_COUNT    7

/* ============== HOLDING REGISTER MAP ============== */
#define HR_LED_DUTY        0
#define HR_DO_STATE        1
#define HR_MODBUS_ID       2
#define HR_BAUD_RATE       3
#define HR_FW_VERSION      4   /* Read Only */

/* =============== INPUT REGISTER MAP =============== */
#define IR_ACCEL_X         0
#define IR_ACCEL_Y         1
#define IR_ACCEL_Z         2
#define IR_TEMPERATURE     3   // x10  Temparature values are multiplied with 10
#define IR_VOLTAGE         4  //  x100 voltage values are multiplied with 100 as cant show float
#define IR_DI_STATE        5
#define IR_UPTIME          6   /* seconds since boot */

/* =============== Write multiple REGISTER MAP =============== */
#define HR_LED_DUTY        0
#define HR_DO_STATE        1
#define HR_MODBUS_ID       2
#define HR_BAUD_RATE       3

extern uint16_t g_modbus_id ;
extern uint32_t g_modbus_baud;

#endif