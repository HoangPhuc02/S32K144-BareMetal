/**
 * @file    touch_sensor_example.c
 * @brief   Touch Sensor Detection using ADC on S32K144
 * @details This example demonstrates how to use the ADC peripheral to detect
 *          capacitive touch sensor inputs. It monitors voltage changes when
 *          a touch plate is touched/released.
 * 
 * @author  PhucPH32
 * @date    December 1, 2025
 * @version 1.0
 * 
 * @hardware_setup
 * - Touch sensor connected to ADC0_SE12 (PTB13)
 * - Optional: LED on PTD0 (Red LED) for visual feedback
 * - Optional: LED on PTD15 (Green LED) for status
 * - Pull-down resistor 10kΩ on touch input recommended
 * - Filter capacitor 100nF recommended for noise reduction
 * 
 * @pin_configuration
 * PTB13 -> ADC0_SE12 (Touch sensor analog input)
 * PTD0  -> GPIO Output (Red LED - Touch detected)
 * PTD15 -> GPIO Output (Green LED - Ready/Idle)
 * 
 * @wiring_diagram
 * Touch Plate
 *      |
 *      +---- 1MΩ ----+---- PTB13 (ADC0_SE12)
 *                    |
 *                   10kΩ (pull-down)
 *                    |
 *                   GND
 *                    
 * Optional: Add 100nF capacitor between PTB13 and GND for filtering
 * 
 * @operation_principle
 * 1. Capacitive touch sensing uses human body capacitance
 * 2. When finger touches plate, capacitance increases
 * 3. RC circuit charging time changes
 * 4. ADC measures voltage change
 * 5. Threshold comparison determines touch state
 * 
 * @copyright Copyright (c) 2025
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "s32k144_reg.h"
#include "../Core/Drivers/adc/adc.h"
#include "../Core/Drivers/gpio/gpio.h"
#include "../Core/Drivers/port/port.h"
#include "../Core/Drivers/clock/clock.h"
#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Touch sensor ADC configuration */
#define TOUCH_ADC_INSTANCE          ADC_INSTANCE_0
#define TOUCH_ADC_CHANNEL           ADC_CHANNEL_AD12    /**< PTB13 = ADC0_SE12 */

/** @brief Touch detection thresholds (12-bit ADC, 0-4095) */
#define TOUCH_THRESHOLD_PRESS       2500    /**< Threshold for touch press detection */
#define TOUCH_THRESHOLD_RELEASE     2000    /**< Threshold for touch release (hysteresis) */
#define TOUCH_BASELINE_SAMPLES      32      /**< Number of samples for baseline calibration */

/** @brief Touch debounce settings */
#define TOUCH_DEBOUNCE_COUNT        5       /**< Consecutive samples needed to confirm touch */
#define TOUCH_SAMPLE_INTERVAL_MS    10      /**< Sampling interval in milliseconds */

/** @brief LED indicators */
#define LED_RED_PORT                PTD
#define LED_RED_PIN                 0U      /**< Red LED - Touch detected */
#define LED_GREEN_PORT              PTD
#define LED_GREEN_PIN               15U     /**< Green LED - Ready status */

/** @brief Touch sensor states */
typedef enum {
    TOUCH_STATE_RELEASED = 0U,              /**< No touch detected */
    TOUCH_STATE_PRESSED  = 1U               /**< Touch detected */
} TouchState_t;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/** @brief Current touch state */
static TouchState_t g_touchState = TOUCH_STATE_RELEASED;

/** @brief Baseline ADC value (no touch) */
static uint16_t g_touchBaseline = 0;

/** @brief Debounce counter */
static uint8_t g_debounceCounter = 0;

/** @brief Touch event counter */
static uint32_t g_touchEventCount = 0;

/** @brief Last ADC reading */
static uint16_t g_lastAdcValue = 0;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/

