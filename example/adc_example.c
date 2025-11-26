/**
 * @file    adc_example.c
 * @brief   ADC Driver Usage Examples for S32K144
 * @details This file demonstrates various usage scenarios of the ADC driver
 *          including single conversion, continuous mode, averaging, and more.
 * 
 * @author  PhucPH32
 * @date    26/11/2025
 * @version 1.0
 * 
 * @note These examples show how to use the ADC driver API
 * @warning Configure PORT and PCC before using ADC
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 26, 2025): Initial version
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "adc.h"
#include "port.h"
#include "gpio.h"
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Reference voltage in millivolts (typically 3.3V) */
#define VREF_MV         (3300U)

/** @brief Number of samples for software averaging */
#define NUM_SAMPLES     (10U)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/* Variable to store conversion result in callback */
static volatile uint16_t g_adcResult = 0U;
static volatile bool g_adcConversionDone = false;

/*******************************************************************************
 * Example 1: Basic Single Channel Conversion
 ******************************************************************************/

/**
 * @brief Example 1: Basic single channel ADC conversion
 * 
 * This example shows:
 * - Basic ADC initialization
 * - Single channel conversion
 * - Reading ADC result
 * - Converting to voltage
 */
void ADC_Example1_BasicSingleConversion(void)
{
    ADC_Status_t status;
    uint16_t adcValue;
    uint32_t voltage;
    
    /* Step 1: Configure ADC */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,              /* Use alternate clock 1 */
        .resolution = ADC_RESOLUTION_12BIT,       /* 12-bit resolution */
        .clockDivider = ADC_CLK_DIV_4,            /* Divide clock by 4 */
        .voltageRef = ADC_VREF_VREFH_VREFL,       /* Use VREFH/VREFL */
        .triggerSource = ADC_TRIGGER_SOFTWARE,    /* Software trigger */
        .continuousMode = false,                  /* Single conversion */
        .dmaEnable = false,                       /* No DMA */
        .interruptEnable = false                  /* No interrupt */
    };
    
    /* Step 2: Enable clock and initialize ADC */
    ADC_EnableClock(ADC_INSTANCE_0);
    status = ADC_Init(ADC_INSTANCE_0, &adcConfig);
    
    if (status != ADC_STATUS_SUCCESS) {
        /* Handle error */
        return;
    }
    
    /* Step 3: Calibrate ADC (recommended after initialization) */
    status = ADC_Calibrate(ADC_INSTANCE_0);
    
    if (status != ADC_STATUS_SUCCESS) {
        /* Handle calibration error */
        return;
    }
    
    /* Step 4: Perform conversion on channel 0 (PTB0/ADC0_SE4) */
    status = ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
    
    if (status == ADC_STATUS_SUCCESS) {
        /* Step 5: Convert to voltage */
        voltage = ADC_ConvertToVoltage(adcValue, ADC_RESOLUTION_12BIT, VREF_MV);
        
        /* Use the values */
        printf("ADC Value: %d, Voltage: %d mV\n", adcValue, voltage);
    }
}

/*******************************************************************************
 * Example 2: Continuous Conversion Mode
 ******************************************************************************/

/**
 * @brief Example 2: Continuous conversion mode
 * 
 * This example shows:
 * - Configuring ADC for continuous conversion
 * - Starting continuous conversion
 * - Reading multiple results
 * - Stopping continuous conversion
 */
void ADC_Example2_ContinuousMode(void)
{
    ADC_Status_t status;
    uint16_t adcValue;
    uint32_t voltage;
    uint32_t i;
    
    /* Configure ADC for continuous mode */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = true,                   /* Enable continuous mode */
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    /* Initialize ADC */
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    /* Start continuous conversion on channel 4 */
    ADC_StartConversion(ADC_INSTANCE_0, ADC_CHANNEL_AD4);
    
    /* Read 10 consecutive samples */
    for (i = 0U; i < 10U; i++) {
        /* Wait for conversion to complete */
        status = ADC_WaitForConversion(ADC_INSTANCE_0, 100000U);
        
        if (status == ADC_STATUS_SUCCESS) {
            /* Get result */
            adcValue = ADC_GetResult(ADC_INSTANCE_0);
            voltage = ADC_ConvertToVoltage(adcValue, ADC_RESOLUTION_12BIT, VREF_MV);
            
            printf("Sample %d: ADC=%d, Voltage=%d mV\n", i, adcValue, voltage);
        }
    }
    
    /* Stop continuous conversion */
    ADC_StopConversion(ADC_INSTANCE_0);
}

/*******************************************************************************
 * Example 3: Hardware Averaging
 ******************************************************************************/

/**
 * @brief Example 3: Hardware averaging for noise reduction
 * 
 * This example shows:
 * - Configuring hardware averaging
 * - Comparing results with and without averaging
 */
