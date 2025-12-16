/**
 * @file    touch_sensor_complete_example.c
 * @brief   Complete Touch Sensor Example with Multi-Channel Support
 * @details Demonstrates a practical capacitive touch sensor application with:
 *          - 4 touch buttons with individual callbacks
 *          - PDB hardware trigger at 100Hz scan rate
 *          - LED indicators for each button
 *          - UART debug output
 *          - Baseline auto-calibration
 *          - Drift compensation
 * 
 * @author  PhucPH32
 * @date    16/12/2025
 * @version 1.0
 * 
 * @hardware_setup
 * Touch Sensors:
 *   - Button 1: ADC0_SE8  (PTB0)  -> Controls Red LED    (PTD15)
 *   - Button 2: ADC0_SE9  (PTB1)  -> Controls Green LED  (PTD16)
 *   - Button 3: ADC0_SE12 (PTB13) -> Controls Blue LED   (PTD0)
 *   - Button 4: ADC0_SE13 (PTB14) -> Triggers Action
 * 
 * Debug Output:
 *   - UART: PTC6/PTC7 @ 115200 baud
 * 
 * @wiring_diagram
 * Each Touch Pad:
 *   Touch Plate ---- 1MΩ ---- ADC Pin (PTBx)
 *                              |
 *                            10kΩ (pull-down)
 *                              |
 *                             GND
 * 
 * Optional: 100nF capacitor to GND for noise filtering
 * 
 * @operation
 * 1. System initializes and calibrates all touch channels
 * 2. PDB triggers ADC scanning at 100Hz (10ms interval)
 * 3. Touch detection with automatic drift compensation
 * 4. Each button controls specific LED or function
 * 5. UART outputs touch events and diagnostics
 * 
 * @copyright Copyright (c) 2025
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../lib/middleware/TouchSensor/touch_sensor.h"
#include "../lib/hal/adc/adc.h"
#include "../lib/hal/pdb/pdb.h"
#include "../lib/hal/gpio/gpio.h"
#include "../lib/hal/port/port.h"
#include "../lib/hal/clock/clock.h"
#include "../lib/hal/uart/uart.h"
#include "pcc_reg.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Touch Button Configuration */
#define TOUCH_BTN1_ADC_CH       (8U)    /* PTB0  - ADC0_SE8  */
#define TOUCH_BTN2_ADC_CH       (9U)    /* PTB1  - ADC0_SE9  */
#define TOUCH_BTN3_ADC_CH       (12U)   /* PTB13 - ADC0_SE12 */
#define TOUCH_BTN4_ADC_CH       (13U)   /* PTB14 - ADC0_SE13 */

#define NUM_TOUCH_BUTTONS       (4U)

/* LED Configuration */
#define LED_RED_PORT            PORT_D
#define LED_RED_GPIO            GPIO_PORT_D
#define LED_RED_PIN             (15U)   /* PTD15 - Red LED */

#define LED_GREEN_PORT          PORT_D
#define LED_GREEN_GPIO          GPIO_PORT_D
#define LED_GREEN_PIN           (16U)   /* PTD16 - Green LED */

#define LED_BLUE_PORT           PORT_D
#define LED_BLUE_GPIO           GPIO_PORT_D
#define LED_BLUE_PIN            (0U)    /* PTD0 - Blue LED */

/* Touch Detection Parameters */
#define TOUCH_THRESHOLD         (200U)  /* Delta from baseline */
#define TOUCH_DEBOUNCE_COUNT    (3U)    /* 3 consecutive samples */
#define TOUCH_SCAN_RATE_HZ      (100U)  /* 100 Hz scan rate */
#define TOUCH_BASELINE_SAMPLES  (32U)   /* Calibration samples */

/* UART Configuration */
#define UART_PORT               PORT_C
#define UART_TX_PIN             (7U)    /* PTC7 - LPUART1_TX */
#define UART_RX_PIN             (6U)    /* PTC6 - LPUART1_RX */
#define UART_BAUDRATE           (115200U)

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

typedef struct {
    uint8_t channel;
    uint8_t adc_ch;
    const char *name;
    GPIO_Type *led_port;
    uint8_t led_pin;
    bool led_state;
    uint32_t press_count;
} touch_button_t;

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/* UART instance */
static LPUART_Type *g_uartInstance = LPUART1;

