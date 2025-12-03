/*
 * @file    scg_reg.h
 * @brief   SCG Register Definitions for S32K144
 */

#ifndef SCG_REG_H_
#define SCG_REG_H_

#include <stdint.h>

/* IO definitions (access restrictions to peripheral registers) */
/**
*   IO Type Qualifiers are used
*   \li to specify the access to peripheral variables.
*   \li for automatic generation of peripheral register debug information.
*/
#ifndef __IO
	#define   __I     volatile const       /*!< Defines 'read only' permissions                 */
	#define     __O     volatile             /*!< Defines 'write only' permissions                */
	#define     __IO    volatile             /*!< Defines 'read / write' permissions              */
#endif

/*!
 * @addtogroup SCG_Peripheral_Access_Layer SCG Peripheral Access Layer
 * @{
 */

/** SCG - Register Layout Typedef */
typedef struct {
  __I  uint32_t VERID;                             /**< Version ID Register, offset: 0x0 */
  __I  uint32_t PARAM;                             /**< Parameter Register, offset: 0x4 */
  uint8_t RESERVED_0[8];
  __I  uint32_t CSR;                               /**< Clock Status Register, offset: 0x10 */
  __IO uint32_t RCCR;                              /**< Run Clock Control Register, offset: 0x14 */
  __IO uint32_t VCCR;                              /**< VLPR Clock Control Register, offset: 0x18 */
  __IO uint32_t HCCR;                              /**< HSRUN Clock Control Register, offset: 0x1C */
  __IO uint32_t CLKOUTCNFG;                        /**< SCG CLKOUT Configuration Register, offset: 0x20 */
  uint8_t RESERVED_1[220];
  __IO uint32_t SOSCCSR;                           /**< System OSC Control Status Register, offset: 0x100 */
  __IO uint32_t SOSCDIV;                           /**< System OSC Divide Register, offset: 0x104 */
  __IO uint32_t SOSCCFG;                           /**< System Oscillator Configuration Register, offset: 0x108 */
  uint8_t RESERVED_2[244];
  __IO uint32_t SIRCCSR;                           /**< Slow IRC Control Status Register, offset: 0x200 */
  __IO uint32_t SIRCDIV;                           /**< Slow IRC Divide Register, offset: 0x204 */
  __IO uint32_t SIRCCFG;                           /**< Slow IRC Configuration Register, offset: 0x208 */
  uint8_t RESERVED_3[244];
  __IO uint32_t FIRCCSR;                           /**< Fast IRC Control Status Register, offset: 0x300 */
  __IO uint32_t FIRCDIV;                           /**< Fast IRC Divide Register, offset: 0x304 */
  __IO uint32_t FIRCCFG;                           /**< Fast IRC Configuration Register, offset: 0x308 */
  uint8_t RESERVED_4[756];
  __IO uint32_t SPLLCSR;                           /**< System PLL Control Status Register, offset: 0x600 */
  __IO uint32_t SPLLDIV;                           /**< System PLL Divide Register, offset: 0x604 */
  __IO uint32_t SPLLCFG;                           /**< System PLL Configuration Register, offset: 0x608 */
} SCG_Type, *SCG_MemMapPtr;

/** Number of instances of the SCG module. */
#define SCG_INSTANCE_COUNT                       (1u)

/* SCG - Peripheral instance base addresses */
/** Peripheral SCG base address */
#define SCG_BASE                              (0x40064000u)
/** Peripheral SCG base pointer */
#define SCG                                   ((SCG_Type *)SCG_BASE)
/** Array initializer of SCG peripheral base addresses */
#define SCG_BASE_ADDRS                        { SCG_BASE }
/** Array initializer of SCG peripheral base pointers */
#define SCG_BASE_PTRS                         { SCG }

/* ----------------------------------------------------------------------------
   -- SCG Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup SCG_Register_Masks SCG Register Masks
 * @{
 */

/*! @name CSR - Clock Status Register */
/*! @{ */

