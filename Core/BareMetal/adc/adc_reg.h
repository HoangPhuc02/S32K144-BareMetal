/**
 * @file    adc_reg.h
 * @brief   ADC Register Definitions for S32K144
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
#define ADC_SC2_TRGPRNUM_MASK  0x6000u
#define ADC_SC2_TRGPRNUM_SHIFT 13u
#define ADC_SC2_TRGPRNUM_WIDTH 2u
#define ADC_SC2_TRGPRNUM(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC2_TRGPRNUM_SHIFT)) & ADC_SC2_TRGPRNUM_MASK)
#define ADC_SC2_TRGSTLAT_MASK  0xF0000u
#define ADC_SC2_TRGSTLAT_SHIFT 16u
#define ADC_SC2_TRGSTLAT_WIDTH 4u
#define ADC_SC2_TRGSTLAT(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC2_TRGSTLAT_SHIFT)) & ADC_SC2_TRGSTLAT_MASK)
#define ADC_SC2_TRGSTERR_MASK  0xF000000u
#define ADC_SC2_TRGSTERR_SHIFT 24u
#define ADC_SC2_TRGSTERR_WIDTH 4u
#define ADC_SC2_TRGSTERR(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC2_TRGSTERR_SHIFT)) & ADC_SC2_TRGSTERR_MASK)
/* SC3 Bit Fields */
#define ADC_SC3_AVGS_MASK  0x3u
#define ADC_SC3_AVGS_SHIFT 0u
#define ADC_SC3_AVGS_WIDTH 2u
#define ADC_SC3_AVGS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC3_AVGS_SHIFT)) & ADC_SC3_AVGS_MASK)
#define ADC_SC3_AVGE_MASK  0x4u
#define ADC_SC3_AVGE_SHIFT 2u
#define ADC_SC3_AVGE_WIDTH 1u
#define ADC_SC3_AVGE(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC3_AVGE_SHIFT)) & ADC_SC3_AVGE_MASK)
#define ADC_SC3_ADCO_MASK  0x8u
#define ADC_SC3_ADCO_SHIFT 3u
#define ADC_SC3_ADCO_WIDTH 1u
#define ADC_SC3_ADCO(x)    (((uint32_t)(((uint32_t)(x)) << ADC_SC3_ADCO_SHIFT)) & ADC_SC3_ADCO_MASK)
#define ADC_SC3_CAL_MASK   0x80u
#define ADC_SC3_CAL_SHIFT  7u
#define ADC_SC3_CAL_WIDTH  1u
#define ADC_SC3_CAL(x)     (((uint32_t)(((uint32_t)(x)) << ADC_SC3_CAL_SHIFT)) & ADC_SC3_CAL_MASK)
/* BASE_OFS Bit Fields */
#define ADC_BASE_OFS_BA_OFS_MASK  0xFFu
#define ADC_BASE_OFS_BA_OFS_SHIFT 0u
#define ADC_BASE_OFS_BA_OFS_WIDTH 8u
#define ADC_BASE_OFS_BA_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_BASE_OFS_BA_OFS_SHIFT)) & ADC_BASE_OFS_BA_OFS_MASK)
/* OFS Bit Fields */
#define ADC_OFS_OFS_MASK  0xFFFFu
#define ADC_OFS_OFS_SHIFT 0u
#define ADC_OFS_OFS_WIDTH 16u
#define ADC_OFS_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_OFS_OFS_SHIFT)) & ADC_OFS_OFS_MASK)
/* USR_OFS Bit Fields */
#define ADC_USR_OFS_USR_OFS_MASK  0xFFu
#define ADC_USR_OFS_USR_OFS_SHIFT 0u
#define ADC_USR_OFS_USR_OFS_WIDTH 8u
#define ADC_USR_OFS_USR_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_USR_OFS_USR_OFS_SHIFT)) & ADC_USR_OFS_USR_OFS_MASK)
/* XOFS Bit Fields */
#define ADC_XOFS_XOFS_MASK  0x3Fu
#define ADC_XOFS_XOFS_SHIFT 0u
#define ADC_XOFS_XOFS_WIDTH 6u
#define ADC_XOFS_XOFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_XOFS_XOFS_SHIFT)) & ADC_XOFS_XOFS_MASK)
/* YOFS Bit Fields */
#define ADC_YOFS_YOFS_MASK  0xFFu
#define ADC_YOFS_YOFS_SHIFT 0u
#define ADC_YOFS_YOFS_WIDTH 8u
#define ADC_YOFS_YOFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_YOFS_YOFS_SHIFT)) & ADC_YOFS_YOFS_MASK)
/* G Bit Fields */
#define ADC_G_G_MASK  0x7FFu
#define ADC_G_G_SHIFT 0u
#define ADC_G_G_WIDTH 11u
#define ADC_G_G(x)    (((uint32_t)(((uint32_t)(x)) << ADC_G_G_SHIFT)) & ADC_G_G_MASK)
/* UG Bit Fields */
#define ADC_UG_UG_MASK  0x3FFu
#define ADC_UG_UG_SHIFT 0u
#define ADC_UG_UG_WIDTH 10u
#define ADC_UG_UG(x)    (((uint32_t)(((uint32_t)(x)) << ADC_UG_UG_SHIFT)) & ADC_UG_UG_MASK)
/* CLPS Bit Fields */
#define ADC_CLPS_CLPS_MASK  0x7Fu
#define ADC_CLPS_CLPS_SHIFT 0u
#define ADC_CLPS_CLPS_WIDTH 7u
#define ADC_CLPS_CLPS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLPS_CLPS_SHIFT)) & ADC_CLPS_CLPS_MASK)
/* CLP3 Bit Fields */
#define ADC_CLP3_CLP3_MASK  0x3FFu
#define ADC_CLP3_CLP3_SHIFT 0u
#define ADC_CLP3_CLP3_WIDTH 10u
#define ADC_CLP3_CLP3(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP3_CLP3_SHIFT)) & ADC_CLP3_CLP3_MASK)
/* CLP2 Bit Fields */
#define ADC_CLP2_CLP2_MASK  0x3FFu
#define ADC_CLP2_CLP2_SHIFT 0u
#define ADC_CLP2_CLP2_WIDTH 10u
#define ADC_CLP2_CLP2(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP2_CLP2_SHIFT)) & ADC_CLP2_CLP2_MASK)
/* CLP1 Bit Fields */
#define ADC_CLP1_CLP1_MASK  0x1FFu
#define ADC_CLP1_CLP1_SHIFT 0u
#define ADC_CLP1_CLP1_WIDTH 9u
#define ADC_CLP1_CLP1(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP1_CLP1_SHIFT)) & ADC_CLP1_CLP1_MASK)
/* CLP0 Bit Fields */
#define ADC_CLP0_CLP0_MASK  0xFFu
#define ADC_CLP0_CLP0_SHIFT 0u
#define ADC_CLP0_CLP0_WIDTH 8u
#define ADC_CLP0_CLP0(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP0_CLP0_SHIFT)) & ADC_CLP0_CLP0_MASK)
/* CLPX Bit Fields */
#define ADC_CLPX_CLPX_MASK  0x7Fu
#define ADC_CLPX_CLPX_SHIFT 0u
#define ADC_CLPX_CLPX_WIDTH 7u
#define ADC_CLPX_CLPX(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLPX_CLPX_SHIFT)) & ADC_CLPX_CLPX_MASK)
/* CLP9 Bit Fields */
#define ADC_CLP9_CLP9_MASK  0x7Fu
#define ADC_CLP9_CLP9_SHIFT 0u
#define ADC_CLP9_CLP9_WIDTH 7u
#define ADC_CLP9_CLP9(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP9_CLP9_SHIFT)) & ADC_CLP9_CLP9_MASK)
/* CLPS_OFS Bit Fields */
#define ADC_CLPS_OFS_CLPS_OFS_MASK  0xFu
#define ADC_CLPS_OFS_CLPS_OFS_SHIFT 0u
#define ADC_CLPS_OFS_CLPS_OFS_WIDTH 4u
#define ADC_CLPS_OFS_CLPS_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLPS_OFS_CLPS_OFS_SHIFT)) & ADC_CLPS_OFS_CLPS_OFS_MASK)
/* CLP3_OFS Bit Fields */
#define ADC_CLP3_OFS_CLP3_OFS_MASK  0xFu
#define ADC_CLP3_OFS_CLP3_OFS_SHIFT 0u
#define ADC_CLP3_OFS_CLP3_OFS_WIDTH 4u
#define ADC_CLP3_OFS_CLP3_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP3_OFS_CLP3_OFS_SHIFT)) & ADC_CLP3_OFS_CLP3_OFS_MASK)
/* CLP2_OFS Bit Fields */
#define ADC_CLP2_OFS_CLP2_OFS_MASK  0xFu
#define ADC_CLP2_OFS_CLP2_OFS_SHIFT 0u
#define ADC_CLP2_OFS_CLP2_OFS_WIDTH 4u
#define ADC_CLP2_OFS_CLP2_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP2_OFS_CLP2_OFS_SHIFT)) & ADC_CLP2_OFS_CLP2_OFS_MASK)
/* CLP1_OFS Bit Fields */
#define ADC_CLP1_OFS_CLP1_OFS_MASK  0xFu
#define ADC_CLP1_OFS_CLP1_OFS_SHIFT 0u
#define ADC_CLP1_OFS_CLP1_OFS_WIDTH 4u
#define ADC_CLP1_OFS_CLP1_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP1_OFS_CLP1_OFS_SHIFT)) & ADC_CLP1_OFS_CLP1_OFS_MASK)
/* CLP0_OFS Bit Fields */
#define ADC_CLP0_OFS_CLP0_OFS_MASK  0xFu
#define ADC_CLP0_OFS_CLP0_OFS_SHIFT 0u
#define ADC_CLP0_OFS_CLP0_OFS_WIDTH 4u
#define ADC_CLP0_OFS_CLP0_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP0_OFS_CLP0_OFS_SHIFT)) & ADC_CLP0_OFS_CLP0_OFS_MASK)
/* CLPX_OFS Bit Fields */
#define ADC_CLPX_OFS_CLPX_OFS_MASK  0xFFFu
#define ADC_CLPX_OFS_CLPX_OFS_SHIFT 0u
#define ADC_CLPX_OFS_CLPX_OFS_WIDTH 12u
#define ADC_CLPX_OFS_CLPX_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLPX_OFS_CLPX_OFS_SHIFT)) & ADC_CLPX_OFS_CLPX_OFS_MASK)
/* CLP9_OFS Bit Fields */
#define ADC_CLP9_OFS_CLP9_OFS_MASK  0xFFFu
#define ADC_CLP9_OFS_CLP9_OFS_SHIFT 0u
#define ADC_CLP9_OFS_CLP9_OFS_WIDTH 12u
#define ADC_CLP9_OFS_CLP9_OFS(x)    (((uint32_t)(((uint32_t)(x)) << ADC_CLP9_OFS_CLP9_OFS_SHIFT)) & ADC_CLP9_OFS_CLP9_OFS_MASK)

/*!
 * @}
 */ /* end of group ADC_Register_Masks */

#endif /* ADC_REG_H */