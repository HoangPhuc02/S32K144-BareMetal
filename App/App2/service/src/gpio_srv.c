/**
 * @file    gpio_srv.c
 * @brief   GPIO Service Layer Implementation
 * @details Implementation của GPIO service layer, wrapper cho GPIO driver
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.1
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/gpio_srv.h"
#include "../../../../Core/BareMetal/gpio/gpio.h"
#include "../../../../Core/BareMetal/port/port.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_CALLBACKS   32  /* Maximum number of interrupt callbacks per port */

/*******************************************************************************
 * Private Types
 ******************************************************************************/
typedef struct {
    gpio_srv_callback_t callback;
    bool enabled;
} gpio_callback_entry_t;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_gpio_initialized = false;

/* Callback storage for each port */
static gpio_callback_entry_t s_callbacks_porta[MAX_CALLBACKS];
static gpio_callback_entry_t s_callbacks_portb[MAX_CALLBACKS];
static gpio_callback_entry_t s_callbacks_portc[MAX_CALLBACKS];
static gpio_callback_entry_t s_callbacks_portd[MAX_CALLBACKS];
static gpio_callback_entry_t s_callbacks_porte[MAX_CALLBACKS];

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static GPIO_Type* GPIO_SRV_GetPortBase(uint8_t port)
{
    switch (port) {
        case 0: return PTA;
        case 1: return PTB;
        case 2: return PTC;
        case 3: return PTD;
        case 4: return PTE;
        default: return NULL;
    }
}

static PORT_Type* GPIO_SRV_GetPORTBase(uint8_t port)
{
    switch (port) {
        case 0: return PORTA;
        case 1: return PORTB;
        case 2: return PORTC;
        case 3: return PORTD;
        case 4: return PORTE;
        default: return NULL;
    }
}

static gpio_callback_entry_t* GPIO_SRV_GetCallbackArray(uint8_t port)
{
    switch (port) {
        case 0: return s_callbacks_porta;
        case 1: return s_callbacks_portb;
        case 2: return s_callbacks_portc;
        case 3: return s_callbacks_portd;
        case 4: return s_callbacks_porte;
        default: return NULL;
    }
}

