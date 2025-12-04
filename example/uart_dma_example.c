/**
 * @file    uart_dma_example.c
 * @brief   UART DMA Transfer Example for S32K144
 * @details
 * Ví dụ sử dụng UART với DMA để truyền và nhận dữ liệu.
 * DMA giúp giảm tải CPU bằng cách tự động chuyển dữ liệu giữa UART và memory.
 * 
 * Hardware Setup:
 * - LPUART1 TX: PTC7
 * - LPUART1 RX: PTC6
 * - Baudrate: 115200 bps
 * - DMA Channel 0: UART TX
 * - DMA Channel 1: UART RX
 * 
 * Features:
 * - DMA-based UART transmission (non-blocking)
 * - DMA-based UART reception (non-blocking)
 * - DMA callback for transfer completion
 * 
 * @author  PhucPH32
 * @date    04/12/2025
 * @version 1.0
 */

#include "uart.h"
#include "dma.h"
#include "port.h"
#include "gpio.h"
#include "pcc_reg.h"
#include "clocks_and_modes.h"
#include <string.h>
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* UART Configuration */
#define UART_INSTANCE               LPUART1
#define UART_PORT                   PORT_C
#define UART_TX_PIN                 (7U)
#define UART_RX_PIN                 (6U)
#define UART_BAUDRATE               (115200U)
#define UART_SRC_CLOCK              (40000000U)  /* 40 MHz from SOSCDIV2 */

/* DMA Configuration */
#define DMA_CHANNEL_TX              (0U)
#define DMA_CHANNEL_RX              (1U)

/* LED Configuration */
#define LED_GREEN_PORT              GPIO_PORT_D
#define LED_GREEN_PIN               (16U)
#define LED_RED_PORT                GPIO_PORT_D
#define LED_RED_PIN                 (15U)

/* Buffer Configuration */
#define TX_BUFFER_SIZE              (128U)
#define RX_BUFFER_SIZE              (128U)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static uint8_t g_txBuffer[TX_BUFFER_SIZE];
static uint8_t g_rxBuffer[RX_BUFFER_SIZE];
static volatile bool g_txComplete = false;
static volatile bool g_rxComplete = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void InitClocks(void);
static void InitLEDs(void);
static void InitUART(void);
static void InitDMA(void);
static void DMA_TxCallback(uint8_t channel, void *userData);
static void DMA_RxCallback(uint8_t channel, void *userData);
static void SimpleDelay(uint32_t ms);

/*******************************************************************************
 * DMA Callbacks
 ******************************************************************************/

/**
 * @brief DMA TX complete callback
 */
static void DMA_TxCallback(uint8_t channel, void *userData)
{
    (void)channel;
    (void)userData;
    
    g_txComplete = true;
    
    /* Toggle Green LED to indicate TX complete */
    GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
}

/**
 * @brief DMA RX complete callback
 */
static void DMA_RxCallback(uint8_t channel, void *userData)
{
    (void)channel;
    (void)userData;
    
    g_rxComplete = true;
    
    /* Toggle Red LED to indicate RX complete */
    GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN);
}

/*******************************************************************************
 * Initialization Functions
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 */
static void InitClocks(void)
{
    /* Enable clocks for PORTC (UART) */
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;
    
    /* Enable clocks for PORTD (LEDs) */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
}

/**
 * @brief Initialize LEDs
 */
static void InitLEDs(void)
{
    /* Configure Green LED */
    PORT_SetPinMux(LED_GREEN_PORT, LED_GREEN_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, 1U);  /* Off */
    
    /* Configure Red LED */
    PORT_SetPinMux(LED_RED_PORT, LED_RED_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_RED_PORT, LED_RED_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, 1U);  /* Off */
}

/**
 * @brief Initialize UART peripheral
 */
static void InitUART(void)
{
    UART_Config_t uartConfig;
    
    /* Enable UART clock */
    UART_EnableClock(1);  /* LPUART1 */
    
    /* Configure UART pins */
    PORT_SetPinMux(UART_PORT, UART_TX_PIN, PORT_MUX_ALT2);  /* PTC7 = LPUART1_TX */
    PORT_SetPinMux(UART_PORT, UART_RX_PIN, PORT_MUX_ALT2);  /* PTC6 = LPUART1_RX */
    
    /* Configure UART */
    UART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate = UART_BAUDRATE;
    
    /* Initialize UART */
    UART_Init(UART_INSTANCE, &uartConfig, UART_SRC_CLOCK);
}

