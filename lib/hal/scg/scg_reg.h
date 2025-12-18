/**
 * @file    scg_reg.h
 * @brief   SCG (System Clock Generator) Register Definitions for S32K144
 * @details This file contains low-level SCG register definitions and macros
 *          for direct hardware access including system clock configuration.
 * 
 * @author  PhucPH32
 * @date    18/12/2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for SCG peripheral
 * @warning Direct register access - use with caution
 * 
 * @par Reference:
 * - S32K1XXRM Rev. 12.1, Chapter 28: System Clock Generator (SCG)
 * 
 */

#ifndef SCG_REG_H
#define SCG_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#ifndef __IO
#define __IO volatile
#endif

#ifndef __I
#define __I volatile const
#endif

#ifndef __O
#define __O volatile
#endif

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

/* Divider Status (DIVCORE, DIVBUS, DIVSLOW) */
#define SCG_CSR_DIVCORE_MASK                     0xF0000u
#define SCG_CSR_DIVCORE_SHIFT                    16u
#define SCG_CSR_DIVCORE_WIDTH                    4u

#define SCG_CSR_DIVBUS_MASK                      0xF00u
#define SCG_CSR_DIVBUS_SHIFT                     8u
#define SCG_CSR_DIVBUS_WIDTH                     4u

#define SCG_CSR_DIVSLOW_MASK                     0xFu
#define SCG_CSR_DIVSLOW_SHIFT                    0u
#define SCG_CSR_DIVSLOW_WIDTH                    4u

/*******************************************************************************
 * SCG_RCCR (Run Clock Control Register) Bit Definitions
 ******************************************************************************/

/* Divider Values */
#define SCG_RCCR_DIVSLOW_MASK                    0xFu
#define SCG_RCCR_DIVSLOW_SHIFT                   0u
#define SCG_RCCR_DIVSLOW_WIDTH                   4u
#define SCG_RCCR_DIVSLOW(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_DIVSLOW_SHIFT))&SCG_RCCR_DIVSLOW_MASK)

#define SCG_RCCR_DIVBUS_MASK                     0xF0u
#define SCG_RCCR_DIVBUS_SHIFT                    4u
#define SCG_RCCR_DIVBUS_WIDTH                    4u
#define SCG_RCCR_DIVBUS(x)                       (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_DIVBUS_SHIFT))&SCG_RCCR_DIVBUS_MASK)

#define SCG_RCCR_DIVCORE_MASK                    0xF0000u
#define SCG_RCCR_DIVCORE_SHIFT                   16u
#define SCG_RCCR_DIVCORE_WIDTH                   4u
#define SCG_RCCR_DIVCORE(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_DIVCORE_SHIFT))&SCG_RCCR_DIVCORE_MASK)

#define SCG_RCCR_SCS_MASK                        0xF000000u
#define SCG_RCCR_SCS_SHIFT                       24u
#define SCG_RCCR_SCS_WIDTH                       4u
#define SCG_RCCR_SCS(x)                          (((uint32_t)(((uint32_t)(x))<<SCG_RCCR_SCS_SHIFT))&SCG_RCCR_SCS_MASK)

/*******************************************************************************
 * SCG_VCCR (VLPR Clock Control Register) Bit Definitions
 ******************************************************************************/

#define SCG_VCCR_DIVSLOW_MASK                    0xFu
#define SCG_VCCR_DIVSLOW_SHIFT                   0u
#define SCG_VCCR_DIVSLOW_WIDTH                   4u
#define SCG_VCCR_DIVSLOW(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_VCCR_DIVSLOW_SHIFT))&SCG_VCCR_DIVSLOW_MASK)

#define SCG_VCCR_DIVBUS_MASK                     0xF0u
#define SCG_VCCR_DIVBUS_SHIFT                    4u
#define SCG_VCCR_DIVBUS_WIDTH                    4u
#define SCG_VCCR_DIVBUS(x)                       (((uint32_t)(((uint32_t)(x))<<SCG_VCCR_DIVBUS_SHIFT))&SCG_VCCR_DIVBUS_MASK)

