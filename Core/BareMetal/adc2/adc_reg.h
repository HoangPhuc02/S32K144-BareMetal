/**
 * @file    adc_reg.h
 * @brief   ADC Register Definitions for S32K144
 * @note    This file is identical to the original in adc folder
 */

#ifndef ADC_REG_H
#define ADC_REG_H

#include <stdint.h>

#define __I  volatile const /*!< Defines 'read only' permissions                 */
#define __O  volatile       /*!< Defines 'write only' permissions                */
#define __IO volatile       /*!< Defines 'read / write' permissions              */

/** ADC - Size of Registers Arrays */
#define ADC_SC1_COUNT 16u
#define ADC_R_COUNT   16u
#define ADC_CV_COUNT  2u

/* Base addresses */
#define ADC0_BASE (0x4003B000u) /** Peripheral ADC0 base address */
#define ADC1_BASE (0x40027000u) /** Peripheral ADC1 base address */

/* Register structure */
typedef struct {
    __IO uint32_t SC1[ADC_SC1_COUNT]; /**< ADC Status and Control Register 1, array offset: 0x0, array step: 0x4 */
    __IO uint32_t CFG1;               /**< ADC Configuration Register 1, offset: 0x40 */
    __IO uint32_t CFG2;               /**< ADC Configuration Register 2, offset: 0x44 */
    __I uint32_t R[ADC_R_COUNT];      /**< ADC Data Result Registers, array offset: 0x48, array step: 0x4 */
    __IO uint32_t CV[ADC_CV_COUNT];   /**< Compare Value Registers, array offset: 0x88, array step: 0x4 */
    __IO uint32_t SC2;                /**< Status and Control Register 2, offset: 0x90 */
    __IO uint32_t SC3;                /**< Status and Control Register 3, offset: 0x94 */
    __IO uint32_t BASE_OFS;           /**< BASE Offset Register, offset: 0x98 */
    __IO uint32_t OFS;                /**< ADC Offset Correction Register, offset: 0x9C */
    __IO uint32_t USR_OFS;            /**< USER Offset Correction Register, offset: 0xA0 */
    __IO uint32_t XOFS;               /**< ADC X Offset Correction Register, offset: 0xA4 */
    __IO uint32_t YOFS;               /**< ADC Y Offset Correction Register, offset: 0xA8 */
    __IO uint32_t G;                  /**< ADC Gain Register, offset: 0xAC */
    __IO uint32_t UG;                 /**< ADC User Gain Register, offset: 0xB0 */
    __IO uint32_t CLPS;               /**< ADC General Calibration Value Register S, offset: 0xB4 */
    __IO uint32_t CLP3;               /**< ADC Plus-Side General Calibration Value Register 3, offset: 0xB8 */
    __IO uint32_t CLP2;               /**< ADC Plus-Side General Calibration Value Register 2, offset: 0xBC */
    __IO uint32_t CLP1;               /**< ADC Plus-Side General Calibration Value Register 1, offset: 0xC0 */
    __IO uint32_t CLP0;               /**< ADC Plus-Side General Calibration Value Register 0, offset: 0xC4 */
    __IO uint32_t CLPX;               /**< ADC Plus-Side General Calibration Value Register X, offset: 0xC8 */
    __IO uint32_t CLP9;               /**< ADC Plus-Side General Calibration Value Register 9, offset: 0xCC */
    __IO uint32_t CLPS_OFS;           /**< ADC General Calibration Offset Value Register S, offset: 0xD0 */
    __IO uint32_t CLP3_OFS;           /**< ADC Plus-Side General Calibration Offset Value Register 3, offset: 0xD4 */
    __IO uint32_t CLP2_OFS;           /**< ADC Plus-Side General Calibration Offset Value Register 2, offset: 0xD8 */
    __IO uint32_t CLP1_OFS;           /**< ADC Plus-Side General Calibration Offset Value Register 1, offset: 0xDC */
    __IO uint32_t CLP0_OFS;           /**< ADC Plus-Side General Calibration Offset Value Register 0, offset: 0xE0 */
    __IO uint32_t CLPX_OFS;           /**< ADC Plus-Side General Calibration Offset Value Register X, offset: 0xE4 */
    __IO uint32_t CLP9_OFS;           /**< ADC Plus-Side General Calibration Offset Value Register 9, offset: 0xE8 */
    uint8_t RESERVED_0[24];
    __IO uint32_t CLMD;               /**< ADC Minus-Side General Calibration Value Register D, offset: 0x104 */
    __IO uint32_t CLMS;               /**< ADC Minus-Side General Calibration Value Register S, offset: 0x108 */
    __IO uint32_t CLM4;               /**< ADC Minus-Side General Calibration Value Register 4, offset: 0x10C */
    __IO uint32_t CLM3;               /**< ADC Minus-Side General Calibration Value Register 3, offset: 0x110 */
    __IO uint32_t CLM2;               /**< ADC Minus-Side General Calibration Value Register 2, offset: 0x114 */
    __IO uint32_t CLM1;               /**< ADC Minus-Side General Calibration Value Register 1, offset: 0x118 */
    __IO uint32_t CLM0;               /**< ADC Minus-Side General Calibration Value Register 0, offset: 0x11C */
    __IO uint32_t CLMX;               /**< ADC Minus-Side General Calibration Value Register X, offset: 0x120 */
    __IO uint32_t CLM9;               /**< ADC Minus-Side General Calibration Value Register 9, offset: 0x124 */
    __IO uint32_t CLMD_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register D, offset: 0x128 */
    __IO uint32_t CLMS_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register S, offset: 0x12C */
    __IO uint32_t CLM4_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register 4, offset: 0x130 */
    __IO uint32_t CLM3_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register 3, offset: 0x134 */
    __IO uint32_t CLM2_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register 2, offset: 0x138 */
    __IO uint32_t CLM1_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register 1, offset: 0x13C */
    __IO uint32_t CLM0_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register 0, offset: 0x140 */
    __IO uint32_t CLMX_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register X, offset: 0x144 */
    __IO uint32_t CLM9_OFS;           /**< ADC Minus-Side General Calibration Offset Value Register 9, offset: 0x148 */
    __IO uint32_t PG;                 /**< ADC Plus-Side Gain Register, offset: 0x14C */
    __IO uint32_t MG;                 /**< ADC Minus-Side Gain Register, offset: 0x150 */
} ADC_Type, *ADC_MemMapPtr;