/* Touch button definitions */
static touch_button_t g_touchButtons[NUM_TOUCH_BUTTONS] = {
    {0, TOUCH_BTN1_ADC_CH, "Button1", LED_RED_GPIO,   LED_RED_PIN,   false, 0},
    {1, TOUCH_BTN2_ADC_CH, "Button2", LED_GREEN_GPIO, LED_GREEN_PIN, false, 0},
    {2, TOUCH_BTN3_ADC_CH, "Button3", LED_BLUE_GPIO,  LED_BLUE_PIN,  false, 0},
    {3, TOUCH_BTN4_ADC_CH, "Button4", NULL,           0,             false, 0}
};

/* System status */
static volatile bool g_systemReady = false;
static volatile uint32_t g_totalTouches = 0;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void InitSystemClocks(void);
static void InitUART(void);
static void InitLEDs(void);
static void InitTouchSensors(void);
static void UART_Printf(const char *format, ...);
static void TouchButton_Callback(uint8_t channel, touch_state_t state, int16_t delta);
static void PrintSystemStatus(void);
static void SimpleDelay(uint32_t ms);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 */
static void InitSystemClocks(void)
{
    /* Enable PORT clocks */
    PCC->PCCn[PCC_PORTB_INDEX] = PCC_PCCn_CGC_MASK;  /* Touch sensor pins */
    PCC->PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK;  /* UART pins */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;  /* LED pins */
    
    /* Enable peripheral clocks */
    PCC->PCCn[PCC_ADC0_INDEX] = PCC_PCCn_CGC_MASK;   /* ADC0 */
    PCC->PCCn[PCC_PDB0_INDEX] = PCC_PCCn_CGC_MASK;   /* PDB0 */
    PCC->PCCn[PCC_LPUART1_INDEX] = PCC_PCCn_PCS(2) | PCC_PCCn_CGC_MASK;  /* UART */
}

/**
 * @brief Initialize UART for debug output
 */
static void InitUART(void)
{
    /* Configure UART pins */
    PORT_SetPinMux(UART_PORT, UART_RX_PIN, PORT_MUX_ALT2);
    PORT_SetPinMux(UART_PORT, UART_TX_PIN, PORT_MUX_ALT2);
    
    /* UART configuration */
    UART_Config_t uartConfig = {
        .baudRate = UART_BAUDRATE,
        .parity = UART_PARITY_DISABLED,
        .stopBits = UART_ONE_STOP_BIT,
        .dataBits = UART_8_DATA_BITS,
        .enableRx = true,
        .enableTx = true
    };
    
    UART_Init(g_uartInstance, &uartConfig, 8000000);  /* 8 MHz SOSCDIV2 */
}

/**
 * @brief UART printf function
 */
static void UART_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len > 0 && len < (int)sizeof(buffer)) {
        UART_SendBlocking(g_uartInstance, (const uint8_t*)buffer, (uint32_t)len);
    }
}

/**
 * @brief Initialize LED indicators
 */
static void InitLEDs(void)
{
    /* Configure LED pins as GPIO outputs */
    PORT_SetPinMux(LED_RED_PORT, LED_RED_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_RED_GPIO, LED_RED_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 1);  /* Off (active low) */
    
    PORT_SetPinMux(LED_GREEN_PORT, LED_GREEN_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_GREEN_GPIO, LED_GREEN_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 1);  /* Off (active low) */
    
    PORT_SetPinMux(LED_BLUE_PORT, LED_BLUE_PIN, PORT_MUX_GPIO);
    GPIO_SetPinDirection(LED_BLUE_GPIO, LED_BLUE_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_BLUE_GPIO, LED_BLUE_PIN, 1);  /* Off (active low) */
}

/**
 * @brief Initialize touch sensor system
 */
