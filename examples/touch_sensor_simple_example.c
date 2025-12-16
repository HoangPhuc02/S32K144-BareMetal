/**
 * @file    touch_sensor_simple_example.c
 * @brief   Simple Touch Sensor Example - Single Button
 * @details Minimalist example for learning touch sensor basics:
 *          - Single touch button
 *          - Software polling (no interrupts)
 *          - LED indicator
 *          - Easy to understand and modify
 * 
 * @author  PhucPH32
 * @date    16/12/2025
 * @version 1.0
 * 
 * @hardware_setup
 * - Touch sensor: ADC0_SE12 (PTB13)
 * - LED indicator: PTD15 (Red LED)
 * 
 * @wiring
 *   Touch Plate ---- 1MΩ ---- PTB13 (ADC0_SE12)
 *                              |
 *                            10kΩ 
 *                              |
 *                             GND
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../lib/middleware/TouchSensor/touch_sensor.h"
#include "../lib/hal/gpio/gpio.h"
#include "../lib/hal/port/port.h"
#include "pcc_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Touch sensor configuration */
#define TOUCH_ADC_CHANNEL       (12U)   /* PTB13 - ADC0_SE12 */
#define TOUCH_THRESHOLD         (200U)  /* Sensitivity threshold */

/* LED configuration */
#define LED_PORT                PORT_D
#define LED_GPIO                GPIO_PORT_D
#define LED_PIN                 (15U)   /* Red LED */

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static volatile bool g_touchDetected = false;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 */
static void InitClocks(void)
{
    /* Enable PORT B for touch sensor */
    PCC->PCCn[PCC_PORTB_INDEX] = PCC_PCCn_CGC_MASK;
    
    /* Enable PORT D for LED */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
    
    /* Enable ADC0 */
    PCC->PCCn[PCC_ADC0_INDEX] = PCC_PCCn_CGC_MASK;
}

/**
 * @brief Initialize LED
 */
static void InitLED(void)
{
    PORT_SetPinMux(LED_PORT, LED_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_GPIO, LED_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_GPIO, LED_PIN, 1);  /* LED off (active low) */
}

/**
 * @brief Touch event callback
 */
static void TouchCallback(uint8_t channel, touch_state_t state, int16_t delta)
{
    (void)channel;  /* Unused */
    (void)delta;    /* Unused */
    
    if (state == TOUCH_STATE_TOUCHED) {
        /* Turn on LED when touched */
        GPIO_WritePin(LED_GPIO, LED_PIN, 0);
        g_touchDetected = true;
    } else if (state == TOUCH_STATE_RELEASED) {
        /* Turn off LED when released */
        GPIO_WritePin(LED_GPIO, LED_PIN, 1);
        g_touchDetected = false;
    }
}

/**
 * @brief Simple delay
 */
static void Delay(uint32_t ms)
{
    volatile uint32_t count = ms * 20000;
    while (count--) {
        __asm("NOP");
    }
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

int main(void)
{
    touch_status_t status;
    
    /* Initialize hardware */
    InitClocks();
    InitLED();
    
    /* Configure touch sensor pin as analog input */
    PORT_SetPinMux(PORT_B, 13, PORT_MUX_ANALOG);
    
    /* Configure touch sensor system - Software trigger (polling) */
    touch_system_config_t sysConfig = {
        .trigger_mode = TOUCH_TRIGGER_SW,
        .scan_period_us = 0,  /* Not used in SW mode */
        .num_channels = 1,
        .callback = TouchCallback
    };
    
    status = TOUCH_Init(&sysConfig);
    if (status != TOUCH_STATUS_SUCCESS) {
        /* Error - blink LED rapidly */
        while (1) {
            GPIO_TogglePin(LED_GPIO, LED_PIN);
            Delay(100);
        }
    }
    
    /* Configure touch channel */
    touch_channel_config_t chConfig = {
        .adc_channel = TOUCH_ADC_CHANNEL,
        .baseline = 0,  /* Will be calibrated */
        .threshold = TOUCH_THRESHOLD,
        .debounce_count = 3,
        .enable_drift_compensation = true
    };
    
    TOUCH_ConfigChannel(0, &chConfig);
    
    /* Calibrate baseline - don't touch during this time */
    /* Blink LED 3 times to indicate calibration */
    for (uint8_t i = 0; i < 3; i++) {
        GPIO_WritePin(LED_GPIO, LED_PIN, 0);
        Delay(200);
        GPIO_WritePin(LED_GPIO, LED_PIN, 1);
        Delay(200);
    }
    
    TOUCH_CalibrateAll(32);  /* 32 samples for baseline */
    
    /* LED ON briefly to indicate ready */
    GPIO_WritePin(LED_GPIO, LED_PIN, 0);
    Delay(500);
    GPIO_WritePin(LED_GPIO, LED_PIN, 1);
    
    /* Start scanning */
    TOUCH_StartScan();
    
    /* Main loop - process touch sensor */
    while (1) {
        /* Process touch sensor (polling mode) */
        TOUCH_Process();
        
        /* Small delay between scans */
        Delay(10);  /* 10ms = 100Hz scan rate */
    }
    
    return 0;
}

/*******************************************************************************
 * Usage Instructions
 ******************************************************************************/

/*
 * HOW TO USE:
 * 
 * 1. Wire up one touch pad:
 *    - Connect 1MΩ resistor between touch plate and PTB13
 *    - Connect 10kΩ resistor between PTB13 and GND
 *    - Optional: 100nF capacitor between PTB13 and GND
 * 
 * 2. Compile and flash this example
 * 
 * 3. After reset:
 *    - LED blinks 3 times (calibration - don't touch!)
 *    - LED turns on briefly (ready)
 *    - LED off (idle state)
 * 
 * 4. Touch the pad:
 *    - LED turns ON
 * 
 * 5. Release the pad:
 *    - LED turns OFF
 * 
 * TROUBLESHOOTING:
 * 
 * - LED doesn't respond to touch:
 *   * Increase wire length between 1MΩ resistor and touch plate
 *   * Use larger touch plate (2cm x 2cm minimum)
 *   * Decrease TOUCH_THRESHOLD (try 150 or 100)
 * 
 * - False triggers (LED flickers):
 *   * Add 100nF capacitor to GND
 *   * Increase debounce_count to 5
 *   * Increase TOUCH_THRESHOLD (try 300)
 * 
 * - LED blinks rapidly on startup:
 *   * Touch sensor initialization failed
 *   * Check ADC peripheral is enabled
 *   * Check pin configuration
 * 
 * NEXT STEPS:
 * 
 * Once this example works, try:
 * - Add more touch buttons (up to 8 channels)
 * - Use UART to print touch values
 * - Switch to PDB hardware trigger (see complete example)
 * - Add different actions for each button
 */
