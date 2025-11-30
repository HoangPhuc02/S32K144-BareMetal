/**
 * @file    can_example.c
 * @brief   FlexCAN Driver Examples for S32K144
 * @details Tập hợp các ví dụ sử dụng CAN driver
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 */

#include "can.h"
#include "gpio.h"
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* CAN message IDs */
#define CAN_ID_SENSOR_DATA      (0x100U)
#define CAN_ID_CONTROL_CMD      (0x200U)
#define CAN_ID_STATUS           (0x300U)
#define CAN_ID_HEARTBEAT        (0x400U)

/* LED pin */
#define LED_PORT                GPIO_PORT_D
#define LED_PIN                 (15U)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static volatile bool g_rxMessageReceived = false;
static can_message_t g_lastRxMessage;

/*******************************************************************************
 * Example 1: Basic CAN Send and Receive (Polling)
 ******************************************************************************/

void Example1_BasicSendReceive(void)
{
    status_t status;
    can_message_t txMsg, rxMsg;
    
    /* Configure CAN0 */
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,    /* 24 MHz */
        .baudRate = 500000,                      /* 500 Kbps */
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    
    /* Initialize CAN */
    status = CAN_Init(&config);
    if (status != STATUS_SUCCESS) {
        printf("CAN Init failed!\n");
        return;
    }
    
    /* Configure RX filter for ID 0x123 */
    can_rx_filter_t rxFilter = {
        .id = 0x123,
        .mask = 0x7FF,          /* Match all 11 bits */
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 16, &rxFilter);
    
    /* Prepare TX message */
    txMsg.id = 0x123;
    txMsg.idType = CAN_ID_STD;
    txMsg.frameType = CAN_FRAME_DATA;
    txMsg.dataLength = 8;
    txMsg.data[0] = 0x01;
    txMsg.data[1] = 0x02;
    txMsg.data[2] = 0x03;
    txMsg.data[3] = 0x04;
    txMsg.data[4] = 0x05;
    txMsg.data[5] = 0x06;
    txMsg.data[6] = 0x07;
    txMsg.data[7] = 0x08;
    
    /* Send message */
    status = CAN_SendBlocking(0, 8, &txMsg, 100);
    if (status == STATUS_SUCCESS) {
        printf("Message sent successfully!\n");
    }
    
    /* Receive message (polling) */
    while (1) {
        status = CAN_Receive(0, 16, &rxMsg);
        if (status == STATUS_SUCCESS) {
            printf("Received message ID: 0x%X, Data: ", rxMsg.id);
            for (uint8_t i = 0; i < rxMsg.dataLength; i++) {
                printf("%02X ", rxMsg.data[i]);
            }
            printf("\n");
            break;
        }
    }
}

/*******************************************************************************
 * Example 2: CAN with Interrupt Callbacks
 ******************************************************************************/

void RxCallback(uint8_t instance, uint8_t mbIndex, void *userData)
{
    /* Đọc message trong callback */
    if (CAN_Receive(instance, mbIndex, &g_lastRxMessage) == STATUS_SUCCESS) {
        g_rxMessageReceived = true;
        
        /* Toggle LED khi nhận message */
        static bool ledState = false;
        if (ledState) {
            GPIO_SetPin(LED_PORT, LED_PIN);
        } else {
            GPIO_ClearPin(LED_PORT, LED_PIN);
        }
        ledState = !ledState;
    }
}

void TxCallback(uint8_t instance, uint8_t mbIndex, void *userData)
{
    printf("Message transmitted successfully from MB%d\n", mbIndex);
}

void ErrorCallback(uint8_t instance, uint32_t errorFlags, void *userData)
{
    printf("CAN Error detected: 0x%08X\n", errorFlags);
    
    if (errorFlags & CAN_ESR1_BOFFINT_MASK) {
        printf("  - Bus Off Error\n");
    }
    if (errorFlags & CAN_ESR1_ERRINT_MASK) {
        printf("  - Error Interrupt\n");
    }
    if (errorFlags & CAN_ESR1_TWRNINT_MASK) {
        printf("  - TX Warning\n");
    }
    if (errorFlags & CAN_ESR1_RWRNINT_MASK) {
        printf("  - RX Warning\n");
    }
}