void ADC_Example3_HardwareAveraging(void)
{
    ADC_Status_t status;
    uint16_t adcValue;
    
    /* Configure ADC */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    /* Read without averaging */
    printf("Without averaging:\n");
    for (uint32_t i = 0U; i < 5U; i++) {
        ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
        printf("  Sample %d: %d\n", i, adcValue);
    }
    
    /* Configure hardware averaging (16 samples) */
    ADC_AverageConfig_t avgConfig = {
        .enable = true,
        .averageMode = ADC_AVERAGE_16
    };
    ADC_ConfigureAveraging(ADC_INSTANCE_0, &avgConfig);
    
    /* Read with averaging */
    printf("\nWith 16-sample averaging:\n");
    for (uint32_t i = 0U; i < 5U; i++) {
        ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
        printf("  Sample %d: %d\n", i, adcValue);
    }
}

/*******************************************************************************
 * Example 4: Multiple Channel Reading
 ******************************************************************************/

/**
 * @brief Example 4: Reading multiple ADC channels sequentially
 * 
 * This example shows:
 * - Reading multiple channels in sequence
 * - Storing results in an array
 */
void ADC_Example4_MultipleChannels(void)
{
    ADC_Status_t status;
    uint16_t adcValues[4];
    uint32_t voltages[4];
    ADC_Channel_t channels[] = {
        ADC_CHANNEL_AD4,   /* Channel 4 */
        ADC_CHANNEL_AD5,   /* Channel 5 */
        ADC_CHANNEL_AD6,   /* Channel 6 */
        ADC_CHANNEL_AD7    /* Channel 7 */
    };
    
    /* Configure ADC */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    /* Read all channels */
    for (uint32_t i = 0U; i < 4U; i++) {
        status = ADC_ReadBlocking(ADC_INSTANCE_0, channels[i], &adcValues[i]);
        
        if (status == ADC_STATUS_SUCCESS) {
            voltages[i] = ADC_ConvertToVoltage(adcValues[i], ADC_RESOLUTION_12BIT, VREF_MV);
            printf("Channel %d: ADC=%d, Voltage=%d mV\n", 
                   channels[i], adcValues[i], voltages[i]);
        }
    }
}

/*******************************************************************************
 * Example 5: Interrupt-Driven Conversion
 ******************************************************************************/

/**
 * @brief ADC conversion complete callback
 */
void ADC_ConversionCallback(ADC_Instance_t instance, uint16_t result)
{
    g_adcResult = result;
    g_adcConversionDone = true;
}

/**
 * @brief Example 5: Interrupt-driven ADC conversion
 * 
 * This example shows:
 * - Configuring ADC with interrupts
 * - Registering callback function
 * - Non-blocking conversion
 */
void ADC_Example5_InterruptMode(void)
{
    uint32_t voltage;
    
    /* Configure ADC with interrupt enabled */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = true                   /* Enable interrupt */
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    /* Register callback function */
    ADC_RegisterCallback(ADC_INSTANCE_0, ADC_ConversionCallback);
    
    /* Enable ADC interrupt in NVIC (implementation dependent) */
    // NVIC_EnableIRQ(ADC0_IRQn);
    
    /* Start conversion */
    g_adcConversionDone = false;
    ADC_StartConversion(ADC_INSTANCE_0, ADC_CHANNEL_AD4);
    
    /* Wait for callback to set flag */
    while (!g_adcConversionDone) {
        /* Do other work here */
    }
    
    /* Process result */
    voltage = ADC_ConvertToVoltage(g_adcResult, ADC_RESOLUTION_12BIT, VREF_MV);
    printf("Conversion complete! ADC=%d, Voltage=%d mV\n", g_adcResult, voltage);
}

/*******************************************************************************
 * Example 6: Software Averaging
 ******************************************************************************/

/**
 * @brief Example 6: Software averaging for improved accuracy
 * 
 * This example shows:
 * - Taking multiple samples
 * - Computing average in software
 * - Comparing with single reading
 */
void ADC_Example6_SoftwareAveraging(void)
{
    ADC_Status_t status;
    uint16_t adcValue;
    uint32_t sum = 0U;
    uint16_t average;
    uint32_t voltage;
    
    /* Configure ADC */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    /* Take single reading */
    ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
    voltage = ADC_ConvertToVoltage(adcValue, ADC_RESOLUTION_12BIT, VREF_MV);
    printf("Single reading: ADC=%d, Voltage=%d mV\n", adcValue, voltage);
    
    /* Take multiple samples and average */
    sum = 0U;
    for (uint32_t i = 0U; i < NUM_SAMPLES; i++) {
        status = ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
        if (status == ADC_STATUS_SUCCESS) {
            sum += adcValue;
        }
    }
    
    average = (uint16_t)(sum / NUM_SAMPLES);
    voltage = ADC_ConvertToVoltage(average, ADC_RESOLUTION_12BIT, VREF_MV);
    printf("Averaged reading (%d samples): ADC=%d, Voltage=%d mV\n", 
           NUM_SAMPLES, average, voltage);
}

