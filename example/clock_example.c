/**
 * @file    clock_example.c
 * @brief   Clock Driver Usage Example for S32K144
 * @details This file demonstrates how to use the clock driver to configure
 *          and manage system clocks on S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    25/11/2025
 * @version 1.0
 * 
 * @note    This example shows various clock configuration scenarios
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 25, 2025): Initial version
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "clock.h"
#include "gpio.h"
#include "port.h"
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* LED Configuration (example: PTD15, PTD16) */
#define LED_RED_PORT        GPIO_PORT_D
#define LED_RED_PIN         15U

#define LED_GREEN_PORT      GPIO_PORT_D
#define LED_GREEN_PIN       16U

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Initialize LEDs for visual feedback
 */
static void InitLEDs(void);

/**
 * @brief Simple delay function
 * @param[in] count Delay count
 */
static void Delay(uint32_t count);

/**
 * @brief Print clock frequencies via UART
 * @note Requires UART to be initialized separately
 */
static void PrintClockFrequencies(void);

/**
 * @brief Example 1: Use default clock configuration (FIRC 48MHz)
 */
static void Example_DefaultClock(void);

/**
 * @brief Example 2: Configure clock with SIRC (8MHz)
 */
static void Example_SlowClock(void);

/**
 * @brief Example 3: Configure clock with SPLL (80MHz)
 */
static void Example_HighSpeedClock(void);

/**
 * @brief Example 4: Switch between different clock sources
 */
static void Example_ClockSwitching(void);

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
}

static void Delay(uint32_t count)
{
    volatile uint32_t i;
    for (i = 0U; i < count; i++) {
        __asm("NOP");
    }
}

