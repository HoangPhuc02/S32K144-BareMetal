/**
 * @file    port.h
 * @brief   PORT Driver API for S32K144
 */

#ifndef PORT_H
#define PORT_H

#include "S32K144.h"
// #include "port_reg.h"

/* Constants */
#define PORT_MAX_PINS (32U)
#define PORT_A        PORTA
#define PORT_B        PORTB
#define PORT_C        PORTC
#define PORT_D        PORTD
#define PORT_E        PORTE

#define PORT_RESISTOR_DISABLE  0x0U
#define PORT_RESISTOR_ENABLE   0x1U
#define PORT_RESISTOR_PULLDOWN 0x0U
#define PORT_RESISTOR_PULLUP   0x1U

#define PORT_MUX_DISABLE 0x0U
#define PORT_MUX_GPIO    0x1U

#define PORT_INTERRUPT_DISABLE   0x0U
#define PORT_INTERRUPT_LOGIC_0   0x8U
#define PORT_INTERRUPT_RISE_EDGE 0x9U
#define PORT_INTERRUPT_FALL_EDGE 0xAU
#define PORT_INTERRUPT_BOTH_EDGE 0xBU
#define PORT_INTERRUPT_LOGIC_1   0xCU

/* Types */
typedef enum {
    PORT_STATUS_SUCCESS = 0x00U,
    PORT_STATUS_ERROR = 0x01U,
    PORT_STATUS_BUSY = 0x02U,
    PORT_STATUS_TIMEOUT = 0x03U,
    PORT_STATUS_INVALID_PARAM = 0x04U,
    PORT_STATUS_TRIGGER_TRUE = 0x05U,
    PORT_STATUS_TRIGGER_FALSE = 0x06U
} port_status_t;

typedef union {
    uint32_t value;
    struct {
        uint32_t PS        : 1; /** Pull Select */
        uint32_t PE        : 1; /** Pull Enable */
        uint32_t reserved0 : 6;
        uint32_t MUX       : 3; /** Pin Mux Control */
        uint32_t reserved1 : 5;
        uint32_t IRQC      : 4; /** Interrupt Configuration */
        uint32_t reserved2 : 12;
    } field;
} port_pin_config_t;

/* API Functions */
port_status_t PORT_Init(PORT_Type *port, uint8_t pin, port_pin_config_t *cfg);
port_status_t PORT_InterruptCheck(PORT_Type *port, uint8_t pin);
port_status_t PORT_InterruptClear(PORT_Type *port, uint8_t pin);

#endif /* PORT_H */