/**
 * @file    board2_main.c
 * @brief   Board 2 Application - CAN to UART Gateway
 * @details This application receives ADC data from Board 1 via CAN at 500 Kbps
 *          and forwards it to PC via UART at 9600 baud.
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "board2_main.h"
#include "../../Core/Drivers/gpio/gpio.h"
#include "../../Core/Drivers/port/port.h"
#include "../../Core/Drivers/can/can.h"
#include "../../Core/Drivers/uart/uart.h"
#include "../../Core/Drivers/clock/clock.h"
#include "../../Core/Drivers/nvic/nvic.h"

/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/** @brief LED port and pin for status indication (PTD0) */
#define LED_PORT            PORTD
#define LED_GPIO            PTD
#define LED_PIN             0U

/** @brief CAN configuration */
#define CAN_INSTANCE        0U
#define CAN_BAUDRATE        500000U   /* 500 Kbps */
#define CAN_MSG_ID          0x100U    /* Message ID for ADC data */
#define CAN_RX_MB           16U       /* Receive Message Buffer */

/** @brief UART configuration */
#define UART_INSTANCE       1U        /* LPUART1 */
#define UART_BAUDRATE       9600U     /* 9600 baud */

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Buffer for UART transmission */
static char s_uartBuffer[64];

/** @brief Flag to indicate new CAN message received */
static volatile bool s_newMessageReceived = false;

/** @brief Received CAN message */
static can_message_t s_receivedMessage;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Initialize system clock
 */
static void Board2_InitClock(void);

/**
 * @brief Initialize GPIO for LED
 */
static void Board2_InitGPIO(void);

/**
 * @brief Initialize CAN
 */
static void Board2_InitCAN(void);

/**
 * @brief Initialize UART
 */
static void Board2_InitUART(void);

/**
 * @brief CAN receive callback
 * @param[in] instance CAN instance
 * @param[in] mb Message buffer number
 * @param[in] message Received message
 */
static void Board2_CANRxCallback(uint8_t instance, uint8_t mb, const can_message_t *message);

/**
 * @brief Process received CAN message and send via UART
 */
static void Board2_ProcessMessage(void);

/**
 * @brief Convert CAN data to ADC value
 * @param[in] data CAN data bytes
 * @return ADC value
 */
static uint16_t Board2_ConvertCANDataToADC(const uint8_t *data);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Main function for Board 2
 */
