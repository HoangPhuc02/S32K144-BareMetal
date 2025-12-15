/**
 * @file    adc_lpit_trigger_example.c
 * @brief   ADC Hardware Trigger using LPIT Example for S32K144
 * @details
 * Example minh họa cách sử dụng LPIT để trigger ADC conversion tự động.
 * LPIT sẽ tạo periodic trigger để ADC thực hiện conversion theo chu kỳ cố định.
 * 
 * Hardware Setup:
 * - ADC0 Channel 0 (PTD0/ADC0_SE0): Analog input
 * - Green LED (PTD16): Toggle mỗi khi có conversion
 * - Red LED (PTD15): Bật khi voltage > threshold
 * 
 * Features:
 * - LPIT trigger ADC tự động (periodic sampling)
 * - ADC interrupt khi conversion complete
 * - Real-time voltage monitoring
 * - Threshold detection
 * 
 * @author  PhucPH32
 * @date    04/12/2025
 * @version 1.0
 */

#include "adc.h"
#include "lpit.h"
#include "port.h"
#include "gpio.h"
#include "nvic.h"
#include "pcc_reg.h"
#include "clocks_and_modes.h"
#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* ADC Configuration */
#define ADC_INSTANCE                ADC_INSTANCE_0
#define ADC_CHANNEL                 ADC_CHANNEL_AD0
#define ADC_INPUT_PORT              PORT_D
#define ADC_INPUT_PIN               (0U)            /* PTD0 = ADC0_SE0 */
#define ADC_VREF_MV                 (3300U)         /* 3.3V reference */

/* LPIT Configuration */
#define LPIT_CHANNEL                (0U)
#define LPIT_CLOCK_FREQ             (48000000U)     /* 48 MHz FIRC */
#define ADC_SAMPLE_RATE_HZ          (1000U)         /* 1 kHz sampling */
#define LPIT_PERIOD                 (LPIT_CLOCK_FREQ / ADC_SAMPLE_RATE_HZ)

/* LED Configuration */
#define LED_GREEN_PORT              GPIO_PORT_D
#define LED_GREEN_PIN               (16U)
#define LED_RED_PORT                GPIO_PORT_D
#define LED_RED_PIN                 (15U)

/* Voltage threshold for LED (in mV) */
#define VOLTAGE_THRESHOLD_MV        (1650U)         /* 1.65V (50% of 3.3V) */

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

static volatile uint32_t g_adcSampleCount = 0;
static volatile uint16_t g_latestAdcValue = 0;
static volatile uint32_t g_latestVoltage = 0;
static volatile bool g_newSampleAvailable = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void InitClocks(void);
static void InitLEDs(void);
static void InitADC(void);
static void InitLPIT(void);
static void ADC_ConversionCallback(ADC_Instance_t instance, uint16_t result);
static void PrintStatistics(void);

/*******************************************************************************
 * ADC Callback
 ******************************************************************************/

/**
 * @brief ADC conversion complete callback
 */
static void ADC_ConversionCallback(ADC_Instance_t instance, uint16_t result)
{
    (void)instance;
    
    /* Store result */
    g_latestAdcValue = result;
    
    /* Convert to voltage */
    g_latestVoltage = ADC_ConvertToVoltage(result, ADC_RESOLUTION_12BIT, ADC_VREF_MV);
    
    /* Increment sample count */
    g_adcSampleCount++;
    
    /* Set flag for main loop */
    g_newSampleAvailable = true;
    
    /* Toggle Green LED to show activity */
    GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
    
    /* Control Red LED based on voltage threshold */
    if (g_latestVoltage > VOLTAGE_THRESHOLD_MV) {
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, 0U);  /* Turn ON */
    } else {
        GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, 1U);  /* Turn OFF */
    }
}

/*******************************************************************************
 * Initialization Functions
 ******************************************************************************/

/**
 * @brief Initialize system clocks
 */
