/**
 * @file    clock_reg.h
 * @brief   SCG (System Clock Generator) Register Definitions for S32K144
 * @details This file contains low-level SCG register definitions and macros
 *          for direct hardware access including system clock configuration.
 * 
 * @author  PhucPH32
 * @date    25/11/2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for SCG peripheral
 * @warning Direct register access - use with caution
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 25, 2025): Initial version
 * 
 */

#ifndef CLOCK_REG_H
#define CLOCK_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include "def_reg.h"

/*******************************************************************************
 * SCG Base Address
 ******************************************************************************/

/** @brief SCG base address */
#define SCG_BASE_ADDR           (0x40064000UL)

/*******************************************************************************
 * SCG Register Structure
 ******************************************************************************/

/**
 * @brief SCG register structure definition
 */
typedef struct {
    __I  uint32_t VERID;                    /**< Offset: 0x000 - Version ID Register */
    __I  uint32_t PARAM;                    /**< Offset: 0x004 - Parameter Register */
         uint32_t RESERVED_0[2];            /**< Reserved */
    __I  uint32_t CSR;                      /**< Offset: 0x010 - Clock Status Register */
    __IO uint32_t RCCR;                     /**< Offset: 0x014 - Run Clock Control Register */
    __IO uint32_t VCCR;                     /**< Offset: 0x018 - VLPR Clock Control Register */
    __IO uint32_t HCCR;                     /**< Offset: 0x01C - HSRUN Clock Control Register */
    __IO uint32_t CLKOUTCNFG;               /**< Offset: 0x020 - SCG CLKOUT Configuration Register */
         uint32_t RESERVED_1[55];           /**< Reserved */
    __IO uint32_t SOSCCSR;                  /**< Offset: 0x100 - System OSC Control Status Register */
    __IO uint32_t SOSCDIV;                  /**< Offset: 0x104 - System OSC Divide Register */
    __IO uint32_t SOSCCFG;                  /**< Offset: 0x108 - System OSC Configuration Register */
         uint32_t RESERVED_2[61];           /**< Reserved */
    __IO uint32_t SIRCCSR;                  /**< Offset: 0x200 - Slow IRC Control Status Register */
    __IO uint32_t SIRCDIV;                  /**< Offset: 0x204 - Slow IRC Divide Register */
    __IO uint32_t SIRCCFG;                  /**< Offset: 0x208 - Slow IRC Configuration Register */
         uint32_t RESERVED_3[61];           /**< Reserved */
    __IO uint32_t FIRCCSR;                  /**< Offset: 0x300 - Fast IRC Control Status Register */
    __IO uint32_t FIRCDIV;                  /**< Offset: 0x304 - Fast IRC Divide Register */
    __IO uint32_t FIRCCFG;                  /**< Offset: 0x308 - Fast IRC Configuration Register */
         uint32_t RESERVED_4[189];          /**< Reserved */
    __IO uint32_t SPLLCSR;                  /**< Offset: 0x600 - System PLL Control Status Register */
    __IO uint32_t SPLLDIV;                  /**< Offset: 0x604 - System PLL Divide Register */
    __IO uint32_t SPLLCFG;                  /**< Offset: 0x608 - System PLL Configuration Register */
} SCG_Type;

/** @brief SCG register pointer */
#define SCG                     ((SCG_Type *)SCG_BASE_ADDR)

/*******************************************************************************
 * SCG_CSR (Clock Status Register) Bit Definitions
 ******************************************************************************/

/* System Clock Source (SCS) */
#define SCG_CSR_SCS_MASK                         0xF000000u
#define SCG_CSR_SCS_SHIFT                        24u
#define SCG_CSR_SCS_WIDTH                        4u
#define SCG_CSR_SCS(x)                           (((uint32_t)(((uint32_t)(x))<<SCG_CSR_SCS_SHIFT))&SCG_CSR_SCS_MASK)

/* System Clock Source Values */
#define SCG_SYSTEM_CLOCK_SRC_NONE                (0x00U)  /**< No clock selected */
#define SCG_SYSTEM_CLOCK_SRC_SOSC                (0x01U)  /**< System OSC */
#define SCG_SYSTEM_CLOCK_SRC_SIRC                (0x02U)  /**< Slow IRC */
#define SCG_SYSTEM_CLOCK_SRC_FIRC                (0x03U)  /**< Fast IRC */
#define SCG_SYSTEM_CLOCK_SRC_SPLL                (0x06U)  /**< System PLL */

/*******************************************************************************
 * SCG_RCCR (Run Clock Control Register) Bit Definitions
 ******************************************************************************/

/* Divider Values */
#define SCG_RCCR_DIVSLOW_MASK                    0xFu
#define SCG_RCCR_DIVSLOW_SHIFT                   0u
#define SCG_RCCR_DIVSLOW(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_DIVSLOW_SHIFT))&SCG_RCCR_DIVSLOW_MASK)

#define SCG_RCCR_DIVBUS_MASK                     0xF0u
#define SCG_RCCR_DIVBUS_SHIFT                    4u
#define SCG_RCCR_DIVBUS(x)                       (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_DIVBUS_SHIFT))&SCG_RCCR_DIVBUS_MASK)

#define SCG_RCCR_DIVCORE_MASK                    0xF0000u
#define SCG_RCCR_DIVCORE_SHIFT                   16u
#define SCG_RCCR_DIVCORE(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_DIVCORE_SHIFT))&SCG_RCCR_DIVCORE_MASK)

