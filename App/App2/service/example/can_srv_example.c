/**
 * @file    can_srv_example.c
 * @brief   CAN Service Example
 * @details Demonstrate cách sử dụng CAN service (FlexCAN)
 * 
 * Hardware Setup:
 * - CAN0_TX: PTE5
 * - CAN0_RX: PTE4
 * - CAN transceiver required (MCP2551, TJA1050, etc.)
 * - Connect to CAN bus
 * 
 * Expected Behavior:
 * - Send CAN messages periodically
 * - Receive CAN messages
 * - Print via UART
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 */

#include "../inc/can_srv.h"
#include "../inc/uart_srv.h"
#include "../inc/clock_srv_v2.h"
#include <stdio.h>

/* CAN Message IDs */
#define CAN_ID_SENSOR_DATA      0x100
#define CAN_ID_CONTROL_CMD      0x200
#define CAN_ID_STATUS           0x300

void CAN_BasicExample(void)
{
    can_srv_status_t status;
    can_srv_config_t can_cfg;
    can_srv_message_t tx_msg, rx_msg;
    
    /* 1. Initialize clock system */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    /* 2. Enable peripheral clocks */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTE, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_FLEXCAN0, CLOCK_SRV_V2_PCS_SOSCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    
    /* 3. Initialize UART for debug */
    UART_SRV_Init(115200);
    UART_SRV_SendString("\r\n=== CAN Service Example ===\r\n");
    
    /* 4. Configure CAN */
    can_cfg.baudrate = 500000;          /* 500 kbps */
    can_cfg.filter_id = CAN_ID_CONTROL_CMD;
    can_cfg.filter_mask = 0x7FF;        /* Match exact ID */
    can_cfg.filter_extended = false;    /* Standard 11-bit ID */
    
    /* 5. Initialize CAN */
    status = CAN_SRV_Init(&can_cfg);
    if (status != CAN_SRV_SUCCESS) {
        UART_SRV_SendString("CAN Init Failed!\r\n");
        while(1);
    }
    UART_SRV_SendString("CAN Initialized at 500 kbps\r\n\r\n");
    
    /* 6. Prepare TX message */
    tx_msg.id = CAN_ID_SENSOR_DATA;
    tx_msg.dlc = 8;
    tx_msg.is_extended = false;
    tx_msg.data[0] = 0x12;
    tx_msg.data[1] = 0x34;
    tx_msg.data[2] = 0x56;
    tx_msg.data[3] = 0x78;
    tx_msg.data[4] = 0x9A;
    tx_msg.data[5] = 0xBC;
    tx_msg.data[6] = 0xDE;
    tx_msg.data[7] = 0xF0;
    
    uint32_t counter = 0;
    
    /* 7. Main loop */
    while(1) {
        /* Update counter in message */
        tx_msg.data[0] = (counter >> 24) & 0xFF;
        tx_msg.data[1] = (counter >> 16) & 0xFF;
        tx_msg.data[2] = (counter >> 8) & 0xFF;
        tx_msg.data[3] = counter & 0xFF;
        
        /* Send CAN message */
        status = CAN_SRV_Send(&tx_msg);
        
        if (status == CAN_SRV_SUCCESS) {
            char buffer[64];
            sprintf(buffer, "TX: ID=0x%03X, Data[0-3]=0x%08lX\r\n", 
                    tx_msg.id, counter);
            UART_SRV_SendString(buffer);
        } else {
            UART_SRV_SendString("TX Failed!\r\n");
        }
        
        /* Try to receive message */
        status = CAN_SRV_Receive(&rx_msg);
        
        if (status == CAN_SRV_SUCCESS) {
            char buffer[128];
            sprintf(buffer, "RX: ID=0x%03X, DLC=%u, Data=[%02X %02X %02X %02X %02X %02X %02X %02X]\r\n",
                    rx_msg.id,
                    rx_msg.dlc,
                    rx_msg.data[0], rx_msg.data[1], rx_msg.data[2], rx_msg.data[3],
                    rx_msg.data[4], rx_msg.data[5], rx_msg.data[6], rx_msg.data[7]);
            UART_SRV_SendString(buffer);
        }
        
        counter++;
        
        /* Delay 1 second */
        for(volatile uint32_t i = 0; i < 2000000; i++);
    }
}

/* Example: Send sensor data */
void CAN_SensorDataExample(void)
{
    can_srv_config_t can_cfg = {
        .baudrate = 250000,     /* 250 kbps */
        .filter_id = 0,
        .filter_mask = 0,
        .filter_extended = false
    };
    
    can_srv_message_t msg;
    
    /* Initialize */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_FLEXCAN0, CLOCK_SRV_V2_PCS_SOSCDIV2);
    CAN_SRV_Init(&can_cfg);
    
    /* Simulate sensor readings */
    uint16_t temperature = 250;  /* 25.0°C */
    uint16_t pressure = 1013;    /* 101.3 kPa */
    uint16_t humidity = 650;     /* 65.0% */
    
    while(1) {
        /* Pack sensor data into CAN message */
        msg.id = CAN_ID_SENSOR_DATA;
        msg.dlc = 6;
        msg.is_extended = false;
        
        msg.data[0] = (temperature >> 8) & 0xFF;
        msg.data[1] = temperature & 0xFF;
        msg.data[2] = (pressure >> 8) & 0xFF;
        msg.data[3] = pressure & 0xFF;
        msg.data[4] = (humidity >> 8) & 0xFF;
        msg.data[5] = humidity & 0xFF;
        
        CAN_SRV_Send(&msg);
        
        /* Update values (simulate changes) */
        temperature += 1;
        pressure += 1;
        humidity += 1;
        
        /* Delay 100ms */
        for(volatile uint32_t i = 0; i < 200000; i++);
    }
}

/* Example: Extended ID (29-bit) */
void CAN_ExtendedIDExample(void)
{
    can_srv_config_t can_cfg = {
        .baudrate = 500000,
        .filter_id = 0x12345678,
        .filter_mask = 0x1FFFFFFF,
        .filter_extended = true
    };
    
    can_srv_message_t msg;
    
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_FLEXCAN0, CLOCK_SRV_V2_PCS_SOSCDIV2);
    CAN_SRV_Init(&can_cfg);
    
    /* Send with 29-bit ID */
    msg.id = 0x12345678;
    msg.dlc = 8;
    msg.is_extended = true;
    
    for(uint8_t i = 0; i < 8; i++) {
        msg.data[i] = i;
    }
    
    while(1) {
        CAN_SRV_Send(&msg);
        
        /* Delay */
        for(volatile uint32_t i = 0; i < 1000000; i++);
    }
}