#define SCG_CSR_DIVSLOW_MASK                     (0xFU)
#define SCG_CSR_DIVSLOW_SHIFT                    (0U)
#define SCG_CSR_DIVSLOW_WIDTH                    (4U)
#define SCG_CSR_DIVSLOW(x)                       (((uint32_t)(((uint32_t)(x)) << SCG_CSR_DIVSLOW_SHIFT)) & SCG_CSR_DIVSLOW_MASK)

#define SCG_CSR_DIVBUS_MASK                      (0xF0U)
#define SCG_CSR_DIVBUS_SHIFT                     (4U)
#define SCG_CSR_DIVBUS_WIDTH                     (4U)
#define SCG_CSR_DIVBUS(x)                        (((uint32_t)(((uint32_t)(x)) << SCG_CSR_DIVBUS_SHIFT)) & SCG_CSR_DIVBUS_MASK)

#define SCG_CSR_DIVCORE_MASK                     (0xF0000U)
#define SCG_CSR_DIVCORE_SHIFT                    (16U)
#define SCG_CSR_DIVCORE_WIDTH                    (4U)
#define SCG_CSR_DIVCORE(x)                       (((uint32_t)(((uint32_t)(x)) << SCG_CSR_DIVCORE_SHIFT)) & SCG_CSR_DIVCORE_MASK)

#define SCG_CSR_SCS_MASK                         (0xF000000U)
#define SCG_CSR_SCS_SHIFT                        (24U)
#define SCG_CSR_SCS_WIDTH                        (4U)
#define SCG_CSR_SCS(x)                           (((uint32_t)(((uint32_t)(x)) << SCG_CSR_SCS_SHIFT)) & SCG_CSR_SCS_MASK)
/*! @} */

/*! @name RCCR - Run Clock Control Register */
/*! @{ */

#define SCG_RCCR_DIVSLOW_MASK                    (0xFU)
#define SCG_RCCR_DIVSLOW_SHIFT                   (0U)
#define SCG_RCCR_DIVSLOW_WIDTH                   (4U)
#define SCG_RCCR_DIVSLOW(x)                      (((uint32_t)(((uint32_t)(x)) << SCG_RCCR_DIVSLOW_SHIFT)) & SCG_RCCR_DIVSLOW_MASK)

#define SCG_RCCR_DIVBUS_MASK                     (0xF0U)
#define SCG_RCCR_DIVBUS_SHIFT                    (4U)
#define SCG_RCCR_DIVBUS_WIDTH                    (4U)
#define SCG_RCCR_DIVBUS(x)                       (((uint32_t)(((uint32_t)(x)) << SCG_RCCR_DIVBUS_SHIFT)) & SCG_RCCR_DIVBUS_MASK)

#define SCG_RCCR_DIVCORE_MASK                    (0xF0000U)
#define SCG_RCCR_DIVCORE_SHIFT                   (16U)
#define SCG_RCCR_DIVCORE_WIDTH                   (4U)
#define SCG_RCCR_DIVCORE(x)                      (((uint32_t)(((uint32_t)(x)) << SCG_RCCR_DIVCORE_SHIFT)) & SCG_RCCR_DIVCORE_MASK)

#define SCG_RCCR_SCS_MASK                        (0xF000000U)
#define SCG_RCCR_SCS_SHIFT                       (24U)
#define SCG_RCCR_SCS_WIDTH                       (4U)
#define SCG_RCCR_SCS(x)                          (((uint32_t)(((uint32_t)(x)) << SCG_RCCR_SCS_SHIFT)) & SCG_RCCR_SCS_MASK)
/*! @} */

/*! @name SOSCCSR - System OSC Control Status Register */
/*! @{ */

#define SCG_SOSCCSR_SOSCEN_MASK                  (0x1U)
#define SCG_SOSCCSR_SOSCEN_SHIFT                 (0U)
#define SCG_SOSCCSR_SOSCEN_WIDTH                 (1U)
#define SCG_SOSCCSR_SOSCEN(x)                    (((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCEN_SHIFT)) & SCG_SOSCCSR_SOSCEN_MASK)