/**
 * @brief Initialize DMA
 */
static void InitDMA(void)
{
    /* Initialize DMA module */
    DMA_Init();
    
    /* Configure UART TX DMA */
    UART_ConfigTxDMA(UART_INSTANCE, DMA_CHANNEL_TX);
    
    /* Configure UART RX DMA */
    UART_ConfigRxDMA(UART_INSTANCE, DMA_CHANNEL_RX);
    
    /* Install DMA callbacks */
    DMA_InstallCallback(DMA_CHANNEL_TX, DMA_TxCallback, NULL);
    DMA_InstallCallback(DMA_CHANNEL_RX, DMA_RxCallback, NULL);
    
    /* Enable DMA interrupts */
    DMA_EnableChannelInterrupt(DMA_CHANNEL_TX);
    DMA_EnableChannelInterrupt(DMA_CHANNEL_RX);
}

/**
 * @brief Simple delay
 */
static void SimpleDelay(uint32_t ms)
{
    volatile uint32_t count = ms * 20000;
    while (count--) {
        __asm("NOP");
    }
}

/*******************************************************************************
 * Example Functions
 ******************************************************************************/

/**
 * @brief Example 1: Basic DMA Transmission
 */
static void Example_BasicDmaTx(void)
{
    const char *message = "Hello DMA TX!\r\n";
    uint32_t length = strlen(message);
    
    printf("\n=== Example 1: Basic DMA TX ===\n");
    printf("Sending: %s", message);
    
    /* Copy message to TX buffer */
    memcpy(g_txBuffer, message, length);
    
    /* Reset completion flag */
    g_txComplete = false;
    
    /* Start DMA transmission */
    UART_SendDMA(UART_INSTANCE, DMA_CHANNEL_TX, g_txBuffer, length);
    
    /* Wait for completion */
    while (!g_txComplete) {
        /* Could do other work here */
        __asm("NOP");
    }
    
    printf("TX Complete!\n");
}

/**
 * @brief Example 2: Basic DMA Reception
 */
static void Example_BasicDmaRx(void)
{
    uint32_t rxLength = 10U;  /* Expect 10 bytes */
    
    printf("\n=== Example 2: Basic DMA RX ===\n");
    printf("Waiting to receive %lu bytes...\n", rxLength);
    printf("Please send data via UART terminal.\n");
    
    /* Clear RX buffer */
    memset(g_rxBuffer, 0, RX_BUFFER_SIZE);
    
    /* Reset completion flag */
    g_rxComplete = false;
    
    /* Start DMA reception */
    UART_ReceiveDMA(UART_INSTANCE, DMA_CHANNEL_RX, g_rxBuffer, rxLength);
    
    /* Wait for completion (with timeout) */
    uint32_t timeout = 1000000U;
    while (!g_rxComplete && (timeout > 0U)) {
        timeout--;
    }
    
    if (g_rxComplete) {
        printf("RX Complete! Received: ");
        for (uint32_t i = 0; i < rxLength; i++) {
            printf("%c", g_rxBuffer[i]);
        }
        printf("\n");
    } else {
        printf("RX Timeout!\n");
    }
}

/**
 * @brief Example 3: Blocking DMA Transfer
 */
static void Example_BlockingDmaTransfer(void)
{
    const char *message = "Blocking DMA Transfer\r\n";
    uint32_t length = strlen(message);
    UART_Status_t status;
    
    printf("\n=== Example 3: Blocking DMA Transfer ===\n");
    
    /* Copy message to TX buffer */
    memcpy(g_txBuffer, message, length);
    
    /* Send using blocking DMA (function waits until complete) */
    status = UART_SendDMABlocking(UART_INSTANCE, DMA_CHANNEL_TX, g_txBuffer, length);
    
    if (status == UART_STATUS_SUCCESS) {
        printf("Blocking TX completed successfully!\n");
    } else {
        printf("Blocking TX failed with status: %d\n", status);
    }
}

