/**
 * @file    systick_example.c
 * @brief   SysTick Timer Usage Example for S32K144
 * @details This file demonstrates how to use the SysTick driver for timing,
 *          delays, and periodic tasks on S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    25/11/2025
 * @version 1.0
 * 
 * @note    This example shows various SysTick usage scenarios
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 25, 2025): Initial version
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "systick.h"
#include "clock.h"
#include "gpio.h"
#include "port.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* LED Configuration (example: PTD15, PTD16, PTD0) */
#define LED_RED_PORT        GPIO_PORT_D
#define LED_RED_PIN         15U

#define LED_GREEN_PORT      GPIO_PORT_D
#define LED_GREEN_PIN       16U

#define LED_BLUE_PORT       GPIO_PORT_D
#define LED_BLUE_PIN        0U

/* System clock frequency (48MHz FIRC) */
#define SYSTEM_CLOCK_HZ     (48000000U)

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Flag to track LED state changes in callback */
static volatile bool s_ledToggleFlag = false;

/** @brief Counter for periodic task demonstration */
static volatile uint32_t s_periodicCounter = 0U;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Initialize LEDs for visual feedback
 */
static void InitLEDs(void);

/**
 * @brief SysTick callback function - called every 1ms
 */
static void SysTick_Callback(void);

/**
 * @brief Example 1: Basic SysTick initialization and delay
 */
static void Example_BasicDelay(void);

/**
 * @brief Example 2: Millisecond delay with LED blinking
 */
static void Example_MillisecondDelay(void);

/**
 * @brief Example 3: Microsecond delay demonstration
 */
static void Example_MicrosecondDelay(void);

/**
 * @brief Example 4: SysTick interrupt with callback
 */
static void Example_InterruptCallback(void);

/**
 * @brief Example 5: Periodic task scheduling
 */
static void Example_PeriodicTask(void);

/**
 * @brief Example 6: Tick counting and time measurement
 */
static void Example_TickCounting(void);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static void InitLEDs(void)
{
    /* Initialize RED LED */
    GPIO_Init(LED_RED_PORT, LED_RED_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);  /* LED OFF */
    
    /* Initialize GREEN LED */
    GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);  /* LED OFF */
    
    /* Initialize BLUE LED */
    GPIO_Init(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_LEVEL_HIGH);  /* LED OFF */
}

static void SysTick_Callback(void)
{
    /* This callback is called every 1ms */
    s_periodicCounter++;
    
    /* Toggle flag every 500ms (500 ticks) */
    if (s_periodicCounter >= 500U) {
        s_periodicCounter = 0U;
        s_ledToggleFlag = true;
    }
}

static void Example_BasicDelay(void)
{
    /* Configure SysTick for 1ms tick without interrupt */
    SYSTICK_ConfigMillisecond(SYSTEM_CLOCK_HZ, false);
    SYSTICK_Start();
    
    /* Blink RED LED 5 times with 200ms delay */
    for (uint8_t i = 0; i < 5; i++) {
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
        SYSTICK_DelayMs(200U);
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
        SYSTICK_DelayMs(200U);
    }
    
    SYSTICK_Stop();
}

static void Example_MillisecondDelay(void)
{
    /* Configure SysTick for millisecond delays */
    SYSTICK_ConfigMillisecond(SYSTEM_CLOCK_HZ, false);
    SYSTICK_Start();
    
    /* Blink GREEN LED with different delay patterns */
    
    /* Fast blink - 100ms */
    for (uint8_t i = 0; i < 10; i++) {
        GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
        SYSTICK_DelayMs(100U);
    }
    
    SYSTICK_DelayMs(500U);  /* Pause */
    
    /* Slow blink - 500ms */
    for (uint8_t i = 0; i < 4; i++) {
        GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
        SYSTICK_DelayMs(500U);
    }
    
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);  /* LED OFF */
    SYSTICK_Stop();
}