/** Number of instances of the ADC module. */
#define ADC_INSTANCE_COUNT (2u)

/* Register pointers */
#define ADC0 ((ADC_Type *)ADC0_BASE) /** Peripheral ADC0 base pointer */
#define ADC1 ((ADC_Type *)ADC1_BASE) /** Peripheral ADC1 base pointer */

/** Array initializer of ADC peripheral base addresses */
#define ADC_BASE_ADDRS {ADC0_BASE, ADC1_BASE}
/** Array initializer of ADC peripheral base pointers */
#define ADC_BASE_PTRS {ADC0, ADC1}
/** Number of interrupt vector arrays for the ADC module. */
#define ADC_IRQS_ARR_COUNT (1u)
/** Number of interrupt channels for the ADC module. */
#define ADC_IRQS_CH_COUNT (1u)
/** Interrupt vectors for the ADC peripheral type */
#define ADC_IRQS {ADC0_IRQn, ADC1_IRQn}

/* ----------------------------------------------------------------------------
   -- ADC Register Masks
   ---------------------------------------------------------------------------- */

/*!
 * @addtogroup ADC_Register_Masks ADC Register Masks
 * @{
 */

/* SC1 Bit Fields */
#define ADC_SC1_ADCH_MASK  0x1Fu
#define ADC_SC1_ADCH_SHIFT 0u
#define ADC_SC1_ADCH_WIDTH 5u
#define ADC_SC1_ADCH(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC1_ADCH_SHIFT)) & ADC_SC1_ADCH_MASK)
#define ADC_SC1_AIEN_MASK  0x40u
#define ADC_SC1_AIEN_SHIFT 6u
#define ADC_SC1_AIEN_WIDTH 1u
#define ADC_SC1_AIEN(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC1_AIEN_SHIFT)) & ADC_SC1_AIEN_MASK)
#define ADC_SC1_COCO_MASK  0x80u
#define ADC_SC1_COCO_SHIFT 7u
#define ADC_SC1_COCO_WIDTH 1u
#define ADC_SC1_COCO(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC1_COCO_SHIFT)) & ADC_SC1_COCO_MASK)

