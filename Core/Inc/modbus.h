#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>

/* ================= BUFFER SIZES ================= */

#define MODBUS_RX_BUFFER_SIZE   128
#define MODBUS_TX_BUFFER_SIZE   128


/* ================= PUBLIC FUNCTIONS ================= */

void Modbus_StoreByte(uint8_t byte);
void Modbus_Process(void);


/* ================= DEBUG VARIABLES ================= */

extern volatile uint8_t process_frame_flag;
extern volatile uint32_t isr_counter;

#endif