#define SCG_SOSCCSR_LK_MASK                      (0x800000U)
#define SCG_SOSCCSR_LK_SHIFT                     (23U)
#define SCG_SOSCCSR_LK_WIDTH                     (1U)
#define SCG_SOSCCSR_LK(x)                        (((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_LK_SHIFT)) & SCG_SOSCCSR_LK_MASK)

#define SCG_SOSCCSR_SOSCVLD_MASK                 (0x1000000U)
#define SCG_SOSCCSR_SOSCVLD_SHIFT                (24U)
#define SCG_SOSCCSR_SOSCVLD_WIDTH                (1U)
#define SCG_SOSCCSR_SOSCVLD(x)                   (((uint32_t)(((uint32_t)(x)) << SCG_SOSCCSR_SOSCVLD_SHIFT)) & SCG_SOSCCSR_SOSCVLD_MASK)
/*! @} */

/*! @name SOSCDIV - System OSC Divide Register */
/*! @{ */

#define SCG_SOSCDIV_SOSCDIV1_MASK                (0x7U)
#define SCG_SOSCDIV_SOSCDIV1_SHIFT               (0U)
#define SCG_SOSCDIV_SOSCDIV1_WIDTH               (3U)
#define SCG_SOSCDIV_SOSCDIV1(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_SOSCDIV_SOSCDIV1_SHIFT)) & SCG_SOSCDIV_SOSCDIV1_MASK)

#define SCG_SOSCDIV_SOSCDIV2_MASK                (0x700U)
#define SCG_SOSCDIV_SOSCDIV2_SHIFT               (8U)
#define SCG_SOSCDIV_SOSCDIV2_WIDTH               (3U)
#define SCG_SOSCDIV_SOSCDIV2(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_SOSCDIV_SOSCDIV2_SHIFT)) & SCG_SOSCDIV_SOSCDIV2_MASK)
/*! @} */

/*! @name SOSCCFG - System Oscillator Configuration Register */
/*! @{ */

#define SCG_SOSCCFG_EREFS_MASK                   (0x4U)
#define SCG_SOSCCFG_EREFS_SHIFT                  (2U)
#define SCG_SOSCCFG_EREFS_WIDTH                  (1U)
#define SCG_SOSCCFG_EREFS(x)                     (((uint32_t)(((uint32_t)(x)) << SCG_SOSCCFG_EREFS_SHIFT)) & SCG_SOSCCFG_EREFS_MASK)

#define SCG_SOSCCFG_HGO_MASK                     (0x8U)
#define SCG_SOSCCFG_HGO_SHIFT                    (3U)
#define SCG_SOSCCFG_HGO_WIDTH                    (1U)
#define SCG_SOSCCFG_HGO(x)                       (((uint32_t)(((uint32_t)(x)) << SCG_SOSCCFG_HGO_SHIFT)) & SCG_SOSCCFG_HGO_MASK)

#define SCG_SOSCCFG_RANGE_MASK                   (0x30U)
#define SCG_SOSCCFG_RANGE_SHIFT                  (4U)
#define SCG_SOSCCFG_RANGE_WIDTH                  (2U)
#define SCG_SOSCCFG_RANGE(x)                     (((uint32_t)(((uint32_t)(x)) << SCG_SOSCCFG_RANGE_SHIFT)) & SCG_SOSCCFG_RANGE_MASK)
/*! @} */

/*! @name SIRCCSR - Slow IRC Control Status Register */
/*! @{ */

#define SCG_SIRCCSR_SIRCEN_MASK                  (0x1U)
#define SCG_SIRCCSR_SIRCEN_SHIFT                 (0U)
#define SCG_SIRCCSR_SIRCEN_WIDTH                 (1U)
#define SCG_SIRCCSR_SIRCEN(x)                    (((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_SIRCEN_SHIFT)) & SCG_SIRCCSR_SIRCEN_MASK)

