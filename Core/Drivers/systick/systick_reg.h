/**
 * @file    systick_reg.h
 * @brief   SysTick Timer Register Definitions for ARM Cortex-M4
 * @author  PhucPH32
 * @date    November 24, 2025
 */

#ifndef SYSTICK_REG_H
#define SYSTICK_REG_H

#include <stdint.h>
#include "def_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief SysTick Register Structure
 */
typedef struct {
    __IO uint32_t CSR;      /**< Control and Status Register, offset: 0x00 */
    __IO uint32_t RVR;      /**< Reload Value Register, offset: 0x04 */
    __IO uint32_t CVR;      /**< Current Value Register, offset: 0x08 */
    __I  uint32_t CALIB;    /**< Calibration Value Register, offset: 0x0C */
} SysTick_RegType;

/** SysTick base address */
#define SYSTICK_BASE    (0xE000E010u)
#define SYSTICK         ((SysTick_RegType *)SYSTICK_BASE)

/*******************************************************************************
 * CSR - Control and Status Register
 ******************************************************************************/

#define SYSTICK_CSR_ENABLE_SHIFT        (0u)
#define SYSTICK_CSR_ENABLE_MASK         (0x1u << SYSTICK_CSR_ENABLE_SHIFT)
#define SYSTICK_CSR_ENABLE_WIDTH        (1u)
#define SYSTICK_CSR_ENABLE(x)           (((uint32_t)(x) << SYSTICK_CSR_ENABLE_SHIFT) & SYSTICK_CSR_ENABLE_MASK)

#define SYSTICK_CSR_TICKINT_SHIFT       (1u)
#define SYSTICK_CSR_TICKINT_MASK        (0x1u << SYSTICK_CSR_TICKINT_SHIFT)
#define SYSTICK_CSR_TICKINT_WIDTH       (1u)
#define SYSTICK_CSR_TICKINT(x)          (((uint32_t)(x) << SYSTICK_CSR_TICKINT_SHIFT) & SYSTICK_CSR_TICKINT_MASK)

#define SYSTICK_CSR_CLKSOURCE_SHIFT     (2u)
#define SYSTICK_CSR_CLKSOURCE_MASK      (0x1u << SYSTICK_CSR_CLKSOURCE_SHIFT)
#define SYSTICK_CSR_CLKSOURCE_WIDTH     (1u)
#define SYSTICK_CSR_CLKSOURCE(x)        (((uint32_t)(x) << SYSTICK_CSR_CLKSOURCE_SHIFT) & SYSTICK_CSR_CLKSOURCE_MASK)

#define SYSTICK_CSR_COUNTFLAG_SHIFT     (16u)
#define SYSTICK_CSR_COUNTFLAG_MASK      (0x1u << SYSTICK_CSR_COUNTFLAG_SHIFT)
#define SYSTICK_CSR_COUNTFLAG_WIDTH     (1u)

/*******************************************************************************
 * RVR - Reload Value Register
 ******************************************************************************/

#define SYSTICK_RVR_RELOAD_SHIFT        (0u)
#define SYSTICK_RVR_RELOAD_MASK         (0xFFFFFFu << SYSTICK_RVR_RELOAD_SHIFT)
#define SYSTICK_RVR_RELOAD_WIDTH        (24u)
#define SYSTICK_RVR_RELOAD(x)           (((uint32_t)(x) << SYSTICK_RVR_RELOAD_SHIFT) & SYSTICK_RVR_RELOAD_MASK)

/*******************************************************************************
 * CVR - Current Value Register
 ******************************************************************************/

#define SYSTICK_CVR_CURRENT_SHIFT       (0u)
#define SYSTICK_CVR_CURRENT_MASK        (0xFFFFFFu << SYSTICK_CVR_CURRENT_SHIFT)
#define SYSTICK_CVR_CURRENT_WIDTH       (24u)
#define SYSTICK_CVR_CURRENT(x)          (((uint32_t)(x) << SYSTICK_CVR_CURRENT_SHIFT) & SYSTICK_CVR_CURRENT_MASK)

/*******************************************************************************
 * Helper Macros
 ******************************************************************************/

/** Enable SysTick */
#define SYSTICK_ENABLE()        (SYSTICK->CSR |= SYSTICK_CSR_ENABLE_MASK)

/** Disable SysTick */
#define SYSTICK_DISABLE()       (SYSTICK->CSR &= ~SYSTICK_CSR_ENABLE_MASK)

/** Enable SysTick Interrupt */
#define SYSTICK_ENABLE_INT()    (SYSTICK->CSR |= SYSTICK_CSR_TICKINT_MASK)

/** Disable SysTick Interrupt */
#define SYSTICK_DISABLE_INT()   (SYSTICK->CSR &= ~SYSTICK_CSR_TICKINT_MASK)

/** Set processor clock as source */
#define SYSTICK_SET_CLK_PROCESSOR()     (SYSTICK->CSR |= SYSTICK_CSR_CLKSOURCE_MASK)

/** Set external clock as source */
#define SYSTICK_SET_CLK_EXTERNAL()      (SYSTICK->CSR &= ~SYSTICK_CSR_CLKSOURCE_MASK)

/** Check if counter reached 0 */
#define SYSTICK_CHECK_FLAG()    ((SYSTICK->CSR & SYSTICK_CSR_COUNTFLAG_MASK) != 0u)

#endif /* SYSTICK_REG_H */