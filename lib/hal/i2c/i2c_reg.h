/**
 * @file    i2c_reg.h
 * @brief   LPI2C Register Definitions for S32K144
 * @details This file contains low-level LPI2C (Low Power I2C) register definitions 
 *          and macros for direct hardware access.
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for LPI2C peripheral
 * @warning Direct register access - use with caution
 * 
 * @par     Change Log:
 * - Version 1.0 (Nov 24, 2025): Initial version
 * 
 */

#ifndef I2C_REG_H
#define I2C_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include "def_reg.h"

/*******************************************************************************
 * LPI2C Register Structure
 ******************************************************************************/

/**
 * @brief LPI2C register structure definition
 */
typedef struct {
    __I  uint32_t VERID;                /**< Offset: 0x00 - Version ID Register */
    __I  uint32_t PARAM;                /**< Offset: 0x04 - Parameter Register */
         uint8_t  RESERVED_0[8];        /**< Reserved */
    __IO uint32_t MCR;                  /**< Offset: 0x10 - Master Control Register */
    __IO uint32_t MSR;                  /**< Offset: 0x14 - Master Status Register */
    __IO uint32_t MIER;                 /**< Offset: 0x18 - Master Interrupt Enable Register */
    __IO uint32_t MDER;                 /**< Offset: 0x1C - Master DMA Enable Register */
    __IO uint32_t MCFGR0;               /**< Offset: 0x20 - Master Configuration Register 0 */
    __IO uint32_t MCFGR1;               /**< Offset: 0x24 - Master Configuration Register 1 */
    __IO uint32_t MCFGR2;               /**< Offset: 0x28 - Master Configuration Register 2 */
    __IO uint32_t MCFGR3;               /**< Offset: 0x2C - Master Configuration Register 3 */
         uint8_t  RESERVED_1[16];       /**< Reserved */
    __IO uint32_t MDMR;                 /**< Offset: 0x40 - Master Data Match Register */
         uint8_t  RESERVED_2[4];        /**< Reserved */
    __IO uint32_t MCCR0;                /**< Offset: 0x48 - Master Clock Configuration Register 0 */
         uint8_t  RESERVED_3[4];        /**< Reserved */
    __IO uint32_t MCCR1;                /**< Offset: 0x50 - Master Clock Configuration Register 1 */
         uint8_t  RESERVED_4[4];        /**< Reserved */
    __IO uint32_t MFCR;                 /**< Offset: 0x58 - Master FIFO Control Register */
    __I  uint32_t MFSR;                 /**< Offset: 0x5C - Master FIFO Status Register */
    __IO uint32_t MTDR;                 /**< Offset: 0x60 - Master Transmit Data Register */
         uint8_t  RESERVED_5[12];       /**< Reserved */
    __I  uint32_t MRDR;                 /**< Offset: 0x70 - Master Receive Data Register */
         uint8_t  RESERVED_6[156];      /**< Reserved */
    __IO uint32_t SCR;                  /**< Offset: 0x110 - Slave Control Register */
    __IO uint32_t SSR;                  /**< Offset: 0x114 - Slave Status Register */
    __IO uint32_t SIER;                 /**< Offset: 0x118 - Slave Interrupt Enable Register */
    __IO uint32_t SDER;                 /**< Offset: 0x11C - Slave DMA Enable Register */
         uint8_t  RESERVED_7[4];        /**< Reserved */
    __IO uint32_t SCFGR1;               /**< Offset: 0x124 - Slave Configuration Register 1 */
    __IO uint32_t SCFGR2;               /**< Offset: 0x128 - Slave Configuration Register 2 */
         uint8_t  RESERVED_8[20];       /**< Reserved */
    __IO uint32_t SAMR;                 /**< Offset: 0x140 - Slave Address Match Register */
         uint8_t  RESERVED_9[12];       /**< Reserved */
    __I  uint32_t SASR;                 /**< Offset: 0x150 - Slave Address Status Register */
    __IO uint32_t STAR;                 /**< Offset: 0x154 - Slave Transmit ACK Register */
         uint8_t  RESERVED_10[8];       /**< Reserved */
    __IO uint32_t STDR;                 /**< Offset: 0x160 - Slave Transmit Data Register */
         uint8_t  RESERVED_11[12];      /**< Reserved */
    __I  uint32_t SRDR;                 /**< Offset: 0x170 - Slave Receive Data Register */
} LPI2C_RegType;

