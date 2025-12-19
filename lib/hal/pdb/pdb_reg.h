/*
** ###################################################################
**     Processor:           S32K144
**     Reference manual:    S32K1XXRM Rev. 12.1, 02/2020
**     Version:             rev. 1.0, 2025-12-14
**
**     Abstract:
**         PDB (Programmable Delay Block) Register Definitions
**
**     Copyright (c) 2025
**     All rights reserved.
**
** ###################################################################
*/

/**
 * @file    pdb_reg.h
 * @brief   PDB Register Definitions for S32K144
 * @details This file contains register definitions and bit field macros for the PDB module.
 *          PDB provides precise timing for ADC trigger generation.
 * 
 * @author  PhucPH32
 * @date    14/12/2025
 * @version 1.0
 * 
 * @note    Refer to S32K1xx Reference Manual Chapter 47 (PDB) for detailed information
 * @warning Clock must be enabled via PCC before using PDB module
 * 
 * @par Change Log:
 * - Version 1.0 (Dec 14, 2025): Initial PDB register definitions
 */

#ifndef PDB_REG_H
#define PDB_REG_H

#include <stdint.h>
#include "def_reg.h"
/*******************************************************************************
 * PDB - Size of Registers Arrays
 ******************************************************************************/
#define PDB_CH_COUNT                             (2u)
#define PDB_DLY_COUNT                            (8u)

/*******************************************************************************
 * PDB - Register Layout Typedef
 ******************************************************************************/
/**
 * @brief PDB Module Register Structure
 * @details Complete register map for PDB module including:
 *          - SC: Status and Control register
 *          - MOD: Modulus register (counter period)
 *          - CNT: Counter register (read-only)
 *          - IDLY: Interrupt Delay register
 *          - CH[2]: Channel control and delay registers
 *          - POEN: Pulse-Out Enable register
 *          - PODLY: Pulse-Out Delay register
 */
typedef struct {
    __IO uint32_t SC;                                /**< Status and Control register, offset: 0x0 */
    __IO uint32_t MOD;                               /**< Modulus register, offset: 0x4 */
    __I  uint32_t CNT;                               /**< Counter register, offset: 0x8 */
    __IO uint32_t IDLY;                              /**< Interrupt Delay register, offset: 0xC */
    struct {                                         /* offset: 0x10, array step: 0x28 */
        __IO uint32_t C1;                            /**< Channel n Control register 1, array offset: 0x10, array step: 0x28 */
        __IO uint32_t S;                             /**< Channel n Status register, array offset: 0x14, array step: 0x28 */
        __IO uint32_t DLY[PDB_DLY_COUNT];            /**< Channel n Delay 0-7 registers, array offset: 0x18, array step: index*0x28, index2*0x4 */
    } CH[PDB_CH_COUNT];
         uint8_t RESERVED_0[304];
    __IO uint32_t POEN;                              /**< Pulse-Out n Enable register, offset: 0x190 */
    __IO uint32_t PODLY;                             /**< Pulse-Out n Delay register, offset: 0x194 */
} PDB_Type;

/** Number of instances of the PDB module */
#define PDB_INSTANCE_COUNT                       (2u)

/*******************************************************************************
 * PDB - Peripheral Instance Base Addresses
 ******************************************************************************/
/** Peripheral PDB0 base address */
#define PDB0_BASE                                (0x40036000u)
/** Peripheral PDB0 base pointer */
#define PDB0                                     ((PDB_Type *)PDB0_BASE)

/** Peripheral PDB1 base address */
#define PDB1_BASE                                (0x40031000u)
/** Peripheral PDB1 base pointer */
#define PDB1                                     ((PDB_Type *)PDB1_BASE)

/** Array initializer of PDB peripheral base addresses */
#define PDB_BASE_ADDRS                           { PDB0_BASE, PDB1_BASE }
/** Array initializer of PDB peripheral base pointers */
#define PDB_BASE_PTRS                            { PDB0, PDB1 }

/*******************************************************************************
 * PDB_SC - Status and Control Register
 ******************************************************************************/
#define PDB_SC_LDOK_MASK                         0x1u
#define PDB_SC_LDOK_SHIFT                        0u
#define PDB_SC_LDOK_WIDTH                        1u
#define PDB_SC_LDOK(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_SC_LDOK_SHIFT))&PDB_SC_LDOK_MASK)