static void InitTouchSensors(void)
{
    touch_status_t status;
    
    UART_Printf("Initializing Touch Sensor System...\n");
    
    /* Configure touch sensor pins as analog inputs */
    PORT_SetPinMux(PORT_B, 0, PORT_MUX_ANALOG);   /* PTB0  - ADC0_SE8  */
    PORT_SetPinMux(PORT_B, 1, PORT_MUX_ANALOG);   /* PTB1  - ADC0_SE9  */
    PORT_SetPinMux(PORT_B, 13, PORT_MUX_ANALOG);  /* PTB13 - ADC0_SE12 */
    PORT_SetPinMux(PORT_B, 14, PORT_MUX_ANALOG);  /* PTB14 - ADC0_SE13 */
    
    /* Touch system configuration */
    touch_system_config_t sysConfig = {
        .trigger_mode = TOUCH_TRIGGER_HW_PDB,
        .scan_period_us = 10000,  /* 10ms = 100Hz */
        .num_channels = NUM_TOUCH_BUTTONS,
        .callback = TouchButton_Callback
    };
    
    status = TOUCH_Init(&sysConfig);
    if (status != TOUCH_STATUS_SUCCESS) {
        UART_Printf("ERROR: Touch system init failed!\n");
        return;
    }
    UART_Printf("Touch system initialized (PDB trigger @ %d Hz)\n", TOUCH_SCAN_RATE_HZ);
    
    /* Configure each touch channel */
    for (uint8_t i = 0; i < NUM_TOUCH_BUTTONS; i++) {
        touch_channel_config_t chConfig = {
            .adc_channel = g_touchButtons[i].adc_ch,
            .baseline = 0,  /* Will be calibrated */
            .threshold = TOUCH_THRESHOLD,
            .debounce_count = TOUCH_DEBOUNCE_COUNT,
            .enable_drift_compensation = true
        };
        
        status = TOUCH_ConfigChannel(i, &chConfig);
        if (status != TOUCH_STATUS_SUCCESS) {
            UART_Printf("ERROR: Failed to configure %s!\n", g_touchButtons[i].name);
            continue;
        }
        UART_Printf("Configured %s (ADC CH%d)\n", g_touchButtons[i].name, 
                    g_touchButtons[i].adc_ch);
    }
    
    /* Calibrate all channels */
    UART_Printf("\nCalibrating baselines (do not touch)...\n");
    SimpleDelay(500);
    
    status = TOUCH_CalibrateAll(TOUCH_BASELINE_SAMPLES);
    if (status == TOUCH_STATUS_SUCCESS) {
        UART_Printf("Calibration complete!\n\n");
        
        /* Print baseline values */
        for (uint8_t i = 0; i < NUM_TOUCH_BUTTONS; i++) {
            touch_channel_t *ch = NULL;
            if (TOUCH_GetChannelData(i, &ch) == TOUCH_STATUS_SUCCESS) {
                UART_Printf("%s: Baseline = %d\n", g_touchButtons[i].name, ch->baseline);
            }
        }
    } else {
        UART_Printf("ERROR: Calibration failed!\n");
    }
    
    /* Start scanning */
    status = TOUCH_StartScan();
    if (status == TOUCH_STATUS_SUCCESS) {
        UART_Printf("\nTouch sensing started!\n");
        g_systemReady = true;
    } else {
        UART_Printf("ERROR: Failed to start scanning!\n");
    }
}

/**
 * @brief Touch button callback - handles touch events
 */
static void TouchButton_Callback(uint8_t channel, touch_state_t state, int16_t delta)
{
    if (channel >= NUM_TOUCH_BUTTONS) {
        return;
    }
    
    touch_button_t *btn = &g_touchButtons[channel];
    
    switch (state) {
        case TOUCH_STATE_TOUCHED:
            /* Button pressed */
            btn->press_count++;
            g_totalTouches++;
            
            /* Toggle LED if assigned */
            if (btn->led_port != NULL) {
                btn->led_state = !btn->led_state;
                GPIO_WritePin(btn->led_port, btn->led_pin, btn->led_state ? 0 : 1);
            }
            
            /* Special action for Button 4 */
            if (channel == 3) {
                PrintSystemStatus();
            }
            
            UART_Printf("[TOUCH] %s pressed (delta=%d, count=%lu)\n", 
                       btn->name, delta, btn->press_count);
            break;
            
        case TOUCH_STATE_RELEASED:
            /* Button released */
            UART_Printf("[RELEASE] %s released\n", btn->name);
            break;
            
        default:
            break;
    }
}

/**
 * @brief Print system status and statistics
 */
static void PrintSystemStatus(void)
{
    UART_Printf("\n=== Touch Sensor Status ===\n");
    UART_Printf("Total touches: %lu\n", g_totalTouches);
    
    for (uint8_t i = 0; i < NUM_TOUCH_BUTTONS; i++) {
        touch_button_t *btn = &g_touchButtons[i];
        touch_channel_t *ch = NULL;
        
        UART_Printf("\n%s:\n", btn->name);
        UART_Printf("  Press count: %lu\n", btn->press_count);
        
        if (TOUCH_GetChannelData(i, &ch) == TOUCH_STATUS_SUCCESS) {
            UART_Printf("  Raw value: %d\n", ch->raw_value);
            UART_Printf("  Baseline: %d\n", ch->baseline);
            UART_Printf("  Delta: %d\n", ch->delta);
            UART_Printf("  State: %s\n", 
                       (ch->state == TOUCH_STATE_TOUCHED) ? "TOUCHED" : "IDLE");
        }
        
        if (btn->led_port != NULL) {
            UART_Printf("  LED: %s\n", btn->led_state ? "ON" : "OFF");
        }
    }
    UART_Printf("===========================\n\n");
}