static void Example_MicrosecondDelay(void)
{
    /* Demonstrate microsecond delay with BLUE LED */
    
    /* Create PWM-like effect using microsecond delays */
    SYSTICK_Stop();  /* Stop if running */
    
    for (uint16_t duty = 0; duty < 1000; duty += 50) {
        for (uint8_t i = 0; i < 100; i++) {
            /* LED ON for 'duty' microseconds */
            GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_LEVEL_LOW);
            SYSTICK_DelayUs(duty, SYSTEM_CLOCK_HZ);
            
            /* LED OFF for remaining time (1000 - duty) microseconds */
            GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_LEVEL_HIGH);
            SYSTICK_DelayUs(1000U - duty, SYSTEM_CLOCK_HZ);
        }
    }
    
    GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_LEVEL_HIGH);  /* LED OFF */
}

static void Example_InterruptCallback(void)
{
    /* Configure SysTick with interrupt and callback */
    SYSTICK_RegisterCallback(SysTick_Callback);
    SYSTICK_ConfigMillisecond(SYSTEM_CLOCK_HZ, true);
    SYSTICK_Start();
    
    /* Reset flag and counter */
    s_ledToggleFlag = false;
    s_periodicCounter = 0U;
    
    /* Main loop - toggle LED when flag is set by interrupt */
    for (uint8_t i = 0; i < 20; i++) {
        /* Wait for flag from interrupt */
        while (!s_ledToggleFlag) {
            __asm("NOP");
        }
        
        /* Toggle RED LED every 500ms */
        GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN);
        s_ledToggleFlag = false;
    }
    
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);  /* LED OFF */
    SYSTICK_Stop();
    SYSTICK_UnregisterCallback();
}

static void Example_PeriodicTask(void)
{
    /* Configure SysTick for periodic task scheduling */
    SYSTICK_ConfigMillisecond(SYSTEM_CLOCK_HZ, false);
    SYSTICK_Start();
    SYSTICK_ResetTicks();
    
    uint32_t lastTask1Time = 0U;
    uint32_t lastTask2Time = 0U;
    uint32_t lastTask3Time = 0U;
    
    /* Run periodic tasks for 10 seconds */
    uint32_t startTime = SYSTICK_GetTicks();
    
    while ((SYSTICK_GetTicks() - startTime) < 10000U) {
        uint32_t currentTime = SYSTICK_GetTicks();
        
        /* Task 1: Toggle RED LED every 200ms */
        if ((currentTime - lastTask1Time) >= 200U) {
            GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN);
            lastTask1Time = currentTime;
        }
        
        /* Task 2: Toggle GREEN LED every 300ms */
        if ((currentTime - lastTask2Time) >= 300U) {
            GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
            lastTask2Time = currentTime;
        }
        
        /* Task 3: Toggle BLUE LED every 500ms */
        if ((currentTime - lastTask3Time) >= 500U) {
            GPIO_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
            lastTask3Time = currentTime;
        }
    }
    
    /* Turn off all LEDs */
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
    GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_LEVEL_HIGH);
    
    SYSTICK_Stop();
}

static void Example_TickCounting(void)
{
    /* Demonstrate tick counting for time measurement */
    SYSTICK_ConfigMillisecond(SYSTEM_CLOCK_HZ, false);
    SYSTICK_Start();
    SYSTICK_ResetTicks();
    
    /* Measure time for LED blink operation */
    uint32_t startTick = SYSTICK_GetTicks();
    
    /* Perform some operations */
    for (uint8_t i = 0; i < 10; i++) {
        GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
        SYSTICK_DelayMs(50U);
    }
    
    uint32_t endTick = SYSTICK_GetTicks();
    uint32_t elapsedTime = endTick - startTick;
    
    /* Visual indication of elapsed time
     * If elapsed time is approximately 500ms (10 * 50ms), blink BLUE LED
     */
    if ((elapsedTime >= 490U) && (elapsedTime <= 510U)) {
        /* Success - elapsed time is correct */
        for (uint8_t i = 0; i < 3; i++) {
            GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_LEVEL_LOW);
            SYSTICK_DelayMs(100U);
            GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_LEVEL_HIGH);
            SYSTICK_DelayMs(100U);
        }
    }
    
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
    SYSTICK_Stop();
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main function demonstrating SysTick driver usage
 */