#define SCG_VCCR_DIVCORE_MASK                    0xF0000u
#define SCG_VCCR_DIVCORE_SHIFT                   16u
#define SCG_VCCR_DIVCORE_WIDTH                   4u
#define SCG_VCCR_DIVCORE(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_VCCR_DIVCORE_SHIFT))&SCG_VCCR_DIVCORE_MASK)

#define SCG_VCCR_SCS_MASK                        0xF000000u
#define SCG_VCCR_SCS_SHIFT                       24u
#define SCG_VCCR_SCS_WIDTH                       4u
#define SCG_VCCR_SCS(x)                          (((uint32_t)(((uint32_t)(x))<<SCG_VCCR_SCS_SHIFT))&SCG_VCCR_SCS_MASK)

/*******************************************************************************
 * SCG_HCCR (HSRUN Clock Control Register) Bit Definitions
 ******************************************************************************/

#define SCG_HCCR_DIVSLOW_MASK                    0xFu
#define SCG_HCCR_DIVSLOW_SHIFT                   0u
#define SCG_HCCR_DIVSLOW_WIDTH                   4u
#define SCG_HCCR_DIVSLOW(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_HCCR_DIVSLOW_SHIFT))&SCG_HCCR_DIVSLOW_MASK)

#define SCG_HCCR_DIVBUS_MASK                     0xF0u
#define SCG_HCCR_DIVBUS_SHIFT                    4u
#define SCG_HCCR_DIVBUS_WIDTH                    4u
#define SCG_HCCR_DIVBUS(x)                       (((uint32_t)(((uint32_t)(x))<<SCG_HCCR_DIVBUS_SHIFT))&SCG_HCCR_DIVBUS_MASK)

#define SCG_HCCR_DIVCORE_MASK                    0xF0000u
#define SCG_HCCR_DIVCORE_SHIFT                   16u
#define SCG_HCCR_DIVCORE_WIDTH                   4u
#define SCG_HCCR_DIVCORE(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_HCCR_DIVCORE_SHIFT))&SCG_HCCR_DIVCORE_MASK)

#define SCG_HCCR_SCS_MASK                        0xF000000u
#define SCG_HCCR_SCS_SHIFT                       24u
#define SCG_HCCR_SCS_WIDTH                       4u
#define SCG_HCCR_SCS(x)                          (((uint32_t)(((uint32_t)(x))<<SCG_HCCR_SCS_SHIFT))&SCG_HCCR_SCS_MASK)

/*******************************************************************************
 * SOSC (System Oscillator) Control/Status Register Bit Definitions
 ******************************************************************************/

/* SOSC Enable (SOSCEN) */
#define SCG_SOSCCSR_SOSCEN_MASK                  0x1u
#define SCG_SOSCCSR_SOSCEN_SHIFT                 0u
#define SCG_SOSCCSR_SOSCEN_WIDTH                 1u
#define SCG_SOSCCSR_SOSCEN(x)                    (((uint32_t)(((uint32_t)(x))<<SCG_SOSCCSR_SOSCEN_SHIFT))&SCG_SOSCCSR_SOSCEN_MASK)

/* SOSC Stop Enable */
#define SCG_SOSCCSR_SOSCSTEN_MASK                0x2u
#define SCG_SOSCCSR_SOSCSTEN_SHIFT               1u
#define SCG_SOSCCSR_SOSCSTEN_WIDTH               1u

/* SOSC Low Power Enable */
#define SCG_SOSCCSR_SOSCLPEN_MASK                0x4u
#define SCG_SOSCCSR_SOSCLPEN_SHIFT               2u
#define SCG_SOSCCSR_SOSCLPEN_WIDTH               1u

