/**
 * @file    clock_srv_example.c
 * @brief   Clock Service Example (V1 and V2)
 * @details Demonstrate cách sử dụng Clock service để configure system clocks
 * 
 * Hardware Setup:
 * - External crystal 8MHz on XTAL/EXTAL pins
 * - LED on PTD15 for visual indication
 * 
 * Expected Behavior:
 * - Test different clock modes
 * - Display frequencies via UART
 * - LED blink rate changes with clock speed
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 */

#include "../inc/clock_srv.h"
#include "../inc/clock_srv_v2.h"
#include "../inc/uart_srv.h"
#include "../inc/gpio_srv.h"
#include "../inc/port_srv.h"
#include <stdio.h>

#define LED_PORT    PORTD
#define LED_PIN     15

/* Helper function to print frequencies */
void PrintClockInfo_V1(void)
{
    clock_srv_config_t config;
    char buffer[128];
    
    if (CLOCK_SRV_GetConfig(&config) == CLOCK_SRV_SUCCESS) {
        sprintf(buffer, "Core:  %lu Hz\r\n", config.core_clock_hz);
        UART_SRV_SendString(buffer);
        
        sprintf(buffer, "Bus:   %lu Hz\r\n", config.bus_clock_hz);
        UART_SRV_SendString(buffer);
        
        sprintf(buffer, "Flash: %lu Hz\r\n\r\n", config.flash_clock_hz);
        UART_SRV_SendString(buffer);
    }
}

void PrintClockInfo_V2(void)
{
    clock_srv_v2_frequencies_t freq;
    char buffer[128];
    
    if (CLOCK_SRV_V2_GetFrequencies(&freq) == CLOCK_SRV_V2_SUCCESS) {
        sprintf(buffer, "Core:  %lu Hz\r\n", freq.core_hz);
        UART_SRV_SendString(buffer);
        
        sprintf(buffer, "Bus:   %lu Hz\r\n", freq.bus_hz);
        UART_SRV_SendString(buffer);
        
        sprintf(buffer, "Slow:  %lu Hz\r\n", freq.slow_hz);
        UART_SRV_SendString(buffer);
        
        sprintf(buffer, "SOSC:  %lu Hz\r\n", freq.sosc_hz);
        UART_SRV_SendString(buffer);
        
        sprintf(buffer, "SPLL:  %lu Hz\r\n\r\n", freq.spll_hz);
        UART_SRV_SendString(buffer);
    }
}

/* Example 1: Test all V1 preset modes */
void CLOCK_V1_PresetExample(void)
{
    /* Start with 48MHz (no external crystal needed) */
    CLOCK_SRV_Init(CLOCK_SRV_MODE_RUN_48MHZ);
    
    /* Enable UART */
    CLOCK_SRV_EnablePeripheral(CLOCK_SRV_PCC_PORTC);
    CLOCK_SRV_EnablePeripheral(CLOCK_SRV_PCC_LPUART1);
    UART_SRV_Init(115200);
    
    UART_SRV_SendString("\r\n=== Clock Service V1 Example ===\r\n\r\n");
    
    /* Test Mode 1: RUN 80MHz */
    UART_SRV_SendString("Mode 1: RUN_80MHz\r\n");
    CLOCK_SRV_Init(CLOCK_SRV_MODE_RUN_80MHZ);
    PrintClockInfo_V1();
    
    for(volatile uint32_t i = 0; i < 5000000; i++);
    
    /* Test Mode 2: RUN 48MHz */
    UART_SRV_SendString("Mode 2: RUN_48MHz\r\n");
    CLOCK_SRV_Init(CLOCK_SRV_MODE_RUN_48MHZ);
    PrintClockInfo_V1();
    
    for(volatile uint32_t i = 0; i < 5000000; i++);
    
    /* Test Mode 3: HSRUN 112MHz */
    UART_SRV_SendString("Mode 3: HSRUN_112MHz\r\n");
    CLOCK_SRV_Init(CLOCK_SRV_MODE_HSRUN_112MHZ);
    PrintClockInfo_V1();
    
    for(volatile uint32_t i = 0; i < 5000000; i++);
    
    /* Test Mode 4: VLPR 4MHz (low power) */
    UART_SRV_SendString("Mode 4: VLPR_4MHz\r\n");
    CLOCK_SRV_Init(CLOCK_SRV_MODE_VLPR_4MHZ);
    PrintClockInfo_V1();
    
    UART_SRV_SendString("Test complete!\r\n");
    
    while(1);
}