#define SCG_RCCR_SCS_MASK                        0xF000000u
#define SCG_RCCR_SCS_SHIFT                       24u
#define SCG_RCCR_SCS(x)                          (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_SCS_SHIFT))&SCG_RCCR_SCS_MASK)

/*******************************************************************************
 * SOSC (System Oscillator) Control/Status Register Bit Definitions
 ******************************************************************************/

/* SOSC Enable (SOSCEN) */
#define SCG_SOSCCSR_SOSCEN_MASK                  0x1u
#define SCG_SOSCCSR_SOSCEN_SHIFT                 0u

/* SOSC Valid (SOSCVLD) */
#define SCG_SOSCCSR_SOSCVLD_MASK                 0x1000000u
#define SCG_SOSCCSR_SOSCVLD_SHIFT                24u

/* SOSC Lock (LK) */
#define SCG_SOSCCSR_LK_MASK                      0x800000u
#define SCG_SOSCCSR_LK_SHIFT                     23u

/*******************************************************************************
 * SIRC (Slow Internal Reference Clock) Control/Status Register
 ******************************************************************************/

/* SIRC Enable (SIRCEN) */
#define SCG_SIRCCSR_SIRCEN_MASK                  0x1u
#define SCG_SIRCCSR_SIRCEN_SHIFT                 0u

/* SIRC Valid (SIRCVLD) */
#define SCG_SIRCCSR_SIRCVLD_MASK                 0x1000000u
#define SCG_SIRCCSR_SIRCVLD_SHIFT                24u

/* SIRC Low Power Enable */
#define SCG_SIRCCSR_SIRCLPEN_MASK                0x2u
#define SCG_SIRCCSR_SIRCLPEN_SHIFT               1u

/* SIRC Stop Enable */
#define SCG_SIRCCSR_SIRCSTEN_MASK                0x4u
#define SCG_SIRCCSR_SIRCSTEN_SHIFT               2u

/*******************************************************************************
 * FIRC (Fast Internal Reference Clock) Control/Status Register
 ******************************************************************************/

/* FIRC Enable (FIRCEN) */
#define SCG_FIRCCSR_FIRCEN_MASK                  0x1u
#define SCG_FIRCCSR_FIRCEN_SHIFT                 0u

/* FIRC Valid (FIRCVLD) */
#define SCG_FIRCCSR_FIRCVLD_MASK                 0x1000000u
#define SCG_FIRCCSR_FIRCVLD_SHIFT                24u

/* FIRC Regulator Enable */
#define SCG_FIRCCSR_FIRCREGOFF_MASK              0x8u
#define SCG_FIRCCSR_FIRCREGOFF_SHIFT             3u

/*******************************************************************************
 * SPLL (System Phase-Locked Loop) Control/Status Register
 ******************************************************************************/

/* SPLL Enable (SPLLEN) */
#define SCG_SPLLCSR_SPLLEN_MASK                  0x1u
#define SCG_SPLLCSR_SPLLEN_SHIFT                 0u

/* SPLL Valid (SPLLVLD) */
#define SCG_SPLLCSR_SPLLVLD_MASK                 0x1000000u
#define SCG_SPLLCSR_SPLLVLD_SHIFT                24u

/* SPLL Lock (LK) */
#define SCG_SPLLCSR_LK_MASK                      0x800000u
#define SCG_SPLLCSR_LK_SHIFT                     23u

/*******************************************************************************
 * SCG Divide Register Bit Definitions
 ******************************************************************************/

/* Divider 1 */
#define SCG_CLKDIV_DIVIDER1_MASK                 0x7u
#define SCG_CLKDIV_DIVIDER1_SHIFT                0u

/* Divider 2 */
#define SCG_CLKDIV_DIVIDER2_MASK                 0x700u
#define SCG_CLKDIV_DIVIDER2_SHIFT                8u

/*******************************************************************************
 * Clock Divider Values
 ******************************************************************************/

#define SCG_CLOCK_DIVIDER_BY_1                   (0x00U)
#define SCG_CLOCK_DIVIDER_BY_2                   (0x01U)
#define SCG_CLOCK_DIVIDER_BY_3                   (0x02U)
#define SCG_CLOCK_DIVIDER_BY_4                   (0x03U)
#define SCG_CLOCK_DIVIDER_BY_5                   (0x04U)
#define SCG_CLOCK_DIVIDER_BY_6                   (0x05U)
#define SCG_CLOCK_DIVIDER_BY_7                   (0x06U)
#define SCG_CLOCK_DIVIDER_BY_8                   (0x07U)

/*******************************************************************************
 * SCG Helper Macros
 ******************************************************************************/

/**
 * @brief Check if clock source is valid
 */
#define SCG_IS_SOSC_VALID()     ((SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) != 0U)
#define SCG_IS_SIRC_VALID()     ((SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) != 0U)
#define SCG_IS_FIRC_VALID()     ((SCG->FIRCCSR & SCG_FIRCCSR_FIRCVLD_MASK) != 0U)
#define SCG_IS_SPLL_VALID()     ((SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) != 0U)

/**
 * @brief Get current system clock source
 */
#define SCG_GET_SYSTEM_CLOCK_SRC()  ((SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT)

#endif /* CLOCK_REG_H */
