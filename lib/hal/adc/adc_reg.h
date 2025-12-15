/**
 * @file    adc_reg.h
 * @brief   ADC Register Definitions for S32K144
 * @details This file contains low-level ADC register definitions and macros
 *          for direct hardware access including ADC0 and ADC1 configuration.
 * 
 * @author  PhucPH32
 * @date    26/11/2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for ADC peripheral
 * @warning Direct register access - use with caution
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 26, 2025): Initial version
 * 
 */

#ifndef ADC_REG_H
#define ADC_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include "def_reg.h"

/*******************************************************************************
 * ADC Base Addresses
 ******************************************************************************/

/** @brief ADC0 base address */
#define ADC0_BASE_ADDR          (0x4003B000UL)

/** @brief ADC1 base address */
#define ADC1_BASE_ADDR          (0x40027000UL)

/*******************************************************************************
 * ADC Register Structure
 ******************************************************************************/

/** @brief Number of ADC status and control registers */
#define ADC_SC1_COUNT           (16U)

/** @brief Number of ADC result registers */
#define ADC_R_COUNT             (16U)

/** @brief Number of ADC compare value registers */
#define ADC_CV_COUNT            (2U)

/**
 * @brief ADC register structure definition
 */
typedef struct {
    __IO uint32_t SC1[ADC_SC1_COUNT];   /**< ADC Status and Control 1 Registers, offset: 0x0000 */
    __IO uint32_t CFG1;                 /**< ADC Configuration 1 Register, offset: 0x0040 */
    __IO uint32_t CFG2;                 /**< ADC Configuration 2 Register, offset: 0x0044 */
    __I  uint32_t R[ADC_R_COUNT];       /**< ADC Data Result Registers, offset: 0x0048 */
    __IO uint32_t CV[ADC_CV_COUNT];     /**< ADC Compare Value Registers, offset: 0x0088 */
    __IO uint32_t SC2;                  /**< ADC Status and Control 2 Register, offset: 0x0090 */
    __IO uint32_t SC3;                  /**< ADC Status and Control 3 Register, offset: 0x0094 */
    __IO uint32_t BASE_OFS;             /**< ADC Base Offset Register, offset: 0x0098 */
    __IO uint32_t OFS;                  /**< ADC Offset Correction Register, offset: 0x009C */
    __IO uint32_t USR_OFS;              /**< ADC User Offset Correction Register, offset: 0x00A0 */
    __IO uint32_t XOFS;                 /**< ADC X Offset Correction Register, offset: 0x00A4 */
    __IO uint32_t YOFS;                 /**< ADC Y Offset Correction Register, offset: 0x00A8 */
    __IO uint32_t G;                    /**< ADC Gain Register, offset: 0x00AC */
    __IO uint32_t UG;                   /**< ADC User Gain Register, offset: 0x00B0 */
    __IO uint32_t CLPS;                 /**< ADC General Calibration Value Register S, offset: 0x00B4 */
    __IO uint32_t CLP3;                 /**< ADC Plus-Side General Calibration Value 3 Register, offset: 0x00B8 */
    __IO uint32_t CLP2;                 /**< ADC Plus-Side General Calibration Value 2 Register, offset: 0x00BC */
    __IO uint32_t CLP1;                 /**< ADC Plus-Side General Calibration Value 1 Register, offset: 0x00C0 */
    __IO uint32_t CLP0;                 /**< ADC Plus-Side General Calibration Value 0 Register, offset: 0x00C4 */
    __IO uint32_t CLPX;                 /**< ADC Plus-Side General Calibration Value X Register, offset: 0x00C8 */
    __IO uint32_t CLP9;                 /**< ADC Plus-Side General Calibration Value 9 Register, offset: 0x00CC */
    __IO uint32_t CLPS_OFS;             /**< ADC General Calibration Offset Value Register S, offset: 0x00D0 */
    __IO uint32_t CLP3_OFS;             /**< ADC Plus-Side General Calibration Offset Value 3 Register, offset: 0x00D4 */
    __IO uint32_t CLP2_OFS;             /**< ADC Plus-Side General Calibration Offset Value 2 Register, offset: 0x00D8 */
    __IO uint32_t CLP1_OFS;             /**< ADC Plus-Side General Calibration Offset Value 1 Register, offset: 0x00DC */
    __IO uint32_t CLP0_OFS;             /**< ADC Plus-Side General Calibration Offset Value 0 Register, offset: 0x00E0 */
    __IO uint32_t CLPX_OFS;             /**< ADC Plus-Side General Calibration Offset Value X Register, offset: 0x00E4 */
    __IO uint32_t CLP9_OFS;             /**< ADC Plus-Side General Calibration Offset Value 9 Register, offset: 0x00E8 */
} ADC_RegType;