void Example2_InterruptMode(void)
{
    status_t status;
    can_message_t txMsg;
    
    /* Initialize CAN */
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    
    status = CAN_Init(&config);
    if (status != STATUS_SUCCESS) {
        return;
    }
    
    /* Configure RX filter */
    can_rx_filter_t rxFilter = {
        .id = CAN_ID_SENSOR_DATA,
        .mask = 0x7FF,
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 16, &rxFilter);
    
    /* Install callbacks */
    CAN_InstallRxCallback(0, 16, RxCallback, NULL);
    CAN_InstallTxCallback(0, 8, TxCallback, NULL);
    CAN_InstallErrorCallback(0, ErrorCallback, NULL);
    
    /* Enable NVIC interrupts (cần implement trong startup code) */
    // NVIC_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
    // NVIC_EnableIRQ(CAN0_ORed_16_31_MB_IRQn);
    
    /* Send periodic messages */
    while (1) {
        /* Prepare message */
        txMsg.id = CAN_ID_SENSOR_DATA;
        txMsg.idType = CAN_ID_STD;
        txMsg.frameType = CAN_FRAME_DATA;
        txMsg.dataLength = 4;
        txMsg.data[0] = 0xAA;
        txMsg.data[1] = 0xBB;
        txMsg.data[2] = 0xCC;
        txMsg.data[3] = 0xDD;
        
        /* Send message */
        status = CAN_Send(0, 8, &txMsg);
        
        /* Check if message received */
        if (g_rxMessageReceived) {
            printf("Received: ID=0x%X, DLC=%d\n", 
                   g_lastRxMessage.id, 
                   g_lastRxMessage.dataLength);
            g_rxMessageReceived = false;
        }
        
        /* Delay 100ms */
        for (volatile uint32_t i = 0; i < 100000; i++);
    }
}

/*******************************************************************************
 * Example 3: Extended ID (29-bit)
 ******************************************************************************/

void Example3_ExtendedID(void)
{
    status_t status;
    can_message_t txMsg, rxMsg;
    
    /* Initialize CAN */
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    
    CAN_Init(&config);
    
    /* Configure RX filter for Extended ID */
    can_rx_filter_t rxFilter = {
        .id = 0x12345678,           /* 29-bit Extended ID */
        .mask = 0x1FFFFFFF,         /* Match all 29 bits */
        .idType = CAN_ID_EXT
    };
    CAN_ConfigRxFilter(0, 16, &rxFilter);
    
    /* Send Extended ID message */
    txMsg.id = 0x12345678;
    txMsg.idType = CAN_ID_EXT;
    txMsg.frameType = CAN_FRAME_DATA;
    txMsg.dataLength = 8;
    txMsg.data[0] = 0x11;
    txMsg.data[1] = 0x22;
    txMsg.data[2] = 0x33;
    txMsg.data[3] = 0x44;
    txMsg.data[4] = 0x55;
    txMsg.data[5] = 0x66;
    txMsg.data[6] = 0x77;
    txMsg.data[7] = 0x88;
    
    status = CAN_SendBlocking(0, 8, &txMsg, 100);
    if (status == STATUS_SUCCESS) {
        printf("Extended ID message sent!\n");
    }
    
    /* Receive Extended ID message */
    status = CAN_ReceiveBlocking(0, 16, &rxMsg, 1000);
    if (status == STATUS_SUCCESS) {
        printf("Received Extended ID: 0x%08X\n", rxMsg.id);
    }
}

/*******************************************************************************
 * Example 4: Loopback Mode (Self-test)
 ******************************************************************************/

void Example4_LoopbackMode(void)
{
    status_t status;
    can_message_t txMsg, rxMsg;
    
    /* Initialize CAN in Loopback mode */
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_LOOPBACK,          /* Loopback mode */
        .enableSelfReception = true,        /* Enable self-reception */
        .useRxFifo = false
    };
    
    status = CAN_Init(&config);
    if (status != STATUS_SUCCESS) {
        printf("Loopback Init failed!\n");
        return;
    }
    
    /* Configure RX filter */
    can_rx_filter_t rxFilter = {
        .id = 0x555,
        .mask = 0x7FF,
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 16, &rxFilter);
    
    /* Send message */
    txMsg.id = 0x555;
    txMsg.idType = CAN_ID_STD;
    txMsg.frameType = CAN_FRAME_DATA;
    txMsg.dataLength = 4;
    txMsg.data[0] = 0xDE;
    txMsg.data[1] = 0xAD;
    txMsg.data[2] = 0xBE;
    txMsg.data[3] = 0xEF;
    
    printf("Sending loopback message...\n");
    status = CAN_SendBlocking(0, 8, &txMsg, 100);
    
    /* In loopback mode, message will be received immediately */
    status = CAN_ReceiveBlocking(0, 16, &rxMsg, 1000);
    if (status == STATUS_SUCCESS) {
        printf("Loopback test PASSED!\n");
        printf("Received ID: 0x%X, Data: ", rxMsg.id);
        for (uint8_t i = 0; i < rxMsg.dataLength; i++) {
            printf("%02X ", rxMsg.data[i]);
        }
        printf("\n");
    } else {
        printf("Loopback test FAILED!\n");
    }
}