static void TouchSensor_InitHardware(void);
static void TouchSensor_CalibrateBaseline(void);
static void TouchSensor_ProcessReading(uint16_t adcValue);
static void TouchSensor_OnTouchPress(void);
static void TouchSensor_OnTouchRelease(void);
static void Delay_Ms(uint32_t ms);

/*******************************************************************************
 * Example 1: Basic Touch Detection with Polling
 ******************************************************************************/

/**
 * @brief Example 1 - Basic touch sensor detection (polling mode)
 * 
 * This example demonstrates simple touch detection using ADC polling.
 * The LED changes state when touch is detected.
 * 
 * @par Usage:
 * 1. Initialize system and peripherals
 * 2. Calibrate baseline (no touch condition)
 * 3. Continuously read ADC and check threshold
 * 4. Update LED based on touch state
 * 
 * @note Touch the sensor plate to see LED response
 */
void TouchSensor_Example1_BasicPolling(void)
{
    uint16_t adcValue = 0;
    ADC_Status_t status;
    
    /* Initialize hardware and calibrate */
    TouchSensor_InitHardware();
    TouchSensor_CalibrateBaseline();
    
    /* Turn on green LED to indicate ready */
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
    
    /* Main loop - continuous touch sensing */
    while (1)
    {
        /* Read ADC value */
        status = ADC_ReadBlocking(TOUCH_ADC_INSTANCE, TOUCH_ADC_CHANNEL, &adcValue);
        
        if (status == ADC_STATUS_SUCCESS)
        {
            g_lastAdcValue = adcValue;
            
            /* Process reading and update touch state */
            TouchSensor_ProcessReading(adcValue);
        }
        
        /* Delay between samples */
        Delay_Ms(TOUCH_SAMPLE_INTERVAL_MS);
    }
}

/*******************************************************************************
 * Example 2: Touch Detection with Debouncing
 ******************************************************************************/

/**
 * @brief Example 2 - Touch sensor with debounce filtering
 * 
 * This example adds debounce logic to prevent false triggers from noise.
 * Multiple consecutive readings must exceed threshold to confirm touch.
 * 
 * @par Debounce Algorithm:
 * - Count consecutive samples above/below threshold
 * - State change only when count reaches TOUCH_DEBOUNCE_COUNT
 * - Provides stable detection even with noisy signals
 * 
 * @note Better noise immunity than Example 1
 */
void TouchSensor_Example2_WithDebounce(void)
{
    uint16_t adcValue = 0;
    ADC_Status_t status;
    uint8_t pressCounter = 0;
    uint8_t releaseCounter = 0;
    
    /* Initialize hardware and calibrate */
    TouchSensor_InitHardware();
    TouchSensor_CalibrateBaseline();
    
    /* Turn on green LED to indicate ready */
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
    
    /* Main loop with debouncing */
    while (1)
    {
        /* Read ADC value */
        status = ADC_ReadBlocking(TOUCH_ADC_INSTANCE, TOUCH_ADC_CHANNEL, &adcValue);
        
        if (status == ADC_STATUS_SUCCESS)
        {
            g_lastAdcValue = adcValue;
            
            /* Check for touch press */
            if (adcValue > TOUCH_THRESHOLD_PRESS)
            {
                pressCounter++;
                releaseCounter = 0;
                
                /* Confirm touch after consecutive readings */
                if (pressCounter >= TOUCH_DEBOUNCE_COUNT)
                {
                    if (g_touchState == TOUCH_STATE_RELEASED)
                    {
                        g_touchState = TOUCH_STATE_PRESSED;
                        TouchSensor_OnTouchPress();
                    }
                    pressCounter = TOUCH_DEBOUNCE_COUNT; /* Prevent overflow */
                }
            }
            /* Check for touch release */
            else if (adcValue < TOUCH_THRESHOLD_RELEASE)
            {
                releaseCounter++;
                pressCounter = 0;
                
                /* Confirm release after consecutive readings */
                if (releaseCounter >= TOUCH_DEBOUNCE_COUNT)
                {
                    if (g_touchState == TOUCH_STATE_PRESSED)
                    {
                        g_touchState = TOUCH_STATE_RELEASED;
                        TouchSensor_OnTouchRelease();
                    }
                    releaseCounter = TOUCH_DEBOUNCE_COUNT; /* Prevent overflow */
                }
            }
            else
            {
                /* In hysteresis zone - maintain current state */
                pressCounter = 0;
                releaseCounter = 0;
            }
        }
        
        /* Delay between samples */
        Delay_Ms(TOUCH_SAMPLE_INTERVAL_MS);
    }
}