/** @brief ADC0 register pointer */
#define ADC0                    ((ADC_RegType *)ADC0_BASE_ADDR)

/** @brief ADC1 register pointer */
#define ADC1                    ((ADC_RegType *)ADC1_BASE_ADDR)

/*******************************************************************************
 * ADC Status and Control Register 1 (SC1) Bit Definitions
 ******************************************************************************/

/** @brief ADC Input Channel Select Mask */
#define ADC_SC1_ADCH_MASK       (0x1FU)
#define ADC_SC1_ADCH_SHIFT      (0U)
#define ADC_SC1_ADCH(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC1_ADCH_SHIFT)) & ADC_SC1_ADCH_MASK)

/** @brief Interrupt Enable Mask */
#define ADC_SC1_AIEN_MASK       (0x40U)
#define ADC_SC1_AIEN_SHIFT      (6U)
#define ADC_SC1_AIEN(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC1_AIEN_SHIFT)) & ADC_SC1_AIEN_MASK)

/** @brief Conversion Complete Flag Mask */
#define ADC_SC1_COCO_MASK       (0x80U)
#define ADC_SC1_COCO_SHIFT      (7U)
#define ADC_SC1_COCO(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC1_COCO_SHIFT)) & ADC_SC1_COCO_MASK)

/*******************************************************************************
 * ADC Configuration Register 1 (CFG1) Bit Definitions
 ******************************************************************************/

/** @brief ADC Input Clock Select Mask */
#define ADC_CFG1_ADICLK_MASK    (0x03U)
#define ADC_CFG1_ADICLK_SHIFT   (0U)
#define ADC_CFG1_ADICLK(x)      (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_ADICLK_SHIFT)) & ADC_CFG1_ADICLK_MASK)

/** @brief ADC Resolution Select Mask */
#define ADC_CFG1_MODE_MASK      (0x0CU)
#define ADC_CFG1_MODE_SHIFT     (2U)
#define ADC_CFG1_MODE(x)        (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_MODE_SHIFT)) & ADC_CFG1_MODE_MASK)

/** @brief Clock Divide Select Mask */
#define ADC_CFG1_ADIV_MASK      (0x60U)
#define ADC_CFG1_ADIV_SHIFT     (5U)
#define ADC_CFG1_ADIV(x)        (((uint32_t)(((uint32_t)(x)) << ADC_CFG1_ADIV_SHIFT)) & ADC_CFG1_ADIV_MASK)

/*******************************************************************************
 * ADC Configuration Register 2 (CFG2) Bit Definitions
 ******************************************************************************/

/** @brief Long Sample Time Select Mask */
#define ADC_CFG2_SMPLTS_MASK    (0xFFU)
#define ADC_CFG2_SMPLTS_SHIFT   (0U)
#define ADC_CFG2_SMPLTS(x)      (((uint32_t)(((uint32_t)(x)) << ADC_CFG2_SMPLTS_SHIFT)) & ADC_CFG2_SMPLTS_MASK)

/*******************************************************************************
 * ADC Data Result Register (R) Bit Definitions
 ******************************************************************************/

/** @brief ADC Data Result Mask */
#define ADC_R_D_MASK            (0x0FFFU)
#define ADC_R_D_SHIFT           (0U)
#define ADC_R_D(x)              (((uint32_t)(((uint32_t)(x)) << ADC_R_D_SHIFT)) & ADC_R_D_MASK)

/*******************************************************************************
 * ADC Status and Control Register 2 (SC2) Bit Definitions
 ******************************************************************************/

/** @brief Voltage Reference Selection Mask */
#define ADC_SC2_REFSEL_MASK     (0x03U)
#define ADC_SC2_REFSEL_SHIFT    (0U)
#define ADC_SC2_REFSEL(x)       (((uint32_t)(((uint32_t)(x)) << ADC_SC2_REFSEL_SHIFT)) & ADC_SC2_REFSEL_MASK)

/** @brief DMA Enable Mask */
#define ADC_SC2_DMAEN_MASK      (0x04U)
#define ADC_SC2_DMAEN_SHIFT     (2U)
#define ADC_SC2_DMAEN(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_DMAEN_SHIFT)) & ADC_SC2_DMAEN_MASK)