static void InitClocks(void)
{
    /* Enable clocks for PORTD (ADC input + LEDs) */
    PCC->PCCn[PCC_PORTD_INDEX] = PCC_PCCn_CGC_MASK;
    
    /* Enable ADC0 clock with FIRC as source */
    PCC->PCCn[PCC_ADC0_INDEX] = PCC_PCCn_PCS(0x3)   /* FIRC (48 MHz) */
                              | PCC_PCCn_CGC_MASK;
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
 * @brief Initialize ADC
 */
static void InitADC(void)
{
    ADC_Config_t adcConfig;
    ADC_Status_t status;
    
    /* Configure ADC input pin as analog */
    PORT_SetPinMux(ADC_INPUT_PORT, ADC_INPUT_PIN, PORT_MUX_ANALOG);
    
    /* Configure ADC */
    adcConfig.clockSource = ADC_CLK_ALT1;           /* Use ALTCLK1 (FIRC) */
    adcConfig.resolution = ADC_RESOLUTION_12BIT;    /* 12-bit resolution */
    adcConfig.clockDivider = ADC_CLK_DIV_4;         /* Divide by 4 */
    adcConfig.voltageRef = ADC_VREF_VREFH_VREFL;    /* VREFH/VREFL */
    adcConfig.triggerSource = ADC_TRIGGER_HARDWARE; /* Hardware trigger */
    adcConfig.continuousMode = false;               /* Single conversion per trigger */
    adcConfig.dmaEnable = false;
    adcConfig.interruptEnable = true;               /* Enable interrupt */
    
    /* Initialize ADC */
    status = ADC_Init(ADC_INSTANCE, &adcConfig);
    if (status != ADC_STATUS_SUCCESS) {
        printf("ADC Init failed!\n");
        return;
    }
    
    /* Calibrate ADC */
    status = ADC_Calibrate(ADC_INSTANCE);
    if (status != ADC_STATUS_SUCCESS) {
        printf("ADC Calibration failed!\n");
        return;
    }
    
    /* Register callback */
    ADC_RegisterCallback(ADC_INSTANCE, ADC_ConversionCallback);
    
    /* Configure for hardware trigger */
    ADC_ConfigHardwareTrigger(ADC_INSTANCE, ADC_CHANNEL, true);
    
    /* Enable ADC interrupt in NVIC */
    NVIC_SetPriority(ADC0_IRQn, 3);
    NVIC_EnableIRQ(ADC0_IRQn);
    
    printf("ADC initialized and calibrated\n");
    printf("  - Resolution: 12-bit\n");
    printf("  - Trigger: LPIT hardware trigger\n");
    printf("  - Sample rate: %lu Hz\n", ADC_SAMPLE_RATE_HZ);
}

/**
 * @brief Initialize LPIT
 */
static void InitLPIT(void)
{
    lpit_channel_config_t lpitConfig;
    status_t status;
    
    /* Initialize LPIT with FIRC clock */
    status = LPIT_Init(LPIT_CLK_SRC_FIRC);
    if (status != STATUS_SUCCESS) {
        printf("LPIT Init failed!\n");
        return;
    }
    
    /* Configure LPIT channel 0 to trigger ADC */
    lpitConfig.channel = LPIT_CHANNEL;
    lpitConfig.mode = LPIT_MODE_32BIT_PERIODIC;
    lpitConfig.period = LPIT_PERIOD;
    lpitConfig.enableInterrupt = false;             /* No LPIT interrupt needed */
    lpitConfig.chainChannel = false;
    lpitConfig.startOnTrigger = false;
    lpitConfig.stopOnInterrupt = false;
    lpitConfig.reloadOnTrigger = false;
    
    /* Configure and start LPIT */
    status = LPIT_ConfigChannel(&lpitConfig);
    if (status != STATUS_SUCCESS) {
        printf("LPIT Config failed!\n");
        return;
    }
    
    status = LPIT_StartChannel(LPIT_CHANNEL);
    if (status != STATUS_SUCCESS) {
        printf("LPIT Start failed!\n");
        return;
    }
    
    printf("LPIT initialized and started\n");
    printf("  - Trigger frequency: %lu Hz\n", ADC_SAMPLE_RATE_HZ);
    printf("  - Period: %lu ticks\n", LPIT_PERIOD);
}

/*******************************************************************************
 * Utility Functions
 ******************************************************************************/

/**
 * @brief Print ADC statistics
 */
static void PrintStatistics(void)
{
    printf("\n=== ADC Statistics ===\n");
    printf("Sample count: %lu\n", g_adcSampleCount);
    printf("Latest ADC value: %u (0x%03X)\n", g_latestAdcValue, g_latestAdcValue);
    printf("Latest voltage: %lu mV\n", g_latestVoltage);
    printf("Threshold: %u mV\n", VOLTAGE_THRESHOLD_MV);
    printf("======================\n\n");
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main function
 */
int main(void)
{
    uint32_t printCounter = 0;
    const uint32_t PRINT_INTERVAL = 1000;  /* Print every 1000 samples */
    
    /* Initialize system */
    SOSC_init_8MHz();
    SPLL_init_160MHz();
    NormalRUNmode_80MHz();
    
    printf("\n");
    printf("===========================================\n");
    printf("  ADC LPIT Hardware Trigger Example\n");
    printf("===========================================\n");
    printf("Hardware:\n");
    printf("  - ADC0 CH0 (PTD0): Analog input\n");
    printf("  - Green LED (PTD16): Activity indicator\n");
    printf("  - Red LED (PTD15): Voltage > %.2fV\n", VOLTAGE_THRESHOLD_MV / 1000.0f);
    printf("  - LPIT CH0: ADC trigger at %lu Hz\n", ADC_SAMPLE_RATE_HZ);
    printf("\n");
    printf("Operation:\n");
    printf("  - LPIT triggers ADC conversion periodically\n");
    printf("  - Green LED toggles on each sample\n");
    printf("  - Red LED turns on when voltage > threshold\n");
    printf("  - Statistics printed every %lu samples\n", PRINT_INTERVAL);
    printf("===========================================\n\n");
    
    /* Initialize peripherals */
    InitClocks();
    InitLEDs();
    InitADC();
    InitLPIT();
    
    printf("System running...\n\n");
    
    /* Main loop */
    while (1) {
        /* Check if new sample is available */
        if (g_newSampleAvailable) {
            g_newSampleAvailable = false;
            printCounter++;
            
            /* Print statistics periodically */
            if (printCounter >= PRINT_INTERVAL) {
                printCounter = 0;
                PrintStatistics();
            }
        }
        
        /* Optional: Enter low power mode */
        __asm("WFI");  /* Wait for interrupt */
    }
    
    return 0;
}

/*******************************************************************************
 * Example Functions
 ******************************************************************************/

/**
 * @brief Example 1: Change sampling rate dynamically
 */
void Example_ChangeSamplingRate(uint32_t newRateHz)
{
    uint32_t newPeriod;
    
    printf("Changing sampling rate to %lu Hz...\n", newRateHz);
    
    /* Stop LPIT */
    LPIT_StopChannel(LPIT_CHANNEL);
    
    /* Calculate new period */
    newPeriod = LPIT_CLOCK_FREQ / newRateHz;
    
    /* Reconfigure LPIT */
    lpit_channel_config_t lpitConfig = {
        .channel = LPIT_CHANNEL,
        .mode = LPIT_MODE_32BIT_PERIODIC,
        .period = newPeriod,
        .enableInterrupt = false,
        .chainChannel = false,
        .startOnTrigger = false,
        .stopOnInterrupt = false,
        .reloadOnTrigger = false
    };
    
    LPIT_ConfigChannel(&lpitConfig);
    LPIT_StartChannel(LPIT_CHANNEL);
    
    printf("New sampling rate: %lu Hz (period: %lu ticks)\n", newRateHz, newPeriod);
}

/**
 * @brief Example 2: Start/Stop ADC sampling
 */
void Example_ControlSampling(bool enable)
{
    if (enable) {
        printf("Starting ADC sampling...\n");
        ADC_StartHardwareTrigger(ADC_INSTANCE, ADC_CHANNEL);
        LPIT_StartChannel(LPIT_CHANNEL);
    } else {
        printf("Stopping ADC sampling...\n");
        LPIT_StopChannel(LPIT_CHANNEL);
        ADC_StopHardwareTrigger(ADC_INSTANCE);
    }
}

/**
 * @brief Example 3: Get average voltage over N samples
 */
uint32_t Example_GetAverageVoltage(uint32_t numSamples)
{
    uint32_t sum = 0;
    uint32_t startCount = g_adcSampleCount;
    uint32_t timeout = numSamples * 2;  /* Timeout protection */
    
    printf("Collecting %lu samples for average...\n", numSamples);
    
    /* Wait for required number of samples */
    while ((g_adcSampleCount - startCount) < numSamples && timeout > 0) {
        if (g_newSampleAvailable) {
            g_newSampleAvailable = false;
            sum += g_latestVoltage;
            timeout--;
        }
    }
    
    uint32_t average = sum / numSamples;
    printf("Average voltage: %lu mV\n", average);
    
    return average;
}