/*******************************************************************************
 * Example 3: Touch Detection with Adaptive Threshold
 ******************************************************************************/

/**
 * @brief Example 3 - Touch sensor with adaptive threshold calibration
 * 
 * This example continuously adjusts the baseline and threshold based on
 * environmental changes (temperature, humidity, interference).
 * 
 * @par Adaptive Algorithm:
 * - Periodically recalibrate baseline during no-touch periods
 * - Adjust threshold relative to current baseline
 * - Compensates for drift over time
 * 
 * @note Most robust solution for long-term operation
 */
void TouchSensor_Example3_AdaptiveThreshold(void)
{
    uint16_t adcValue = 0;
    ADC_Status_t status;
    uint32_t sampleCount = 0;
    const uint32_t RECALIBRATE_INTERVAL = 1000; /* Recalibrate every 1000 samples */
    
    /* Initialize hardware and calibrate */
    TouchSensor_InitHardware();
    TouchSensor_CalibrateBaseline();
    
    /* Turn on green LED to indicate ready */
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
    
    /* Main loop with adaptive threshold */
    while (1)
    {
        /* Read ADC value */
        status = ADC_ReadBlocking(TOUCH_ADC_INSTANCE, TOUCH_ADC_CHANNEL, &adcValue);
        
        if (status == ADC_STATUS_SUCCESS)
        {
            g_lastAdcValue = adcValue;
            
            /* Process reading with current thresholds */
            TouchSensor_ProcessReading(adcValue);
            
            /* Periodic recalibration when not touched */
            sampleCount++;
            if (sampleCount >= RECALIBRATE_INTERVAL)
            {
                sampleCount = 0;
                
                /* Only recalibrate if sensor is not currently touched */
                if (g_touchState == TOUCH_STATE_RELEASED)
                {
                    /* Slowly adjust baseline (moving average) */
                    g_touchBaseline = (g_touchBaseline * 3 + adcValue) / 4;
                }
            }
        }
        
        /* Delay between samples */
        Delay_Ms(TOUCH_SAMPLE_INTERVAL_MS);
    }
}

/*******************************************************************************
 * Example 4: Multi-Touch Gesture Detection
 ******************************************************************************/

/**
 * @brief Example 4 - Detect touch patterns (tap, hold, double-tap)
 * 
 * This example demonstrates gesture detection using timing analysis:
 * - Short touch (<500ms) = Single tap
 * - Long touch (>1000ms) = Hold
 * - Two taps within 500ms = Double tap
 * 
 * @par Gesture Detection:
 * - Track touch duration
 * - Measure time between touches
 * - Classify based on timing patterns
 * 
 * @note Can be extended for swipe, multi-finger gestures with multiple sensors
 */
