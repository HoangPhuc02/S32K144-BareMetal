/**
 * @file    gpio_srv_example.c
 * @brief   GPIO Service Example with Button Interrupt
 * @details Demonstrate cách sử dụng GPIO service với external interrupt cho buttons
 * 
 * Hardware Setup:
 * - Button trên PTC12 (SW2 on EVB) with pull-up
 * - Button trên PTC13 (SW3 on EVB) with pull-up  
 * - LED Blue trên PTD0
 * - LED Red trên PTD15
 * - LED Green trên PTD16
 * 
 * Expected Behavior:
 * - Press SW2 (PTC12): Toggle Blue LED
 * - Press SW3 (PTC13): Toggle Red LED
 * - Green LED blinks continuously (no interrupt)
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 */

#include "../inc/gpio_srv.h"
#include "../inc/port_srv.h"
#include "../inc/clock_srv_v2.h"
#include "../inc/uart_srv.h"

/* Button pins */
#define BTN_SW2_PORT    2   /* PORTC */
#define BTN_SW2_PIN     12
#define BTN_SW3_PORT    2   /* PORTC */
#define BTN_SW3_PIN     13

/* LED pins */
#define LED_BLUE_PORT   3   /* PORTD */
#define LED_BLUE_PIN    0
#define LED_RED_PORT    3   /* PORTD */
#define LED_RED_PIN     15
#define LED_GREEN_PORT  3   /* PORTD */
#define LED_GREEN_PIN   16

/* Callback counters for debugging */
static volatile uint32_t sw2_press_count = 0;
static volatile uint32_t sw3_press_count = 0;

/**
 * @brief Button SW2 interrupt callback
 */
void Button_SW2_Callback(uint8_t port, uint8_t pin)
{
    (void)port; /* Unused */
    (void)pin;  /* Unused */
    
    /* Toggle Blue LED */
    GPIO_SRV_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
    
    /* Increment counter */
    sw2_press_count++;
    
    /* Optional: Send UART message */
    UART_SRV_SendString("SW2 Pressed!\r\n");
}

/**
 * @brief Button SW3 interrupt callback
 */
void Button_SW3_Callback(uint8_t port, uint8_t pin)
{
    (void)port;
    (void)pin;
    
    /* Toggle Red LED */
    GPIO_SRV_TogglePin(LED_RED_PORT, LED_RED_PIN);
    
    /* Increment counter */
    sw3_press_count++;
    
    UART_SRV_SendString("SW3 Pressed!\r\n");
}

/**
 * @brief Basic GPIO example - Simple LED control
 */
void GPIO_BasicExample(void)
{
    /* 1. Initialize clock */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    /* 2. Enable PORT clocks */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTD, CLOCK_SRV_V2_PCS_NONE);
    
    /* 3. Configure LED pins */
    port_srv_pin_config_t led_cfg = {
        .port = LED_BLUE_PORT,
        .pin = LED_BLUE_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&led_cfg);
    
    /* 4. Initialize GPIO */
    GPIO_SRV_Init();
    GPIO_SRV_ConfigOutput(LED_BLUE_PORT, LED_BLUE_PIN);
    
    /* 5. Blink LED */
    while(1) {
        GPIO_SRV_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, 1);
        for(volatile uint32_t i = 0; i < 1000000; i++);
        
        GPIO_SRV_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, 0);
        for(volatile uint32_t i = 0; i < 1000000; i++);
    }
}

/**
 * @brief Button interrupt example - Main demo
 */
void GPIO_ButtonInterruptExample(void)
{
    /* 1. Initialize clock system */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    
    /* 2. Enable peripheral clocks */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTD, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    
    /* 3. Initialize UART for debug */
    UART_SRV_Init(115200);
    UART_SRV_SendString("\r\n=== GPIO Button Interrupt Example ===\r\n");
    UART_SRV_SendString("Press SW2 (PTC12) to toggle Blue LED\r\n");
    UART_SRV_SendString("Press SW3 (PTC13) to toggle Red LED\r\n\r\n");
    
    /* 4. Configure button pins with pull-up */
    port_srv_pin_config_t btn_cfg = {
        .port = BTN_SW2_PORT,
        .pin = BTN_SW2_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_UP,
        .interrupt_config = PORT_INT_DISABLE  /* Will configure via GPIO_SRV */
    };
    PORT_SRV_ConfigPin(&btn_cfg);
    
    btn_cfg.pin = BTN_SW3_PIN;
    PORT_SRV_ConfigPin(&btn_cfg);
    
    /* 5. Configure LED pins */
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
    
    led_cfg.pin = LED_GREEN_PIN;
    PORT_SRV_ConfigPin(&led_cfg);
    
    /* 6. Initialize GPIO */
    GPIO_SRV_Init();
    
    /* Configure buttons as inputs */
    GPIO_SRV_ConfigInput(BTN_SW2_PORT, BTN_SW2_PIN);
    GPIO_SRV_ConfigInput(BTN_SW3_PORT, BTN_SW3_PIN);
    
    /* Configure LEDs as outputs */
    GPIO_SRV_ConfigOutput(LED_BLUE_PORT, LED_BLUE_PIN);
    GPIO_SRV_ConfigOutput(LED_RED_PORT, LED_RED_PIN);
    GPIO_SRV_ConfigOutput(LED_GREEN_PORT, LED_GREEN_PIN);
    
    /* Turn off all LEDs initially */
    GPIO_SRV_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, 0);
    GPIO_SRV_WritePin(LED_RED_PORT, LED_RED_PIN, 0);
    GPIO_SRV_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, 0);
    
    /* 7. Enable interrupts on button pins (falling edge - button press) */
    GPIO_SRV_EnableInterrupt(BTN_SW2_PORT, BTN_SW2_PIN, 
                             GPIO_SRV_INT_FALLING_EDGE,
                             Button_SW2_Callback);
    
    GPIO_SRV_EnableInterrupt(BTN_SW3_PORT, BTN_SW3_PIN,
                             GPIO_SRV_INT_FALLING_EDGE,
                             Button_SW3_Callback);
    
    UART_SRV_SendString("System ready!\r\n\r\n");
    
    /* 8. Main loop - Blink green LED */
    while(1) {
        /* Green LED blinks continuously */
        GPIO_SRV_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
        
        /* Delay 500ms */
        for(volatile uint32_t i = 0; i < 1000000; i++);
        
        /* Print button press counters every 10 blinks */
        static uint8_t blink_count = 0;
        if (++blink_count >= 10) {
            blink_count = 0;
            
            char buffer[64];
            sprintf(buffer, "SW2: %lu | SW3: %lu\r\n", 
                    sw2_press_count, sw3_press_count);
            UART_SRV_SendString(buffer);
        }
    }
}

