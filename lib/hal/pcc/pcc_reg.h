/**
 * @file    pcc_reg.h
 * @brief   PCC Register Definitions for S32K144
 * @details This file contains low-level PCC register definitions and macros
 *          for direct hardware access including peripheral clock control.
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for PCC peripheral
 * @warning Direct register access - use with caution
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 22, 2025): Initial version
 * 
 */

#ifndef PCC_REG_H
#define PCC_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include "def_reg.h"


/*******************************************************************************
 * PCC Base Addresses
 ******************************************************************************/

/** @brief PCC base address */
#define PCC_BASE_ADDR           (0x40065000UL)

/*******************************************************************************
 * PCC (Peripheral Clock Control)
 ******************************************************************************/

/** @brief PORT - Size of Registers Arrays */
#define PCC_PCCn_COUNT        (116u)  /**< Number of PCC registers per PCC */

/**
 * @brief PCC register structure definition
 */
typedef struct {
    __IO uint32_t PCCn[PCC_PCCn_COUNT];   /**< PCC register array */
} PCC_RegType;


/** @brief PCC register pointer */
#define PCC                     ((PCC_RegType *)PCC_BASE_ADDR)

/*******************************************************************************
 * ADC PCC Index Offsets (moved to pcc.h enum)
 ******************************************************************************/
/* Note: Peripheral indices are now defined in pcc.h as enum for type safety */


/**
 * @brief Peripheral index enumeration for easy access
 * @note These values match S32K144.h PCC indices to avoid conflicts
 * @warning Do not use enum if S32K144.h is included (use #defines instead)
 */
#ifndef S32K144_H_  /* Only define if S32K144.h is not included */
typedef enum {
PCC_FTFC_INDEX                           = 11,
PCC_DMAMUX_INDEX                         = 11,
PCC_FlexCAN0_INDEX                       = 11,
PCC_FlexCAN1_INDEX                       = 11,
PCC_FTM3_INDEX                           = 11,
PCC_ADC1_INDEX                           = 11,
PCC_FlexCAN2_INDEX                       = 11,
PCC_LPSPI0_INDEX                         = 11,
PCC_LPSPI1_INDEX                         = 11,
PCC_LPSPI2_INDEX                         = 11,
PCC_PDB1_INDEX                           = 11,
PCC_CRC_INDEX                            = 11,
PCC_PDB0_INDEX                           = 11,
PCC_LPIT_INDEX                           = 11,
PCC_FTM0_INDEX                           = 11,
PCC_FTM1_INDEX                           = 11,
PCC_FTM2_INDEX                           = 11,
PCC_ADC0_INDEX                           = 11,
PCC_RTC_INDEX                            = 11,
PCC_LPTMR0_INDEX                         = 11,
PCC_PORTA_INDEX                          = 11,
PCC_PORTB_INDEX                          = 11,
PCC_PORTC_INDEX                          = 11,
PCC_PORTD_INDEX                          = 11,
PCC_PORTE_INDEX                          = 11,
PCC_FlexIO_INDEX                         = 11,
PCC_EWM_INDEX                            = 11,
PCC_LPI2C0_INDEX                         = 112,
PCC_LPUART0_INDEX                        = 116,
PCC_LPUART1_INDEX                        = 117,
PCC_LPUART2_INDEX                        = 118,
PCC_CMP0_INDEX                           = 115
} pcc_peripheral_index_t;
#endif /* S32K144_H_ */





/* ----------------------------------------------------------------------------
   -- PCC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup PCC_Register_Masks PCC Register Masks
 * @{
 */

/* PCCn Bit Fields */
#define PCC_PCCn_PCD_MASK                        0x7u
#define PCC_PCCn_PCD_SHIFT                       0u
#define PCC_PCCn_PCD_WIDTH                       3u
#define PCC_PCCn_PCD(x)                          (((uint32_t)(((uint32_t)(x))<<PCC_PCCn_PCD_SHIFT))&PCC_PCCn_PCD_MASK)
#define PCC_PCCn_FRAC_MASK                       0x8u
#define PCC_PCCn_FRAC_SHIFT                      3u
#define PCC_PCCn_FRAC_WIDTH                      1u
#define PCC_PCCn_FRAC(x)                         (((uint32_t)(((uint32_t)(x))<<PCC_PCCn_FRAC_SHIFT))&PCC_PCCn_FRAC_MASK)
#define PCC_PCCn_PCS_MASK                        0x7000000u
#define PCC_PCCn_PCS_SHIFT                       24u
#define PCC_PCCn_PCS_WIDTH                       3u
#define PCC_PCCn_PCS(x)                          (((uint32_t)(((uint32_t)(x))<<PCC_PCCn_PCS_SHIFT))&PCC_PCCn_PCS_MASK)
#define PCC_PCCn_CGC_MASK                        0x40000000u
#define PCC_PCCn_CGC_SHIFT                       30u
#define PCC_PCCn_CGC_WIDTH                       1u
#define PCC_PCCn_CGC(x)                          (((uint32_t)(((uint32_t)(x))<<PCC_PCCn_CGC_SHIFT))&PCC_PCCn_CGC_MASK)
#define PCC_PCCn_PR_MASK                         0x80000000u
#define PCC_PCCn_PR_SHIFT                        31u
#define PCC_PCCn_PR_WIDTH                        1u
#define PCC_PCCn_PR(x)                           (((uint32_t)(((uint32_t)(x))<<PCC_PCCn_PR_SHIFT))&PCC_PCCn_PR_MASK)


#endif /* PCC_REG_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