/** Number of instances of the LPI2C module */
#define LPI2C_INSTANCE_COUNT            (1U)

/*******************************************************************************
 * LPI2C Base Addresses
 ******************************************************************************/

/** @brief LPI2C0 base address */
#define LPI2C0_BASE_ADDR                (0x40066000UL)

/** Array initializer of LPI2C peripheral base addresses */
#define LPI2C_BASE_ADDRS                { LPI2C0_BASE_ADDR }

/*******************************************************************************
 * LPI2C Register Pointers
 ******************************************************************************/

/** @brief LPI2C0 register pointer */
#define LPI2C0                          ((LPI2C_RegType *)LPI2C0_BASE_ADDR)

/*******************************************************************************
 * LPI2C Master Control Register (MCR) Bit Definitions
 ******************************************************************************/

/* Master Enable (MEN) */
#define LPI2C_MCR_MEN_SHIFT             (0U)
#define LPI2C_MCR_MEN_MASK              (0x00000001UL)
#define LPI2C_MCR_MEN_WIDTH             (1U)
#define LPI2C_MCR_MEN(x)                (((uint32_t)(x) << LPI2C_MCR_MEN_SHIFT) & LPI2C_MCR_MEN_MASK)

/* Software Reset (RST) */
#define LPI2C_MCR_RST_SHIFT             (1U)
#define LPI2C_MCR_RST_MASK              (0x00000002UL)
#define LPI2C_MCR_RST_WIDTH             (1U)
#define LPI2C_MCR_RST(x)                (((uint32_t)(x) << LPI2C_MCR_RST_SHIFT) & LPI2C_MCR_RST_MASK)

/* Doze mode enable (DOZEN) */
#define LPI2C_MCR_DOZEN_SHIFT           (2U)
#define LPI2C_MCR_DOZEN_MASK            (0x00000004UL)
#define LPI2C_MCR_DOZEN_WIDTH           (1U)
#define LPI2C_MCR_DOZEN(x)              (((uint32_t)(x) << LPI2C_MCR_DOZEN_SHIFT) & LPI2C_MCR_DOZEN_MASK)

/* Debug Enable (DBGEN) */
#define LPI2C_MCR_DBGEN_SHIFT           (3U)
#define LPI2C_MCR_DBGEN_MASK            (0x00000008UL)
#define LPI2C_MCR_DBGEN_WIDTH           (1U)
#define LPI2C_MCR_DBGEN(x)              (((uint32_t)(x) << LPI2C_MCR_DBGEN_SHIFT) & LPI2C_MCR_DBGEN_MASK)

/* Reset Transmit FIFO (RTF) */
#define LPI2C_MCR_RTF_SHIFT             (8U)
#define LPI2C_MCR_RTF_MASK              (0x00000100UL)
#define LPI2C_MCR_RTF_WIDTH             (1U)
#define LPI2C_MCR_RTF(x)                (((uint32_t)(x) << LPI2C_MCR_RTF_SHIFT) & LPI2C_MCR_RTF_MASK)

/* Reset Receive FIFO (RRF) */
#define LPI2C_MCR_RRF_SHIFT             (9U)
#define LPI2C_MCR_RRF_MASK              (0x00000200UL)
#define LPI2C_MCR_RRF_WIDTH             (1U)
#define LPI2C_MCR_RRF(x)                (((uint32_t)(x) << LPI2C_MCR_RRF_SHIFT) & LPI2C_MCR_RRF_MASK)

/*******************************************************************************
 * LPI2C Master Status Register (MSR) Bit Definitions
 ******************************************************************************/

/* Transmit Data Flag (TDF) */
#define LPI2C_MSR_TDF_SHIFT             (0U)
#define LPI2C_MSR_TDF_MASK              (0x00000001UL)
#define LPI2C_MSR_TDF_WIDTH             (1U)

/* Receive Data Flag (RDF) */
#define LPI2C_MSR_RDF_SHIFT             (1U)
#define LPI2C_MSR_RDF_MASK              (0x00000002UL)
#define LPI2C_MSR_RDF_WIDTH             (1U)

/* End Packet Flag (EPF) */
#define LPI2C_MSR_EPF_SHIFT             (8U)
#define LPI2C_MSR_EPF_MASK              (0x00000100UL)
#define LPI2C_MSR_EPF_WIDTH             (1U)