#define PDB_SC_CONT_MASK                         0x2u
#define PDB_SC_CONT_SHIFT                        1u
#define PDB_SC_CONT_WIDTH                        1u
#define PDB_SC_CONT(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_SC_CONT_SHIFT))&PDB_SC_CONT_MASK)
#define PDB_SC_MULT_MASK                         0xCu
#define PDB_SC_MULT_SHIFT                        2u
#define PDB_SC_MULT_WIDTH                        2u
#define PDB_SC_MULT(x)                           (((uint32_t)(((uint32_t)(x))<<PDB_SC_MULT_SHIFT))&PDB_SC_MULT_MASK)
#define PDB_SC_PDBIE_MASK                        0x20u
#define PDB_SC_PDBIE_SHIFT                       5u
#define PDB_SC_PDBIE_WIDTH                       1u
#define PDB_SC_PDBIE(x)                          (((uint32_t)(((uint32_t)(x))<<PDB_SC_PDBIE_SHIFT))&PDB_SC_PDBIE_MASK)
#define PDB_SC_PDBIF_MASK                        0x40u
#define PDB_SC_PDBIF_SHIFT                       6u
#define PDB_SC_PDBIF_WIDTH                       1u
#define PDB_SC_PDBIF(x)                          (((uint32_t)(((uint32_t)(x))<<PDB_SC_PDBIF_SHIFT))&PDB_SC_PDBIF_MASK)
#define PDB_SC_PDBEN_MASK                        0x80u
#define PDB_SC_PDBEN_SHIFT                       7u
#define PDB_SC_PDBEN_WIDTH                       1u
#define PDB_SC_PDBEN(x)                          (((uint32_t)(((uint32_t)(x))<<PDB_SC_PDBEN_SHIFT))&PDB_SC_PDBEN_MASK)
#define PDB_SC_TRGSEL_MASK                       0xF00u
#define PDB_SC_TRGSEL_SHIFT                      8u
#define PDB_SC_TRGSEL_WIDTH                      4u
#define PDB_SC_TRGSEL(x)                         (((uint32_t)(((uint32_t)(x))<<PDB_SC_TRGSEL_SHIFT))&PDB_SC_TRGSEL_MASK)
#define PDB_SC_PRESCALER_MASK                    0x7000u
#define PDB_SC_PRESCALER_SHIFT                   12u
#define PDB_SC_PRESCALER_WIDTH                   3u
#define PDB_SC_PRESCALER(x)                      (((uint32_t)(((uint32_t)(x))<<PDB_SC_PRESCALER_SHIFT))&PDB_SC_PRESCALER_MASK)
#define PDB_SC_DMAEN_MASK                        0x8000u
#define PDB_SC_DMAEN_SHIFT                       15u
#define PDB_SC_DMAEN_WIDTH                       1u
#define PDB_SC_DMAEN(x)                          (((uint32_t)(((uint32_t)(x))<<PDB_SC_DMAEN_SHIFT))&PDB_SC_DMAEN_MASK)
#define PDB_SC_SWTRIG_MASK                       0x10000u
#define PDB_SC_SWTRIG_SHIFT                      16u
#define PDB_SC_SWTRIG_WIDTH                      1u
#define PDB_SC_SWTRIG(x)                         (((uint32_t)(((uint32_t)(x))<<PDB_SC_SWTRIG_SHIFT))&PDB_SC_SWTRIG_MASK)
#define PDB_SC_PDBEIE_MASK                       0x20000u
#define PDB_SC_PDBEIE_SHIFT                      17u
#define PDB_SC_PDBEIE_WIDTH                      1u
#define PDB_SC_PDBEIE(x)                         (((uint32_t)(((uint32_t)(x))<<PDB_SC_PDBEIE_SHIFT))&PDB_SC_PDBEIE_MASK)
#define PDB_SC_LDMOD_MASK                        0xC0000u
#define PDB_SC_LDMOD_SHIFT                       18u
#define PDB_SC_LDMOD_WIDTH                       2u
#define PDB_SC_LDMOD(x)                          (((uint32_t)(((uint32_t)(x))<<PDB_SC_LDMOD_SHIFT))&PDB_SC_LDMOD_MASK)

/*******************************************************************************
 * PDB_CH_C1 - Channel Control Register 1
 ******************************************************************************/
#define PDB_C1_EN_MASK                           0xFFu
#define PDB_C1_EN_SHIFT                          0u
#define PDB_C1_EN_WIDTH                          8u
#define PDB_C1_EN(x)                             (((uint32_t)(((uint32_t)(x))<<PDB_C1_EN_SHIFT))&PDB_C1_EN_MASK)
#define PDB_C1_TOS_MASK                          0xFF00u
#define PDB_C1_TOS_SHIFT                         8u
#define PDB_C1_TOS_WIDTH                         8u
#define PDB_C1_TOS(x)                            (((uint32_t)(((uint32_t)(x))<<PDB_C1_TOS_SHIFT))&PDB_C1_TOS_MASK)
#define PDB_C1_BB_MASK                           0xFF0000u
#define PDB_C1_BB_SHIFT                          16u
#define PDB_C1_BB_WIDTH                          8u
#define PDB_C1_BB(x)                             (((uint32_t)(((uint32_t)(x))<<PDB_C1_BB_SHIFT))&PDB_C1_BB_MASK)

/*******************************************************************************
 * PDB Register Indices
 ******************************************************************************/
#define PCC_PDB0_INDEX                          (36u)
#define PCC_PDB1_INDEX                          (37u)

#endif /* PDB_REG_H */