void TouchSensor_Example4_GestureDetection(void)
{
    uint16_t adcValue = 0;
    ADC_Status_t status;
    
    /* Gesture timing variables */
    uint32_t touchStartTime = 0;
    uint32_t touchDuration = 0;
    uint32_t lastTouchTime = 0;
    uint32_t currentTime = 0;
    
    /* Gesture thresholds (in milliseconds) */
    const uint32_t TAP_THRESHOLD = 500;
    const uint32_t HOLD_THRESHOLD = 1000;
    const uint32_t DOUBLE_TAP_WINDOW = 500;
    
    bool lastTouchWasTap = false;
    
    /* Initialize hardware and calibrate */
    TouchSensor_InitHardware();
    TouchSensor_CalibrateBaseline();
    
    /* Turn on green LED to indicate ready */
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
    
    /* Main loop - gesture detection */
    while (1)
    {
        /* Read ADC value */
        status = ADC_ReadBlocking(TOUCH_ADC_INSTANCE, TOUCH_ADC_CHANNEL, &adcValue);
        
        if (status == ADC_STATUS_SUCCESS)
        {
            g_lastAdcValue = adcValue;
            currentTime++; /* Simple time counter (increments every loop) */
            
            /* Touch detected */
            if (adcValue > TOUCH_THRESHOLD_PRESS && g_touchState == TOUCH_STATE_RELEASED)
            {
                g_touchState = TOUCH_STATE_PRESSED;
                touchStartTime = currentTime;
                
                /* Visual feedback */
                GPIO_WritePin(GPIOD, LED_RED_PIN, 1);
            }
            /* Touch released */
            else if (adcValue < TOUCH_THRESHOLD_RELEASE && g_touchState == TOUCH_STATE_PRESSED)
            {
                g_touchState = TOUCH_STATE_RELEASED;
                touchDuration = currentTime - touchStartTime;
                
                /* Classify gesture based on duration */
                if (touchDuration < TAP_THRESHOLD)
                {
                    /* Check for double-tap */
                    if (lastTouchWasTap && (currentTime - lastTouchTime) < DOUBLE_TAP_WINDOW)
                    {
                        /* DOUBLE TAP detected */
                        /* Blink LED rapidly 3 times */
                        for (int i = 0; i < 3; i++)
                        {
                            GPIO_WritePin(GPIOD, LED_RED_PIN, 1);
                            Delay_Ms(50);
                            GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
                            Delay_Ms(50);
                        }
                        lastTouchWasTap = false;
                    }
                    else
                    {
                        /* SINGLE TAP detected */
                        GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
                        Delay_Ms(100);
                        GPIO_WritePin(GPIOD, LED_RED_PIN, 1);
                        Delay_Ms(100);
                        GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
                        
                        lastTouchWasTap = true;
                        lastTouchTime = currentTime;
                    }
                }
                else if (touchDuration > HOLD_THRESHOLD)
                {
                    /* HOLD detected */
                    /* LED stays on for 1 second */
                    GPIO_WritePin(GPIOD, LED_RED_PIN, 1);
                    Delay_Ms(1000);
                    GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
                    
                    lastTouchWasTap = false;
                }
                else
                {
                    /* Medium duration - just turn off LED */
                    GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
                    lastTouchWasTap = false;
                }
            }
        }
        
        /* Delay between samples */
        Delay_Ms(TOUCH_SAMPLE_INTERVAL_MS);
    }
}

/*******************************************************************************
 * Example 5: Touch Sensor with UART Output
 ******************************************************************************/

/**
 * @brief Example 5 - Touch sensor with UART debug output
 * 
 * This example outputs touch sensor data over UART for debugging and analysis.
 * Useful for calibrating thresholds and understanding sensor behavior.
 * 
 * @par UART Output Format:
 * Time(ms), ADC_Value, Baseline, State, Touch_Count
 * 
 * @note Requires UART driver and terminal program (115200 baud)
 * @see uart_example.c for UART setup
 */
