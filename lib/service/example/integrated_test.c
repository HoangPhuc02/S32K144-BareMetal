/**
 * @file    integrated_test.c
 * @brief   Integrated Service Test - Complete System Demo
 * @details
 * Test case phức tạp kết hợp tất cả service layers:
 * 
 * FLOW:
 * 1. Nhấn nút SW2 (PTC12) → Gửi CAN frame START (ID 0x100) - Loopback mode
 * 2. CAN RX interrupt nhận frame START → Set flag bắt đầu ADC sampling
 * 3. LPIT timer 1s → Đọc ADC → Gửi CAN frame DATA (ID 0x200) với giá trị ADC
 * 4. CAN RX nhận frame DATA → UART truyền thông báo giá trị ADC
 * 
 * Hardware Setup:
 * - Button SW2: PTC12 (with pull-up)
 * - ADC input: PTB3 (ADC0_SE12) - Potentiometer 0-5V
 * - CAN: Loopback mode (no external transceiver needed)
 * - UART: PTC6/PTC7 - 115200 baud
 * - LED Blue: PTD0 - Status indicator
 * - LED Red: PTD15 - CAN activity indicator
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/gpio_srv.h"
#include "../inc/port_srv.h"
#include "../inc/can_srv.h"
#include "../inc/adc_srv.h"
#include "../inc/lpit_srv.h"
#include "../inc/uart_srv.h"
#include "../inc/clock_srv_v2.h"
#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Hardware pins */
#define BTN_START_PORT      2       /* PORTC */
#define BTN_START_PIN       12      /* SW2 */

#define ADC_CHANNEL         12      /* ADC0_SE12 */
#define ADC_PORT            PORTB
#define ADC_PIN             3

#define LED_BLUE_PORT       3       /* PORTD */
#define LED_BLUE_PIN        0
#define LED_RED_PORT        3       /* PORTD */
#define LED_RED_PIN         15

/* CAN Message IDs */
#define CAN_ID_START        0x100   /* Start command */
#define CAN_ID_ADC_DATA     0x200   /* ADC data */

/* Timing */
#define ADC_SAMPLE_PERIOD_US    1000000U    /* 1 second */

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
/* State machine flags */
static volatile bool g_system_started = false;
static volatile bool g_adc_ready = false;
static volatile uint16_t g_adc_value = 0;
static volatile uint16_t g_adc_voltage = 0;

/* Statistics */
static volatile uint32_t g_button_press_count = 0;
static volatile uint32_t g_can_tx_count = 0;
static volatile uint32_t g_can_rx_count = 0;
static volatile uint32_t g_adc_sample_count = 0;

/*******************************************************************************
 * Callback Functions
 ******************************************************************************/

/**
 * @brief Button interrupt callback - START sequence
 */
void Button_Start_Callback(uint8_t port, uint8_t pin)
{
    (void)port;
    (void)pin;
    
    can_srv_message_t start_msg;
    
    /* Increment counter */
    g_button_press_count++;
    
    /* Visual feedback - Toggle Blue LED */
    GPIO_SRV_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
    
    /* Send START command via CAN */
    start_msg.id = CAN_ID_START;
    start_msg.dlc = 8;
    start_msg.is_extended = false;
    start_msg.data[0] = 0xAA;   /* Start marker */
    start_msg.data[1] = 0x55;
    start_msg.data[2] = (g_button_press_count >> 24) & 0xFF;
    start_msg.data[3] = (g_button_press_count >> 16) & 0xFF;
    start_msg.data[4] = (g_button_press_count >> 8) & 0xFF;
    start_msg.data[5] = g_button_press_count & 0xFF;
    start_msg.data[6] = 0x00;
    start_msg.data[7] = 0x00;
    
    if (CAN_SRV_Send(&start_msg) == CAN_SRV_SUCCESS) {
        g_can_tx_count++;
        UART_SRV_SendString("[BTN] START command sent via CAN\r\n");
    } else {
        UART_SRV_SendString("[BTN] ERROR: CAN TX failed!\r\n");
    }
}

/**
 * @brief LPIT timer callback - ADC sampling every 1 second
 */