/* SOSC Error */
#define SCG_SOSCCSR_SOSCERR_MASK                 0x4000000u
#define SCG_SOSCCSR_SOSCERR_SHIFT                26u
#define SCG_SOSCCSR_SOSCERR_WIDTH                1u

/* SOSC Valid (SOSCVLD) */
#define SCG_SOSCCSR_SOSCVLD_MASK                 0x1000000u
#define SCG_SOSCCSR_SOSCVLD_SHIFT                24u
#define SCG_SOSCCSR_SOSCVLD_WIDTH                1u

/* SOSC Selected */
#define SCG_SOSCCSR_SOSCSEL_MASK                 0x2000000u
#define SCG_SOSCCSR_SOSCSEL_SHIFT                25u
#define SCG_SOSCCSR_SOSCSEL_WIDTH                1u

/* SOSC Lock (LK) */
#define SCG_SOSCCSR_LK_MASK                      0x800000u
#define SCG_SOSCCSR_LK_SHIFT                     23u
#define SCG_SOSCCSR_LK_WIDTH                     1u

/*******************************************************************************
 * SOSC Divide Register Bit Definitions
 ******************************************************************************/

#define SCG_SOSCDIV_SOSCDIV1_MASK                0x7u
#define SCG_SOSCDIV_SOSCDIV1_SHIFT               0u
#define SCG_SOSCDIV_SOSCDIV1_WIDTH               3u
#define SCG_SOSCDIV_SOSCDIV1(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_SOSCDIV_SOSCDIV1_SHIFT))&SCG_SOSCDIV_SOSCDIV1_MASK)

#define SCG_SOSCDIV_SOSCDIV2_MASK                0x700u
#define SCG_SOSCDIV_SOSCDIV2_SHIFT               8u
#define SCG_SOSCDIV_SOSCDIV2_WIDTH               3u
#define SCG_SOSCDIV_SOSCDIV2(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_SOSCDIV_SOSCDIV2_SHIFT))&SCG_SOSCDIV_SOSCDIV2_MASK)

/*******************************************************************************
 * SOSC Configuration Register Bit Definitions
 ******************************************************************************/

/* SOSC External Reference Select */
#define SCG_SOSCCFG_EREFS_MASK                   0x4u
#define SCG_SOSCCFG_EREFS_SHIFT                  2u
#define SCG_SOSCCFG_EREFS_WIDTH                  1u
#define SCG_SOSCCFG_EREFS(x)                     (((uint32_t)(((uint32_t)(x))<<SCG_SOSCCFG_EREFS_SHIFT))&SCG_SOSCCFG_EREFS_MASK)

/* SOSC High Gain */
#define SCG_SOSCCFG_HGO_MASK                     0x8u
#define SCG_SOSCCFG_HGO_SHIFT                    3u
#define SCG_SOSCCFG_HGO_WIDTH                    1u
#define SCG_SOSCCFG_HGO(x)                       (((uint32_t)(((uint32_t)(x))<<SCG_SOSCCFG_HGO_SHIFT))&SCG_SOSCCFG_HGO_MASK)

/* SOSC Range Select */
#define SCG_SOSCCFG_RANGE_MASK                   0x30u
#define SCG_SOSCCFG_RANGE_SHIFT                  4u
#define SCG_SOSCCFG_RANGE_WIDTH                  2u
#define SCG_SOSCCFG_RANGE(x)                     (((uint32_t)(((uint32_t)(x))<<SCG_SOSCCFG_RANGE_SHIFT))&SCG_SOSCCFG_RANGE_MASK)

/*******************************************************************************
 * SIRC (Slow Internal Reference Clock) Control/Status Register
 ******************************************************************************/

/* SIRC Enable (SIRCEN) */
#define SCG_SIRCCSR_SIRCEN_MASK                  0x1u
#define SCG_SIRCCSR_SIRCEN_SHIFT                 0u
#define SCG_SIRCCSR_SIRCEN_WIDTH                 1u
#define SCG_SIRCCSR_SIRCEN(x)                    (((uint32_t)(((uint32_t)(x))<<SCG_SIRCCSR_SIRCEN_SHIFT))&SCG_SIRCCSR_SIRCEN_MASK)