int main(void)
{
    /* Initialize system clock to 48MHz */
    CLOCK_SetDefaultConfig();
    
    /* Initialize LEDs for visual feedback */
    InitLEDs();
    
    /* Small delay to stabilize */
    for (volatile uint32_t i = 0; i < 100000; i++) {
        __asm("NOP");
    }
    
    /* Example 1: Basic delay demonstration */
    Example_BasicDelay();
    for (volatile uint32_t i = 0; i < 500000; i++) __asm("NOP");
    
    /* Example 2: Millisecond delay with LED patterns */
    Example_MillisecondDelay();
    for (volatile uint32_t i = 0; i < 500000; i++) __asm("NOP");
    
    /* Example 3: Microsecond delay (PWM effect) */
    Example_MicrosecondDelay();
    for (volatile uint32_t i = 0; i < 500000; i++) __asm("NOP");
    
    /* Example 4: Interrupt-based callback */
    Example_InterruptCallback();
    for (volatile uint32_t i = 0; i < 500000; i++) __asm("NOP");
    
    /* Example 5: Periodic task scheduling */
    Example_PeriodicTask();
    for (volatile uint32_t i = 0; i < 500000; i++) __asm("NOP");
    
    /* Example 6: Tick counting and time measurement */
    Example_TickCounting();
    
    /* Main loop - heartbeat with SysTick */
    SYSTICK_ConfigMillisecond(SYSTEM_CLOCK_HZ, false);
    SYSTICK_Start();
    
    while (1) {
        /* Heartbeat LED (GREEN) - toggle every second */
        GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
        SYSTICK_DelayMs(100U);
        GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
        SYSTICK_DelayMs(900U);
    }
    
    return 0;
}

/*******************************************************************************
 * Additional Example Functions
 ******************************************************************************/

/**
 * @brief Example: Precise timing with SysTick
 * @note This function demonstrates high-precision timing
 */
void Example_PreciseTiming(void)
{
    /* Configure SysTick for 10us ticks */
    SYSTICK_ConfigMicrosecond(SYSTEM_CLOCK_HZ, 10U, false);
    SYSTICK_Start();
    SYSTICK_ResetTicks();
    
    /* Generate precise 1ms pulse on LED */
    uint32_t startTick = SYSTICK_GetTicks();
    
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
    
    /* Wait for exactly 100 ticks (100 * 10us = 1ms) */
    while ((SYSTICK_GetTicks() - startTick) < 100U) {
        __asm("NOP");
    }
    
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
    
    SYSTICK_Stop();
}

/**
 * @brief Example: Non-blocking delay pattern
 * @note This shows how to implement non-blocking delays
 */
void Example_NonBlockingDelay(void)
{
    SYSTICK_ConfigMillisecond(SYSTEM_CLOCK_HZ, false);
    SYSTICK_Start();
    
    uint32_t lastToggleTime = SYSTICK_GetTicks();
    uint32_t toggleInterval = 250U;  /* 250ms */
    
    /* Run for 5 seconds with non-blocking LED toggle */
    uint32_t startTime = SYSTICK_GetTicks();
    
    while ((SYSTICK_GetTicks() - startTime) < 5000U) {
        uint32_t currentTime = SYSTICK_GetTicks();
        
        /* Non-blocking toggle check */
        if ((currentTime - lastToggleTime) >= toggleInterval) {
            GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
            lastToggleTime = currentTime;
        }
        
        /* Other tasks can be performed here */
        __asm("NOP");
    }
    
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
    SYSTICK_Stop();
}

/**
 * @brief SysTick interrupt handler (must be defined in vector table)
 * @note This links the interrupt to the driver's IRQ handler
 */
void SysTick_Handler(void)
{
    SYSTICK_IRQHandler();
}