/*******************************************************************************
 * Example 5: Multiple Message Buffers
 ******************************************************************************/

void Example5_MultipleMBs(void)
{
    status_t status;
    can_message_t txMsg1, txMsg2, txMsg3;
    can_message_t rxMsg;
    
    /* Initialize CAN */
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    
    CAN_Init(&config);
    
    /* Configure multiple RX filters */
    can_rx_filter_t rxFilter1 = {
        .id = CAN_ID_SENSOR_DATA,
        .mask = 0x7FF,
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 16, &rxFilter1);
    
    can_rx_filter_t rxFilter2 = {
        .id = CAN_ID_CONTROL_CMD,
        .mask = 0x7FF,
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 17, &rxFilter2);
    
    can_rx_filter_t rxFilter3 = {
        .id = CAN_ID_STATUS,
        .mask = 0x7FF,
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 18, &rxFilter3);
    
    /* Send different messages using different TX MBs */
    
    /* Message 1: Sensor data */
    txMsg1.id = CAN_ID_SENSOR_DATA;
    txMsg1.idType = CAN_ID_STD;
    txMsg1.frameType = CAN_FRAME_DATA;
    txMsg1.dataLength = 4;
    txMsg1.data[0] = 0x12;  /* Temperature */
    txMsg1.data[1] = 0x34;
    txMsg1.data[2] = 0x56;  /* Humidity */
    txMsg1.data[3] = 0x78;
    CAN_Send(0, 8, &txMsg1);
    
    /* Message 2: Control command */
    txMsg2.id = CAN_ID_CONTROL_CMD;
    txMsg2.idType = CAN_ID_STD;
    txMsg2.frameType = CAN_FRAME_DATA;
    txMsg2.dataLength = 2;
    txMsg2.data[0] = 0x01;  /* Command: Start */
    txMsg2.data[1] = 0xFF;  /* Parameter */
    CAN_Send(0, 9, &txMsg2);
    
    /* Message 3: Status */
    txMsg3.id = CAN_ID_STATUS;
    txMsg3.idType = CAN_ID_STD;
    txMsg3.frameType = CAN_FRAME_DATA;
    txMsg3.dataLength = 1;
    txMsg3.data[0] = 0xAA;  /* Status: OK */
    CAN_Send(0, 10, &txMsg3);
    
    /* Receive from multiple MBs */
    while (1) {
        /* Check MB16 (Sensor data) */
        if (CAN_Receive(0, 16, &rxMsg) == STATUS_SUCCESS) {
            printf("Sensor Data: Temp=%02X%02X, Hum=%02X%02X\n",
                   rxMsg.data[0], rxMsg.data[1], 
                   rxMsg.data[2], rxMsg.data[3]);
        }
        
        /* Check MB17 (Control command) */
        if (CAN_Receive(0, 17, &rxMsg) == STATUS_SUCCESS) {
            printf("Control Command: CMD=%02X, Param=%02X\n",
                   rxMsg.data[0], rxMsg.data[1]);
        }
        
        /* Check MB18 (Status) */
        if (CAN_Receive(0, 18, &rxMsg) == STATUS_SUCCESS) {
            printf("Status: %02X\n", rxMsg.data[0]);
        }
    }
}

/*******************************************************************************
 * Example 6: Error Monitoring
 ******************************************************************************/

void Example6_ErrorMonitoring(void)
{
    status_t status;
    can_error_state_t errorState;
    uint8_t txErrors, rxErrors;
    
    /* Initialize CAN */
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    
    CAN_Init(&config);
    
    /* Monitoring loop */
    while (1) {
        /* Get error state */
        status = CAN_GetErrorState(0, &errorState);
        if (status == STATUS_SUCCESS) {
            switch (errorState) {
                case CAN_ERROR_ACTIVE:
                    printf("CAN Status: Error Active (Normal)\n");
                    break;
                case CAN_ERROR_PASSIVE:
                    printf("CAN Status: Error Passive (Warning!)\n");
                    break;
                case CAN_ERROR_BUS_OFF:
                    printf("CAN Status: Bus Off (Critical!)\n");
                    /* Need to reinitialize CAN */
                    CAN_Deinit(0);
                    CAN_Init(&config);
                    break;
            }
        }
        
        /* Get error counters */
        status = CAN_GetErrorCounters(0, &txErrors, &rxErrors);
        if (status == STATUS_SUCCESS) {
            printf("Error Counters - TX: %d, RX: %d\n", txErrors, rxErrors);
            
            if (txErrors > 96 || rxErrors > 96) {
                printf("Warning: Error count approaching passive threshold!\n");
            }
        }
        
        /* Delay 1 second */
        for (volatile uint32_t i = 0; i < 1000000; i++);
    }
}