/* CFG1 Bit Fields */
#define ADC_CFG1_ADICLK_MASK   0x3u
#define ADC_CFG1_ADICLK_SHIFT  0u
#define ADC_CFG1_ADICLK_WIDTH  2u
#define ADC_CFG1_ADICLK(x)     (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_ADICLK_SHIFT)) & ADC_CFG1_ADICLK_MASK)
#define ADC_CFG1_MODE_MASK     0xCu
#define ADC_CFG1_MODE_SHIFT    2u
#define ADC_CFG1_MODE_WIDTH    2u
#define ADC_CFG1_MODE(x)       (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_MODE_SHIFT)) & ADC_CFG1_MODE_MASK)
#define ADC_CFG1_ADIV_MASK     0x60u
#define ADC_CFG1_ADIV_SHIFT    5u
#define ADC_CFG1_ADIV_WIDTH    2u
#define ADC_CFG1_ADIV(x)       (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_ADIV_SHIFT)) & ADC_CFG1_ADIV_MASK)
#define ADC_CFG1_CLRLTRG_MASK  0x100u
#define ADC_CFG1_CLRLTRG_SHIFT 8u
#define ADC_CFG1_CLRLTRG_WIDTH 1u
#define ADC_CFG1_CLRLTRG(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_CLRLTRG_SHIFT)) & ADC_CFG1_CLRLTRG_MASK)

/* CFG2 Bit Fields */
#define ADC_CFG2_SMPLTS_MASK  0xFFu
#define ADC_CFG2_SMPLTS_SHIFT 0u
#define ADC_CFG2_SMPLTS_WIDTH 8u
#define ADC_CFG2_SMPLTS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CFG2_SMPLTS_SHIFT)) & ADC_CFG2_SMPLTS_MASK)

/* R Bit Fields */
#define ADC_R_D_MASK  0xFFFu
#define ADC_R_D_SHIFT 0u
#define ADC_R_D_WIDTH 12u
#define ADC_R_D(x)    (((uint32_t)(((uint32_t)(x)) << ADC_R_D_SHIFT)) & ADC_R_D_MASK)

/* CV Bit Fields */
#define ADC_CV_CV_MASK  0xFFFFu
#define ADC_CV_CV_SHIFT 0u
#define ADC_CV_CV_WIDTH 16u
#define ADC_CV_CV(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CV_CV_SHIFT)) & ADC_CV_CV_MASK)

