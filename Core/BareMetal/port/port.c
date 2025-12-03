/**
 * @file    port.c
 * @brief   PORT Driver Implementation
 */

#include "port.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Public Functions
 ******************************************************************************/
port_status_t PORT_Init(PORT_Type *port, uint8_t pin, port_pin_config_t *cfg) {
    if (pin >= PORT_MAX_PINS) {
        return PORT_STATUS_INVALID_PARAM;
    }
    port->PCR[pin] = cfg->value;
    return PORT_STATUS_SUCCESS;
}
port_status_t PORT_InterruptCheck(PORT_Type *port, uint8_t pin) {
    if (pin >= PORT_MAX_PINS) {
        return PORT_STATUS_INVALID_PARAM;
    }
    if (port->ISFR & (1U << pin)) {
        return PORT_STATUS_TRIGGER_TRUE;
    } else {
        return PORT_STATUS_TRIGGER_FALSE;
    }
}
port_status_t PORT_InterruptClear(PORT_Type *port, uint8_t pin) {
    if (pin >= PORT_MAX_PINS) {
        return PORT_STATUS_INVALID_PARAM;
    }
    port->ISFR |= (1U << pin);
    return PORT_STATUS_SUCCESS;
}