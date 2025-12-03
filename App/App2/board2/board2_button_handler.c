/**
 * @file board2_button_handler.c
 * @brief Board 2 Button Handler (Implementation)
 * @details Button debouncing and event processing
 * 
 * @date December 3, 2025
 * @version 1.0
 */

#include "board2_button_handler.h"
#include "board2_can_handler.h"
#include "app_config.h"
#include "protocol.h"

/* Driver API includes */
// #include "gpio.h"
// #include "port.h"
// #include "pcc.h"

/*******************************************************************************
 * PRIVATE TYPES
 ******************************************************************************/

/** Button state structure */
typedef struct {
    bool pressed;                           /**< Current pressed state */
    bool last_pressed;                      /**< Last pressed state */
    uint32_t debounce_counter;              /**< Debounce counter (ms) */
    button_event_t pending_event;           /**< Pending event */
} button_state_t;

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/

/** Button handler state */
static struct {
    bool initialized;                       /**< Initialization flag */
    button_state_t buttons[2];              /**< Button states */
    button_callback_t callback;             /**< Event callback */
} s_buttonHandler = {
    .initialized = false,
    .callback = NULL
};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

/**
 * @brief Initialize button GPIOs
 * @return APP_STATUS_SUCCESS if successful
 */
static app_status_t ButtonHandler_InitGpio(void);

/**
 * @brief Read button state from GPIO
 * @param[in] button Button ID
 * @return true if pressed (low), false if released (high)
 */
static bool ButtonHandler_ReadGpio(button_id_t button);

/**
 * @brief Process single button
 * @param[in] button Button ID
 */
static void ButtonHandler_ProcessButton(button_id_t button);

/**
 * @brief Handle button press event
 * @param[in] button Button ID that was pressed
 */
static void ButtonHandler_OnButtonPressed(button_id_t button);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize button handler
 */