void ADC_Sample_Callback(void)
{
    adc_srv_config_t adc_cfg;
    can_srv_message_t data_msg;
    
    /* Only sample if system is started */
    if (!g_system_started) {
        return;
    }
    
    /* Read ADC value */
    adc_cfg.channel = ADC_CHANNEL;
    
    if (ADC_SRV_Read(&adc_cfg) == ADC_SRV_SUCCESS) {
        g_adc_value = adc_cfg.raw_value;
        g_adc_voltage = adc_cfg.voltage_mv;
        g_adc_sample_count++;
        g_adc_ready = true;
        
        /* Visual feedback - Blink Red LED */
        GPIO_SRV_WritePin(LED_RED_PORT, LED_RED_PIN, 1);
        
        /* Send ADC data via CAN */
        data_msg.id = CAN_ID_ADC_DATA;
        data_msg.dlc = 8;
        data_msg.is_extended = false;
        data_msg.data[0] = 0xDD;    /* Data marker */
        data_msg.data[1] = 0xAA;
        data_msg.data[2] = (g_adc_value >> 8) & 0xFF;      /* ADC raw high */
        data_msg.data[3] = g_adc_value & 0xFF;             /* ADC raw low */
        data_msg.data[4] = (g_adc_voltage >> 8) & 0xFF;    /* Voltage high */
        data_msg.data[5] = g_adc_voltage & 0xFF;           /* Voltage low */
        data_msg.data[6] = (g_adc_sample_count >> 8) & 0xFF;
        data_msg.data[7] = g_adc_sample_count & 0xFF;
        
        if (CAN_SRV_Send(&data_msg) == CAN_SRV_SUCCESS) {
            g_can_tx_count++;
            
            char buffer[64];
            sprintf(buffer, "[ADC] Sample #%lu: %u mV (Raw: %u)\r\n", 
                    g_adc_sample_count, g_adc_voltage, g_adc_value);
            UART_SRV_SendString(buffer);
        }
        
        /* Turn off Red LED after short time */
        for(volatile uint32_t i = 0; i < 10000; i++);
        GPIO_SRV_WritePin(LED_RED_PORT, LED_RED_PIN, 0);
        
    } else {
        UART_SRV_SendString("[ADC] ERROR: Read failed!\r\n");
    }
}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Process received CAN messages
 */
static void ProcessCANMessage(can_srv_message_t *msg)
{
    char buffer[128];
    
    g_can_rx_count++;
    
    /* Toggle Red LED for CAN activity */
    GPIO_SRV_TogglePin(LED_RED_PORT, LED_RED_PIN);
    
    if (msg->id == CAN_ID_START) {
        /* START command received */
        sprintf(buffer, "\r\n[CAN-RX] START command received! (Count: %lu)\r\n",
                g_can_rx_count);
        UART_SRV_SendString(buffer);
        
        /* Set flag to start ADC sampling */
        g_system_started = true;
        
        UART_SRV_SendString("[SYSTEM] ADC sampling STARTED - Period: 1s\r\n");
        UART_SRV_SendString("==========================================\r\n\r\n");
        
        /* Turn on Blue LED to indicate system running */
        GPIO_SRV_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, 1);
    }
    else if (msg->id == CAN_ID_ADC_DATA) {
        /* ADC data received */
        uint16_t raw_value = (msg->data[2] << 8) | msg->data[3];
        uint16_t voltage = (msg->data[4] << 8) | msg->data[5];
        uint16_t sample_num = (msg->data[6] << 8) | msg->data[7];
        
        sprintf(buffer, "[CAN-RX] ADC Data #%u: %u mV (Raw: %u)\r\n",
                sample_num, voltage, raw_value);
        UART_SRV_SendString(buffer);
        
        sprintf(buffer, "[UART-TX] Transmitting ADC value: %u mV\r\n\r\n", voltage);
        UART_SRV_SendString(buffer);
    }
    else {
        /* Unknown message */
        sprintf(buffer, "[CAN-RX] Unknown ID: 0x%03X, DLC: %u\r\n",
                msg->id, msg->dlc);
        UART_SRV_SendString(buffer);
    }
}

/**
 * @brief Initialize hardware peripherals
 */
