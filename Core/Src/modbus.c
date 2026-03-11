#include "modbus.h"
#include "config.h"
#include "crc16.h"
#include "stm32f4xx_ll_usart.h"
#include "rtos_handles.h"
#include "FreeRTOS.h"
#include "task.h"

static uint8_t rx_buffer[MODBUS_RX_BUFFER_SIZE];
static uint8_t tx_buffer[MODBUS_TX_BUFFER_SIZE];

static uint16_t rx_index = 0;

volatile uint8_t process_frame_flag = 0;
volatile uint32_t isr_counter = 0;




extern uint16_t g_modbus_id;
extern uint32_t g_modbus_baud;


/* ================= STORE BYTE FROM ISR ================= */

void Modbus_StoreByte(uint8_t byte)
{
    rx_buffer[rx_index++] = byte;

    if (rx_index >= MODBUS_RX_BUFFER_SIZE)
        rx_index = 0;

    /* Wait until we have minimum Modbus frame */
    if (rx_index < 8)
        return;

    uint8_t func = rx_buffer[1];
    uint16_t expected_len = 8;

    /* FC 0x10 has variable length */
    if(func == 0x10)
    {
        uint8_t byte_count = rx_buffer[6];
        expected_len = 9 + byte_count;
    }

    /* If full frame received */
    if(rx_index >= expected_len)
    {
        uint16_t crc_calc = Modbus_CRC16(rx_buffer, expected_len - 2);
        uint16_t crc_recv = rx_buffer[expected_len-2] | (rx_buffer[expected_len-1] << 8);

        if(crc_calc == crc_recv)
        {
            process_frame_flag = 1;
        }
    }
}


/* ================= UART SEND ================= */

static void Modbus_Send(uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        while (!LL_USART_IsActiveFlag_TXE(USART2));
        LL_USART_TransmitData8(USART2, tx_buffer[i]);
    }

    while (!LL_USART_IsActiveFlag_TC(USART2));
}


/* ================= MAIN MODBUS PROCESS ================= */

void Modbus_Process(void)