void Board2_Main(void)
{
    /* Initialize system */
    Board2_InitClock();
    Board2_InitGPIO();
    Board2_InitCAN();
    Board2_InitUART();
    
    /* Enable global interrupts */
    NVIC_EnableGlobalIRQ(0U);
    
    /* Send welcome message */
    const char *welcomeMsg = "\r\n=== Board 2 - CAN to UART Gateway ===\r\n";
    UART_SendBlocking(UART_INSTANCE, (const uint8_t *)welcomeMsg, 
                      strlen(welcomeMsg), 1000U);
    
    /* Main loop */
    while (1) {
        /* Process received CAN messages */
        if (s_newMessageReceived) {
            s_newMessageReceived = false;
            Board2_ProcessMessage();
            
            /* Toggle LED to indicate activity */
            GPIO_TogglePin(LED_GPIO, LED_PIN);
        }
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Initialize system clock
 */
static void Board2_InitClock(void)
{
    clock_config_t clockConfig;
    
    /* Configure SOSC (8 MHz external crystal) */
    clockConfig.soscConfig.enable = true;
    clockConfig.soscConfig.frequency = 8000000U;
    clockConfig.soscConfig.range = CLOCK_SOSC_RANGE_HIGH;
    clockConfig.soscConfig.div1 = CLOCK_DIV_1;
    clockConfig.soscConfig.div2 = CLOCK_DIV_1;
    
    /* Configure SPLL (80 MHz) */
    clockConfig.spllConfig.enable = true;
    clockConfig.spllConfig.prediv = 0U;  /* Divide by 1 */
    clockConfig.spllConfig.mult = 20U;   /* 8MHz * 20 / 2 = 80MHz */
    clockConfig.spllConfig.div1 = CLOCK_DIV_2;
    clockConfig.spllConfig.div2 = CLOCK_DIV_4;
    
    /* Configure system clock */
    clockConfig.systemClockSource = CLOCK_SRC_SPLL;
    clockConfig.systemClockDiv = CLOCK_DIV_1;
    clockConfig.busClockDiv = CLOCK_DIV_2;
    clockConfig.flashClockDiv = CLOCK_DIV_4;
    
    /* Initialize clock */
    CLOCK_Init(&clockConfig);
}

/**
 * @brief Initialize GPIO for LED
 */
static void Board2_InitGPIO(void)
{
    /* Enable PORT D clock */
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
    
    /* Configure LED (PTD0) */
    PORT_SetMux(LED_PORT, LED_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_GPIO, LED_PIN, GPIO_OUTPUT);
    GPIO_WritePin(LED_GPIO, LED_PIN, 0U);
}

/**
 * @brief Initialize CAN
 */
static void Board2_InitCAN(void)
{
    can_config_t canConfig;
    
    /* Configure CAN */
    canConfig.baudrate = CAN_BAUDRATE;
    canConfig.enableLoopback = false;
    canConfig.enableListenOnly = false;
    canConfig.maxMailboxNum = 32U;
    canConfig.enableSelfWakeup = false;
    canConfig.clkSrc = CAN_CLK_SOURCE_PERIPH;
    
    /* Initialize CAN */
    CAN_Init(CAN_INSTANCE, &canConfig);
    
    /* Configure receive callback */
    CAN_SetRxCallback(CAN_INSTANCE, CAN_RX_MB, Board2_CANRxCallback);
    
    /* Start receiving messages with ID 0x100 */
    can_message_t rxMsg;
    rxMsg.id = CAN_MSG_ID;
    rxMsg.isExtended = false;
    CAN_Receive(CAN_INSTANCE, CAN_RX_MB, &rxMsg);
}

/**
 * @brief Initialize UART
 */
static void Board2_InitUART(void)
{
    uart_config_t uartConfig;
    
    /* Configure UART */
    uartConfig.baudrate = UART_BAUDRATE;
    uartConfig.dataBits = UART_DATA_8BITS;
    uartConfig.parity = UART_PARITY_NONE;
    uartConfig.stopBits = UART_STOP_1BIT;
    uartConfig.enableTx = true;
    uartConfig.enableRx = false;  /* Only need TX for this application */
    
    /* Initialize UART */
    UART_Init(UART_INSTANCE, &uartConfig);
}

/**
 * @brief CAN receive callback
 */
static void Board2_CANRxCallback(uint8_t instance, uint8_t mb, const can_message_t *message)
{
    (void)instance;
    (void)mb;
    
    /* Copy received message */
    s_receivedMessage = *message;
    s_newMessageReceived = true;
    
    /* Restart receiving for next message */
    can_message_t rxMsg;
    rxMsg.id = CAN_MSG_ID;
    rxMsg.isExtended = false;
    CAN_Receive(CAN_INSTANCE, CAN_RX_MB, &rxMsg);
}

/**
 * @brief Process received CAN message and send via UART
 */
static void Board2_ProcessMessage(void)
{
    uint16_t adcValue;
    uint32_t length;
    
    /* Convert CAN data to ADC value */
    adcValue = Board2_ConvertCANDataToADC(s_receivedMessage.data);
    
    /* Format message for UART */
    length = snprintf(s_uartBuffer, sizeof(s_uartBuffer),
                      "ADC Value: %u (0x%03X)\r\n", adcValue, adcValue);
    
    /* Send via UART */
    UART_SendBlocking(UART_INSTANCE, (const uint8_t *)s_uartBuffer, 
                      length, 1000U);
}

/**
 * @brief Convert CAN data to ADC value
 * @param[in] data CAN data bytes (ASCII digits)
 * @return ADC value
 */
static uint16_t Board2_ConvertCANDataToADC(const uint8_t *data)
{
    uint16_t value = 0;
    
    /* Convert ASCII digits to number */
    /* data[4] = thousands, data[5] = hundreds, data[6] = tens, data[7] = units */
    value += (data[4] - '0') * 1000U;
    value += (data[5] - '0') * 100U;
    value += (data[6] - '0') * 10U;
    value += (data[7] - '0') * 1U;
    
    return value;
}

/*******************************************************************************
 * Standard Library Functions
 ******************************************************************************/

/**
 * @brief Simple strlen implementation
 */
static size_t strlen(const char *str)
{
    const char *s = str;
    while (*s) {
        s++;
    }
    return (s - str);
}

/**
 * @brief Simple snprintf implementation for formatting
 */
static int snprintf(char *str, size_t size, const char *format, ...)
{
    /* Simple implementation for "ADC Value: %u (0x%03X)\r\n" format */
    uint16_t value = 0;
    uint16_t hexValue = 0;
    char *ptr = str;
    const char *prefix = "ADC Value: ";
    const char *middle = " (0x";
    const char *suffix = ")\r\n";
    
    /* Get variadic arguments */
    __builtin_va_list args;
    __builtin_va_start(args, format);
    value = __builtin_va_arg(args, uint32_t);
    hexValue = __builtin_va_arg(args, uint32_t);
    __builtin_va_end(args);
    
    /* Copy prefix */
    while (*prefix && (ptr - str) < (int)size - 1) {
        *ptr++ = *prefix++;
    }
    
    /* Convert decimal value to string */
    char temp[10];
    int i = 0;
    uint16_t val = value;
    do {
        temp[i++] = (val % 10) + '0';
        val /= 10;
    } while (val > 0);
    
    while (i > 0 && (ptr - str) < (int)size - 1) {
        *ptr++ = temp[--i];
    }
    
    /* Copy middle */
    while (*middle && (ptr - str) < (int)size - 1) {
        *ptr++ = *middle++;
    }
    
    /* Convert hex value to string (3 digits) */
    const char hexDigits[] = "0123456789ABCDEF";
    if ((ptr - str) < (int)size - 3) {
        *ptr++ = hexDigits[(hexValue >> 8) & 0xF];
        *ptr++ = hexDigits[(hexValue >> 4) & 0xF];
        *ptr++ = hexDigits[hexValue & 0xF];
    }
    
    /* Copy suffix */
    while (*suffix && (ptr - str) < (int)size - 1) {
        *ptr++ = *suffix++;
    }
    
    *ptr = '\0';
    return (ptr - str);
}