/* SC2 Bit Fields */
#define ADC_SC2_REFSEL_MASK    0x3u
#define ADC_SC2_REFSEL_SHIFT   0u
#define ADC_SC2_REFSEL_WIDTH   2u
#define ADC_SC2_REFSEL(x)      (((uint32_t)(((uint32_t)(x)) << ADC_SC2_REFSEL_SHIFT)) & ADC_SC2_REFSEL_MASK)
#define ADC_SC2_DMAEN_MASK     0x4u
#define ADC_SC2_DMAEN_SHIFT    2u
#define ADC_SC2_DMAEN_WIDTH    1u
#define ADC_SC2_DMAEN(x)       (((uint32_t)(((uint32_t)(x)) << ADC_SC2_DMAEN_SHIFT)) & ADC_SC2_DMAEN_MASK)
#define ADC_SC2_ACREN_MASK     0x8u
#define ADC_SC2_ACREN_SHIFT    3u
#define ADC_SC2_ACREN_WIDTH    1u
#define ADC_SC2_ACREN(x)       (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACREN_SHIFT)) & ADC_SC2_ACREN_MASK)
#define ADC_SC2_ACFGT_MASK     0x10u
#define ADC_SC2_ACFGT_SHIFT    4u
#define ADC_SC2_ACFGT_WIDTH    1u
#define ADC_SC2_ACFGT(x)       (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACFGT_SHIFT)) & ADC_SC2_ACFGT_MASK)
#define ADC_SC2_ACFE_MASK      0x20u
#define ADC_SC2_ACFE_SHIFT     5u
#define ADC_SC2_ACFE_WIDTH     1u
#define ADC_SC2_ACFE(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACFE_SHIFT)) & ADC_SC2_ACFE_MASK)
#define ADC_SC2_ADTRG_MASK     0x40u
#define ADC_SC2_ADTRG_SHIFT    6u
#define ADC_SC2_ADTRG_WIDTH    1u
#define ADC_SC2_ADTRG(x)       (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ADTRG_SHIFT)) & ADC_SC2_ADTRG_MASK)
#define ADC_SC2_ADACT_MASK     0x80u
#define ADC_SC2_ADACT_SHIFT    7u
#define ADC_SC2_ADACT_WIDTH    1u
#define ADC_SC2_ADACT(x)       (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ADACT_SHIFT)) & ADC_SC2_ADACT_MASK)

/* SC3 Bit Fields */
#define ADC_SC3_AVGS_MASK      0x3u
#define ADC_SC3_AVGS_SHIFT     0u
#define ADC_SC3_AVGS_WIDTH     2u
#define ADC_SC3_AVGS(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC3_AVGS_SHIFT)) & ADC_SC3_AVGS_MASK)
#define ADC_SC3_AVGE_MASK      0x4u
#define ADC_SC3_AVGE_SHIFT     2u
#define ADC_SC3_AVGE_WIDTH     1u
#define ADC_SC3_AVGE(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC3_AVGE_SHIFT)) & ADC_SC3_AVGE_MASK)
#define ADC_SC3_ADCO_MASK      0x8u
#define ADC_SC3_ADCO_SHIFT     3u
#define ADC_SC3_ADCO_WIDTH     1u
#define ADC_SC3_ADCO(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC3_ADCO_SHIFT)) & ADC_SC3_ADCO_MASK)
#define ADC_SC3_CAL_MASK       0x80u
#define ADC_SC3_CAL_SHIFT      7u
#define ADC_SC3_CAL_WIDTH      1u
#define ADC_SC3_CAL(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC3_CAL_SHIFT)) & ADC_SC3_CAL_MASK)
#define ADC_SC3_CALF_MASK      0x40u
#define ADC_SC3_CALF_SHIFT     6u
#define ADC_SC3_CALF_WIDTH     1u
#define ADC_SC3_CALF(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC3_CALF_SHIFT)) & ADC_SC3_CALF_MASK)

/* Calibration register bit fields */
#define ADC_CLP4_MASK          0x3FFu
#define ADC_CLP3_MASK          0x1FFu
#define ADC_CLP2_MASK          0xFFu
#define ADC_CLP1_MASK          0x7Fu
#define ADC_CLP0_MASK          0x3Fu
#define ADC_CLPX_MASK          0x7Fu
#define ADC_CLP9_MASK          0x7Fu
#define ADC_CLPS_MASK          0x3Fu

#define ADC_CLM4_MASK          0x3FFu
#define ADC_CLM3_MASK          0x1FFu
#define ADC_CLM2_MASK          0xFFu
#define ADC_CLM1_MASK          0x7Fu
#define ADC_CLM0_MASK          0x3Fu
#define ADC_CLMX_MASK          0x7Fu
#define ADC_CLM9_MASK          0x7Fu
#define ADC_CLMS_MASK          0x3Fu

/*!
 * @}
 */ /* end of group ADC_Register_Masks */

#endif /* ADC_REG_H */