/* STOP Detect Flag (SDF) */
#define LPI2C_MSR_SDF_SHIFT             (9U)
#define LPI2C_MSR_SDF_MASK              (0x00000200UL)
#define LPI2C_MSR_SDF_WIDTH             (1U)

/* NACK Detect Flag (NDF) */
#define LPI2C_MSR_NDF_SHIFT             (10U)
#define LPI2C_MSR_NDF_MASK              (0x00000400UL)
#define LPI2C_MSR_NDF_WIDTH             (1U)

/* Arbitration Lost Flag (ALF) */
#define LPI2C_MSR_ALF_SHIFT             (11U)
#define LPI2C_MSR_ALF_MASK              (0x00000800UL)
#define LPI2C_MSR_ALF_WIDTH             (1U)

/* FIFO Error Flag (FEF) */
#define LPI2C_MSR_FEF_SHIFT             (12U)
#define LPI2C_MSR_FEF_MASK              (0x00001000UL)
#define LPI2C_MSR_FEF_WIDTH             (1U)

/* Master Busy Flag (MBF) */
#define LPI2C_MSR_MBF_SHIFT             (24U)
#define LPI2C_MSR_MBF_MASK              (0x01000000UL)
#define LPI2C_MSR_MBF_WIDTH             (1U)

/* Bus Busy Flag (BBF) */
#define LPI2C_MSR_BBF_SHIFT             (25U)
#define LPI2C_MSR_BBF_MASK              (0x02000000UL)
#define LPI2C_MSR_BBF_WIDTH             (1U)

/*******************************************************************************
 * LPI2C Master Clock Configuration Register 0 (MCCR0) Bit Definitions
 ******************************************************************************/

/* Clock Low Period (CLKLO) */
#define LPI2C_MCCR0_CLKLO_SHIFT         (0U)
#define LPI2C_MCCR0_CLKLO_MASK          (0x0000003FUL)
#define LPI2C_MCCR0_CLKLO_WIDTH         (6U)
#define LPI2C_MCCR0_CLKLO(x)            (((uint32_t)(x) << LPI2C_MCCR0_CLKLO_SHIFT) & LPI2C_MCCR0_CLKLO_MASK)

/* Clock High Period (CLKHI) */
#define LPI2C_MCCR0_CLKHI_SHIFT         (8U)
#define LPI2C_MCCR0_CLKHI_MASK          (0x00003F00UL)
#define LPI2C_MCCR0_CLKHI_WIDTH         (6U)
#define LPI2C_MCCR0_CLKHI(x)            (((uint32_t)(x) << LPI2C_MCCR0_CLKHI_SHIFT) & LPI2C_MCCR0_CLKHI_MASK)

/* Setup Hold Delay (SETHOLD) */
#define LPI2C_MCCR0_SETHOLD_SHIFT       (16U)
#define LPI2C_MCCR0_SETHOLD_MASK        (0x003F0000UL)
#define LPI2C_MCCR0_SETHOLD_WIDTH       (6U)
#define LPI2C_MCCR0_SETHOLD(x)          (((uint32_t)(x) << LPI2C_MCCR0_SETHOLD_SHIFT) & LPI2C_MCCR0_SETHOLD_MASK)

/* Data Valid Delay (DATAVD) */
#define LPI2C_MCCR0_DATAVD_SHIFT        (24U)
#define LPI2C_MCCR0_DATAVD_MASK         (0x3F000000UL)
#define LPI2C_MCCR0_DATAVD_WIDTH        (6U)
#define LPI2C_MCCR0_DATAVD(x)           (((uint32_t)(x) << LPI2C_MCCR0_DATAVD_SHIFT) & LPI2C_MCCR0_DATAVD_MASK)

/*******************************************************************************
 * LPI2C Master Configuration Register 1 (MCFGR1) Bit Definitions
 ******************************************************************************/

/* Prescaler (PRESCALE) */
#define LPI2C_MCFGR1_PRESCALE_SHIFT     (0U)
#define LPI2C_MCFGR1_PRESCALE_MASK      (0x00000007UL)
#define LPI2C_MCFGR1_PRESCALE_WIDTH     (3U)
#define LPI2C_MCFGR1_PRESCALE(x)        (((uint32_t)(x) << LPI2C_MCFGR1_PRESCALE_SHIFT) & LPI2C_MCFGR1_PRESCALE_MASK)