/*******************************************************************************
 * Example 7: Remote Frame
 ******************************************************************************/

void Example7_RemoteFrame(void)
{
    status_t status;
    can_message_t remoteRequest, dataResponse;
    
    /* Initialize CAN */
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    
    CAN_Init(&config);
    
    /* Node A: Send Remote Frame request */
    remoteRequest.id = 0x456;
    remoteRequest.idType = CAN_ID_STD;
    remoteRequest.frameType = CAN_FRAME_REMOTE;  /* Remote frame */
    remoteRequest.dataLength = 8;                /* DLC of requested data */
    
    printf("Sending Remote Frame request...\n");
    CAN_Send(0, 8, &remoteRequest);
    
    /* Node B: Receive Remote Frame and respond with Data Frame */
    can_rx_filter_t rxFilter = {
        .id = 0x456,
        .mask = 0x7FF,
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 16, &rxFilter);
    
    /* Wait for remote request */
    status = CAN_ReceiveBlocking(0, 16, &dataResponse, 1000);
    if (status == STATUS_SUCCESS && 
        dataResponse.frameType == CAN_FRAME_REMOTE) {
        
        printf("Remote Frame received, sending data response...\n");
        
        /* Send data response */
        dataResponse.frameType = CAN_FRAME_DATA;
        dataResponse.data[0] = 0x11;
        dataResponse.data[1] = 0x22;
        dataResponse.data[2] = 0x33;
        dataResponse.data[3] = 0x44;
        dataResponse.data[4] = 0x55;
        dataResponse.data[5] = 0x66;
        dataResponse.data[6] = 0x77;
        dataResponse.data[7] = 0x88;
        
        CAN_Send(0, 9, &dataResponse);
    }
}

/*******************************************************************************
 * Example 8: CAN Gateway (2 CAN interfaces)
 ******************************************************************************/

void Example8_CANGateway(void)
{
    status_t status;
    can_message_t msg;
    
    /* Initialize CAN0 (500 Kbps) */
    can_config_t config0 = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    CAN_Init(&config0);
    
    /* Initialize CAN1 (250 Kbps) */
    can_config_t config1 = {
        .instance = 1,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 250000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    CAN_Init(&config1);
    
    /* Configure RX filters */
    can_rx_filter_t rxFilter = {
        .id = 0x000,
        .mask = 0x000,      /* Accept all messages */
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 16, &rxFilter);
    CAN_ConfigRxFilter(1, 16, &rxFilter);
    
    printf("CAN Gateway started...\n");
    
    /* Gateway loop: Forward messages between CAN0 and CAN1 */
    while (1) {
        /* CAN0 -> CAN1 */
        if (CAN_Receive(0, 16, &msg) == STATUS_SUCCESS) {
            printf("CAN0->CAN1: ID=0x%X\n", msg.id);
            CAN_Send(1, 8, &msg);
        }
        
        /* CAN1 -> CAN0 */
        if (CAN_Receive(1, 16, &msg) == STATUS_SUCCESS) {
            printf("CAN1->CAN0: ID=0x%X\n", msg.id);
            CAN_Send(0, 8, &msg);
        }
    }
}

/*******************************************************************************
 * Main function để chạy examples
 ******************************************************************************/

int main(void)
{
    /* Initialize system */
    // SystemInit();
    
    /* Configure CAN pins */
    // PTE4 = CAN0_RX, PTE5 = CAN0_TX
    // PORT_ConfigurePin(PORTE, 4, PORT_MUX_ALT5);
    // PORT_ConfigurePin(PORTE, 5, PORT_MUX_ALT5);
    
    /* Run example */
    // Example1_BasicSendReceive();
    // Example2_InterruptMode();
    // Example3_ExtendedID();
    // Example4_LoopbackMode();
    // Example5_MultipleMBs();
    // Example6_ErrorMonitoring();
    // Example7_RemoteFrame();
    // Example8_CANGateway();
    
    while (1) {
        /* Main loop */
    }
    
    return 0;
}
