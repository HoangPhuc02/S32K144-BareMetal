/**
 * @file    gpio_example.c
 * @brief   GPIO Driver Usage Examples for S32K144
 * @details This file contains practical examples demonstrating GPIO usage
 *          including RGB LED control, button polling, and button interrupts.
 * 
 * @author  PhucPH32
 * @date    November 24, 2025
 * @version 1.0
 * 
 * @note    Hardware connections:
 *          - PTD0:  RGB LED - RED   (Active Low)
 *          - PTD15: RGB LED - GREEN (Active Low)
 *          - PTD16: RGB LED - BLUE  (Active Low)
 *          - PTC12: Button SW2 (Active Low with pull-up)
 *          - PTC13: Button SW3 (Active Low with pull-up)
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "gpio.h"
#include "port.h"
#include <stdio.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* RGB LED Pins - PTD0, PTD15, PTD16 (Active Low) */
#define LED_RED_PORT        GPIO_PORT_D
#define LED_RED_PIN         0U

#define LED_GREEN_PORT      GPIO_PORT_D
#define LED_GREEN_PIN       15U

#define LED_BLUE_PORT       GPIO_PORT_D
#define LED_BLUE_PIN        16U

/* Button Pins - PTC12, PTC13 (Active Low with internal pull-up) */
#define BTN_SW2_PORT        GPIO_PORT_C
#define BTN_SW2_PIN         12U

#define BTN_SW3_PORT        GPIO_PORT_C
#define BTN_SW3_PIN         13U

/* LED States (Active Low) */
#define LED_ON              GPIO_LEVEL_LOW
#define LED_OFF             GPIO_LEVEL_HIGH

/* Button States (Active Low) */
#define BTN_PRESSED         GPIO_LEVEL_LOW
#define BTN_RELEASED        GPIO_LEVEL_HIGH

/* Debounce delay in ms */
#define DEBOUNCE_DELAY_MS   20

/*******************************************************************************
 * Global Variables
 ******************************************************************************/

/* Button interrupt flags */
volatile bool g_btn_sw2_pressed = false;
volatile bool g_btn_sw3_pressed = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

static void Delay_Ms(uint32_t ms);
static void RGB_LED_Init(void);
static void Buttons_Init(void);
static void RGB_SetColor(bool red, bool green, bool blue);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Simple delay in milliseconds (approximate)
 */
static void Delay_Ms(uint32_t ms)
{
    /* Approximate delay for 80 MHz CPU clock */
    for (volatile uint32_t i = 0; i < ms * 8000; i++);
}

/**
 * @brief Initialize RGB LED pins
 */
static void RGB_LED_Init(void)
{
    /* Initialize RED LED - PTD0 */
    GPIO_Init(LED_RED_PORT, LED_RED_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, LED_OFF);
    
    /* Initialize GREEN LED - PTD15 */
    GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_OFF);
    
    /* Initialize BLUE LED - PTD16 */
    GPIO_Init(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_DIR_OUTPUT);
    GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, LED_OFF);
    
    printf("RGB LED initialized (PTD0=RED, PTD15=GREEN, PTD16=BLUE)\n");
}

/**
 * @brief Initialize button pins
 */
static void Buttons_Init(void)
{
    /* Enable PORT C clock */
    PORT_EnableClock(PORT_C);
    
    /* Configure SW2 button - PTC12 as GPIO input with pull-up */
    PORT_SetPinMux(PORT_C, BTN_SW2_PIN, PORT_MUX_GPIO);
    PORT_SetPullConfig(PORT_C, BTN_SW2_PIN, PORT_PULL_UP);
    GPIO_SetPinDirection(BTN_SW2_PORT, BTN_SW2_PIN, GPIO_DIR_INPUT);
    
    /* Configure SW3 button - PTC13 as GPIO input with pull-up */
    PORT_SetPinMux(PORT_C, BTN_SW3_PIN, PORT_MUX_GPIO);
    PORT_SetPullConfig(PORT_C, BTN_SW3_PIN, PORT_PULL_UP);
    GPIO_SetPinDirection(BTN_SW3_PORT, BTN_SW3_PIN, GPIO_DIR_INPUT);
    
    printf("Buttons initialized (PTC12=SW2, PTC13=SW3)\n");
}

/**
 * @brief Set RGB LED color
 * @param red true = ON, false = OFF
 * @param green true = ON, false = OFF
 * @param blue true = ON, false = OFF
 */
