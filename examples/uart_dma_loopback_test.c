/**
 * @file    uart_dma_loopback_test.c
 * @brief   UART loopback smoke test using DMA on S32K144
 * @details
 * Simple application that proves the DMA + UART drivers work together. The demo
 * continuously transfers a 32-byte pattern through LPUART1 using DMA channel 0
 * for TX and DMA channel 1 for RX. TX and RX pins (PTC7/PTC6) must be tied
 * together externally so the transmitted frame can be observed by the receiver.
 * A green LED (PTD16) toggles every successful iteration and the console prints
 * pass/fail status along with the received payload.
 *
 * Hardware wiring:
 *  - Short PTC7 (LPUART1_TX) to PTC6 (LPUART1_RX) for local loopback
 *  - Optional USB/UART dongle for observing frames at 115200-8N1
 *  - PTD16 LED (on-board green) used as status indicator
 */

#include "clocks_and_modes.h"
#include "pcc.h"
#include "port.h"
#include "gpio.h"
#include "uart.h"
#include "dma.h"
#include "status.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/********************************* Configuration ********************************/ 
#define UART_INSTANCE                    LPUART1
#define UART_INSTANCE_INDEX              (1U)
#define UART_TX_PIN                      (7U)      /* PTC7 */
#define UART_RX_PIN                      (6U)      /* PTC6 */
#define UART_PORT                        PORT_C
#define UART_BAUDRATE                    (115200U)
#define UART_SRC_CLOCK_HZ                (40000000U) /* Bus clock */

#define DMA_CHANNEL_TX                   (0U)
#define DMA_CHANNEL_RX                   (1U)

#define LOOPBACK_TEST_BYTES              (32U)

#define STATUS_LED_PORT                  GPIO_PORT_D
#define STATUS_LED_PIN                   (16U)     /* PTD16 */

/******************************* Static Buffers *********************************/
static uint8_t s_txBuffer[LOOPBACK_TEST_BYTES];
static uint8_t s_rxBuffer[LOOPBACK_TEST_BYTES];

static volatile bool s_txDone = false;
static volatile bool s_rxDone = false;

/*************************** Forward Declarations ******************************/
static void SystemClock_Config(void);
static void PortClock_Config(void);
static void StatusLed_Init(void);
static void Uart_Init(void);
static void Dma_InitForUart(void);
static void DmaTxCallback(uint8_t channel, void *userData);
static void DmaRxCallback(uint8_t channel, void *userData);
static bool RunLoopbackOnce(void);
static void DelayMs(uint32_t ms);

/***************************** Helper Functions *********************************/
static void SystemClock_Config(void)
{
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
}

static void PortClock_Config(void)
{
    (void)PCC_EnablePeripheralClock(PCC_PORTC_INDEX);
    (void)PCC_EnablePeripheralClock(PCC_PORTD_INDEX);
}

static void StatusLed_Init(void)
{
    PORT_SetPinMux(PORT_D, STATUS_LED_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, 1U); /* LED off */
}

static void Uart_Init(void)
{
    UART_Config_t config;

    UART_EnableClock(UART_INSTANCE_INDEX);
    PORT_SetPinMux(UART_PORT, UART_TX_PIN, PORT_MUX_ALT2);
    PORT_SetPinMux(UART_PORT, UART_RX_PIN, PORT_MUX_ALT2);

    UART_GetDefaultConfig(&config);
    config.baudRate = UART_BAUDRATE;

    (void)UART_Init(UART_INSTANCE, &config, UART_SRC_CLOCK_HZ);
}

static void Dma_InitForUart(void)
{
    if (DMA_Init() != STATUS_SUCCESS) {
        printf("DMA init failed\r\n");
        for (;;) {
            /* Trap - DMA must be available for this test */
        }
    }

    (void)UART_ConfigTxDMA(UART_INSTANCE, DMA_CHANNEL_TX);
    (void)UART_ConfigRxDMA(UART_INSTANCE, DMA_CHANNEL_RX);

    DMA_InstallCallback(DMA_CHANNEL_TX, DmaTxCallback, NULL);
    DMA_InstallCallback(DMA_CHANNEL_RX, DmaRxCallback, NULL);

    DMA_EnableChannelInterrupt(DMA_CHANNEL_TX);
    DMA_EnableChannelInterrupt(DMA_CHANNEL_RX);
}

static void DmaTxCallback(uint8_t channel, void *userData)
{
    (void)channel;
    (void)userData;
    s_txDone = true;
}

static void DmaRxCallback(uint8_t channel, void *userData)
{
    (void)channel;
    (void)userData;
    s_rxDone = true;
}

static bool RunLoopbackOnce(void)
{
    /* Prepare a simple incrementing pattern */
    for (uint32_t i = 0U; i < LOOPBACK_TEST_BYTES; i++) {
        s_txBuffer[i] = (uint8_t)(i + 0x31U); /* ASCII range for readability */
    }
    memset(s_rxBuffer, 0, sizeof(s_rxBuffer));

    s_txDone = false;
    s_rxDone = false;

    if (UART_ReceiveDMA(UART_INSTANCE, DMA_CHANNEL_RX, s_rxBuffer, LOOPBACK_TEST_BYTES) != UART_STATUS_SUCCESS) {
        return false;
    }

    if (UART_SendDMA(UART_INSTANCE, DMA_CHANNEL_TX, s_txBuffer, LOOPBACK_TEST_BYTES) != UART_STATUS_SUCCESS) {
        return false;
    }

    uint32_t timeout = 2000000UL;
    while ((!s_txDone || !s_rxDone) && (timeout-- > 0UL)) {
        __asm("NOP");
    }

    if (!s_txDone || !s_rxDone) {
        return false;
    }

    return (memcmp(s_txBuffer, s_rxBuffer, LOOPBACK_TEST_BYTES) == 0);
}

static void DelayMs(uint32_t ms)
{
    volatile uint32_t loops = ms * 20000UL;
    while (loops-- > 0UL) {
        __asm("NOP");
    }
}

/*********************************** Main **************************************/
int main(void)
{
    SystemClock_Config();
    PortClock_Config();
    StatusLed_Init();
    Uart_Init();
    Dma_InitForUart();

    printf("\r\n=======================================\r\n");
    printf(" UART DMA Loopback Test (LPUART1)\r\n");
    printf(" TX Pin : PTC7\r\n");
    printf(" RX Pin : PTC6\r\n");
    printf(" DMA TX : Channel %u\r\n", DMA_CHANNEL_TX);
    printf(" DMA RX : Channel %u\r\n", DMA_CHANNEL_RX);
    printf(" Buffer : %u bytes\r\n", (unsigned)LOOPBACK_TEST_BYTES);
    printf("=======================================\r\n\r\n");

    while (1) {
        bool pass = RunLoopbackOnce();

        if (pass) {
            GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, 0U); /* LED on */
            printf("DMA loopback PASS - %.*s\r\n", (int)LOOPBACK_TEST_BYTES, (char *)s_rxBuffer);
        } else {
            GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, 1U); /* LED off */
            printf("DMA loopback FAIL\r\n");
        }

        DelayMs(500U);
        GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
        DelayMs(500U);
    }
}
