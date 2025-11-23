/**
 * @file    gpio_reg.h
 * @brief   GPIO Register Definitions for S32K144
 * @details This file contains low-level GPIO register definitions and macros
 *          for direct hardware access.
 * 
 * @author  PhucPH32
 * @date    November 22, 2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for GPIO peripheral
 * @warning Direct register access - use with caution
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 22, 2025): Initial version
 * 
 */

#ifndef GPIO_REG_H
#define GPIO_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * GPIO Base Addresses
 ******************************************************************************/

/** @brief GPIO Port A base address */
#define PTA_BASE_ADDR           (0x400FF000UL)

/** @brief GPIO Port B base address */
#define PTB_BASE_ADDR           (0x400FF040UL)

/** @brief GPIO Port C base address */
#define PTC_BASE_ADDR           (0x400FF080UL)

/** @brief GPIO Port D base address */
#define PTD_BASE_ADDR           (0x400FF0C0UL)

/** @brief GPIO Port E base address */
#define PTE_BASE_ADDR           (0x400FF100UL)

/*******************************************************************************
 * GPIO Register Structure
 ******************************************************************************/

/**
 * @brief GPIO register structure definition
 */
typedef struct {
    volatile uint32_t PDOR;    /**< Offset: 0x00 - Port Data Output Register */
    volatile uint32_t PSOR;    /**< Offset: 0x04 - Port Set Output Register */
    volatile uint32_t PCOR;    /**< Offset: 0x08 - Port Clear Output Register */
    volatile uint32_t PTOR;    /**< Offset: 0x0C - Port Toggle Output Register */
    volatile uint32_t PDIR;    /**< Offset: 0x10 - Port Data Input Register */
    volatile uint32_t PDDR;    /**< Offset: 0x14 - Port Data Direction Register */
    volatile uint32_t PIDR;    /**< Offset: 0x18 - Port Input Disable Register */
} GPIO_RegType;

/*******************************************************************************
 * GPIO Register Pointers
 ******************************************************************************/

/** @brief GPIO Port A register pointer */
#define PTA                     ((GPIO_RegType *)PTA_BASE_ADDR)

/** @brief GPIO Port B register pointer */
#define PTB                     ((GPIO_RegType *)PTB_BASE_ADDR)

/** @brief GPIO Port C register pointer */
#define PTC                     ((GPIO_RegType *)PTC_BASE_ADDR)

/** @brief GPIO Port D register pointer */
#define PTD                     ((GPIO_RegType *)PTD_BASE_ADDR)

/** @brief GPIO Port E register pointer */
#define PTE                     ((GPIO_RegType *)PTE_BASE_ADDR)

/*******************************************************************************
 * GPIO Register Bit Definitions
 ******************************************************************************/

/* PDOR - Port Data Output Register */
/** @brief Port Data Output bit mask */
#define GPIO_PDOR_PDO(x)        (((uint32_t)(x)) & 0xFFFFFFFFU)

/* PSOR - Port Set Output Register */
/** @brief Port Set Output bit mask */
#define GPIO_PSOR_PTSO(x)       (((uint32_t)(x)) & 0xFFFFFFFFU)

/* PCOR - Port Clear Output Register */
/** @brief Port Clear Output bit mask */
#define GPIO_PCOR_PTCO(x)       (((uint32_t)(x)) & 0xFFFFFFFFU)

/* PTOR - Port Toggle Output Register */
/** @brief Port Toggle Output bit mask */
#define GPIO_PTOR_PTTO(x)       (((uint32_t)(x)) & 0xFFFFFFFFU)

/* PDIR - Port Data Input Register */
/** @brief Port Data Input bit mask */
#define GPIO_PDIR_PDI(x)        (((uint32_t)(x)) & 0xFFFFFFFFU)

/* PDDR - Port Data Direction Register */
/** @brief Port Data Direction bit mask - 0: input, 1: output */
#define GPIO_PDDR_PDD(x)        (((uint32_t)(x)) & 0xFFFFFFFFU)

