/**
 * @file    gpio.c
 * @brief   GPIO Driver Implementation for S32K144
 * @details This file contains the implementation of GPIO driver functions
 *          for S32K144 microcontroller.
 * 
 * @author  PhucPH32
 * @date    November 22, 2025
 * @version 1.0
 * 
 * @note    This implementation provides register-level GPIO control
 * @warning Always enable PORT clock before using GPIO functions
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 22, 2025): Initial version
 * 
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "gpio.h"
#include "port.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief GPIO base addresses */
#define PTA_BASE    (0x400FF000UL)
#define PTB_BASE    (0x400FF040UL)
#define PTC_BASE    (0x400FF080UL)
#define PTD_BASE    (0x400FF0C0UL)
#define PTE_BASE    (0x400FF100UL)

/** @brief GPIO register structure */
typedef struct {
    volatile uint32_t PDOR;    /**< Port Data Output Register */
    volatile uint32_t PSOR;    /**< Port Set Output Register */
    volatile uint32_t PCOR;    /**< Port Clear Output Register */
    volatile uint32_t PTOR;    /**< Port Toggle Output Register */
    volatile uint32_t PDIR;    /**< Port Data Input Register */
    volatile uint32_t PDDR;    /**< Port Data Direction Register */
    volatile uint32_t PIDR;    /**< Port Input Disable Register */
} GPIO_Type;

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Array of GPIO base addresses */
static GPIO_Type * const s_gpioBases[] = {
    (GPIO_Type *)PTA_BASE,
    (GPIO_Type *)PTB_BASE,
    (GPIO_Type *)PTC_BASE,
    (GPIO_Type *)PTD_BASE,
    (GPIO_Type *)PTE_BASE
};

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Get GPIO base address
 * @param[in] port GPIO port number
 * @return Pointer to GPIO register structure
 */
static inline GPIO_Type* GPIO_GetBase(gpio_port_t port);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static inline GPIO_Type* GPIO_GetBase(gpio_port_t port)
{
    return s_gpioBases[port];
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

void GPIO_Init(gpio_port_t port, uint8_t pin, gpio_direction_t direction)
{
    /* Enable PORT clock */
    PORT_EnableClock((port_name_t)port);
    
    /* Configure pin as GPIO */
    PORT_SetPinMux((port_name_t)port, pin, PORT_MUX_GPIO);
    
    /* Set pin direction */
    GPIO_SetPinDirection(port, pin, direction);
}

void GPIO_WritePin(gpio_port_t port, uint8_t pin, gpio_level_t value)
{
    GPIO_Type *base = GPIO_GetBase(port);
    
    if (value == GPIO_LEVEL_HIGH) {
        base->PSOR = (1U << pin);  /* Set pin */
    } else {
        base->PCOR = (1U << pin);  /* Clear pin */
    }
}

gpio_level_t GPIO_ReadPin(gpio_port_t port, uint8_t pin)
{
    GPIO_Type *base = GPIO_GetBase(port);
    
    return (gpio_level_t)((base->PDIR >> pin) & 0x1U);
}

void GPIO_TogglePin(gpio_port_t port, uint8_t pin)
{
    GPIO_Type *base = GPIO_GetBase(port);
    
    base->PTOR = (1U << pin);
}

void GPIO_SetPinDirection(gpio_port_t port, uint8_t pin, gpio_direction_t direction)
{
    GPIO_Type *base = GPIO_GetBase(port);
    
    if (direction == GPIO_DIR_OUTPUT) {
        base->PDDR |= (1U << pin);   /* Set as output */
    } else {
        base->PDDR &= ~(1U << pin);  /* Set as input */
    }
}

/*******************************************************************************
 * EOF
 ******************************************************************************/