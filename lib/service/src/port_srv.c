/**
 * @file    port_srv.c
 * @brief   PORT Service Layer Implementation
 * @details Implementation của PORT service layer, wrapper cho PORT driver
 * 
 * @author  PhucPH32
 * @date    05/12/2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "../inc/port_srv.h"
#include "../../../../Core/BareMetal/port/port.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static bool s_port_initialized = false;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
static PORT_Type* PORT_SRV_GetPortBase(uint8_t port)
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

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

port_srv_status_t PORT_SRV_Init(void)
{
    s_port_initialized = true;
    return PORT_SRV_SUCCESS;
}

port_srv_status_t PORT_SRV_ConfigPin(const port_srv_pin_config_t *config)
{
    if (!s_port_initialized) {
        return PORT_SRV_NOT_INITIALIZED;
    }
    
    if (config == NULL) {
        return PORT_SRV_ERROR;
    }
    
    PORT_Type *port_base = PORT_SRV_GetPortBase(config->port);
    if (port_base == NULL) {
        return PORT_SRV_ERROR;
    }
    
    port_pin_config_t port_cfg;
    port_cfg.value = 0;
    
    /* Configure MUX */
    port_cfg.field.MUX = config->mux;
    
    /* Configure Pull */
    if (config->pull == PORT_SRV_PULL_UP) {
        port_cfg.field.PE = PORT_RESISTOR_ENABLE;
        port_cfg.field.PS = PORT_RESISTOR_PULLUP;
    } else if (config->pull == PORT_SRV_PULL_DOWN) {
        port_cfg.field.PE = PORT_RESISTOR_ENABLE;
        port_cfg.field.PS = PORT_RESISTOR_PULLDOWN;
    } else {
        port_cfg.field.PE = PORT_RESISTOR_DISABLE;
    }
    
    /* Configure Interrupt */
    switch (config->interrupt) {
        case PORT_SRV_INT_RISING:
            port_cfg.field.IRQC = PORT_INTERRUPT_RISE_EDGE;
            break;
        case PORT_SRV_INT_FALLING:
            port_cfg.field.IRQC = PORT_INTERRUPT_FALL_EDGE;
            break;
        case PORT_SRV_INT_BOTH:
            port_cfg.field.IRQC = PORT_INTERRUPT_BOTH_EDGE;
            break;
        default:
            port_cfg.field.IRQC = PORT_INTERRUPT_DISABLE;
            break;
    }
    
    port_status_t status = PORT_Init(port_base, config->pin, &port_cfg);
    
    return (status == PORT_STATUS_SUCCESS) ? PORT_SRV_SUCCESS : PORT_SRV_ERROR;
}

port_srv_status_t PORT_SRV_SetMux(uint8_t port, uint8_t pin, port_srv_mux_t mux)
{
    if (!s_port_initialized) {
        return PORT_SRV_NOT_INITIALIZED;
    }
    
    PORT_Type *port_base = PORT_SRV_GetPortBase(port);
    if (port_base == NULL) {
        return PORT_SRV_ERROR;
    }
    
    port_pin_config_t port_cfg;
    port_cfg.value = 0;
    port_cfg.field.MUX = mux;
    
    port_status_t status = PORT_Init(port_base, pin, &port_cfg);
    
    return (status == PORT_STATUS_SUCCESS) ? PORT_SRV_SUCCESS : PORT_SRV_ERROR;
}

port_srv_status_t PORT_SRV_ConfigInterrupt(uint8_t port, uint8_t pin, port_srv_interrupt_t interrupt)
{
    if (!s_port_initialized) {
        return PORT_SRV_NOT_INITIALIZED;
    }
    
    PORT_Type *port_base = PORT_SRV_GetPortBase(port);
    if (port_base == NULL) {
        return PORT_SRV_ERROR;
    }
    
    port_pin_config_t port_cfg;
    port_cfg.value = 0;
    port_cfg.field.MUX = PORT_MUX_GPIO;
    
    switch (interrupt) {
        case PORT_SRV_INT_RISING:
            port_cfg.field.IRQC = PORT_INTERRUPT_RISE_EDGE;
            break;
        case PORT_SRV_INT_FALLING:
            port_cfg.field.IRQC = PORT_INTERRUPT_FALL_EDGE;
            break;
        case PORT_SRV_INT_BOTH:
            port_cfg.field.IRQC = PORT_INTERRUPT_BOTH_EDGE;
            break;
        default:
            port_cfg.field.IRQC = PORT_INTERRUPT_DISABLE;
            break;
    }
    
    port_status_t status = PORT_Init(port_base, pin, &port_cfg);
    
    return (status == PORT_STATUS_SUCCESS) ? PORT_SRV_SUCCESS : PORT_SRV_ERROR;
}

port_srv_status_t PORT_SRV_ClearInterruptFlag(uint8_t port, uint8_t pin)
{
    if (!s_port_initialized) {
        return PORT_SRV_NOT_INITIALIZED;
    }
    
    PORT_Type *port_base = PORT_SRV_GetPortBase(port);
    if (port_base == NULL) {
        return PORT_SRV_ERROR;
    }
    
    port_status_t status = PORT_InterruptClear(port_base, pin);
    
    return (status == PORT_STATUS_SUCCESS) ? PORT_SRV_SUCCESS : PORT_SRV_ERROR;
}