static void RGB_SetColor(bool red, bool green, bool blue)
{
    GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, red ? LED_ON : LED_OFF);
    GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, green ? LED_ON : LED_OFF);
    GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, blue ? LED_ON : LED_OFF);
}

/*******************************************************************************
 * Example 1: RGB LED Basic Control
 ******************************************************************************/

/**
 * @brief Test all RGB colors sequentially
 */
void Example1_RGB_BasicTest(void)
{
    printf("\n=== Example 1: RGB LED Basic Test ===\n");
    
    /* Turn off all LEDs */
    RGB_SetColor(false, false, false);
    Delay_Ms(500);
    
    /* RED */
    printf("RED ON\n");
    RGB_SetColor(true, false, false);
    Delay_Ms(1000);
    
    /* GREEN */
    printf("GREEN ON\n");
    RGB_SetColor(false, true, false);
    Delay_Ms(1000);
    
    /* BLUE */
    printf("BLUE ON\n");
    RGB_SetColor(false, false, true);
    Delay_Ms(1000);
    
    /* YELLOW (RED + GREEN) */
    printf("YELLOW ON\n");
    RGB_SetColor(true, true, false);
    Delay_Ms(1000);
    
    /* CYAN (GREEN + BLUE) */
    printf("CYAN ON\n");
    RGB_SetColor(false, true, true);
    Delay_Ms(1000);
    
    /* MAGENTA (RED + BLUE) */
    printf("MAGENTA ON\n");
    RGB_SetColor(true, false, true);
    Delay_Ms(1000);
    
    /* WHITE (ALL ON) */
    printf("WHITE ON\n");
    RGB_SetColor(true, true, true);
    Delay_Ms(1000);
    
    /* OFF */
    printf("ALL OFF\n");
    RGB_SetColor(false, false, false);
    
    printf("Basic test complete!\n");
}

/*******************************************************************************
 * Example 2: RGB LED Blink Pattern
 ******************************************************************************/

/**
 * @brief Blink each LED color in sequence
 */
void Example2_RGB_BlinkPattern(void)
{
    printf("\n=== Example 2: RGB LED Blink Pattern ===\n");
    
    for (int cycle = 0; cycle < 3; cycle++) {
        printf("Cycle %d/3\n", cycle + 1);
        
        /* Blink RED */
        for (int i = 0; i < 3; i++) {
            GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, LED_ON);
            Delay_Ms(200);
            GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, LED_OFF);
            Delay_Ms(200);
        }
        
        /* Blink GREEN */
        for (int i = 0; i < 3; i++) {
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_ON);
            Delay_Ms(200);
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_OFF);
            Delay_Ms(200);
        }
        
        /* Blink BLUE */
        for (int i = 0; i < 3; i++) {
            GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, LED_ON);
            Delay_Ms(200);
            GPIO_WritePin(LED_BLUE_PORT, LED_BLUE_PIN, LED_OFF);
            Delay_Ms(200);
        }
    }
    
    printf("Blink pattern complete!\n");
}

/*******************************************************************************
 * Example 3: RGB LED Toggle
 ******************************************************************************/

/**
 * @brief Demonstrate GPIO toggle function
 */
void Example3_RGB_Toggle(void)
{
    printf("\n=== Example 3: RGB LED Toggle ===\n");
    
    /* Start with all LEDs OFF */
    RGB_SetColor(false, false, false);
    
    /* Toggle each LED rapidly */
    for (int i = 0; i < 10; i++) {
        GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN);
        Delay_Ms(100);
        
        GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
        Delay_Ms(100);
        
        GPIO_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
        Delay_Ms(100);
    }
    
    /* Turn off all */
    RGB_SetColor(false, false, false);
    
    printf("Toggle test complete!\n");
}

/*******************************************************************************
 * Example 4: Button Polling - Control LED with Button
 ******************************************************************************/

/**
 * @brief Read button with debouncing (polling)
 */
static bool Button_ReadWithDebounce(gpio_port_t port, uint8_t pin)
{
    if (GPIO_ReadPin(port, pin) == BTN_PRESSED) {
        Delay_Ms(DEBOUNCE_DELAY_MS);
        if (GPIO_ReadPin(port, pin) == BTN_PRESSED) {
            /* Wait for button release */
            while (GPIO_ReadPin(port, pin) == BTN_PRESSED);
            Delay_Ms(DEBOUNCE_DELAY_MS);
            return true;
        }
    }
    return false;
}