/**
 * @brief Simple delay function
 */
static void SimpleDelay(uint32_t ms)
{
    volatile uint32_t count = ms * 20000;  /* Approximate for 80 MHz */
    while (count--) {
        __asm("NOP");
    }
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main application entry point
 */
int main(void)
{
    /* Initialize system clocks (80 MHz) */
    /* Note: Add your clock initialization here */
    /* SOSC_init_8MHz(); */
    /* SPLL_init_160MHz(); */
    /* NormalRUNmode_80MHz(); */
    
    /* Initialize peripherals */
    InitSystemClocks();
    InitUART();
    
    /* Print welcome message */
    UART_Printf("\n");
    UART_Printf("========================================\n");
    UART_Printf("  Touch Sensor Example - S32K144\n");
    UART_Printf("========================================\n");
    UART_Printf("Hardware Setup:\n");
    UART_Printf("  Button 1 (PTB0):  Controls Red LED\n");
    UART_Printf("  Button 2 (PTB1):  Controls Green LED\n");
    UART_Printf("  Button 3 (PTB13): Controls Blue LED\n");
    UART_Printf("  Button 4 (PTB14): Shows system status\n");
    UART_Printf("\n");
    UART_Printf("Features:\n");
    UART_Printf("  - PDB hardware trigger @ 100Hz\n");
    UART_Printf("  - Auto baseline calibration\n");
    UART_Printf("  - Drift compensation enabled\n");
    UART_Printf("  - Debounce filtering (3 samples)\n");
    UART_Printf("========================================\n\n");
    
    /* Initialize hardware */
    InitLEDs();
    InitTouchSensors();
    
    /* Main loop */
    if (g_systemReady) {
        UART_Printf("System ready! Touch buttons to test.\n\n");
        
        /* Blink all LEDs to indicate ready */
        for (uint8_t i = 0; i < 3; i++) {
            GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 0);
            GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 0);
            GPIO_WritePin(LED_BLUE_GPIO, LED_BLUE_PIN, 0);
            SimpleDelay(100);
            GPIO_WritePin(LED_RED_GPIO, LED_RED_PIN, 1);
            GPIO_WritePin(LED_GREEN_GPIO, LED_GREEN_PIN, 1);
            GPIO_WritePin(LED_BLUE_GPIO, LED_BLUE_PIN, 1);
            SimpleDelay(100);
        }
        
        while (1) {
            /* Touch processing happens in interrupt (PDB-triggered ADC) */
            /* Main loop can do other tasks or enter low power mode */
            __asm("WFI");  /* Wait for interrupt */
        }
    } else {
        UART_Printf("ERROR: System initialization failed!\n");
        while (1) {
            /* Error state - blink red LED */
            GPIO_TogglePin(LED_RED_GPIO, LED_RED_PIN);
            SimpleDelay(500);
        }
    }
    
    return 0;
}

/*******************************************************************************
 * Example Usage Notes
 ******************************************************************************/

/*
 * WIRING EXAMPLE FOR ONE TOUCH PAD:
 * 
 * Components needed per pad:
 * - Copper touch plate (2cm x 2cm minimum)
 * - 1 MΩ resistor (high impedance)
 * - 10 kΩ resistor (pull-down)
 * - Optional: 100 nF capacitor (noise filter)
 * 
 * Connection:
 * 
 *     +3.3V
 *       |
 *       R (1MΩ)
 *       |
 *       +-------- Touch Plate
 *       |
 *       +-------- PTBx (ADC input)
 *       |
 *       R (10kΩ)
 *       |
 *      GND
 * 
 * Optional capacitor: 100nF between PTBx and GND
 * 
 * CALIBRATION:
 * - System automatically calibrates on startup
 * - Do not touch pads during calibration (first 2 seconds)
 * - Baseline values are displayed via UART
 * - Drift compensation adjusts baseline over time
 * 
 * THRESHOLD ADJUSTMENT:
 * - Default threshold: 200 ADC counts from baseline
 * - Increase if too sensitive (false triggers)
 * - Decrease if not sensitive enough (missed touches)
 * - Typical delta when touched: 300-800 counts
 * 
 * DEBUGGING:
 * - Monitor UART output for touch events
 * - Press Button 4 to see system status
 * - Check baseline and delta values
 * - Raw ADC values should be stable when not touched
 * 
 * PERFORMANCE:
 * - Scan rate: 100 Hz (10ms per channel)
 * - Response time: 30-50ms (with debouncing)
 * - CPU usage: <1% (hardware triggered)
 * - Support up to 8 channels
 */