static void Hardware_Init(void)
{
    can_srv_config_t can_cfg;
    lpit_srv_config_t lpit_cfg;
    
    UART_SRV_SendString("\r\n");
    UART_SRV_SendString("========================================\r\n");
    UART_SRV_SendString("  Integrated Service Test - Starting   \r\n");
    UART_SRV_SendString("========================================\r\n\r\n");
    
    /* 1. Clock System */
    UART_SRV_SendString("[INIT] Configuring clock system...\r\n");
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    /* 2. Enable peripheral clocks */
    UART_SRV_SendString("[INIT] Enabling peripheral clocks...\r\n");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTB, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTD, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTE, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_ADC0, CLOCK_SRV_V2_PCS_FIRCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPIT, CLOCK_SRV_V2_PCS_FIRCDIV2);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_FLEXCAN0, CLOCK_SRV_V2_PCS_SOSCDIV2);
    
    /* 3. Configure button pin with pull-up */
    UART_SRV_SendString("[INIT] Configuring button...\r\n");
    port_srv_pin_config_t btn_cfg = {
        .port = BTN_START_PORT,
        .pin = BTN_START_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_UP,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&btn_cfg);
    
    /* 4. Configure ADC pin (analog mode) */
    UART_SRV_SendString("[INIT] Configuring ADC pin...\r\n");
    port_srv_pin_config_t adc_pin_cfg = {
        .port = ADC_PORT,
        .pin = ADC_PIN,
        .mux = PORT_MUX_ANALOG,
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&adc_pin_cfg);
    
    /* 5. Configure LED pins */
    UART_SRV_SendString("[INIT] Configuring LEDs...\r\n");
    port_srv_pin_config_t led_cfg = {
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    
    led_cfg.port = LED_BLUE_PORT;
    led_cfg.pin = LED_BLUE_PIN;
    PORT_SRV_ConfigPin(&led_cfg);
    
    led_cfg.pin = LED_RED_PIN;
    PORT_SRV_ConfigPin(&led_cfg);
    
    /* 6. Initialize GPIO */
    UART_SRV_SendString("[INIT] Initializing GPIO...\r\n");
    GPIO_SRV_Init();
    GPIO_SRV_ConfigInput(BTN_START_PORT, BTN_START_PIN);
    GPIO_SRV_ConfigOutput(LED_BLUE_PORT, LED_BLUE_PIN);
    GPIO_SRV_ConfigOutput(LED_RED_PORT, LED_RED_PIN);
    
    /* Turn off LEDs */
    GPIO_SRV_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, 0);
    GPIO_SRV_WritePin(LED_RED_PORT, LED_RED_PIN, 0);
    
    /* 7. Enable button interrupt */
    UART_SRV_SendString("[INIT] Enabling button interrupt...\r\n");
    GPIO_SRV_EnableInterrupt(BTN_START_PORT, BTN_START_PIN,
                             GPIO_SRV_INT_FALLING_EDGE,
                             Button_Start_Callback);
    
    /* 8. Initialize ADC */
    UART_SRV_SendString("[INIT] Initializing ADC...\r\n");
    if (ADC_SRV_Init() != ADC_SRV_SUCCESS) {
        UART_SRV_SendString("[ERROR] ADC initialization failed!\r\n");
        while(1);
    }
    
    UART_SRV_SendString("[INIT] Calibrating ADC...\r\n");
    if (ADC_SRV_Calibrate() != ADC_SRV_SUCCESS) {
        UART_SRV_SendString("[ERROR] ADC calibration failed!\r\n");
        while(1);
    }
    
    /* 9. Initialize LPIT timer */
    UART_SRV_SendString("[INIT] Initializing LPIT timer...\r\n");
    if (LPIT_SRV_Init() != LPIT_SRV_SUCCESS) {
        UART_SRV_SendString("[ERROR] LPIT initialization failed!\r\n");
        while(1);
    }
    
    /* Configure 1 second timer for ADC sampling */
    lpit_cfg.channel = 0;
    lpit_cfg.period_us = ADC_SAMPLE_PERIOD_US;
    lpit_cfg.callback = ADC_Sample_Callback;
    lpit_cfg.is_running = false;
    
    if (LPIT_SRV_StartTimer(&lpit_cfg) != LPIT_SRV_SUCCESS) {
        UART_SRV_SendString("[ERROR] LPIT timer start failed!\r\n");
        while(1);
    }
    
    /* 10. Initialize CAN (Loopback mode) */
    UART_SRV_SendString("[INIT] Initializing CAN (Loopback)...\r\n");
    can_cfg.baudrate = 500000;      /* 500 kbps */
    can_cfg.filter_id = 0x000;      /* Accept all IDs */
    can_cfg.filter_mask = 0x000;
    can_cfg.filter_extended = false;
    
    if (CAN_SRV_Init(&can_cfg) != CAN_SRV_SUCCESS) {
        UART_SRV_SendString("[ERROR] CAN initialization failed!\r\n");
        while(1);
    }
    
    UART_SRV_SendString("\r\n[INIT] Hardware initialization complete!\r\n\r\n");
    UART_SRV_SendString("========================================\r\n");
    UART_SRV_SendString("  System Ready - Press SW2 to Start    \r\n");
    UART_SRV_SendString("========================================\r\n\r\n");
}