/**
 * @brief Control RGB LED using button polling
 */
void Example4_ButtonPolling(void)
{
    printf("\n=== Example 4: Button Polling ===\n");
    printf("SW2 (PTC12): Cycle through colors\n");
    printf("SW3 (PTC13): Turn OFF all LEDs\n");
    printf("Press both buttons simultaneously to exit\n\n");
    
    uint8_t colorIndex = 0;
    const char *colorNames[] = {"OFF", "RED", "GREEN", "BLUE", "YELLOW", "CYAN", "MAGENTA", "WHITE"};
    
    /* Start with all LEDs OFF */
    RGB_SetColor(false, false, false);
    
    while (1) {
        /* Check SW2 button - cycle colors */
        if (Button_ReadWithDebounce(BTN_SW2_PORT, BTN_SW2_PIN)) {
            colorIndex++;
            if (colorIndex > 7) colorIndex = 0;
            
            printf("Color: %s\n", colorNames[colorIndex]);
            
            switch (colorIndex) {
                case 0: RGB_SetColor(false, false, false); break; /* OFF */
                case 1: RGB_SetColor(true, false, false); break;  /* RED */
                case 2: RGB_SetColor(false, true, false); break;  /* GREEN */
                case 3: RGB_SetColor(false, false, true); break;  /* BLUE */
                case 4: RGB_SetColor(true, true, false); break;   /* YELLOW */
                case 5: RGB_SetColor(false, true, true); break;   /* CYAN */
                case 6: RGB_SetColor(true, false, true); break;   /* MAGENTA */
                case 7: RGB_SetColor(true, true, true); break;    /* WHITE */
            }
        }
        
        /* Check SW3 button - turn off */
        if (Button_ReadWithDebounce(BTN_SW3_PORT, BTN_SW3_PIN)) {
            printf("All LEDs OFF\n");
            RGB_SetColor(false, false, false);
            colorIndex = 0;
        }
        
        /* Check if both buttons pressed - exit */
        if (GPIO_ReadPin(BTN_SW2_PORT, BTN_SW2_PIN) == BTN_PRESSED &&
            GPIO_ReadPin(BTN_SW3_PORT, BTN_SW3_PIN) == BTN_PRESSED) {
            printf("Both buttons pressed - exiting polling demo\n");
            Delay_Ms(500);
            break;
        }
    }
    
    /* Turn off all LEDs */
    RGB_SetColor(false, false, false);
    printf("Button polling demo complete!\n");
}

/*******************************************************************************
 * Example 5: Button Interrupt - Control LED with Interrupt
 ******************************************************************************/

/**
 * @brief Configure button interrupts
 */
static void Button_ConfigureInterrupts(void)
{
    /* Configure SW2 (PTC12) for falling edge interrupt */
    PORT_SetPinInterrupt(PORT_C, BTN_SW2_PIN, PORT_INT_FALLING_EDGE);
    PORT_ClearPinInterruptFlag(PORT_C, BTN_SW2_PIN);
    
    /* Configure SW3 (PTC13) for falling edge interrupt */
    PORT_SetPinInterrupt(PORT_C, BTN_SW3_PIN, PORT_INT_FALLING_EDGE);
    PORT_ClearPinInterruptFlag(PORT_C, BTN_SW3_PIN);
    
    /* Enable PORTC interrupt in NVIC */
    /* Note: In real implementation, you would enable NVIC here */
    /* NVIC_EnableIRQ(PORTC_IRQn); */
    /* NVIC_SetPriority(PORTC_IRQn, 3); */
    
    printf("Button interrupts configured\n");
}

/**
 * @brief Disable button interrupts
 */
static void Button_DisableInterrupts(void)
{
    /* Disable interrupts */
    PORT_SetPinInterrupt(PORT_C, BTN_SW2_PIN, PORT_INT_DISABLED);
    PORT_SetPinInterrupt(PORT_C, BTN_SW3_PIN, PORT_INT_DISABLED);
    
    /* Disable NVIC */
    /* NVIC_DisableIRQ(PORTC_IRQn); */
    
    printf("Button interrupts disabled\n");
}

/**
 * @brief PORT C interrupt handler (should be in main application)
 * @note This is an example implementation
 */