/* SIRC Stop Enable */
#define SCG_SIRCCSR_SIRCSTEN_MASK                0x2u
#define SCG_SIRCCSR_SIRCSTEN_SHIFT               1u
#define SCG_SIRCCSR_SIRCSTEN_WIDTH               1u

/* SIRC Low Power Enable */
#define SCG_SIRCCSR_SIRCLPEN_MASK                0x4u
#define SCG_SIRCCSR_SIRCLPEN_SHIFT               2u
#define SCG_SIRCCSR_SIRCLPEN_WIDTH               1u

/* SIRC Lock */
#define SCG_SIRCCSR_LK_MASK                      0x800000u
#define SCG_SIRCCSR_LK_SHIFT                     23u
#define SCG_SIRCCSR_LK_WIDTH                     1u

/* SIRC Valid (SIRCVLD) */
#define SCG_SIRCCSR_SIRCVLD_MASK                 0x1000000u
#define SCG_SIRCCSR_SIRCVLD_SHIFT                24u
#define SCG_SIRCCSR_SIRCVLD_WIDTH                1u

/* SIRC Selected */
#define SCG_SIRCCSR_SIRCSEL_MASK                 0x2000000u
#define SCG_SIRCCSR_SIRCSEL_SHIFT                25u
#define SCG_SIRCCSR_SIRCSEL_WIDTH                1u

/*******************************************************************************
 * SIRC Divide Register Bit Definitions
 ******************************************************************************/

#define SCG_SIRCDIV_SIRCDIV1_MASK                0x7u
#define SCG_SIRCDIV_SIRCDIV1_SHIFT               0u
#define SCG_SIRCDIV_SIRCDIV1_WIDTH               3u
#define SCG_SIRCDIV_SIRCDIV1(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_SIRCDIV_SIRCDIV1_SHIFT))&SCG_SIRCDIV_SIRCDIV1_MASK)

#define SCG_SIRCDIV_SIRCDIV2_MASK                0x700u
#define SCG_SIRCDIV_SIRCDIV2_SHIFT               8u
#define SCG_SIRCDIV_SIRCDIV2_WIDTH               3u
#define SCG_SIRCDIV_SIRCDIV2(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_SIRCDIV_SIRCDIV2_SHIFT))&SCG_SIRCDIV_SIRCDIV2_MASK)

/*******************************************************************************
 * SIRC Configuration Register Bit Definitions
 ******************************************************************************/

/* SIRC Range */
#define SCG_SIRCCFG_RANGE_MASK                   0x1u
#define SCG_SIRCCFG_RANGE_SHIFT                  0u
#define SCG_SIRCCFG_RANGE_WIDTH                  1u
#define SCG_SIRCCFG_RANGE(x)                     (((uint32_t)(((uint32_t)(x))<<SCG_SIRCCFG_RANGE_SHIFT))&SCG_SIRCCFG_RANGE_MASK)

/*******************************************************************************
 * FIRC (Fast Internal Reference Clock) Control/Status Register
 ******************************************************************************/

/* FIRC Enable (FIRCEN) */
#define SCG_FIRCCSR_FIRCEN_MASK                  0x1u
#define SCG_FIRCCSR_FIRCEN_SHIFT                 0u
#define SCG_FIRCCSR_FIRCEN_WIDTH                 1u
#define SCG_FIRCCSR_FIRCEN(x)                    (((uint32_t)(((uint32_t)(x))<<SCG_FIRCCSR_FIRCEN_SHIFT))&SCG_FIRCCSR_FIRCEN_MASK)

/* FIRC Regulator Enable */
#define SCG_FIRCCSR_FIRCREGOFF_MASK              0x8u
#define SCG_FIRCCSR_FIRCREGOFF_SHIFT             3u
#define SCG_FIRCCSR_FIRCREGOFF_WIDTH             1u