/**
 * @brief Print system statistics
 */
static void Print_Statistics(void)
{
    char buffer[128];
    
    UART_SRV_SendString("\r\n--- System Statistics ---\r\n");
    
    sprintf(buffer, "Button Presses:  %lu\r\n", g_button_press_count);
    UART_SRV_SendString(buffer);
    
    sprintf(buffer, "CAN TX Messages: %lu\r\n", g_can_tx_count);
    UART_SRV_SendString(buffer);
    
    sprintf(buffer, "CAN RX Messages: %lu\r\n", g_can_rx_count);
    UART_SRV_SendString(buffer);
    
    sprintf(buffer, "ADC Samples:     %lu\r\n", g_adc_sample_count);
    UART_SRV_SendString(buffer);
    
    sprintf(buffer, "System State:    %s\r\n", 
            g_system_started ? "RUNNING" : "IDLE");
    UART_SRV_SendString(buffer);
    
    if (g_adc_ready) {
        sprintf(buffer, "Last ADC Value:  %u mV (Raw: %u)\r\n",
                g_adc_voltage, g_adc_value);
        UART_SRV_SendString(buffer);
    }
    
    UART_SRV_SendString("-------------------------\r\n\r\n");
}

/*******************************************************************************
 * Main Test Function
 ******************************************************************************/

/**
 * @brief Main integrated test function
 */
void Integrated_Test_Main(void)
{
    can_srv_message_t rx_msg;
    uint32_t loop_count = 0;
    
    /* Initialize UART first for debug output */
    CLOCK_SRV_V2_InitPreset("RUN_48MHz");  /* Temporary for UART */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_FIRCDIV2);
    UART_SRV_Init(115200);
    
    /* Initialize all hardware */
    Hardware_Init();
    
    /* Main loop */
    while(1) {
        /* Check for received CAN messages */
        if (CAN_SRV_Receive(&rx_msg) == CAN_SRV_SUCCESS) {
            ProcessCANMessage(&rx_msg);
        }
        
        /* Print statistics every 50 loops */
        loop_count++;
        if (loop_count >= 50000) {
            loop_count = 0;
            Print_Statistics();
        }
        
        /* Small delay to prevent flooding */
        for(volatile uint32_t i = 0; i < 1000; i++);
    }
}

/*******************************************************************************
 * Interrupt Handlers - Add to startup.c
 ******************************************************************************/

/**
 * @brief PORTC interrupt handler for button
 */
void PORTC_IRQHandler(void)
{
    GPIO_SRV_PORTC_IRQHandler();
}

/**
 * @brief LPIT interrupt handler for timer
 */
void LPIT0_Ch0_IRQHandler(void)
{
    /* LPIT service handles this internally */
}

/*******************************************************************************
 * Usage Instructions
 ******************************************************************************/

/*
 * TO USE THIS TEST:
 * 
 * 1. Add to main.c:
 *    #include "App/App2/service/example/integrated_test.c"
 * 
 * 2. In main():
 *    int main(void) {
 *        Integrated_Test_Main();
 *        while(1);
 *    }
 * 
 * 3. Enable interrupts in startup.c or before main():
 *    NVIC_EnableIRQ(PORTC_IRQn);
 *    NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
 * 
 * 4. Connect hardware:
 *    - Button SW2 on PTC12
 *    - Potentiometer on PTB3 (0-5V)
 *    - UART TX/RX on PTC6/PTC7
 *    - LEDs on PTD0, PTD15
 * 
 * 5. Open terminal (115200 baud) and observe output
 * 
 * 6. Press SW2 button to start test sequence
 * 
 * EXPECTED BEHAVIOR:
 * - Press button → CAN START message sent (loopback)
 * - CAN receives START → System begins ADC sampling every 1s
 * - Every 1s: ADC read → CAN DATA message sent
 * - CAN receives DATA → UART prints ADC value
 * - Blue LED = System running
 * - Red LED = CAN activity
 */