void PORTC_IRQHandler(void)
{
    /* Check SW2 interrupt flag */
    if (PORT_GetPinInterruptFlag(PORT_C, BTN_SW2_PIN)) {
        /* Clear flag */
        PORT_ClearPinInterruptFlag(PORT_C, BTN_SW2_PIN);
        
        /* Set flag for main loop */
        g_btn_sw2_pressed = true;
    }
    
    /* Check SW3 interrupt flag */
    if (PORT_GetPinInterruptFlag(PORT_C, BTN_SW3_PIN)) {
        /* Clear flag */
        PORT_ClearPinInterruptFlag(PORT_C, BTN_SW3_PIN);
        
        /* Set flag for main loop */
        g_btn_sw3_pressed = true;
    }
}

/**
 * @brief Control RGB LED using button interrupts
 */
void Example5_ButtonInterrupt(void)
{
    printf("\n=== Example 5: Button Interrupt ===\n");
    printf("SW2 (PTC12): Toggle RED LED\n");
    printf("SW3 (PTC13): Toggle GREEN LED\n");
    printf("Press both buttons to exit\n\n");
    
    /* Configure interrupts */
    Button_ConfigureInterrupts();
    
    /* Start with all LEDs OFF */
    RGB_SetColor(false, false, false);
    
    bool redState = false;
    bool greenState = false;
    uint32_t timeout = 0;
    const uint32_t TIMEOUT_COUNT = 100000; /* ~10 seconds */
    
    /* Main loop - check interrupt flags */
    while (timeout < TIMEOUT_COUNT) {
        /* Check SW2 button flag */
        if (g_btn_sw2_pressed) {
            g_btn_sw2_pressed = false;
            
            /* Debounce */
            Delay_Ms(DEBOUNCE_DELAY_MS);
            
            /* Toggle RED LED */
            redState = !redState;
            GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, redState ? LED_ON : LED_OFF);
            printf("RED LED: %s\n", redState ? "ON" : "OFF");
        }
        
        /* Check SW3 button flag */
        if (g_btn_sw3_pressed) {
            g_btn_sw3_pressed = false;
            
            /* Debounce */
            Delay_Ms(DEBOUNCE_DELAY_MS);
            
            /* Toggle GREEN LED */
            greenState = !greenState;
            GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, greenState ? LED_ON : LED_OFF);
            printf("GREEN LED: %s\n", greenState ? "ON" : "OFF");
        }
        
        /* Check if both buttons pressed simultaneously (poll for exit) */
        if (GPIO_ReadPin(BTN_SW2_PORT, BTN_SW2_PIN) == BTN_PRESSED &&
            GPIO_ReadPin(BTN_SW3_PORT, BTN_SW3_PIN) == BTN_PRESSED) {
            printf("Both buttons pressed - exiting interrupt demo\n");
            Delay_Ms(500);
            break;
        }
        
        timeout++;
    }
    
    /* Disable interrupts */
    Button_DisableInterrupts();
    
    /* Turn off all LEDs */
    RGB_SetColor(false, false, false);
    
    printf("Button interrupt demo complete!\n");
}

/*******************************************************************************
 * Main Function
 ******************************************************************************/

/**
 * @brief Main function - runs all GPIO examples
 */
void GPIO_ExamplesMain(void)
{
    printf("\n");
    printf("=====================================\n");
    printf("  S32K144 GPIO Examples - RGB LED   \n");
    printf("=====================================\n\n");
    
    /* Initialize hardware */
    RGB_LED_Init();
    Buttons_Init();
    
    /* Small delay after initialization */
    Delay_Ms(500);
    
    /* Example 1: Basic RGB test */
    Example1_RGB_BasicTest();
    Delay_Ms(1000);
    
    /* Example 2: Blink pattern */
    Example2_RGB_BlinkPattern();
    Delay_Ms(1000);
    
    /* Example 3: Toggle test */
    Example3_RGB_Toggle();
    Delay_Ms(1000);
    
    /* Example 4: Button polling */
    Example4_ButtonPolling();
    Delay_Ms(1000);
    
    /* Example 5: Button interrupt */
    Example5_ButtonInterrupt();
    Delay_Ms(1000);
    
    /* Final message */
    printf("\n");
    printf("=====================================\n");
    printf("  All GPIO Examples Completed!      \n");
    printf("=====================================\n");
}

/*******************************************************************************
 * EOF
 ******************************************************************************/