/* FIRC Lock */
#define SCG_FIRCCSR_LK_MASK                      0x800000u
#define SCG_FIRCCSR_LK_SHIFT                     23u
#define SCG_FIRCCSR_LK_WIDTH                     1u

/* FIRC Valid (FIRCVLD) */
#define SCG_FIRCCSR_FIRCVLD_MASK                 0x1000000u
#define SCG_FIRCCSR_FIRCVLD_SHIFT                24u
#define SCG_FIRCCSR_FIRCVLD_WIDTH                1u

/* FIRC Selected */
#define SCG_FIRCCSR_FIRCSEL_MASK                 0x2000000u
#define SCG_FIRCCSR_FIRCSEL_SHIFT                25u
#define SCG_FIRCCSR_FIRCSEL_WIDTH                1u

/* FIRC Error */
#define SCG_FIRCCSR_FIRCERR_MASK                 0x4000000u
#define SCG_FIRCCSR_FIRCERR_SHIFT                26u
#define SCG_FIRCCSR_FIRCERR_WIDTH                1u

/*******************************************************************************
 * FIRC Divide Register Bit Definitions
 ******************************************************************************/

#define SCG_FIRCDIV_FIRCDIV1_MASK                0x7u
#define SCG_FIRCDIV_FIRCDIV1_SHIFT               0u
#define SCG_FIRCDIV_FIRCDIV1_WIDTH               3u
#define SCG_FIRCDIV_FIRCDIV1(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_FIRCDIV_FIRCDIV1_SHIFT))&SCG_FIRCDIV_FIRCDIV1_MASK)

#define SCG_FIRCDIV_FIRCDIV2_MASK                0x700u
#define SCG_FIRCDIV_FIRCDIV2_SHIFT               8u
#define SCG_FIRCDIV_FIRCDIV2_WIDTH               3u
#define SCG_FIRCDIV_FIRCDIV2(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_FIRCDIV_FIRCDIV2_SHIFT))&SCG_FIRCDIV_FIRCDIV2_MASK)

/*******************************************************************************
 * FIRC Configuration Register Bit Definitions
 ******************************************************************************/

/* FIRC Range */
#define SCG_FIRCCFG_RANGE_MASK                   0x3u
#define SCG_FIRCCFG_RANGE_SHIFT                  0u
#define SCG_FIRCCFG_RANGE_WIDTH                  2u
#define SCG_FIRCCFG_RANGE(x)                     (((uint32_t)(((uint32_t)(x))<<SCG_FIRCCFG_RANGE_SHIFT))&SCG_FIRCCFG_RANGE_MASK)

/*******************************************************************************
 * SPLL (System PLL) Control/Status Register Bit Definitions
 ******************************************************************************/

/* SPLL Enable (SPLLEN) */
#define SCG_SPLLCSR_SPLLEN_MASK                  0x1u
#define SCG_SPLLCSR_SPLLEN_SHIFT                 0u
#define SCG_SPLLCSR_SPLLEN_WIDTH                 1u
#define SCG_SPLLCSR_SPLLEN(x)                    (((uint32_t)(((uint32_t)(x))<<SCG_SPLLCSR_SPLLEN_SHIFT))&SCG_SPLLCSR_SPLLEN_MASK)

/* SPLL Clock Monitor */
#define SCG_SPLLCSR_SPLLCM_MASK                  0x10000u
#define SCG_SPLLCSR_SPLLCM_SHIFT                 16u
#define SCG_SPLLCSR_SPLLCM_WIDTH                 1u

/* SPLL Clock Monitor Reset Enable */
#define SCG_SPLLCSR_SPLLCMRE_MASK                0x20000u
#define SCG_SPLLCSR_SPLLCMRE_SHIFT               17u
#define SCG_SPLLCSR_SPLLCMRE_WIDTH               1u

