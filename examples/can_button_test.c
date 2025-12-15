/**
 * @file    can_button_test.c
 * @brief   CAN Test with Button Interrupt Example
 * @details Example sử dụng nút nhấn với ngắt để trigger việc gửi CAN messages
 *
 * Hardware Setup:
 * - Button: PTC12 (SW2 on EVB) - với pull-up, active low
 * - LED: PTD15 (Red LED) - để báo hiệu transmission
 * - CAN TX: PTE4
 * - CAN RX: PTE5
 *
 * Functionality:
 * - Nhấn nút SW2 để bắt đầu gửi CAN messages
 * - LED sáng khi đang transmit
 * - Gửi burst 10 messages với counter tăng dần
 * - Hiển thị status qua UART (optional)
 *
 * @author  PhucPH32
 * @date    03/12/2025
 * @version 1.0
 */

#include "can.h"
#include "gpio.h"
#include "port.h"
#include "nvic.h"
#include "pcc_reg.h"
#include "clocks_and_modes.h"
#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Button Configuration */
#define BUTTON_PORT             PORT_C
#define BUTTON_GPIO_PORT        GPIO_PORT_C
#define BUTTON_PIN              (12U)
#define BUTTON_IRQn             PORTC_IRQn

/* LED Configuration */
#define LED_PORT                PORT_D
#define LED_GPIO_PORT           GPIO_PORT_D
#define LED_PIN                 (15U)

/* CAN Configuration */
#define CAN_GPIO_PORT           GPIO_PORT_E
#define CAN_TX_PIN              (5U)
#define CAN_RX_PIN              (4U)             
#define CAN_TEST_ID             (0x123U)
#define CAN_TX_MAILBOX          (8U)
#define CAN_RX_MAILBOX          (16U)
#define CAN_BAUDRATE            (500000U)    /* 500 kbps */

/* Test Configuration */
#define TEST_MESSAGE_COUNT      (10U)
#define DEBOUNCE_DELAY_MS       (50U)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static volatile bool g_buttonPressed = false;
static volatile uint32_t g_messageCounter = 0;
static volatile bool g_testRunning = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void InitClocks(void);
static void InitLED(void);
static void InitButton(void);
static void InitCAN(void);
static void SendCANTestMessage(uint32_t counter);
static void SimpleDelay(uint32_t ms);

/*******************************************************************************
 * Interrupt Handlers
 ******************************************************************************/

/**
 * @brief PORTC interrupt handler (Button)
 */
void PORTC_IRQHandler(void)
{
    /* Check if interrupt is from our button pin */
    if (PORT_GetPinInterruptFlag(BUTTON_PORT, BUTTON_PIN)) {
        /* Clear interrupt flag */
        PORT_ClearPinInterruptFlag(BUTTON_PORT, BUTTON_PIN);

        /* Simple debounce - check if button is actually pressed (low) */
        if (GPIO_ReadPin(BUTTON_GPIO_PORT, BUTTON_PIN) == 0U) {
            /* Set flag to trigger test in main loop */
            g_buttonPressed = true;
        }
    }
}

/**
 * @brief CAN0 mailbox interrupt handler
 */
void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    can_message_t rxMsg;

    /* Check if message received on RX mailbox */
    // if (CAN_IsMbInterruptPending(0, CAN_RX_MAILBOX)) {
        if (CAN_Receive(0, CAN_RX_MAILBOX, &rxMsg) == STATUS_SUCCESS) {
            printf("CAN RX: ID=0x%03lX, Data=", rxMsg.id);
            for (uint8_t i = 0; i < rxMsg.dataLength; i++) {
                printf("%02X ", rxMsg.data[i]);
            }
            printf("\n");
        }
    // }

    /* Clear interrupt flag */
    // CAN_ClearMbInterruptFlag(0, CAN_RX_MAILBOX);
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 */
static void InitClocks(void)
{
    /* Enable clocks for PORTC (Button) */
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;

    /* Enable clocks for PORTD (LED) */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;

    /* Enable clocks for PORTE (CAN) */
    PCC->PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK;

    /* Enable CAN0 clock */
    PCC->PCCn[PCC_FLEXCAN0_INDEX] = PCC_PCCn_CGC_MASK;
}

/**
 * @brief Initialize LED
 */
static void InitLED(void)
{
    /* Configure LED pin as GPIO */
    PORT_SetPinMux(LED_GPIO_PORT, LED_PIN, PORT_MUX_GPIO);

    /* Configure as output */
    GPIO_SetPinDirection(LED_GPIO_PORT, LED_PIN, GPIO_DIR_OUTPUT);

    /* Turn off LED initially (LED is active low on EVB) */
    GPIO_WritePin(LED_GPIO_PORT, LED_PIN, 1U);
}

/**
 * @brief Initialize Button with interrupt
 */
static void InitButton(void)
{
    /* Configure button pin as GPIO with pull-up */
    port_pin_config_t buttonConfig = {
        .base = PORTC,
        .mux = PORT_MUX_GPIO,
        .pull = PORT_PULL_UP,
        .drive = PORT_DRIVE_LOW,
        .interrupt = PORT_INT_FALLING_EDGE,  /* Interrupt on falling edge (button press) */
        .passiveFilter = false,
        .digitalFilter = true                 /* Enable digital filter for debouncing */
    };
    PORT_ConfigurePin(BUTTON_PORT, BUTTON_PIN, &buttonConfig);

    /* Configure as input */
    GPIO_SetPinDirection(BUTTON_GPIO_PORT, BUTTON_PIN, GPIO_DIR_INPUT);

    /* Enable PORTC interrupt in NVIC */
    NVIC_SetPriority(BUTTON_IRQn, 3);
    NVIC_EnableIRQ(BUTTON_IRQn);
}