static uint32_t GPIO_SRV_ConvertInterruptType(gpio_srv_interrupt_t trigger)
{
    switch (trigger) {
        case GPIO_SRV_INT_RISING_EDGE:
            return PORT_INTERRUPT_RISE_EDGE;
        case GPIO_SRV_INT_FALLING_EDGE:
            return PORT_INTERRUPT_FALL_EDGE;
        case GPIO_SRV_INT_BOTH_EDGES:
            return PORT_INTERRUPT_BOTH_EDGE;
        case GPIO_SRV_INT_LOGIC_LOW:
            return PORT_INTERRUPT_LOGIC_0;
        case GPIO_SRV_INT_LOGIC_HIGH:
            return PORT_INTERRUPT_LOGIC_1;
        case GPIO_SRV_INT_DISABLE:
        default:
            return PORT_INTERRUPT_DISABLE;
    }
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

gpio_srv_status_t GPIO_SRV_Init(void)
{
    s_gpio_initialized = true;
    return GPIO_SRV_SUCCESS;
}

gpio_srv_status_t GPIO_SRV_ConfigInput(uint8_t port, uint8_t pin)
{
    if (!s_gpio_initialized) {
        return GPIO_SRV_NOT_INITIALIZED;
    }
    
    GPIO_Type *gpio_base = GPIO_SRV_GetPortBase(port);
    if (gpio_base == NULL) {
        return GPIO_SRV_ERROR;
    }
    
    gpio_status_t status = GPIO_Init(gpio_base, pin, GPIO_INPUT);
    
    return (status == GPIO_STATUS_SUCCESS) ? GPIO_SRV_SUCCESS : GPIO_SRV_ERROR;
}

gpio_srv_status_t GPIO_SRV_ConfigOutput(uint8_t port, uint8_t pin)
{
    if (!s_gpio_initialized) {
        return GPIO_SRV_NOT_INITIALIZED;
    }
    
    GPIO_Type *gpio_base = GPIO_SRV_GetPortBase(port);
    if (gpio_base == NULL) {
        return GPIO_SRV_ERROR;
    }
    
    gpio_status_t status = GPIO_Init(gpio_base, pin, GPIO_OUTPUT);
    
    return (status == GPIO_STATUS_SUCCESS) ? GPIO_SRV_SUCCESS : GPIO_SRV_ERROR;
}

uint8_t GPIO_SRV_Read(uint8_t port, uint8_t pin)
{
    GPIO_Type *gpio_base = GPIO_SRV_GetPortBase(port);
    if (gpio_base == NULL) {
        return 0;
    }
    
    return (uint8_t)((gpio_base->PDIR >> pin) & 0x01U);
}

gpio_srv_status_t GPIO_SRV_Write(uint8_t port, uint8_t pin, uint8_t value)
{
    if (!s_gpio_initialized) {
        return GPIO_SRV_NOT_INITIALIZED;
    }
    
    GPIO_Type *gpio_base = GPIO_SRV_GetPortBase(port);
    if (gpio_base == NULL) {
        return GPIO_SRV_ERROR;
    }
    
    gpio_status_t status;
    if (value) {
        status = GPIO_SetPin(gpio_base, pin);
    } else {
        status = GPIO_ClearPin(gpio_base, pin);
    }
    
    return (status == GPIO_STATUS_SUCCESS) ? GPIO_SRV_SUCCESS : GPIO_SRV_ERROR;
}

gpio_srv_status_t GPIO_SRV_Toggle(uint8_t port, uint8_t pin)
{
    if (!s_gpio_initialized) {
        return GPIO_SRV_NOT_INITIALIZED;
    }
    
    GPIO_Type *gpio_base = GPIO_SRV_GetPortBase(port);
    if (gpio_base == NULL) {
        return GPIO_SRV_ERROR;
    }
    
    gpio_status_t status = GPIO_TogglePin(gpio_base, pin);
    
    return (status == GPIO_STATUS_SUCCESS) ? GPIO_SRV_SUCCESS : GPIO_SRV_ERROR;
}

gpio_srv_status_t GPIO_SRV_EnableInterrupt(uint8_t port, uint8_t pin, 
                                            gpio_srv_interrupt_t trigger,
                                            gpio_srv_callback_t callback)
{
    if (!s_gpio_initialized) {
        return GPIO_SRV_NOT_INITIALIZED;
    }
    
    if (pin >= MAX_CALLBACKS) {
        return GPIO_SRV_ERROR;
    }
    
    PORT_Type *port_base = GPIO_SRV_GetPORTBase(port);
    if (port_base == NULL) {
        return GPIO_SRV_ERROR;
    }
    
    /* Store callback if provided */
    gpio_callback_entry_t *callbacks = GPIO_SRV_GetCallbackArray(port);
    if (callbacks != NULL && callback != NULL) {
        callbacks[pin].callback = callback;
        callbacks[pin].enabled = true;
    }
    
    /* Configure PORT interrupt */
    port_pin_config_t port_cfg;
    port_cfg.value = 0;
    port_cfg.field.IRQC = GPIO_SRV_ConvertInterruptType(trigger);
    
    port_status_t status = PORT_Init(port_base, pin, &port_cfg);
    
    return (status == PORT_STATUS_SUCCESS) ? GPIO_SRV_SUCCESS : GPIO_SRV_ERROR;
}

gpio_srv_status_t GPIO_SRV_DisableInterrupt(uint8_t port, uint8_t pin)
{
    if (!s_gpio_initialized) {
        return GPIO_SRV_NOT_INITIALIZED;
    }
    
    if (pin >= MAX_CALLBACKS) {
        return GPIO_SRV_ERROR;
    }
    
    PORT_Type *port_base = GPIO_SRV_GetPORTBase(port);
    if (port_base == NULL) {
        return GPIO_SRV_ERROR;
    }
    
    /* Disable callback */
    gpio_callback_entry_t *callbacks = GPIO_SRV_GetCallbackArray(port);
    if (callbacks != NULL) {
        callbacks[pin].enabled = false;
        callbacks[pin].callback = NULL;
    }
    
    /* Disable PORT interrupt */
    port_pin_config_t port_cfg;
    port_cfg.value = 0;
    port_cfg.field.IRQC = PORT_INTERRUPT_DISABLE;
    
    port_status_t status = PORT_Init(port_base, pin, &port_cfg);
    
    return (status == PORT_STATUS_SUCCESS) ? GPIO_SRV_SUCCESS : GPIO_SRV_ERROR;
}

bool GPIO_SRV_IsInterruptPending(uint8_t port, uint8_t pin)
{
    PORT_Type *port_base = GPIO_SRV_GetPORTBase(port);
    if (port_base == NULL) {
        return false;
    }
    
    port_status_t status = PORT_InterruptCheck(port_base, pin);
    
    return (status == PORT_STATUS_TRIGGER_TRUE);
}

gpio_srv_status_t GPIO_SRV_ClearInterrupt(uint8_t port, uint8_t pin)
{
    if (!s_gpio_initialized) {
        return GPIO_SRV_NOT_INITIALIZED;
    }
    
    PORT_Type *port_base = GPIO_SRV_GetPORTBase(port);
    if (port_base == NULL) {
        return GPIO_SRV_ERROR;
    }
    
    port_status_t status = PORT_InterruptClear(port_base, pin);
    
    return (status == PORT_STATUS_SUCCESS) ? GPIO_SRV_SUCCESS : GPIO_SRV_ERROR;
}

/* Alias functions for convenience */
uint8_t GPIO_SRV_ReadPin(uint8_t port, uint8_t pin)
{
    return GPIO_SRV_Read(port, pin);
}

gpio_srv_status_t GPIO_SRV_WritePin(uint8_t port, uint8_t pin, uint8_t value)
{
    return GPIO_SRV_Write(port, pin, value);
}

gpio_srv_status_t GPIO_SRV_TogglePin(uint8_t port, uint8_t pin)
{
    return GPIO_SRV_Toggle(port, pin);
}

/*******************************************************************************
 * Interrupt Handlers (User should implement in application)
 ******************************************************************************/

/**
 * @brief PORT A interrupt handler
 * @note User should call this from PORTA_IRQHandler() in application
 */
void GPIO_SRV_PORTA_IRQHandler(void)
{
    for (uint8_t pin = 0; pin < MAX_CALLBACKS; pin++) {
        if (GPIO_SRV_IsInterruptPending(0, pin)) {
            /* Execute callback if registered */
            if (s_callbacks_porta[pin].enabled && s_callbacks_porta[pin].callback != NULL) {
                s_callbacks_porta[pin].callback(0, pin);
            }
            /* Clear interrupt flag */
            GPIO_SRV_ClearInterrupt(0, pin);
        }
    }
}

/**
 * @brief PORT B interrupt handler
 * @note User should call this from PORTB_IRQHandler() in application
 */
void GPIO_SRV_PORTB_IRQHandler(void)
{
    for (uint8_t pin = 0; pin < MAX_CALLBACKS; pin++) {
        if (GPIO_SRV_IsInterruptPending(1, pin)) {
            if (s_callbacks_portb[pin].enabled && s_callbacks_portb[pin].callback != NULL) {
                s_callbacks_portb[pin].callback(1, pin);
            }
            GPIO_SRV_ClearInterrupt(1, pin);
        }
    }
}

/**
 * @brief PORT C interrupt handler
 * @note User should call this from PORTC_IRQHandler() in application
 */
void GPIO_SRV_PORTC_IRQHandler(void)
{
    for (uint8_t pin = 0; pin < MAX_CALLBACKS; pin++) {
        if (GPIO_SRV_IsInterruptPending(2, pin)) {
            if (s_callbacks_portc[pin].enabled && s_callbacks_portc[pin].callback != NULL) {
                s_callbacks_portc[pin].callback(2, pin);
            }
            GPIO_SRV_ClearInterrupt(2, pin);
        }
    }
}

/**
 * @brief PORT D interrupt handler
 * @note User should call this from PORTD_IRQHandler() in application
 */
void GPIO_SRV_PORTD_IRQHandler(void)
{
    for (uint8_t pin = 0; pin < MAX_CALLBACKS; pin++) {
        if (GPIO_SRV_IsInterruptPending(3, pin)) {
            if (s_callbacks_portd[pin].enabled && s_callbacks_portd[pin].callback != NULL) {
                s_callbacks_portd[pin].callback(3, pin);
            }
            GPIO_SRV_ClearInterrupt(3, pin);
        }
    }
}

/**
 * @brief PORT E interrupt handler
 * @note User should call this from PORTE_IRQHandler() in application
 */
void GPIO_SRV_PORTE_IRQHandler(void)
{
    for (uint8_t pin = 0; pin < MAX_CALLBACKS; pin++) {
        if (GPIO_SRV_IsInterruptPending(4, pin)) {
            if (s_callbacks_porte[pin].enabled && s_callbacks_porte[pin].callback != NULL) {
                s_callbacks_porte[pin].callback(4, pin);
            }
            GPIO_SRV_ClearInterrupt(4, pin);
        }
    }
}