/* Automatic STOP Generation (AUTOSTOP) */
#define LPI2C_MCFGR1_AUTOSTOP_SHIFT     (8U)
#define LPI2C_MCFGR1_AUTOSTOP_MASK      (0x00000100UL)
#define LPI2C_MCFGR1_AUTOSTOP_WIDTH     (1U)
#define LPI2C_MCFGR1_AUTOSTOP(x)        (((uint32_t)(x) << LPI2C_MCFGR1_AUTOSTOP_SHIFT) & LPI2C_MCFGR1_AUTOSTOP_MASK)

/* NACK/ACK Ignore (IGNACK) */
#define LPI2C_MCFGR1_IGNACK_SHIFT       (9U)
#define LPI2C_MCFGR1_IGNACK_MASK        (0x00000200UL)
#define LPI2C_MCFGR1_IGNACK_WIDTH       (1U)
#define LPI2C_MCFGR1_IGNACK(x)          (((uint32_t)(x) << LPI2C_MCFGR1_IGNACK_SHIFT) & LPI2C_MCFGR1_IGNACK_MASK)

/* Timeout Configuration (TIMECFG) */
#define LPI2C_MCFGR1_TIMECFG_SHIFT      (10U)
#define LPI2C_MCFGR1_TIMECFG_MASK       (0x00000400UL)
#define LPI2C_MCFGR1_TIMECFG_WIDTH      (1U)
#define LPI2C_MCFGR1_TIMECFG(x)         (((uint32_t)(x) << LPI2C_MCFGR1_TIMECFG_SHIFT) & LPI2C_MCFGR1_TIMECFG_MASK)

/* Pin Configuration (PINCFG) */
#define LPI2C_MCFGR1_PINCFG_SHIFT       (24U)
#define LPI2C_MCFGR1_PINCFG_MASK        (0x07000000UL)
#define LPI2C_MCFGR1_PINCFG_WIDTH       (3U)
#define LPI2C_MCFGR1_PINCFG(x)          (((uint32_t)(x) << LPI2C_MCFGR1_PINCFG_SHIFT) & LPI2C_MCFGR1_PINCFG_MASK)

/*******************************************************************************
 * LPI2C Helper Macros
 ******************************************************************************/

/** @brief Enable LPI2C master */
#define LPI2C_ENABLE_MASTER(base)       ((base)->MCR |= LPI2C_MCR_MEN_MASK)

/** @brief Disable LPI2C master */
#define LPI2C_DISABLE_MASTER(base)      ((base)->MCR &= ~LPI2C_MCR_MEN_MASK)

/** @brief Software reset LPI2C */
#define LPI2C_SW_RESET(base)            ((base)->MCR |= LPI2C_MCR_RST_MASK)

/** @brief Reset transmit FIFO */
#define LPI2C_RESET_TX_FIFO(base)       ((base)->MCR |= LPI2C_MCR_RTF_MASK)

/** @brief Reset receive FIFO */
#define LPI2C_RESET_RX_FIFO(base)       ((base)->MCR |= LPI2C_MCR_RRF_MASK)

/** @brief Check if transmit data flag is set */
#define LPI2C_IS_TX_READY(base)         (((base)->MSR & LPI2C_MSR_TDF_MASK) != 0U)

/** @brief Check if receive data flag is set */
#define LPI2C_IS_RX_READY(base)         (((base)->MSR & LPI2C_MSR_RDF_MASK) != 0U)

/** @brief Check if bus is busy */
#define LPI2C_IS_BUS_BUSY(base)         (((base)->MSR & LPI2C_MSR_BBF_MASK) != 0U)

/** @brief Check if master is busy */
#define LPI2C_IS_MASTER_BUSY(base)      (((base)->MSR & LPI2C_MSR_MBF_MASK) != 0U)

/** @brief Clear all status flags */
#define LPI2C_CLEAR_STATUS(base)        ((base)->MSR = 0x00003F00UL)

/** @brief Write transmit data */
#define LPI2C_WRITE_DATA(base, data)    ((base)->MTDR = (data))

/** @brief Read receive data */
#define LPI2C_READ_DATA(base)           ((base)->MRDR)



#endif /* I2C_REG_H */