void TouchSensor_Example5_WithUARTDebug(void)
{
    uint16_t adcValue = 0;
    ADC_Status_t status;
    uint32_t timeMs = 0;
    
    /* Initialize hardware and calibrate */
    TouchSensor_InitHardware();
    TouchSensor_CalibrateBaseline();
    
    /* TODO: Initialize UART (see uart_example.c) */
    /* UART_Init(LPUART1, 115200); */
    
    /* Print header */
    /* UART_SendString("Time(ms),ADC_Value,Baseline,State,Touch_Count\n"); */
    
    /* Turn on green LED to indicate ready */
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
    
    /* Main loop with UART output */
    while (1)
    {
        /* Read ADC value */
        status = ADC_ReadBlocking(TOUCH_ADC_INSTANCE, TOUCH_ADC_CHANNEL, &adcValue);
        
        if (status == ADC_STATUS_SUCCESS)
        {
            g_lastAdcValue = adcValue;
            
            /* Process reading */
            TouchSensor_ProcessReading(adcValue);
            
            /* Send data over UART every 100ms */
            if ((timeMs % 100) == 0)
            {
                /* Format: Time, ADC, Baseline, State, Count */
                /* Example output: "1200,2650,1950,PRESSED,5" */
                
                /* TODO: Implement UART printf */
                /*
                UART_Printf("%lu,%u,%u,%s,%lu\n",
                    timeMs,
                    adcValue,
                    g_touchBaseline,
                    (g_touchState == TOUCH_STATE_PRESSED) ? "PRESSED" : "RELEASED",
                    g_touchEventCount
                );
                */
            }
        }
        
        timeMs += TOUCH_SAMPLE_INTERVAL_MS;
        Delay_Ms(TOUCH_SAMPLE_INTERVAL_MS);
    }
}

/*******************************************************************************
 * Helper Functions
 ******************************************************************************/

/**
 * @brief Initialize hardware for touch sensor
 * 
 * Configures:
 * - System clock
 * - ADC peripheral
 * - GPIO for LEDs
 * - PORT configuration
 */
static void TouchSensor_InitHardware(void)
{
    ADC_Config_t adcConfig;
    ADC_AverageConfig_t avgConfig;
    
    /* Enable clocks for peripherals */
    PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;     /* ADC0 clock */
    PCC->PCCn[PCC_PORTB_INDEX] |= PCC_PCCn_CGC_MASK;    /* PORTB clock */
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;    /* PORTD clock */
    
    /* Configure PTB13 as ADC input (ADC0_SE12) */
    PORTB->PCR[13] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[13] |= PORT_PCR_MUX(0);  /* Analog function (disable digital) */
    
    /* Configure LED pins as GPIO output */
    PORTD->PCR[LED_RED_PIN] = PORT_PCR_MUX(1);      /* GPIO function */
    PORTD->PCR[LED_GREEN_PIN] = PORT_PCR_MUX(1);    /* GPIO function */
    
    PTD->PDDR |= (1U << LED_RED_PIN);       /* Output direction */
    PTD->PDDR |= (1U << LED_GREEN_PIN);     /* Output direction */
    
    /* Initialize LEDs off */
    GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 0);
    
    /* Configure ADC */
    adcConfig.clockSource = ADC_CLK_ALT1;
    adcConfig.resolution = ADC_RESOLUTION_12BIT;        /* 12-bit for better sensitivity */
    adcConfig.clockDivider = ADC_CLK_DIV_4;
    adcConfig.voltageRef = ADC_VREF_VREFH_VREFL;
    adcConfig.triggerSource = ADC_TRIGGER_SOFTWARE;
    adcConfig.continuousMode = false;
    adcConfig.dmaEnable = false;
    adcConfig.interruptEnable = false;
    
    ADC_Init(TOUCH_ADC_INSTANCE, &adcConfig);
    
    /* Enable hardware averaging for noise reduction */
    avgConfig.enable = true;
    avgConfig.averageMode = ADC_AVERAGE_16;     /* Average 16 samples */
    ADC_ConfigureAveraging(TOUCH_ADC_INSTANCE, &avgConfig);
    
    /* Perform ADC calibration */
    if (ADC_Calibrate(TOUCH_ADC_INSTANCE) != ADC_STATUS_SUCCESS)
    {
        /* Calibration failed - blink red LED rapidly */
        while (1)
        {
            GPIO_WritePin(GPIOD, LED_RED_PIN, 1);
            Delay_Ms(100);
            GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
            Delay_Ms(100);
        }
    }
}

