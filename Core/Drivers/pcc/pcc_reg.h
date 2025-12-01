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
 * ADC PCC Index Offsets
 ******************************************************************************/
#define PCC_ADC1_INDEX                           39
#define PCC_ADC0_INDEX                           59

/*******************************************************************************
 * PORT PCC Index Offsets
 ******************************************************************************/
/** @brief PCC PORT A base address */
#define PCC_PORTA_INDEX         (73U)

/** @brief PCC PORT B base address */
#define PCC_PORTB_INDEX         (74U)

/** @brief PCC PORT C base address */
#define PCC_PORTC_INDEX         (75U)

/** @brief PCC PORT D base address */
#define PCC_PORTD_INDEX         (76U)

/** @brief PCC PORT E base address */
#define PCC_PORTE_INDEX         (77U)

/*******************************************************************************
 * I2C PCC Index Offsets
 ******************************************************************************/
#define PCC_LPI2C0_INDEX                         102

/*******************************************************************************
 * UART PCC Index Offsets
 ******************************************************************************/
#define PCC_LPUART0_INDEX                        106
#define PCC_LPUART1_INDEX                        107
#define PCC_LPUART2_INDEX                        108

#define PCC_FLEXCAN0_INDEX                       36
#define PCC_FLEXCAN1_INDEX                       37
#define PCC_FLEXCAN2_INDEX                       43
#define PCC_FTM3_INDEX                           38
#define PCC_ADC1_INDEX                           39





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