/**
 * @brief Polled button example (without interrupt)
 */
void GPIO_ButtonPolledExample(void)
{
    /* Initialize system */
    CLOCK_SRV_V2_InitPreset("RUN_48MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTD, CLOCK_SRV_V2_PCS_NONE);
    
    /* Configure pins */
    port_srv_pin_config_t btn_cfg = {
        .port = BTN_SW2_PORT,
        .pin = BTN_SW2_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_UP,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&btn_cfg);
    
    port_srv_pin_config_t led_cfg = {
        .port = LED_BLUE_PORT,
        .pin = LED_BLUE_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&led_cfg);
    
    GPIO_SRV_Init();
    GPIO_SRV_ConfigInput(BTN_SW2_PORT, BTN_SW2_PIN);
    GPIO_SRV_ConfigOutput(LED_BLUE_PORT, LED_BLUE_PIN);
    
    /* Poll button state */
    uint8_t last_state = 1;
    
    while(1) {
        uint8_t current_state = GPIO_SRV_ReadPin(BTN_SW2_PORT, BTN_SW2_PIN);
        
        /* Detect button press (falling edge) */
        if (last_state == 1 && current_state == 0) {
            GPIO_SRV_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
            
            /* Debounce delay */
            for(volatile uint32_t i = 0; i < 100000; i++);
        }
        
        last_state = current_state;
    }
}

/**
 * @brief Both edges interrupt example
 */
void GPIO_BothEdgesExample(void)
{
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTC, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPUART1, CLOCK_SRV_V2_PCS_SOSCDIV2);
    
    UART_SRV_Init(115200);
    UART_SRV_SendString("\r\n=== Both Edges Interrupt Example ===\r\n");
    
    port_srv_pin_config_t btn_cfg = {
        .port = BTN_SW2_PORT,
        .pin = BTN_SW2_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_UP,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&btn_cfg);
    
    GPIO_SRV_Init();
    GPIO_SRV_ConfigInput(BTN_SW2_PORT, BTN_SW2_PIN);
    
    /* Enable interrupt on both edges */
    GPIO_SRV_EnableInterrupt(BTN_SW2_PORT, BTN_SW2_PIN,
                             GPIO_SRV_INT_BOTH_EDGES,
                             NULL);  /* No callback, will poll in main loop */
    
    while(1) {
        /* Check interrupt flag */
        if (GPIO_SRV_IsInterruptPending(BTN_SW2_PORT, BTN_SW2_PIN)) {
            /* Read current button state */
            uint8_t state = GPIO_SRV_ReadPin(BTN_SW2_PORT, BTN_SW2_PIN);
            
            if (state == 0) {
                UART_SRV_SendString("Button PRESSED (falling edge)\r\n");
            } else {
                UART_SRV_SendString("Button RELEASED (rising edge)\r\n");
            }
            
            /* Clear interrupt flag */
            GPIO_SRV_ClearInterrupt(BTN_SW2_PORT, BTN_SW2_PIN);
        }
        
        /* Small delay */
        for(volatile uint32_t i = 0; i < 10000; i++);
    }
}

/*******************************************************************************
 * Interrupt Handlers - Add these to startup.c or main.c
 ******************************************************************************/

/**
 * @brief PORTC interrupt handler
 * @note Add this to your startup vector table
 */
void PORTC_IRQHandler(void)
{
    GPIO_SRV_PORTC_IRQHandler();
}

/**
 * @brief PORTD interrupt handler
 * @note Add this to your startup vector table if using PORTD interrupts
 */
void PORTD_IRQHandler(void)
{
    GPIO_SRV_PORTD_IRQHandler();
}
