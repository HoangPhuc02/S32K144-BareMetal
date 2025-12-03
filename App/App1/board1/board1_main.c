/**
 * @file    board1_main.c
 * @brief   Board 1 Application - ADC Reading and CAN Transmission
 * @details This application reads ADC value when button 1 is pressed, 
 *          uses LPIT timer to read ADC every 1 second, and transmits 
 *          the value to Board 2 via CAN at 500 Kbps.
 *          Button 2 stops the ADC reading and transmission.
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "board1_main.h"
#include "../../Core/Drivers/gpio/gpio.h"
#include "../../Core/Drivers/port/port.h"
#include "../../Core/Drivers/adc/adc.h"
#include "../../Core/Drivers/lpit/lpit.h"
#include "../../Core/Drivers/can/can.h"
#include "../../Core/Drivers/clock/clock.h"
#include "../../Core/Drivers/nvic/nvic.h"

/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/** @brief Button 1 port and pin (PTD3) */
#define BUTTON1_PORT        PORTD
#define BUTTON1_GPIO        PTD
#define BUTTON1_PIN         3U

/** @brief Button 2 port and pin (PTD5) */
#define BUTTON2_PORT        PORTD
#define BUTTON2_GPIO        PTD
#define BUTTON2_PIN         5U

/** @brief LED port and pin for status indication (PTD0) */
#define LED_PORT            PORTD
#define LED_GPIO            PTD
#define LED_PIN             0U

/** @brief ADC channel for reading (ADC0 - Channel 12) */
#define ADC_INSTANCE        0U
#define ADC_CHANNEL         12U

/** @brief LPIT channel for 1 second timer */
#define LPIT_CHANNEL        0U
#define LPIT_PERIOD_US      1000000U  /* 1 second */

/** @brief CAN configuration */
#define CAN_INSTANCE        0U
#define CAN_BAUDRATE        500000U   /* 500 Kbps */
#define CAN_MSG_ID          0x100U    /* Message ID for ADC data */

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Flag to control ADC reading and transmission */
static volatile bool s_isRunning = false;

/** @brief Latest ADC value */
static volatile uint16_t s_adcValue = 0U;

/** @brief Button debounce counter */
static volatile uint8_t s_button1Counter = 0U;
static volatile uint8_t s_button2Counter = 0U;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Initialize system clock
 */
static void Board1_InitClock(void);

/**
 * @brief Initialize GPIO for buttons and LED
 */
static void Board1_InitGPIO(void);

/**
 * @brief Initialize ADC
 */
static void Board1_InitADC(void);

/**
 * @brief Initialize LPIT timer
 */
static void Board1_InitLPIT(void);

/**
 * @brief Initialize CAN
 */
static void Board1_InitCAN(void);

/**
 * @brief Button 1 interrupt handler (Start ADC reading)
 */
static void Board1_Button1Handler(void);

/**
 * @brief Button 2 interrupt handler (Stop ADC reading)
 */
static void Board1_Button2Handler(void);

/**
 * @brief LPIT interrupt handler (Read ADC every 1 second)
 */
static void Board1_LPITHandler(void);

/**
 * @brief Send ADC value via CAN
 * @param[in] adcValue ADC value to send (0-4095)
 */
static void Board1_SendADCViaCAN(uint16_t adcValue);

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Main function for Board 1
 */