/**
 * @brief Example 4: Echo with DMA
 */
static void Example_EchoWithDMA(void)
{
    uint32_t echoLength = 20U;
    
    printf("\n=== Example 4: Echo with DMA ===\n");
    printf("Send %lu characters, they will be echoed back.\n", echoLength);
    
    /* Clear RX buffer */
    memset(g_rxBuffer, 0, RX_BUFFER_SIZE);
    
    /* Receive data using DMA (blocking) */
    UART_Status_t rxStatus = UART_ReceiveDMABlocking(UART_INSTANCE, DMA_CHANNEL_RX, 
                                                     g_rxBuffer, echoLength);
    
    if (rxStatus == UART_STATUS_SUCCESS) {
        printf("Received %lu bytes, echoing back...\n", echoLength);
        
        /* Echo back using DMA (blocking) */
        UART_Status_t txStatus = UART_SendDMABlocking(UART_INSTANCE, DMA_CHANNEL_TX,
                                                      g_rxBuffer, echoLength);
        
        if (txStatus == UART_STATUS_SUCCESS) {
            printf("Echo complete!\n");
        } else {
            printf("Echo TX failed!\n");
        }
    } else {
        printf("Receive timeout!\n");
    }
}

/**
 * @brief Example 5: Large Data Transfer
 */
static void Example_LargeDataTransfer(void)
{
    uint32_t dataSize = TX_BUFFER_SIZE;
    
    printf("\n=== Example 5: Large Data Transfer ===\n");
    printf("Sending %lu bytes via DMA...\n", dataSize);
    
    /* Fill buffer with pattern */
    for (uint32_t i = 0; i < dataSize; i++) {
        g_txBuffer[i] = (uint8_t)(i & 0xFF);
    }
    
    /* Reset completion flag */
    g_txComplete = false;
    
    /* Start DMA transmission */
    UART_SendDMA(UART_INSTANCE, DMA_CHANNEL_TX, g_txBuffer, dataSize);
    
    /* Wait for completion */
    while (!g_txComplete) {
        /* Could update progress bar here */
        __asm("NOP");
    }
    
    printf("Large transfer complete!\n");
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main function
 */
int main(void)
{
    /* Initialize system */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    
    InitClocks();
    InitLEDs();
    InitUART();
    InitDMA();
    
    printf("\n");
    printf("===========================================\n");
    printf("  UART DMA Transfer Example\n");
    printf("===========================================\n");
    printf("Hardware:\n");
    printf("  - UART: LPUART1 (PTC6/PTC7)\n");
    printf("  - Baudrate: 115200 bps\n");
    printf("  - DMA TX: Channel 0\n");
    printf("  - DMA RX: Channel 1\n");
    printf("  - Green LED: TX complete indicator\n");
    printf("  - Red LED: RX complete indicator\n");
    printf("===========================================\n\n");
    
    /* Run examples */
    while (1) {
        /* Example 1: Basic DMA TX */
        Example_BasicDmaTx();
        SimpleDelay(1000);
        
        /* Example 2: Basic DMA RX (comment out if no input available) */
        // Example_BasicDmaRx();
        // SimpleDelay(1000);
        
        /* Example 3: Blocking DMA Transfer */
        Example_BlockingDmaTransfer();
        SimpleDelay(1000);
        
        /* Example 4: Echo with DMA (comment out if no input available) */
        // Example_EchoWithDMA();
        // SimpleDelay(1000);
        
        /* Example 5: Large Data Transfer */
        Example_LargeDataTransfer();
        SimpleDelay(2000);
        
        printf("\n--- Loop complete, restarting in 3 seconds ---\n\n");
        SimpleDelay(3000);
    }
    
    return 0;
}

/**
 * @brief DMA Channel 0 IRQ Handler (TX)
 */
void DMA0_IRQHandler(void)
{
    DMA_IRQHandler(DMA_CHANNEL_TX);
}

/**
 * @brief DMA Channel 1 IRQ Handler (RX)
 */
void DMA1_IRQHandler(void)
{
    DMA_IRQHandler(DMA_CHANNEL_RX);
}