#define SCG_SIRCCSR_LK_MASK                      (0x800000U)
#define SCG_SIRCCSR_LK_SHIFT                     (23U)
#define SCG_SIRCCSR_LK_WIDTH                     (1U)
#define SCG_SIRCCSR_LK(x)                        (((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_LK_SHIFT)) & SCG_SIRCCSR_LK_MASK)

#define SCG_SIRCCSR_SIRCVLD_MASK                 (0x1000000U)
#define SCG_SIRCCSR_SIRCVLD_SHIFT                (24U)
#define SCG_SIRCCSR_SIRCVLD_WIDTH                (1U)
#define SCG_SIRCCSR_SIRCVLD(x)                   (((uint32_t)(((uint32_t)(x)) << SCG_SIRCCSR_SIRCVLD_SHIFT)) & SCG_SIRCCSR_SIRCVLD_MASK)
/*! @} */

/*! @name SIRCDIV - Slow IRC Divide Register */
/*! @{ */

#define SCG_SIRCDIV_SIRCDIV1_MASK                (0x7U)
#define SCG_SIRCDIV_SIRCDIV1_SHIFT               (0U)
#define SCG_SIRCDIV_SIRCDIV1_WIDTH               (3U)
#define SCG_SIRCDIV_SIRCDIV1(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_SIRCDIV_SIRCDIV1_SHIFT)) & SCG_SIRCDIV_SIRCDIV1_MASK)

#define SCG_SIRCDIV_SIRCDIV2_MASK                (0x700U)
#define SCG_SIRCDIV_SIRCDIV2_SHIFT               (8U)
#define SCG_SIRCDIV_SIRCDIV2_WIDTH               (3U)
#define SCG_SIRCDIV_SIRCDIV2(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_SIRCDIV_SIRCDIV2_SHIFT)) & SCG_SIRCDIV_SIRCDIV2_MASK)
/*! @} */

/*! @name FIRCCSR - Fast IRC Control Status Register */
/*! @{ */

#define SCG_FIRCCSR_FIRCEN_MASK                  (0x1U)
#define SCG_FIRCCSR_FIRCEN_SHIFT                 (0U)
#define SCG_FIRCCSR_FIRCEN_WIDTH                 (1U)
#define SCG_FIRCCSR_FIRCEN(x)                    (((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCEN_SHIFT)) & SCG_FIRCCSR_FIRCEN_MASK)

#define SCG_FIRCCSR_LK_MASK                      (0x800000U)
#define SCG_FIRCCSR_LK_SHIFT                     (23U)
#define SCG_FIRCCSR_LK_WIDTH                     (1U)
#define SCG_FIRCCSR_LK(x)                        (((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_LK_SHIFT)) & SCG_FIRCCSR_LK_MASK)

#define SCG_FIRCCSR_FIRCVLD_MASK                 (0x1000000U)
#define SCG_FIRCCSR_FIRCVLD_SHIFT                (24U)
#define SCG_FIRCCSR_FIRCVLD_WIDTH                (1U)
#define SCG_FIRCCSR_FIRCVLD(x)                   (((uint32_t)(((uint32_t)(x)) << SCG_FIRCCSR_FIRCVLD_SHIFT)) & SCG_FIRCCSR_FIRCVLD_MASK)
/*! @} */

/*! @name FIRCDIV - Fast IRC Divide Register */
/*! @{ */

#define SCG_FIRCDIV_FIRCDIV1_MASK                (0x7U)
#define SCG_FIRCDIV_FIRCDIV1_SHIFT               (0U)
#define SCG_FIRCDIV_FIRCDIV1_WIDTH               (3U)
#define SCG_FIRCDIV_FIRCDIV1(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_FIRCDIV_FIRCDIV1_SHIFT)) & SCG_FIRCDIV_FIRCDIV1_MASK)

#define SCG_FIRCDIV_FIRCDIV2_MASK                (0x700U)
#define SCG_FIRCDIV_FIRCDIV2_SHIFT               (8U)
#define SCG_FIRCDIV_FIRCDIV2_WIDTH               (3U)
#define SCG_FIRCDIV_FIRCDIV2(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_FIRCDIV_FIRCDIV2_SHIFT)) & SCG_FIRCDIV_FIRCDIV2_MASK)
/*! @} */