/* SPLL Lock */
#define SCG_SPLLCSR_LK_MASK                      0x800000u
#define SCG_SPLLCSR_LK_SHIFT                     23u
#define SCG_SPLLCSR_LK_WIDTH                     1u

/* SPLL Valid (SPLLVLD) */
#define SCG_SPLLCSR_SPLLVLD_MASK                 0x1000000u
#define SCG_SPLLCSR_SPLLVLD_SHIFT                24u
#define SCG_SPLLCSR_SPLLVLD_WIDTH                1u

/* SPLL Selected */
#define SCG_SPLLCSR_SPLLSEL_MASK                 0x2000000u
#define SCG_SPLLCSR_SPLLSEL_SHIFT                25u
#define SCG_SPLLCSR_SPLLSEL_WIDTH                1u

/* SPLL Error */
#define SCG_SPLLCSR_SPLLERR_MASK                 0x4000000u
#define SCG_SPLLCSR_SPLLERR_SHIFT                26u
#define SCG_SPLLCSR_SPLLERR_WIDTH                1u

/*******************************************************************************
 * SPLL Divide Register Bit Definitions
 ******************************************************************************/

#define SCG_SPLLDIV_SPLLDIV1_MASK                0x7u
#define SCG_SPLLDIV_SPLLDIV1_SHIFT               0u
#define SCG_SPLLDIV_SPLLDIV1_WIDTH               3u
#define SCG_SPLLDIV_SPLLDIV1(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_SPLLDIV_SPLLDIV1_SHIFT))&SCG_SPLLDIV_SPLLDIV1_MASK)

#define SCG_SPLLDIV_SPLLDIV2_MASK                0x700u
#define SCG_SPLLDIV_SPLLDIV2_SHIFT               8u
#define SCG_SPLLDIV_SPLLDIV2_WIDTH               3u
#define SCG_SPLLDIV_SPLLDIV2(x)                  (((uint32_t)(((uint32_t)(x))<<SCG_SPLLDIV_SPLLDIV2_SHIFT))&SCG_SPLLDIV_SPLLDIV2_MASK)

/*******************************************************************************
 * SPLL Configuration Register Bit Definitions
 ******************************************************************************/

/* SPLL Pre-Divider */
#define SCG_SPLLCFG_PREDIV_MASK                  0x700u
#define SCG_SPLLCFG_PREDIV_SHIFT                 8u
#define SCG_SPLLCFG_PREDIV_WIDTH                 3u
#define SCG_SPLLCFG_PREDIV(x)                    (((uint32_t)(((uint32_t)(x))<<SCG_SPLLCFG_PREDIV_SHIFT))&SCG_SPLLCFG_PREDIV_MASK)

/* SPLL Multiplier */
#define SCG_SPLLCFG_MULT_MASK                    0x1F0000u
#define SCG_SPLLCFG_MULT_SHIFT                   16u
#define SCG_SPLLCFG_MULT_WIDTH                   5u
#define SCG_SPLLCFG_MULT(x)                      (((uint32_t)(((uint32_t)(x))<<SCG_SPLLCFG_MULT_SHIFT))&SCG_SPLLCFG_MULT_MASK)

/*******************************************************************************
 * CLKOUT Configuration Register Bit Definitions
 ******************************************************************************/

#define SCG_CLKOUTCNFG_CLKOUTSEL_MASK            0xF000000u
#define SCG_CLKOUTCNFG_CLKOUTSEL_SHIFT           24u
#define SCG_CLKOUTCNFG_CLKOUTSEL_WIDTH           4u
#define SCG_CLKOUTCNFG_CLKOUTSEL(x)              (((uint32_t)(((uint32_t)(x))<<SCG_CLKOUTCNFG_CLKOUTSEL_SHIFT))&SCG_CLKOUTCNFG_CLKOUTSEL_MASK)

#endif /* SCG_REG_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