/* Example 2: V2 with custom configuration */
void CLOCK_V2_CustomExample(void)
{
    /* Initialize with preset first */
    CLOCK_SRV_V2_InitPreset("RUN_48MHz");
    
    /* Enable peripherals */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_FIRCDIV2);
    
    UART_SRV_Init(115200);
    UART_SRV_SendString("\r\n=== Clock Service V2 Custom Example ===\r\n\r\n");
    
    /* Create custom 100MHz configuration */
    UART_SRV_SendString("Custom Config: 100MHz from SPLL\r\n");
    
    clock_srv_v2_config_t custom_cfg = {
        .sosc = {
            .enable = true,
            .freq_hz = 8000000,
            .range = CLOCK_SRV_V2_SOSC_RANGE_HIGH,
            .use_external_ref = false,
            .div1 = CLOCK_SRV_V2_DIV_1,
            .div2 = CLOCK_SRV_V2_DIV_2
        },
        .spll = {
            .enable = true,
            .prediv = CLOCK_SRV_V2_SPLL_PREDIV_1,  /* /1 */
            .mult = 25,                              /* x25 */
            .div1 = CLOCK_SRV_V2_DIV_2,             /* 8MHz * 25 / 2 = 100MHz */
            .div2 = CLOCK_SRV_V2_DIV_4
        },
        .sys = {
            .source = CLOCK_SRV_V2_SOURCE_SPLL,
            .core_div = CLOCK_SRV_V2_DIV_1,         /* Core = 100MHz */
            .bus_div = CLOCK_SRV_V2_DIV_2,          /* Bus = 50MHz */
            .slow_div = CLOCK_SRV_V2_DIV_4          /* Slow = 25MHz */
        }
    };
    
    /* Validate before applying */
    if (CLOCK_SRV_V2_ValidateConfig(&custom_cfg) != CLOCK_SRV_V2_SUCCESS) {
        UART_SRV_SendString("Invalid configuration!\r\n");
        while(1);
    }
    
    /* Apply custom configuration */
    CLOCK_SRV_V2_Init(&custom_cfg);
    
    UART_SRV_SendString("Configuration applied!\r\n");
    PrintClockInfo_V2();
    
    while(1);
}

/* Example 3: Dynamic clock switching */
void CLOCK_V2_DynamicSwitchExample(void)
{
    /* Initialize */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTD, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    
    UART_SRV_Init(115200);
    GPIO_SRV_Init();
    
    /* Configure LED */
    port_srv_pin_config_t led_cfg = {
        .port = LED_PORT,
        .pin = LED_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&led_cfg);
    GPIO_SRV_ConfigOutput(LED_PORT, LED_PIN);
    
    UART_SRV_SendString("\r\n=== Dynamic Clock Switching ===\r\n");
    UART_SRV_SendString("Watch LED blink rate change!\r\n\r\n");
    
    uint8_t mode = 0;
    
    while(1) {
        switch(mode) {
            case 0:
                UART_SRV_SendString("Switching to 80MHz...\r\n");
                CLOCK_SRV_V2_InitPreset("RUN_80MHz");
                break;
                
            case 1:
                UART_SRV_SendString("Switching to 48MHz...\r\n");
                CLOCK_SRV_V2_InitPreset("RUN_48MHz");
                break;
                
            case 2:
                UART_SRV_SendString("Switching to 112MHz...\r\n");
                CLOCK_SRV_V2_InitPreset("HSRUN_112MHz");
                break;
                
            case 3:
                UART_SRV_SendString("Switching to 4MHz...\r\n");
                CLOCK_SRV_V2_InitPreset("VLPR_4MHz");
                break;
        }
        
        PrintClockInfo_V2();
        
        /* Blink LED 10 times at this speed */
        for(uint8_t i = 0; i < 10; i++) {
            GPIO_SRV_TogglePin(LED_PORT, LED_PIN);
            for(volatile uint32_t j = 0; j < 500000; j++);
        }
        
        mode = (mode + 1) % 4;
    }
}

/* Example 4: Calculate SPLL frequency */
void CLOCK_V2_CalculatorExample(void)
{
    CLOCK_SRV_V2_InitPreset("RUN_48MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_FIRCDIV2);
    
    UART_SRV_Init(115200);
    
    UART_SRV_SendString("\r\n=== SPLL Frequency Calculator ===\r\n\r\n");
    
    uint32_t sosc = 8000000;  /* 8 MHz */
    char buffer[64];
    
    UART_SRV_SendString("SOSC = 8 MHz\r\n\r\n");
    
    /* Calculate various SPLL configurations */
    for(uint8_t prediv = 0; prediv < 4; prediv++) {
        for(uint8_t mult = 20; mult <= 40; mult += 5) {
            uint32_t spll_freq = CLOCK_SRV_V2_CalculateSPLLFreq(sosc, prediv, mult);
            
            sprintf(buffer, "PREDIV=%u, MULT=%u -> SPLL=%lu MHz\r\n",
                    prediv + 1, mult, spll_freq / 1000000);
            UART_SRV_SendString(buffer);
        }
        UART_SRV_SendString("\r\n");
    }
    
    while(1);
}