void Board1_Main(void)
{
    /* Initialize system */
    Board1_InitClock();
    Board1_InitGPIO();
    Board1_InitADC();
    Board1_InitLPIT();
    Board1_InitCAN();
    
    /* Enable global interrupts */
    NVIC_EnableGlobalIRQ(0U);
    
    /* Main loop */
    while (1) {
        /* Toggle LED to indicate system is running */
        GPIO_TogglePin(LED_GPIO, LED_PIN);
        
        /* Simple delay */
        for (volatile uint32_t i = 0; i < 500000U; i++) {
            __asm("nop");
        }
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Initialize system clock
 */
static void Board1_InitClock(void)
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
 * @brief Initialize GPIO for buttons and LED
 */
static void Board1_InitGPIO(void)
{
    /* Enable PORT D clock */
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
    
    /* Configure Button 1 (PTD3) */
    PORT_SetMux(BUTTON1_PORT, BUTTON1_PIN, PORT_MUX_GPIO);
    PORT_SetPullConfig(BUTTON1_PORT, BUTTON1_PIN, PORT_PULL_UP);
    PORT_SetInterruptConfig(BUTTON1_PORT, BUTTON1_PIN, PORT_INT_FALLING_EDGE);
    GPIO_SetPinDirection(BUTTON1_GPIO, BUTTON1_PIN, GPIO_INPUT);
    
    /* Configure Button 2 (PTD5) */
    PORT_SetMux(BUTTON2_PORT, BUTTON2_PIN, PORT_MUX_GPIO);
    PORT_SetPullConfig(BUTTON2_PORT, BUTTON2_PIN, PORT_PULL_UP);
    PORT_SetInterruptConfig(BUTTON2_PORT, BUTTON2_PIN, PORT_INT_FALLING_EDGE);
    GPIO_SetPinDirection(BUTTON2_GPIO, BUTTON2_PIN, GPIO_INPUT);
    
    /* Configure LED (PTD0) */
    PORT_SetMux(LED_PORT, LED_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_GPIO, LED_PIN, GPIO_OUTPUT);
    GPIO_WritePin(LED_GPIO, LED_PIN, 0U);
    
    /* Enable PORT D interrupt */
    NVIC_EnableIRQ(PORTD_IRQn);
    NVIC_SetPriority(PORTD_IRQn, 3U);
}

/**
 * @brief Initialize ADC
 */
static void Board1_InitADC(void)
{
    adc_config_t adcConfig;
    
    /* Configure ADC */
    adcConfig.resolution = ADC_RESOLUTION_12BIT;
    adcConfig.clockDiv = ADC_CLK_DIV_2;
    adcConfig.sampleTime = ADC_SAMPLE_TIME_LONG;
    adcConfig.trigger = ADC_TRIGGER_SOFTWARE;
    adcConfig.continuousMode = false;
    adcConfig.voltageRef = ADC_VREF_VREFH;
    
    /* Initialize ADC */
    ADC_Init(ADC_INSTANCE, &adcConfig);
}

/**
 * @brief Initialize LPIT timer
 */
static void Board1_InitLPIT(void)
{
    lpit_config_t lpitConfig;
    
    /* Configure LPIT channel 0 for 1 second period */
    lpitConfig.enableChannel = true;
    lpitConfig.chainChannel = false;
    lpitConfig.timerMode = LPIT_TIMER_MODE_PERIODIC;
    lpitConfig.triggerSource = LPIT_TRIGGER_INTERNAL;
    lpitConfig.triggerSelect = 0U;
    lpitConfig.reloadOnTrigger = false;
    lpitConfig.stopOnInterrupt = false;
    lpitConfig.startOnTrigger = false;
    lpitConfig.period = LPIT_PERIOD_US;
    lpitConfig.enableInterrupt = true;
    
    /* Initialize LPIT */
    LPIT_Init();
    LPIT_ConfigChannel(LPIT_CHANNEL, &lpitConfig);
    
    /* Enable LPIT interrupt */
    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
    NVIC_SetPriority(LPIT0_Ch0_IRQn, 2U);
}

/**
 * @brief Initialize CAN
 */
static void Board1_InitCAN(void)
{
    can_config_t canConfig;
    
    /* Configure CAN */
    canConfig.baudrate = CAN_BAUDRATE;
    canConfig.enableLoopback = false;
    canConfig.enableListenOnly = false;
    canConfig.maxMailboxNum = 16U;
    canConfig.enableSelfWakeup = false;
    canConfig.clkSrc = CAN_CLK_SOURCE_PERIPH;
    
    /* Initialize CAN */
    CAN_Init(CAN_INSTANCE, &canConfig);
}

/**
 * @brief Button 1 interrupt handler (Start ADC reading)
 */
static void Board1_Button1Handler(void)
{
    /* Debounce button */
    s_button1Counter++;
    if (s_button1Counter < 3U) {
        return;
    }
    s_button1Counter = 0U;
    
    /* Start ADC reading and transmission */
    if (!s_isRunning) {
        s_isRunning = true;
        
        /* Start LPIT timer */
        LPIT_StartChannel(LPIT_CHANNEL);
        
        /* Turn on LED to indicate running */
        GPIO_WritePin(LED_GPIO, LED_PIN, 1U);
    }
}

/**
 * @brief Button 2 interrupt handler (Stop ADC reading)
 */
static void Board1_Button2Handler(void)
{
    /* Debounce button */
    s_button2Counter++;
    if (s_button2Counter < 3U) {
        return;
    }
    s_button2Counter = 0U;
    
    /* Stop ADC reading and transmission */
    if (s_isRunning) {
        s_isRunning = false;
        
        /* Stop LPIT timer */
        LPIT_StopChannel(LPIT_CHANNEL);
        
        /* Turn off LED */
        GPIO_WritePin(LED_GPIO, LED_PIN, 0U);
    }
}

/**
 * @brief LPIT interrupt handler (Read ADC every 1 second)
 */
static void Board1_LPITHandler(void)
{
    /* Clear interrupt flag */
    LPIT_ClearInterruptFlag(LPIT_CHANNEL);
    
    /* Read ADC if running */
    if (s_isRunning) {
        /* Start ADC conversion */
        ADC_StartConversion(ADC_INSTANCE, ADC_CHANNEL);
        
        /* Wait for conversion to complete */
        while (!ADC_IsConversionComplete(ADC_INSTANCE)) {
            /* Wait */
        }
        
        /* Read ADC value */
        s_adcValue = ADC_GetConversionResult(ADC_INSTANCE);
        
        /* Send ADC value via CAN */
        Board1_SendADCViaCAN(s_adcValue);
    }
}

/**
 * @brief Send ADC value via CAN
 * @param[in] adcValue ADC value to send (0-4095)
 */
static void Board1_SendADCViaCAN(uint16_t adcValue)
{
    can_message_t msg;
    uint8_t data[8] = {0};
    
    /* Convert ADC value to ASCII digits */
    /* Example: ADC = 456 -> data[5]=4, data[6]=5, data[7]=6 */
    data[7] = (adcValue % 10) + '0';        /* Units digit */
    data[6] = ((adcValue / 10) % 10) + '0'; /* Tens digit */
    data[5] = ((adcValue / 100) % 10) + '0';/* Hundreds digit */
    data[4] = ((adcValue / 1000) % 10) + '0';/* Thousands digit */
    
    /* Configure CAN message */
    msg.id = CAN_MSG_ID;
    msg.length = 8U;
    msg.isExtended = false;
    msg.isRemote = false;
    for (uint8_t i = 0; i < 8U; i++) {
        msg.data[i] = data[i];
    }
    
    /* Send message */
    CAN_Send(CAN_INSTANCE, 8U, &msg);
}

/*******************************************************************************
 * Interrupt Service Routines
 ******************************************************************************/

/**
 * @brief PORT D interrupt handler
 */
void PORTD_IRQHandler(void)
{
    /* Check Button 1 interrupt */
    if (PORT_GetInterruptFlag(BUTTON1_PORT, BUTTON1_PIN)) {
        PORT_ClearInterruptFlag(BUTTON1_PORT, BUTTON1_PIN);
        Board1_Button1Handler();
    }
    
    /* Check Button 2 interrupt */
    if (PORT_GetInterruptFlag(BUTTON2_PORT, BUTTON2_PIN)) {
        PORT_ClearInterruptFlag(BUTTON2_PORT, BUTTON2_PIN);
        Board1_Button2Handler();
    }
}

/**
 * @brief LPIT Channel 0 interrupt handler
 */
void LPIT0_Ch0_IRQHandler(void)
{
    Board1_LPITHandler();
}