/**
 * @brief Calibrate baseline ADC value (no touch condition)
 * 
 * Takes multiple samples and averages them to establish baseline.
 * Should be called when sensor is definitely not touched.
 */
static void TouchSensor_CalibrateBaseline(void)
{
    uint32_t sum = 0;
    uint16_t adcValue = 0;
    
    /* Blink green LED during calibration */
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
    Delay_Ms(500);
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 0);
    Delay_Ms(500);
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 1);
    
    /* Take multiple samples */
    for (uint8_t i = 0; i < TOUCH_BASELINE_SAMPLES; i++)
    {
        if (ADC_ReadBlocking(TOUCH_ADC_INSTANCE, TOUCH_ADC_CHANNEL, &adcValue) == ADC_STATUS_SUCCESS)
        {
            sum += adcValue;
        }
        Delay_Ms(10);
    }
    
    /* Calculate average baseline */
    g_touchBaseline = sum / TOUCH_BASELINE_SAMPLES;
    
    /* Turn off green LED */
    GPIO_WritePin(GPIOD, LED_GREEN_PIN, 0);
}

/**
 * @brief Process ADC reading and update touch state
 * 
 * @param adcValue Current ADC reading
 */
static void TouchSensor_ProcessReading(uint16_t adcValue)
{
    /* Check for touch press (with hysteresis) */
    if (adcValue > TOUCH_THRESHOLD_PRESS && g_touchState == TOUCH_STATE_RELEASED)
    {
        g_touchState = TOUCH_STATE_PRESSED;
        TouchSensor_OnTouchPress();
    }
    /* Check for touch release (with hysteresis) */
    else if (adcValue < TOUCH_THRESHOLD_RELEASE && g_touchState == TOUCH_STATE_PRESSED)
    {
        g_touchState = TOUCH_STATE_RELEASED;
        TouchSensor_OnTouchRelease();
    }
}

/**
 * @brief Callback when touch is pressed
 */
static void TouchSensor_OnTouchPress(void)
{
    /* Turn on red LED */
    GPIO_WritePin(GPIOD, LED_RED_PIN, 1);
    
    /* Increment touch event counter */
    g_touchEventCount++;
}

/**
 * @brief Callback when touch is released
 */
static void TouchSensor_OnTouchRelease(void)
{
    /* Turn off red LED */
    GPIO_WritePin(GPIOD, LED_RED_PIN, 0);
}

/**
 * @brief Simple delay function (approximate)
 * 
 * @param ms Delay in milliseconds
 * @note Not accurate - for demonstration only. Use SysTick for precise timing.
 */
static void Delay_Ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        for (volatile uint32_t j = 0; j < 8000; j++)
        {
            __asm("NOP");
        }
    }
}

/*******************************************************************************
 * Utility Functions for Advanced Users
 ******************************************************************************/

/**
 * @brief Get current touch state
 * @return Current touch state (PRESSED or RELEASED)
 */
TouchState_t TouchSensor_GetState(void)
{
    return g_touchState;
}

/**
 * @brief Get last ADC reading
 * @return Last ADC value (0-4095 for 12-bit)
 */
uint16_t TouchSensor_GetLastReading(void)
{
    return g_lastAdcValue;
}

/**
 * @brief Get baseline ADC value
 * @return Baseline ADC value
 */
uint16_t TouchSensor_GetBaseline(void)
{
    return g_touchBaseline;
}

/**
 * @brief Get total touch event count
 * @return Number of touch events since startup
 */
uint32_t TouchSensor_GetTouchCount(void)
{
    return g_touchEventCount;
}

/**
 * @brief Reset touch event counter
 */
void TouchSensor_ResetTouchCount(void)
{
    g_touchEventCount = 0;
}

/*******************************************************************************
 * End of File
 ******************************************************************************/