app_status_t ButtonHandler_Init(void)
{
    app_status_t status;
    
    /* Check if already initialized */
    if (s_buttonHandler.initialized) {
        return APP_STATUS_ALREADY_INITIALIZED;
    }
    
    /* Initialize GPIOs */
    status = ButtonHandler_InitGpio();
    if (status != APP_STATUS_SUCCESS) {
        return status;
    }
    
    /* Initialize button states */
    for (uint8_t i = 0; i < 2; i++) {
        s_buttonHandler.buttons[i].pressed = false;
        s_buttonHandler.buttons[i].last_pressed = false;
        s_buttonHandler.buttons[i].debounce_counter = 0;
        s_buttonHandler.buttons[i].pending_event = BUTTON_EVENT_NONE;
    }
    
    /* Mark as initialized */
    s_buttonHandler.initialized = true;
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Process button events
 */
void ButtonHandler_Process(void)
{
    if (!s_buttonHandler.initialized) {
        return;
    }
    
    /* Process both buttons */
    ButtonHandler_ProcessButton(BUTTON_1);
    ButtonHandler_ProcessButton(BUTTON_2);
}

/**
 * @brief Register button callback
 */
void ButtonHandler_RegisterCallback(button_callback_t callback)
{
    s_buttonHandler.callback = callback;
}

/**
 * @brief Get button state
 */
bool ButtonHandler_IsPressed(button_id_t button)
{
    if (button >= 2) {
        return false;
    }
    
    return s_buttonHandler.buttons[button].pressed;
}

/**
 * @brief PORT interrupt handler
 */
void ButtonHandler_PortIrqHandler(void)
{
    /* Check Button 1 interrupt flag */
    // if (PORT_IsInterruptPending(BUTTON1_PORT, BUTTON1_PIN)) {
    //     /* Clear flag */
    //     PORT_ClearInterruptFlag(BUTTON1_PORT, BUTTON1_PIN);
    //     
    //     /* Mark button as pressed (will be debounced in Process) */
    //     s_buttonHandler.buttons[BUTTON_1].pending_event = BUTTON_EVENT_PRESSED;
    // }
    
    /* Check Button 2 interrupt flag */
    // if (PORT_IsInterruptPending(BUTTON2_PORT, BUTTON2_PIN)) {
    //     /* Clear flag */
    //     PORT_ClearInterruptFlag(BUTTON2_PORT, BUTTON2_PIN);
    //     
    //     /* Mark button as pressed (will be debounced in Process) */
    //     s_buttonHandler.buttons[BUTTON_2].pending_event = BUTTON_EVENT_PRESSED;
    // }
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/

/**
 * @brief Initialize button GPIOs
 */
static app_status_t ButtonHandler_InitGpio(void)
{
    /* Enable PORT clock */
    // PCC_EnableClock(PCC_PORTC_INDEX);
    
    /* Configure Button 1 (PTC12) */
    // PORT_SetPinMux(BUTTON1_PORT, BUTTON1_PIN, PORT_MUX_GPIO);
    // PORT_SetPullUp(BUTTON1_PORT, BUTTON1_PIN, true);  /* Enable pull-up */
    // PORT_SetInterruptConfig(BUTTON1_PORT, BUTTON1_PIN, PORT_IRQ_FALLING);  /* Falling edge */
    // GPIO_SetPinDirection(BUTTON1_GPIO, BUTTON1_PIN, GPIO_INPUT);
    
    /* Configure Button 2 (PTC13) */
    // PORT_SetPinMux(BUTTON2_PORT, BUTTON2_PIN, PORT_MUX_GPIO);
    // PORT_SetPullUp(BUTTON2_PORT, BUTTON2_PIN, true);  /* Enable pull-up */
    // PORT_SetInterruptConfig(BUTTON2_PORT, BUTTON2_PIN, PORT_IRQ_FALLING);  /* Falling edge */
    // GPIO_SetPinDirection(BUTTON2_GPIO, BUTTON2_PIN, GPIO_INPUT);
    
    /* Enable PORT interrupt */
    // NVIC_SetPriority(BUTTON1_IRQ, 2);
    // NVIC_EnableIRQ(BUTTON1_IRQ);
    
    return APP_STATUS_SUCCESS;
}

/**
 * @brief Read button state from GPIO
 */
static bool ButtonHandler_ReadGpio(button_id_t button)
{
    bool state = false;
    
    /* Read GPIO pin (button pressed = low) */
    if (button == BUTTON_1) {
        // state = !GPIO_ReadPin(BUTTON1_GPIO, BUTTON1_PIN);  /* Invert: pressed = true */
    } else if (button == BUTTON_2) {
        // state = !GPIO_ReadPin(BUTTON2_GPIO, BUTTON2_PIN);  /* Invert: pressed = true */
    }
    
    return state;
}

/**
 * @brief Process single button
 */
static void ButtonHandler_ProcessButton(button_id_t button)
{
    button_state_t *btn = &s_buttonHandler.buttons[button];
    
    /* Read current state */
    bool current_state = ButtonHandler_ReadGpio(button);
    
    /* Debounce logic */
    if (current_state != btn->last_pressed) {
        /* State changed - start/reset debounce counter */
        btn->debounce_counter = BUTTON_DEBOUNCE_MS;
        btn->last_pressed = current_state;
    } else if (btn->debounce_counter > 0) {
        /* Decrement counter */
        btn->debounce_counter--;
        
        /* If counter reached zero, state is stable */
        if (btn->debounce_counter == 0) {
            /* Update pressed state */
            if (current_state && !btn->pressed) {
                /* Button was pressed */
                btn->pressed = true;
                ButtonHandler_OnButtonPressed(button);
                
                /* Call callback */
                if (s_buttonHandler.callback != NULL) {
                    s_buttonHandler.callback(button, BUTTON_EVENT_PRESSED);
                }
            } else if (!current_state && btn->pressed) {
                /* Button was released */
                btn->pressed = false;
                
                /* Call callback */
                if (s_buttonHandler.callback != NULL) {
                    s_buttonHandler.callback(button, BUTTON_EVENT_RELEASED);
                }
            }
        }
    }
}

/**
 * @brief Handle button press event
 */
static void ButtonHandler_OnButtonPressed(button_id_t button)
{
    /* Send CAN command based on button */
    if (button == BUTTON_1) {
        /* Button 1: Start ADC */
        Board2_CanHandler_SendCommand(CMD_START_ADC);
    } else if (button == BUTTON_2) {
        /* Button 2: Stop ADC */
        Board2_CanHandler_SendCommand(CMD_STOP_ADC);
    }
}

/*******************************************************************************
 * INTERRUPT SERVICE ROUTINES
 ******************************************************************************/

/**
 * @brief PORTC interrupt handler
 */
void PORTC_IRQHandler(void)
{
    /* Call handler */
    ButtonHandler_PortIrqHandler();
}