/*******************************************************************************
 * Example 7: Reading Temperature Sensor (Internal)
 ******************************************************************************/

/**
 * @brief Example 7: Reading internal temperature sensor
 * 
 * This example shows:
 * - Reading internal temperature sensor channel
 * - Converting to temperature (requires calibration data)
 * 
 * @note The exact channel and conversion formula may vary
 */
void ADC_Example7_TemperatureSensor(void)
{
    ADC_Status_t status;
    uint16_t adcValue;
    
    /* Configure ADC */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    /* Configure hardware averaging for better accuracy */
    ADC_AverageConfig_t avgConfig = {
        .enable = true,
        .averageMode = ADC_AVERAGE_32
    };
    ADC_ConfigureAveraging(ADC_INSTANCE_0, &avgConfig);
    
    /* Read temperature sensor (typically channel 26 or 27) */
    /* Note: Check your device datasheet for the correct channel */
    status = ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD14, &adcValue);
    
    if (status == ADC_STATUS_SUCCESS) {
        /* Temperature conversion requires calibration parameters
         * This is a simplified example
         */
        printf("Temperature sensor ADC value: %d\n", adcValue);
        
        /* Actual temperature calculation would use:
         * Temperature = 25 + (Vtemp - V_TEMP25) / Slope
         * Where V_TEMP25 and Slope are from factory calibration
         */
    }
}

/*******************************************************************************
 * Example 8: Potentiometer Reading with Display
 ******************************************************************************/

/**
 * @brief Example 8: Reading potentiometer and displaying percentage
 * 
 * This example shows:
 * - Practical application: reading potentiometer
 * - Converting to percentage (0-100%)
 * - Continuous monitoring
 */
void ADC_Example8_Potentiometer(void)
{
    ADC_Status_t status;
    uint16_t adcValue;
    uint32_t voltage;
    uint32_t percentage;
    const uint32_t maxADC = 4095U;  /* 12-bit max value */
    
    /* Configure ADC */
    ADC_Config_t adcConfig = {
        .clockSource = ADC_CLK_ALT1,
        .resolution = ADC_RESOLUTION_12BIT,
        .clockDivider = ADC_CLK_DIV_4,
        .voltageRef = ADC_VREF_VREFH_VREFL,
        .triggerSource = ADC_TRIGGER_SOFTWARE,
        .continuousMode = false,
        .dmaEnable = false,
        .interruptEnable = false
    };
    
    ADC_EnableClock(ADC_INSTANCE_0);
    ADC_Init(ADC_INSTANCE_0, &adcConfig);
    ADC_Calibrate(ADC_INSTANCE_0);
    
    /* Enable hardware averaging for stable readings */
    ADC_AverageConfig_t avgConfig = {
        .enable = true,
        .averageMode = ADC_AVERAGE_16
    };
    ADC_ConfigureAveraging(ADC_INSTANCE_0, &avgConfig);
    
    /* Read potentiometer continuously */
    printf("Reading potentiometer (Ctrl+C to stop):\n");
    
    while (1) {
        status = ADC_ReadBlocking(ADC_INSTANCE_0, ADC_CHANNEL_AD4, &adcValue);
        
        if (status == ADC_STATUS_SUCCESS) {
            /* Convert to voltage */
            voltage = ADC_ConvertToVoltage(adcValue, ADC_RESOLUTION_12BIT, VREF_MV);
            
            /* Convert to percentage */
            percentage = (adcValue * 100U) / maxADC;
            
            printf("ADC: %4d | Voltage: %4d mV | Position: %3d%%\n", 
                   adcValue, voltage, percentage);
        }
        
        /* Add delay between readings (implementation dependent) */
        // SYSTICK_DelayMs(100);
    }
}

/*******************************************************************************
 * Main Example Function
 ******************************************************************************/

/**
 * @brief Main function to run all ADC examples
 */
void ADC_RunExamples(void)
{
    printf("\n=== ADC Driver Examples ===\n\n");
    
    printf("Example 1: Basic Single Conversion\n");
    ADC_Example1_BasicSingleConversion();
    printf("\n");
    
    printf("Example 2: Continuous Mode\n");
    ADC_Example2_ContinuousMode();
    printf("\n");
    
    printf("Example 3: Hardware Averaging\n");
    ADC_Example3_HardwareAveraging();
    printf("\n");
    
    printf("Example 4: Multiple Channels\n");
    ADC_Example4_MultipleChannels();
    printf("\n");
    
    printf("Example 5: Interrupt Mode\n");
    ADC_Example5_InterruptMode();
    printf("\n");
    
    printf("Example 6: Software Averaging\n");
    ADC_Example6_SoftwareAveraging();
    printf("\n");
    
    printf("Example 7: Temperature Sensor\n");
    ADC_Example7_TemperatureSensor();
    printf("\n");
    
    printf("Example 8: Potentiometer Reading\n");
    // ADC_Example8_Potentiometer(); /* Runs infinitely */
    printf("\n");
}