/** @brief Compare Function Range Enable Mask */
#define ADC_SC2_ACREN_MASK      (0x08U)
#define ADC_SC2_ACREN_SHIFT     (3U)
#define ADC_SC2_ACREN(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACREN_SHIFT)) & ADC_SC2_ACREN_MASK)

/** @brief Compare Function Greater Than Enable Mask */
#define ADC_SC2_ACFGT_MASK      (0x10U)
#define ADC_SC2_ACFGT_SHIFT     (4U)
#define ADC_SC2_ACFGT(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACFGT_SHIFT)) & ADC_SC2_ACFGT_MASK)

/** @brief Compare Function Enable Mask */
#define ADC_SC2_ACFE_MASK       (0x20U)
#define ADC_SC2_ACFE_SHIFT      (5U)
#define ADC_SC2_ACFE(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ACFE_SHIFT)) & ADC_SC2_ACFE_MASK)

/** @brief Conversion Trigger Select Mask */
#define ADC_SC2_ADTRG_MASK      (0x40U)
#define ADC_SC2_ADTRG_SHIFT     (6U)
#define ADC_SC2_ADTRG(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ADTRG_SHIFT)) & ADC_SC2_ADTRG_MASK)

/** @brief Conversion Active Mask */
#define ADC_SC2_ADACT_MASK      (0x80U)
#define ADC_SC2_ADACT_SHIFT     (7U)
#define ADC_SC2_ADACT(x)        (((uint32_t)(((uint32_t)(x)) << ADC_SC2_ADACT_SHIFT)) & ADC_SC2_ADACT_MASK)

/*******************************************************************************
 * ADC Status and Control Register 3 (SC3) Bit Definitions
 ******************************************************************************/

/** @brief Hardware Average Enable Mask */
#define ADC_SC3_AVGE_MASK       (0x01U)
#define ADC_SC3_AVGE_SHIFT      (0U)
#define ADC_SC3_AVGE(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC3_AVGE_SHIFT)) & ADC_SC3_AVGE_MASK)

/** @brief Hardware Average Select Mask */
#define ADC_SC3_AVGS_MASK       (0x06U)
#define ADC_SC3_AVGS_SHIFT      (1U)
#define ADC_SC3_AVGS(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC3_AVGS_SHIFT)) & ADC_SC3_AVGS_MASK)

/** @brief Continuous Conversion Enable Mask */
#define ADC_SC3_ADCO_MASK       (0x08U)
#define ADC_SC3_ADCO_SHIFT      (3U)
#define ADC_SC3_ADCO(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC3_ADCO_SHIFT)) & ADC_SC3_ADCO_MASK)

/** @brief Calibration Mask */
#define ADC_SC3_CAL_MASK        (0x80U)
#define ADC_SC3_CAL_SHIFT       (7U)
#define ADC_SC3_CAL(x)          (((uint32_t)(((uint32_t)(x)) << ADC_SC3_CAL_SHIFT)) & ADC_SC3_CAL_MASK)

/** @brief Calibration Failed Flag Mask */
#define ADC_SC3_CALF_MASK       (0x40U)
#define ADC_SC3_CALF_SHIFT      (6U)
#define ADC_SC3_CALF(x)         (((uint32_t)(((uint32_t)(x)) << ADC_SC3_CALF_SHIFT)) & ADC_SC3_CALF_MASK)

/*******************************************************************************
 * Helper Macros
 ******************************************************************************/

/** @brief Check if conversion is complete */
#define ADC_IS_CONVERSION_COMPLETE(base)    (((base)->SC1[0] & ADC_SC1_COCO_MASK) != 0U)

/** @brief Check if conversion is active */
#define ADC_IS_CONVERSION_ACTIVE(base)      (((base)->SC2 & ADC_SC2_ADACT_MASK) != 0U)

/** @brief Check if calibration failed */
#define ADC_IS_CALIBRATION_FAILED(base)     (((base)->SC3 & ADC_SC3_CALF_MASK) != 0U)

/** @brief Read conversion result */
#define ADC_GET_RESULT(base, index)         ((base)->R[(index)])

/** @brief Start conversion on channel */
#define ADC_START_CONVERSION(base, channel) ((base)->SC1[0] = ADC_SC1_ADCH(channel))

/** @brief Disable ADC channel (stop conversion) */
#define ADC_DISABLE_CHANNEL(base)           ((base)->SC1[0] = ADC_SC1_ADCH(0x1FU))

#endif /* ADC_REG_H */