static void PrintClockFrequencies(void)
{
    clock_frequencies_t freqs;
    CLOCK_GetFrequencies(&freqs);
    
    /* Note: This requires UART to be initialized
     * Uncomment if UART is available:
     * 
     * printf("System Clock: %lu Hz\n", freqs.systemClockFreq);
     * printf("Core Clock:   %lu Hz\n", freqs.coreClockFreq);
     * printf("Bus Clock:    %lu Hz\n", freqs.busClockFreq);
     * printf("Slow Clock:   %lu Hz\n", freqs.slowClockFreq);
     */
    
    /* For demonstration, blink LED based on frequency */
    if (freqs.coreClockFreq > 40000000U) {
        /* High frequency - blink fast */
        GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
        Delay(100000U);
        GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
    } else {
        /* Low frequency - blink slow */
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
        Delay(100000U);
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void Example_DefaultClock(void)
{
    /* Configure system clock to default settings:
     * - Clock Source: FIRC (48MHz)
     * - Core Clock:   48MHz (div by 1)
     * - Bus Clock:    24MHz (div by 2)
     * - Slow Clock:   12MHz (div by 4)
     */
    
    bool status = CLOCK_SetDefaultConfig();
    
    if (status) {
        /* Success - Blink GREEN LED */
        for (uint8_t i = 0; i < 3; i++) {
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
            Delay(200000U);
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
            Delay(200000U);
        }
        
        /* Print frequencies */
        PrintClockFrequencies();
    } else {
        /* Error - Blink RED LED */
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
        Delay(1000000U);
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
    }
}

static void Example_SlowClock(void)
{
    /* Configure system clock with SIRC (8MHz):
     * - Clock Source: SIRC (8MHz)
     * - Core Clock:   8MHz (div by 1)
     * - Bus Clock:    4MHz (div by 2)
     * - Slow Clock:   2MHz (div by 4)
     */
    
    clock_config_t config = {
        .clockSource = CLOCK_SRC_SIRC,
        .divCore = CLOCK_DIV_BY_1,
        .divBus = CLOCK_DIV_BY_2,
        .divSlow = CLOCK_DIV_BY_4
    };
    
    bool status = CLOCK_Init(&config);
    
    if (status) {
        /* Success - Blink GREEN LED slowly */
        for (uint8_t i = 0; i < 3; i++) {
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
            Delay(50000U);  /* Slower delay due to lower clock */
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
            Delay(50000U);
        }
        
        PrintClockFrequencies();
    } else {
        /* Error */
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
    }
}

static void Example_HighSpeedClock(void)
{
    /* Configure system clock with SPLL (80MHz):
     * - Clock Source: SPLL (80MHz from 8MHz SOSC)
     * - Core Clock:   80MHz (div by 1)
     * - Bus Clock:    40MHz (div by 2)
     * - Slow Clock:   20MHz (div by 4)
     */
    
    clock_config_t config = {
        .clockSource = CLOCK_SRC_SPLL,
        .divCore = CLOCK_DIV_BY_1,
        .divBus = CLOCK_DIV_BY_2,
        .divSlow = CLOCK_DIV_BY_4
    };
    
    bool status = CLOCK_Init(&config);
    
    if (status) {
        /* Success - Blink GREEN LED fast */
        for (uint8_t i = 0; i < 5; i++) {
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
            Delay(500000U);  /* Faster blink at high clock */
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
            Delay(500000U);
        }
        
        PrintClockFrequencies();
    } else {
        /* Error - requires external oscillator */
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
    }
}

static void Example_ClockSwitching(void)
{
    /* Demonstrate switching between different clock sources */
    
    /* 1. Start with SIRC (8MHz) */
    CLOCK_EnableSIRC();
    CLOCK_SetSystemClockSource(CLOCK_SRC_SIRC);
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
    Delay(100000U);
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
    Delay(100000U);
    
    /* 2. Switch to FIRC (48MHz) */
    CLOCK_EnableFIRC();
    CLOCK_SetSystemClockSource(CLOCK_SRC_FIRC);
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
    Delay(500000U);
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
    Delay(500000U);
    
    /* 3. Check current clock source */
    clock_source_t currentSource = CLOCK_GetSystemClockSource();
    if (currentSource == CLOCK_SRC_FIRC) {
        /* Success - both LEDs blink */
        for (uint8_t i = 0; i < 2; i++) {
            GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
            Delay(200000U);
            GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
            Delay(200000U);
        }
    }
    
    /* Print final frequencies */
    PrintClockFrequencies();
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main function demonstrating clock driver usage
 */
int main(void)
{
    /* Initialize LEDs for visual feedback */
    InitLEDs();
    
    /* Example 1: Use default clock configuration (FIRC 48MHz) */
    Example_DefaultClock();
    Delay(1000000U);
    
    /* Example 2: Configure slow clock (SIRC 8MHz) */
    Example_SlowClock();
    Delay(1000000U);
    
    /* Example 3: Configure high-speed clock (SPLL 80MHz) */
    /* Note: Requires external oscillator to be connected */
    // Example_HighSpeedClock();
    // Delay(1000000U);
    
    /* Example 4: Demonstrate clock switching */
    Example_ClockSwitching();
    
    /* Main loop */
    while (1) {
        /* Toggle GREEN LED to show system is running */
        GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
        
        /* Delay based on current clock frequency */
        uint32_t coreFreq = CLOCK_GetCoreClockFreq();
        uint32_t delayCount = coreFreq / 10U;  /* Adjust delay to clock */
        Delay(delayCount);
        
        /* Get and display clock information periodically */
        static uint32_t counter = 0;
        if (++counter > 10) {
            counter = 0;
            PrintClockFrequencies();
        }
    }
    
    return 0;
}

/*******************************************************************************
 * Additional Example Functions
 ******************************************************************************/

/**
 * @brief Example: Configure specific clock frequencies
 * @note This function shows how to achieve specific clock frequencies
 */
void Example_CustomFrequencies(void)
{
    /* Goal: Core = 40MHz, Bus = 20MHz, Slow = 10MHz using FIRC */
    
    /* Enable FIRC (48MHz) */
    CLOCK_EnableFIRC();
    
    /* Set custom dividers to get closer to desired frequencies
     * Note: With FIRC at 48MHz:
     * - Core = 48MHz / 1 = 48MHz
     * - Bus = 48MHz / 2 = 24MHz  
     * - Slow = 48MHz / 4 = 12MHz
     * 
     * For exactly 40MHz core, would need to use SPLL with appropriate multipliers
     */
    
    CLOCK_SetDividers(CLOCK_DIV_BY_1, CLOCK_DIV_BY_2, CLOCK_DIV_BY_4);
    CLOCK_SetSystemClockSource(CLOCK_SRC_FIRC);
    
    /* Verify frequencies */
    clock_frequencies_t freqs;
    CLOCK_GetFrequencies(&freqs);
    
    /* Visual indication */
    if (freqs.coreClockFreq >= 40000000U) {
        GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
    }
}

/**
 * @brief Example: Check clock source validity
 * @note This function shows how to verify clock sources
 */
void Example_CheckClockValidity(void)
{
    /* Check which clock sources are available and valid */
    
    bool sircValid = CLOCK_IsSIRCValid();
    bool fircValid = CLOCK_IsFIRCValid();
    bool soscValid = CLOCK_IsSOSCValid();
    bool spllValid = CLOCK_IsSPLLValid();
    
    /* Visual indication using LEDs */
    if (sircValid && fircValid) {
        /* Both internal clocks are valid */
        GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_LOW);
    }
    
    if (soscValid || spllValid) {
        /* External oscillator or PLL is valid */
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_LOW);
    }
    
    Delay(1000000U);
    
    /* Turn off LEDs */
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_LEVEL_HIGH);
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, GPIO_LEVEL_HIGH);
}
