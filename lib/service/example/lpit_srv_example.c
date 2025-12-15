/**
 * @file    lpit_srv_example.c
 * @brief   LPIT Service Example
 * @details Demonstrate cách sử dụng LPIT service với timer callbacks
 * 
 * Hardware Setup:
 * - LED trên PTD0 (Blue LED)
 * - LED trên PTD15 (Red LED)
 * 
 * Expected Behavior:
 * - Blue LED toggle mỗi 500ms (Channel 0)
 * - Red LED toggle mỗi 1000ms (Channel 1)
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 */

#include "../inc/lpit_srv.h"
#include "../inc/gpio_srv.h"
#include "../inc/port_srv.h"
#include "../inc/clock_srv_v2.h"

/* LED pins */
#define LED_BLUE_PORT   PORTD
#define LED_BLUE_PIN    0
#define LED_RED_PORT    PORTD
#define LED_RED_PIN     15

/* Timer periods in microseconds */
#define TIMER_500MS     500000U
#define TIMER_1000MS    1000000U

/* Callback functions */
void BlueLED_Callback(void)
{
    GPIO_SRV_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
}

void RedLED_Callback(void)
{
    GPIO_SRV_TogglePin(LED_RED_PORT, LED_RED_PIN);
}

void LPIT_Example(void)
{
    lpit_srv_status_t status;
    
    /* 1. Initialize clock system (48MHz for simple setup) */
    CLOCK_SRV_V2_InitPreset("RUN_48MHz");
    
    /* 2. Enable peripheral clocks */
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_PORTD, CLOCK_SRV_V2_PCS_NONE);
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPIT, CLOCK_SRV_V2_PCS_FIRCDIV2);
    
    /* 3. Configure LED pins */
    port_srv_pin_config_t led_blue_cfg = {
        .port = LED_BLUE_PORT,
        .pin = LED_BLUE_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&led_blue_cfg);
    
    port_srv_pin_config_t led_red_cfg = {
        .port = LED_RED_PORT,
        .pin = LED_RED_PIN,
        .mux = PORT_MUX_GPIO,
        .pull_config = PORT_PULL_DISABLE,
        .interrupt_config = PORT_INT_DISABLE
    };
    PORT_SRV_ConfigPin(&led_red_cfg);
    
    /* 4. Initialize GPIO */
    GPIO_SRV_Init();
    GPIO_SRV_ConfigOutput(LED_BLUE_PORT, LED_BLUE_PIN);
    GPIO_SRV_ConfigOutput(LED_RED_PORT, LED_RED_PIN);
    
    /* Turn off LEDs initially */
    GPIO_SRV_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, 0);
    GPIO_SRV_WritePin(LED_RED_PORT, LED_RED_PIN, 0);
    
    /* 5. Initialize LPIT */
    status = LPIT_SRV_Init();
    if (status != LPIT_SRV_SUCCESS) {
        /* Error handling */
        while(1);
    }
    
    /* 6. Configure timer channels with callbacks */
    lpit_srv_config_t blue_timer = {
        .channel = 0,
        .period_us = TIMER_500MS,
        .callback = BlueLED_Callback,
        .is_running = false
    };
    
    lpit_srv_config_t red_timer = {
        .channel = 1,
        .period_us = TIMER_1000MS,
        .callback = RedLED_Callback,
        .is_running = false
    };
    
    /* 7. Start timers */
    status = LPIT_SRV_StartTimer(&blue_timer);
    if (status != LPIT_SRV_SUCCESS) {
        while(1);
    }
    
    status = LPIT_SRV_StartTimer(&red_timer);
    if (status != LPIT_SRV_SUCCESS) {
        while(1);
    }
    
    /* 8. Main loop - Timers run in interrupt */
    while(1) {
        /* Do other tasks here */
        /* LEDs will toggle automatically via callbacks */
    }
}

/* Advanced example: Start/Stop timer dynamically */
void LPIT_AdvancedExample(void)
{
    lpit_srv_config_t timer_cfg = {
        .channel = 2,
        .period_us = 2000000U,  /* 2 seconds */
        .callback = NULL,
        .is_running = false
    };
    
    /* Initialize system */
    CLOCK_SRV_V2_InitPreset("RUN_80MHz");
    CLOCK_SRV_V2_EnablePeripheral(CLOCK_SRV_V2_LPIT, CLOCK_SRV_V2_PCS_FIRCDIV2);
    LPIT_SRV_Init();
    
    while(1) {
        /* Start timer */
        LPIT_SRV_StartTimer(&timer_cfg);
        
        /* Wait 10 seconds */
        for(uint32_t i = 0; i < 10000000; i++);
        
        /* Stop timer */
        LPIT_SRV_StopTimer(timer_cfg.channel);
        
        /* Wait 5 seconds */
        for(uint32_t i = 0; i < 5000000; i++);
    }
}