/*! @name SPLLCSR - System PLL Control Status Register */
/*! @{ */

#define SCG_SPLLCSR_SPLLEN_MASK                  (0x1U)
#define SCG_SPLLCSR_SPLLEN_SHIFT                 (0U)
#define SCG_SPLLCSR_SPLLEN_WIDTH                 (1U)
#define SCG_SPLLCSR_SPLLEN(x)                    (((uint32_t)(((uint32_t)(x)) << SCG_SPLLCSR_SPLLEN_SHIFT)) & SCG_SPLLCSR_SPLLEN_MASK)

#define SCG_SPLLCSR_LK_MASK                      (0x800000U)
#define SCG_SPLLCSR_LK_SHIFT                     (23U)
#define SCG_SPLLCSR_LK_WIDTH                     (1U)
#define SCG_SPLLCSR_LK(x)                        (((uint32_t)(((uint32_t)(x)) << SCG_SPLLCSR_LK_SHIFT)) & SCG_SPLLCSR_LK_MASK)

#define SCG_SPLLCSR_SPLLVLD_MASK                 (0x1000000U)
#define SCG_SPLLCSR_SPLLVLD_SHIFT                (24U)
#define SCG_SPLLCSR_SPLLVLD_WIDTH                (1U)
#define SCG_SPLLCSR_SPLLVLD(x)                   (((uint32_t)(((uint32_t)(x)) << SCG_SPLLCSR_SPLLVLD_SHIFT)) & SCG_SPLLCSR_SPLLVLD_MASK)
/*! @} */

/*! @name SPLLDIV - System PLL Divide Register */
/*! @{ */

#define SCG_SPLLDIV_SPLLDIV1_MASK                (0x7U)
#define SCG_SPLLDIV_SPLLDIV1_SHIFT               (0U)
#define SCG_SPLLDIV_SPLLDIV1_WIDTH               (3U)
#define SCG_SPLLDIV_SPLLDIV1(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_SPLLDIV_SPLLDIV1_SHIFT)) & SCG_SPLLDIV_SPLLDIV1_MASK)

#define SCG_SPLLDIV_SPLLDIV2_MASK                (0x700U)
#define SCG_SPLLDIV_SPLLDIV2_SHIFT               (8U)
#define SCG_SPLLDIV_SPLLDIV2_WIDTH               (3U)
#define SCG_SPLLDIV_SPLLDIV2(x)                  (((uint32_t)(((uint32_t)(x)) << SCG_SPLLDIV_SPLLDIV2_SHIFT)) & SCG_SPLLDIV_SPLLDIV2_MASK)
/*! @} */

/*! @name SPLLCFG - System PLL Configuration Register */
/*! @{ */

#define SCG_SPLLCFG_PREDIV_MASK                  (0x700U)
#define SCG_SPLLCFG_PREDIV_SHIFT                 (8U)
#define SCG_SPLLCFG_PREDIV_WIDTH                 (3U)
#define SCG_SPLLCFG_PREDIV(x)                    (((uint32_t)(((uint32_t)(x)) << SCG_SPLLCFG_PREDIV_SHIFT)) & SCG_SPLLCFG_PREDIV_MASK)

#define SCG_SPLLCFG_MULT_MASK                    (0x1F0000U)
#define SCG_SPLLCFG_MULT_SHIFT                   (16U)
#define SCG_SPLLCFG_MULT_WIDTH                   (5U)
#define SCG_SPLLCFG_MULT(x)                      (((uint32_t)(((uint32_t)(x)) << SCG_SPLLCFG_MULT_SHIFT)) & SCG_SPLLCFG_MULT_MASK)
/*! @} */

/*!
 * @}
 */ /* end of group SCG_Register_Masks */

/*!
 * @}
 */ /* end of group SCG_Peripheral_Access_Layer */


#endif /* SCG_REG_H_ */
