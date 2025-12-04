/**
 * @file    adc_srv_example.c
 * @brief   ADC Service Example
 * @details Demonstrate cách sử dụng ADC service để đọc analog input
 * 
 * Hardware Setup:
 * - Potentiometer/Analog sensor nối vào ADC0_SE12 (PTB3)
 * - Voltage range: 0-5V (hoặc 0-3.3V tùy hardware)
 * 
 * Expected Behavior:
 * - Đọc giá trị ADC từ channel 12
 * - Convert sang voltage (mV)
 * - Print qua UART
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 */

#include "../inc/adc_srv.h"
#include "../inc/uart_srv.h"
#include "../inc/port_srv.h"
#include "../inc/clock_srv_v2.h"
#include <stdio.h>

/* ADC Configuration */
#define ADC_CHANNEL     12      /* ADC0_SE12 */
#define ADC_PORT        PORTB
#define ADC_PIN         3

/* Function to print ADC value via UART */
void PrintADCValue(uint16_t raw, uint16_t voltage_mv)
{
    char buffer[64];
    sprintf(buffer, "ADC Raw: %4u | Voltage: %4u mV\r\n", raw, voltage_mv);
    UART_SRV_SendString(buffer);
}

void ADC_BasicExample(void)
{
    adc_srv_status_t status;
    adc_srv_config_t adc_cfg;
    
    /* 1. Initialize clock system */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    /* 2. Enable peripheral clocks */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTB, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_ADC0, CLOCK_SRV_V2_PCS_FIRCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    
    /* 3. Configure ADC pin (analog mode - no pull) */
    port_srv_pin_config_t adc_pin_cfg = {
        .port = ADC_PORT,
        .pin = ADC_PIN,
        .mux = PORT_MUX_ANALOG,         /* Analog mode */
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&adc_pin_cfg);
    
    /* 4. Initialize UART for output */
    UART_SRV_Init(115200);
    UART_SRV_SendString("\r\n=== ADC Service Example ===\r\n");
    UART_SRV_SendString("Reading ADC Channel 12 (PTB3)\r\n\r\n");
    
    /* 5. Initialize ADC */
    status = ADC_SRV_Init();
    if (status != ADC_SRV_SUCCESS) {
        UART_SRV_SendString("ADC Init Failed!\r\n");
        while(1);
    }
    
    /* 6. Calibrate ADC */
    UART_SRV_SendString("Calibrating ADC...\r\n");
    status = ADC_SRV_Calibrate();
    if (status != ADC_SRV_SUCCESS) {
        UART_SRV_SendString("ADC Calibration Failed!\r\n");
        while(1);
    }
    UART_SRV_SendString("Calibration Complete!\r\n\r\n");
    
    /* 7. Configure ADC channel */
    adc_cfg.channel = ADC_CHANNEL;
    
    /* 8. Main loop - Read ADC continuously */
    while(1) {
        /* Read ADC value */
        status = ADC_SRV_Read(&adc_cfg);
        
        if (status == ADC_SRV_SUCCESS) {
            /* Print values */
            PrintADCValue(adc_cfg.raw_value, adc_cfg.voltage_mv);
        } else {
            UART_SRV_SendString("ADC Read Error!\r\n");
        }
        
        /* Delay 500ms */
        for(volatile uint32_t i = 0; i < 1000000; i++);
    }
}

/* Example: Read multiple channels */
void ADC_MultiChannelExample(void)
{
    adc_srv_config_t channels[3];
    uint8_t channel_list[] = {12, 13, 14};  /* ADC0_SE12, SE13, SE14 */
    
    /* Initialize system */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_ADC0, CLOCK_SRV_V2_PCS_FIRCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    
    UART_SRV_Init(115200);
    ADC_SRV_Init();
    ADC_SRV_Calibrate();
    
    UART_SRV_SendString("\r\n=== Multi-Channel ADC Example ===\r\n\r\n");
    
    while(1) {
        UART_SRV_SendString("--- Reading All Channels ---\r\n");
        
        /* Read all channels */
        for(uint8_t i = 0; i < 3; i++) {
            channels[i].channel = channel_list[i];
            
            if (ADC_SRV_Read(&channels[i]) == ADC_SRV_SUCCESS) {
                char buffer[64];
                sprintf(buffer, "CH%2u: %4u (Raw) | %4u mV\r\n", 
                        channels[i].channel,
                        channels[i].raw_value,
                        channels[i].voltage_mv);
                UART_SRV_SendString(buffer);
            }
        }
        
        UART_SRV_SendString("\r\n");
        
        /* Delay 1 second */
        for(volatile uint32_t i = 0; i < 2000000; i++);
    }
}

/* Example: Voltage threshold monitoring */
void ADC_ThresholdExample(void)
{
    adc_srv_config_t adc_cfg = {.channel = ADC_CHANNEL};
    uint16_t threshold_mv = 2500;  /* 2.5V threshold */
    
    /* Initialize system */
    CLOCK_SRV_V2_InitPreset("RUN_48MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_ADC0, CLOCK_SRV_V2_PCS_FIRCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_FIRCDIV2);
    
    UART_SRV_Init(115200);
    ADC_SRV_Init();
    ADC_SRV_Calibrate();
    
    UART_SRV_SendString("\r\n=== Threshold Monitoring Example ===\r\n");
    UART_SRV_SendString("Threshold: 2500 mV\r\n\r\n");
    
    while(1) {
        ADC_SRV_Read(&adc_cfg);
        
        if (adc_cfg.voltage_mv > threshold_mv) {
            UART_SRV_SendString("WARNING: Voltage ABOVE threshold!\r\n");
        } else {
            UART_SRV_SendString("OK: Voltage below threshold\r\n");
        }
        
        /* Delay */
        for(volatile uint32_t i = 0; i < 1000000; i++);
    }
}