/**
 * @brief Initialize CAN peripheral
 */
static void InitCAN(void)
{
    status_t status;

    /* Configure CAN pins */
    PORT_SetPinMux(CAN_GPIO_PORT, CAN_RX_PIN, PORT_MUX_ALT5);  /* CAN0_RX */
    PORT_SetPinMux(CAN_GPIO_PORT, CAN_TX_PIN, PORT_MUX_ALT5);  /* CAN0_TX */

    /* Configure CAN */
    can_config_t canConfig = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_SOSCDIV2,    /* 40 MHz */
        .baudRate = CAN_BAUDRATE,                /* 500 kbps */
        // .mode = CAN_MODE_NORMAL,
        // .enableSelfReception = false,
        
        .mode = CAN_MODE_LOOPBACK,
        .enableSelfReception = true,
        .useRxFifo = false
    };

    status = CAN_Init(&canConfig);
    if (status != STATUS_SUCCESS) {
        printf("CAN Init failed!\n");
        return;
    }

    /* Configure RX filter to receive messages with ID 0x123 */
    can_rx_filter_t rxFilter = {
        .id = CAN_TEST_ID,
        .mask = 0x7FF,                           /* Match all 11 bits */
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, CAN_RX_MAILBOX, &rxFilter);

    /* Enable CAN interrupt in NVIC */
    NVIC_SetPriority(CAN0_ORed_0_15_MB_IRQn, 4);
    NVIC_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);

    printf("CAN initialized: 500 kbps, ID=0x%03X\n", CAN_TEST_ID);
}

/**
 * @brief Send a CAN test message with counter
 */
static void SendCANTestMessage(uint32_t counter)
{
    status_t status;
    can_message_t txMsg;

    /* Prepare test message */
    txMsg.id = CAN_TEST_ID;
    txMsg.idType = CAN_ID_STD;
    txMsg.frameType = CAN_FRAME_DATA;
    txMsg.dataLength = 8;

    /* Fill data with counter and pattern */
    txMsg.data[0] = (uint8_t)(counter >> 24);
    txMsg.data[1] = (uint8_t)(counter >> 16);
    txMsg.data[2] = (uint8_t)(counter >> 8);
    txMsg.data[3] = (uint8_t)(counter);
    txMsg.data[4] = 0xAA;
    txMsg.data[5] = 0x55;
    txMsg.data[6] = 0xDE;
    txMsg.data[7] = 0xAD;

    /* Turn on LED during transmission */
    GPIO_WritePin(LED_GPIO_PORT, LED_PIN, 0U);  /* Active low */

    /* Send message */
    status = CAN_SendBlocking(0, CAN_TX_MAILBOX, &txMsg, 100);

    if (status == STATUS_SUCCESS) {
        printf("CAN TX [%lu]: ", counter);
        for (uint8_t i = 0; i < txMsg.dataLength; i++) {
            printf("%02X ", txMsg.data[i]);
        }
        printf("\n");
    } else {
        printf("CAN TX failed: status=%d\n", status);
    }

    /* Turn off LED */
    GPIO_WritePin(LED_GPIO_PORT, LED_PIN, 1U);
}

/**
 * @brief Simple delay function
 */
static void SimpleDelay(uint32_t ms)
{
    /* Approximate delay at 80 MHz
     * Each loop iteration takes ~4 cycles
     * 80,000,000 / 4 = 20,000,000 iterations per second
     * 20,000 iterations per millisecond
     */
    volatile uint32_t count = ms * 20000;
    while (count--) {
        __asm("NOP");
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Main function
 */
int main(void)
{
    printf("\n");
    printf("===========================================\n");
    printf("  CAN Button Test Example\n");
    printf("===========================================\n");
    printf("Hardware:\n");
    printf("  - Button: PTC12 (SW2)\n");
    printf("  - LED: PTD15 (Red)\n");
    printf("  - CAN: PTE4/PTE5\n");
    printf("\n");
    printf("Operation:\n");
    printf("  1. Press SW2 button\n");
    printf("  2. System sends %d CAN messages\n", TEST_MESSAGE_COUNT);
    printf("  3. LED blinks during transmission\n");
    printf("  4. Messages displayed on console\n");
    printf("\n");
    printf("Waiting for button press...\n");
    printf("===========================================\n\n");

    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    /* Initialize peripherals */
    InitClocks();
    InitLED();
    InitButton();
    InitCAN();

    /* Main loop */
    while (1) {
        /* Check if button was pressed */
        if (g_buttonPressed && !g_testRunning) {
            g_buttonPressed = false;
            g_testRunning = true;

            printf("\n*** Button pressed! Starting CAN test ***\n");

            /* Send burst of test messages */
            for (uint32_t i = 0; i < TEST_MESSAGE_COUNT; i++) {
                SendCANTestMessage(g_messageCounter++);
                SimpleDelay(100);  /* 100ms delay between messages */
            }

            printf("*** Test complete! ***\n");
            printf("Total messages sent: %lu\n", g_messageCounter);
            printf("Waiting for next button press...\n\n");

            g_testRunning = false;

            /* Debounce delay */
            SimpleDelay(DEBOUNCE_DELAY_MS);
        }

        /* Optional: Add low power mode here */
        __asm("WFI");  /* Wait for interrupt */
    }

    return 0;
}