{    
    if (!process_frame_flag)
        return;
    
    process_frame_flag = 0;

    uint8_t slave = rx_buffer[0];
    uint8_t func  = rx_buffer[1];

    if (slave != g_modbus_id)
    {
        rx_index = 0;
        return;
    }

    uint16_t crc_calc = Modbus_CRC16(rx_buffer, rx_index - 2);
    uint16_t crc_recv = rx_buffer[rx_index-2] | (rx_buffer[rx_index-1] << 8);

    if (crc_calc != crc_recv)
    {
        rx_index = 0;
        return;
    }

    /* ================= FC 0x03 ================= */

    if (func == 0x03)
    {
        uint16_t start = (rx_buffer[2] << 8) | rx_buffer[3];
        uint16_t qty   = (rx_buffer[4] << 8) | rx_buffer[5];

        uint16_t holding[HOLDING_REG_COUNT];

        holding[HR_LED_DUTY]  = g_sensorData.led_duty;
        holding[HR_DO_STATE]  = g_sensorData.do_state;
        holding[HR_MODBUS_ID] = g_modbus_id;
        holding[HR_BAUD_RATE] = g_modbus_baud;
        holding[HR_FW_VERSION]= FIRMWARE_VERSION;

        tx_buffer[0] = g_modbus_id;
        tx_buffer[1] = 0x03;
        tx_buffer[2] = qty * 2;

        for(uint16_t i=0;i<qty;i++)
        {
            uint16_t val = holding[start+i];

            tx_buffer[3 + i*2] = val >> 8;
            tx_buffer[4 + i*2] = val & 0xFF;
        }

        uint16_t crc = Modbus_CRC16(tx_buffer, 3 + qty*2);

        tx_buffer[3 + qty*2] = crc & 0xFF;
        tx_buffer[4 + qty*2] = crc >> 8;

        Modbus_Send(5 + qty*2);
    }

    /* ================= FC 0x04 ================= */

    else if(func == 0x04)
    {
        uint16_t start = (rx_buffer[2] << 8) | rx_buffer[3];
        uint16_t qty   = (rx_buffer[4] << 8) | rx_buffer[5];

        int16_t input[INPUT_REG_COUNT];
        
        xSemaphoreTake(systemStateMutex, portMAX_DELAY);      
        uint32_t uptime = g_systemState.uptime_sec;
        xSemaphoreGive(systemStateMutex);
        

        

        input[IR_ACCEL_X] = g_sensorData.accel_x;
        input[IR_ACCEL_Y] = g_sensorData.accel_y;
        input[IR_ACCEL_Z] = g_sensorData.accel_z;
        input[IR_TEMPERATURE] = (int16_t)(g_sensorData.temperature * 10);
        input[IR_VOLTAGE] = (int16_t)(g_sensorData.voltage * 100);
        input[IR_DI_STATE] = g_sensorData.di_state;
        input[IR_UPTIME] = (uint16_t)uptime;

        tx_buffer[0] = g_modbus_id;
        tx_buffer[1] = 0x04;
        tx_buffer[2] = qty * 2;

        for(uint16_t i=0;i<qty;i++)
        {
            uint16_t val = input[start+i];

            tx_buffer[3 + i*2] = val >> 8;
            tx_buffer[4 + i*2] = val & 0xFF;
        }

        uint16_t crc = Modbus_CRC16(tx_buffer, 3 + qty*2);

        tx_buffer[3 + qty*2] = crc & 0xFF;
        tx_buffer[4 + qty*2] = crc >> 8;

        Modbus_Send(5 + qty*2);
    }

    /* ================= FC 0x06 ================= */

   /* ================= FC 0x06 ================= */

else if(func == 0x06)
{
    uint16_t reg = (rx_buffer[2] << 8) | rx_buffer[3];
    uint16_t val = (rx_buffer[4] << 8) | rx_buffer[5];

    if(reg == HR_LED_DUTY)
        g_sensorData.led_duty = val;

    else if(reg == HR_DO_STATE)
        g_sensorData.do_state = val;

    else if(reg == HR_MODBUS_ID)
        g_modbus_id = val;

    else if(reg == HR_BAUD_RATE)
        g_modbus_baud = val;

    /* Echo request */
    for(int i=0;i<8;i++)
        tx_buffer[i] = rx_buffer[i];

    Modbus_Send(8);
}

    /* ================= FC 0x10 ================= */

   else if(func == 0x10)
{
    uint16_t start = (rx_buffer[2] << 8) | rx_buffer[3];
    uint16_t qty   = (rx_buffer[4] << 8) | rx_buffer[5];

    if(start + qty > HOLDING_REG_COUNT)
    {
        rx_index = 0;
        return;
    }

    for(uint16_t i = 0; i < qty; i++)
    {
        uint16_t val = (rx_buffer[7 + i*2] << 8) |
                       (rx_buffer[8 + i*2]);

        uint16_t reg = start + i;

        if(reg == HR_LED_DUTY)
            g_sensorData.led_duty = val;

        else if(reg == HR_DO_STATE)
            g_sensorData.do_state = val;

        else if(reg == HR_MODBUS_ID)
            g_modbus_id = val;

        else if(reg == HR_BAUD_RATE)
            g_modbus_baud = val;
    }

    /* Response frame */
    tx_buffer[0] = g_modbus_id;
    tx_buffer[1] = 0x10;
    tx_buffer[2] = rx_buffer[2];
    tx_buffer[3] = rx_buffer[3];
    tx_buffer[4] = rx_buffer[4];
    tx_buffer[5] = rx_buffer[5];

    uint16_t crc = Modbus_CRC16(tx_buffer, 6);

    tx_buffer[6] = crc & 0xFF;
    tx_buffer[7] = crc >> 8;

    Modbus_Send(8);
}

    rx_index = 0;
}