/* PIDR - Port Input Disable Register */
/** @brief Port Input Disable bit mask - 0: enabled, 1: disabled */
#define GPIO_PIDR_PID(x)        (((uint32_t)(x)) & 0xFFFFFFFFU)

/*******************************************************************************
 * GPIO Helper Macros
 ******************************************************************************/

/**
 * @brief Create pin mask for single pin
 * @param pin Pin number (0-31)
 */
#define GPIO_PIN(pin)           (1U << (pin))

/**
 * @brief Set pin as output
 * @param base GPIO base pointer
 * @param pin Pin number (0-31)
 */
#define GPIO_SET_OUTPUT(base, pin)      ((base)->PDDR |= GPIO_PIN(pin))

/**
 * @brief Set pin as input
 * @param base GPIO base pointer
 * @param pin Pin number (0-31)
 */
#define GPIO_SET_INPUT(base, pin)       ((base)->PDDR &= ~GPIO_PIN(pin))

/**
 * @brief Set pin high
 * @param base GPIO base pointer
 * @param pin Pin number (0-31)
 */
#define GPIO_SET_HIGH(base, pin)        ((base)->PSOR = GPIO_PIN(pin))

/**
 * @brief Set pin low
 * @param base GPIO base pointer
 * @param pin Pin number (0-31)
 */
#define GPIO_SET_LOW(base, pin)         ((base)->PCOR = GPIO_PIN(pin))

/**
 * @brief Toggle pin
 * @param base GPIO base pointer
 * @param pin Pin number (0-31)
 */
#define GPIO_TOGGLE(base, pin)          ((base)->PTOR = GPIO_PIN(pin))

/**
 * @brief Read pin input value
 * @param base GPIO base pointer
 * @param pin Pin number (0-31)
 * @return Pin value (0 or 1)
 */
#define GPIO_READ_PIN(base, pin)        (((base)->PDIR >> (pin)) & 0x1U)

/**
 * @brief Write pin output value
 * @param base GPIO base pointer
 * @param pin Pin number (0-31)
 * @param value Value to write (0 or 1)
 */
#define GPIO_WRITE_PIN(base, pin, value) \
    do { \
        if (value) { \
            GPIO_SET_HIGH(base, pin); \
        } else { \
            GPIO_SET_LOW(base, pin); \
        } \
    } while(0)

/**
 * @brief Get port data direction register value
 * @param base GPIO base pointer
 * @return PDDR register value
 */
#define GPIO_GET_DIRECTION(base)        ((base)->PDDR)

/**
 * @brief Set multiple pins direction
 * @param base GPIO base pointer
 * @param mask Pin mask
 * @param dir Direction (0: input, 1: output)
 */
#define GPIO_SET_PINS_DIRECTION(base, mask, dir) \
    do { \
        if (dir) { \
            (base)->PDDR |= (mask); \
        } else { \
            (base)->PDDR &= ~(mask); \
        } \
    } while(0)

/**
 * @brief Set multiple pins output value
 * @param base GPIO base pointer
 * @param mask Pin mask
 */
#define GPIO_SET_PINS(base, mask)       ((base)->PSOR = (mask))

/**
 * @brief Clear multiple pins output value
 * @param base GPIO base pointer
 * @param mask Pin mask
 */
#define GPIO_CLEAR_PINS(base, mask)     ((base)->PCOR = (mask))

/**
 * @brief Toggle multiple pins
 * @param base GPIO base pointer
 * @param mask Pin mask
 */
#define GPIO_TOGGLE_PINS(base, mask)    ((base)->PTOR = (mask))

/**
 * @brief Read port input data
 * @param base GPIO base pointer
 * @return PDIR register value
 */
#define GPIO_READ_PORT(base)            ((base)->PDIR)

/**
 * @brief Write port output data
 * @param base GPIO base pointer
 * @param value Value to write
 */
#define GPIO_WRITE_PORT(base, value)    ((base)->PDOR = (value))

#endif /* GPIO_REG_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
