/**
 * @file    uart_reg.h
 * @brief   LPUART Register Definitions for S32K144
 * @details This file contains low-level LPUART (Low Power UART) register definitions 
 *          and macros for direct hardware access.
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 * 
 * @note    These are raw register definitions for LPUART peripheral
 * @warning Direct register access - use with caution
 * 
 * @par     Change Log:
 * - Version 1.0 (Nov 24, 2025): Initial version
 * 
 */

#ifndef UART_REG_H
#define UART_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include "def_reg.h"

/*******************************************************************************
 * LPUART Register Structure
 ******************************************************************************/

/**
 * @brief LPUART register structure definition
 */
typedef struct {
    __I  uint32_t VERID;                /**< Offset: 0x00 - Version ID Register */
    __I  uint32_t PARAM;                /**< Offset: 0x04 - Parameter Register */
    __IO uint32_t GLOBAL;               /**< Offset: 0x08 - LPUART Global Register */
    __IO uint32_t PINCFG;               /**< Offset: 0x0C - LPUART Pin Configuration Register */
    __IO uint32_t BAUD;                 /**< Offset: 0x10 - LPUART Baud Rate Register */
    __IO uint32_t STAT;                 /**< Offset: 0x14 - LPUART Status Register */
    __IO uint32_t CTRL;                 /**< Offset: 0x18 - LPUART Control Register */
    __IO uint32_t DATA;                 /**< Offset: 0x1C - LPUART Data Register */
    __IO uint32_t MATCH;                /**< Offset: 0x20 - LPUART Match Address Register */
    __IO uint32_t MODIR;                /**< Offset: 0x24 - LPUART Modem IrDA Register */
    __IO uint32_t FIFO;                 /**< Offset: 0x28 - LPUART FIFO Register */
    __IO uint32_t WATER;                /**< Offset: 0x2C - LPUART Watermark Register */
} LPUART_RegType;

/** Number of instances of the LPUART module */
#define LPUART_INSTANCE_COUNT           (3U)

/*******************************************************************************
 * LPUART Base Addresses
 ******************************************************************************/

/** @brief LPUART0 base address */
#define LPUART0_BASE_ADDR               (0x4006A000UL)

/** @brief LPUART1 base address */
#define LPUART1_BASE_ADDR               (0x4006B000UL)

/** @brief LPUART2 base address */
#define LPUART2_BASE_ADDR               (0x4006C000UL)

/** Array initializer of LPUART peripheral base addresses */
#define LPUART_BASE_ADDRS               { LPUART0_BASE_ADDR, LPUART1_BASE_ADDR, LPUART2_BASE_ADDR }

/*******************************************************************************
 * LPUART Register Pointers
 ******************************************************************************/

/** @brief LPUART0 register pointer */
#define LPUART0                         ((LPUART_RegType *)LPUART0_BASE_ADDR)

/** @brief LPUART1 register pointer */
#define LPUART1                         ((LPUART_RegType *)LPUART1_BASE_ADDR)

/** @brief LPUART2 register pointer */
#define LPUART2                         ((LPUART_RegType *)LPUART2_BASE_ADDR)

/*******************************************************************************
 * LPUART Global Register (GLOBAL) Bit Definitions
 ******************************************************************************/

/* Software Reset (RST) */
#define LPUART_GLOBAL_RST_SHIFT         (1U)
#define LPUART_GLOBAL_RST_MASK          (0x00000002UL)
#define LPUART_GLOBAL_RST_WIDTH         (1U)
#define LPUART_GLOBAL_RST(x)            (((uint32_t)(x) << LPUART_GLOBAL_RST_SHIFT) & LPUART_GLOBAL_RST_MASK)

/*******************************************************************************
 * LPUART Baud Rate Register (BAUD) Bit Definitions
 ******************************************************************************/

/* Baud Rate Modulo Divisor (SBR) */
#define LPUART_BAUD_SBR_SHIFT           (0U)
#define LPUART_BAUD_SBR_MASK            (0x00001FFFUL)
#define LPUART_BAUD_SBR_WIDTH           (13U)
#define LPUART_BAUD_SBR(x)              (((uint32_t)(x) << LPUART_BAUD_SBR_SHIFT) & LPUART_BAUD_SBR_MASK)

/* Stop Bit Number Select (SBNS) */
#define LPUART_BAUD_SBNS_SHIFT          (13U)
#define LPUART_BAUD_SBNS_MASK           (0x00002000UL)
#define LPUART_BAUD_SBNS_WIDTH          (1U)
#define LPUART_BAUD_SBNS(x)             (((uint32_t)(x) << LPUART_BAUD_SBNS_SHIFT) & LPUART_BAUD_SBNS_MASK)

/* RX Input Active Edge Interrupt Enable (RXEDGIE) */
#define LPUART_BAUD_RXEDGIE_SHIFT       (14U)
#define LPUART_BAUD_RXEDGIE_MASK        (0x00004000UL)
#define LPUART_BAUD_RXEDGIE_WIDTH       (1U)
#define LPUART_BAUD_RXEDGIE(x)          (((uint32_t)(x) << LPUART_BAUD_RXEDGIE_SHIFT) & LPUART_BAUD_RXEDGIE_MASK)

/* Over Sampling Ratio (OSR) */
#define LPUART_BAUD_OSR_SHIFT           (24U)
#define LPUART_BAUD_OSR_MASK            (0x1F000000UL)
#define LPUART_BAUD_OSR_WIDTH           (5U)
#define LPUART_BAUD_OSR(x)              (((uint32_t)(x) << LPUART_BAUD_OSR_SHIFT) & LPUART_BAUD_OSR_MASK)

/* 10-bit Mode select (M10) */
#define LPUART_BAUD_M10_SHIFT           (29U)
#define LPUART_BAUD_M10_MASK            (0x20000000UL)
#define LPUART_BAUD_M10_WIDTH           (1U)
#define LPUART_BAUD_M10(x)              (((uint32_t)(x) << LPUART_BAUD_M10_SHIFT) & LPUART_BAUD_M10_MASK)

/*******************************************************************************
 * LPUART Status Register (STAT) Bit Definitions
 ******************************************************************************/

/* Match Address 2 Flag (MA2F) */
#define LPUART_STAT_MA2F_SHIFT          (14U)
#define LPUART_STAT_MA2F_MASK           (0x00004000UL)
#define LPUART_STAT_MA2F_WIDTH          (1U)

/* Match Address 1 Flag (MA1F) */
#define LPUART_STAT_MA1F_SHIFT          (15U)
#define LPUART_STAT_MA1F_MASK           (0x00008000UL)
#define LPUART_STAT_MA1F_WIDTH          (1U)

/* Parity Error Flag (PF) */
#define LPUART_STAT_PF_SHIFT            (16U)
#define LPUART_STAT_PF_MASK             (0x00010000UL)
#define LPUART_STAT_PF_WIDTH            (1U)

/* Framing Error Flag (FE) */
#define LPUART_STAT_FE_SHIFT            (17U)
#define LPUART_STAT_FE_MASK             (0x00020000UL)
#define LPUART_STAT_FE_WIDTH            (1U)

/* Noise Flag (NF) */
#define LPUART_STAT_NF_SHIFT            (18U)
#define LPUART_STAT_NF_MASK             (0x00040000UL)
#define LPUART_STAT_NF_WIDTH            (1U)

/* Receiver Overrun Flag (OR) */
#define LPUART_STAT_OR_SHIFT            (19U)
#define LPUART_STAT_OR_MASK             (0x00080000UL)
#define LPUART_STAT_OR_WIDTH            (1U)

/* Idle Line Flag (IDLE) */
#define LPUART_STAT_IDLE_SHIFT          (20U)
#define LPUART_STAT_IDLE_MASK           (0x00100000UL)
#define LPUART_STAT_IDLE_WIDTH          (1U)

/* Receive Data Register Full Flag (RDRF) */
#define LPUART_STAT_RDRF_SHIFT          (21U)
#define LPUART_STAT_RDRF_MASK           (0x00200000UL)
#define LPUART_STAT_RDRF_WIDTH          (1U)

/* Transmission Complete Flag (TC) */
#define LPUART_STAT_TC_SHIFT            (22U)
#define LPUART_STAT_TC_MASK             (0x00400000UL)
#define LPUART_STAT_TC_WIDTH            (1U)

/* Transmit Data Register Empty Flag (TDRE) */
#define LPUART_STAT_TDRE_SHIFT          (23U)
#define LPUART_STAT_TDRE_MASK           (0x00800000UL)
#define LPUART_STAT_TDRE_WIDTH          (1U)

/* Receiver Active Flag (RAF) */
#define LPUART_STAT_RAF_SHIFT           (24U)
#define LPUART_STAT_RAF_MASK            (0x01000000UL)
#define LPUART_STAT_RAF_WIDTH           (1U)

/*******************************************************************************
 * LPUART Control Register (CTRL) Bit Definitions
 ******************************************************************************/

/* Parity Type (PT) */
#define LPUART_CTRL_PT_SHIFT            (0U)
#define LPUART_CTRL_PT_MASK             (0x00000001UL)
#define LPUART_CTRL_PT_WIDTH            (1U)
#define LPUART_CTRL_PT(x)               (((uint32_t)(x) << LPUART_CTRL_PT_SHIFT) & LPUART_CTRL_PT_MASK)

/* Parity Enable (PE) */
#define LPUART_CTRL_PE_SHIFT            (1U)
#define LPUART_CTRL_PE_MASK             (0x00000002UL)
#define LPUART_CTRL_PE_WIDTH            (1U)
#define LPUART_CTRL_PE(x)               (((uint32_t)(x) << LPUART_CTRL_PE_SHIFT) & LPUART_CTRL_PE_MASK)

/* Idle Line Type Select (ILT) */
#define LPUART_CTRL_ILT_SHIFT           (2U)
#define LPUART_CTRL_ILT_MASK            (0x00000004UL)
#define LPUART_CTRL_ILT_WIDTH           (1U)
#define LPUART_CTRL_ILT(x)              (((uint32_t)(x) << LPUART_CTRL_ILT_SHIFT) & LPUART_CTRL_ILT_MASK)

/* Receiver Wakeup Method Select (WAKE) */
#define LPUART_CTRL_WAKE_SHIFT          (3U)
#define LPUART_CTRL_WAKE_MASK           (0x00000008UL)
#define LPUART_CTRL_WAKE_WIDTH          (1U)
#define LPUART_CTRL_WAKE(x)             (((uint32_t)(x) << LPUART_CTRL_WAKE_SHIFT) & LPUART_CTRL_WAKE_MASK)

/* 9-Bit or 8-Bit Mode Select (M) */
#define LPUART_CTRL_M_SHIFT             (4U)
#define LPUART_CTRL_M_MASK              (0x00000010UL)
#define LPUART_CTRL_M_WIDTH             (1U)
#define LPUART_CTRL_M(x)                (((uint32_t)(x) << LPUART_CTRL_M_SHIFT) & LPUART_CTRL_M_MASK)

/* Receiver Source Select (RSRC) */
#define LPUART_CTRL_RSRC_SHIFT          (5U)
#define LPUART_CTRL_RSRC_MASK           (0x00000020UL)
#define LPUART_CTRL_RSRC_WIDTH          (1U)
#define LPUART_CTRL_RSRC(x)             (((uint32_t)(x) << LPUART_CTRL_RSRC_SHIFT) & LPUART_CTRL_RSRC_MASK)

/* Doze Enable (DOZEEN) */
#define LPUART_CTRL_DOZEEN_SHIFT        (6U)
#define LPUART_CTRL_DOZEEN_MASK         (0x00000040UL)
#define LPUART_CTRL_DOZEEN_WIDTH        (1U)
#define LPUART_CTRL_DOZEEN(x)           (((uint32_t)(x) << LPUART_CTRL_DOZEEN_SHIFT) & LPUART_CTRL_DOZEEN_MASK)

/* Loop Mode Select (LOOPS) */
#define LPUART_CTRL_LOOPS_SHIFT         (7U)
#define LPUART_CTRL_LOOPS_MASK          (0x00000080UL)
#define LPUART_CTRL_LOOPS_WIDTH         (1U)
#define LPUART_CTRL_LOOPS(x)            (((uint32_t)(x) << LPUART_CTRL_LOOPS_SHIFT) & LPUART_CTRL_LOOPS_MASK)

/* Idle Configuration (IDLECFG) */
#define LPUART_CTRL_IDLECFG_SHIFT       (8U)
#define LPUART_CTRL_IDLECFG_MASK        (0x00000700UL)
#define LPUART_CTRL_IDLECFG_WIDTH       (3U)
#define LPUART_CTRL_IDLECFG(x)          (((uint32_t)(x) << LPUART_CTRL_IDLECFG_SHIFT) & LPUART_CTRL_IDLECFG_MASK)

/* Match 2 Interrupt Enable (MA2IE) */
#define LPUART_CTRL_MA2IE_SHIFT         (14U)
#define LPUART_CTRL_MA2IE_MASK          (0x00004000UL)
#define LPUART_CTRL_MA2IE_WIDTH         (1U)
#define LPUART_CTRL_MA2IE(x)            (((uint32_t)(x) << LPUART_CTRL_MA2IE_SHIFT) & LPUART_CTRL_MA2IE_MASK)

/* Match 1 Interrupt Enable (MA1IE) */
#define LPUART_CTRL_MA1IE_SHIFT         (15U)
#define LPUART_CTRL_MA1IE_MASK          (0x00008000UL)
#define LPUART_CTRL_MA1IE_WIDTH         (1U)
#define LPUART_CTRL_MA1IE(x)            (((uint32_t)(x) << LPUART_CTRL_MA1IE_SHIFT) & LPUART_CTRL_MA1IE_MASK)

/* Send Break (SBK) */
#define LPUART_CTRL_SBK_SHIFT           (16U)
#define LPUART_CTRL_SBK_MASK            (0x00010000UL)
#define LPUART_CTRL_SBK_WIDTH           (1U)
#define LPUART_CTRL_SBK(x)              (((uint32_t)(x) << LPUART_CTRL_SBK_SHIFT) & LPUART_CTRL_SBK_MASK)

/* Receiver Wakeup Control (RWU) */
#define LPUART_CTRL_RWU_SHIFT           (17U)
#define LPUART_CTRL_RWU_MASK            (0x00020000UL)
#define LPUART_CTRL_RWU_WIDTH           (1U)
#define LPUART_CTRL_RWU(x)              (((uint32_t)(x) << LPUART_CTRL_RWU_SHIFT) & LPUART_CTRL_RWU_MASK)

/* Receiver Enable (RE) */
#define LPUART_CTRL_RE_SHIFT            (18U)
#define LPUART_CTRL_RE_MASK             (0x00040000UL)
#define LPUART_CTRL_RE_WIDTH            (1U)
#define LPUART_CTRL_RE(x)               (((uint32_t)(x) << LPUART_CTRL_RE_SHIFT) & LPUART_CTRL_RE_MASK)

/* Transmitter Enable (TE) */
#define LPUART_CTRL_TE_SHIFT            (19U)
#define LPUART_CTRL_TE_MASK             (0x00080000UL)
#define LPUART_CTRL_TE_WIDTH            (1U)
#define LPUART_CTRL_TE(x)               (((uint32_t)(x) << LPUART_CTRL_TE_SHIFT) & LPUART_CTRL_TE_MASK)

/* Idle Line Interrupt Enable (ILIE) */
#define LPUART_CTRL_ILIE_SHIFT          (20U)
#define LPUART_CTRL_ILIE_MASK           (0x00100000UL)
#define LPUART_CTRL_ILIE_WIDTH          (1U)
#define LPUART_CTRL_ILIE(x)             (((uint32_t)(x) << LPUART_CTRL_ILIE_SHIFT) & LPUART_CTRL_ILIE_MASK)

/* Receiver Interrupt Enable (RIE) */
#define LPUART_CTRL_RIE_SHIFT           (21U)
#define LPUART_CTRL_RIE_MASK            (0x00200000UL)
#define LPUART_CTRL_RIE_WIDTH           (1U)
#define LPUART_CTRL_RIE(x)              (((uint32_t)(x) << LPUART_CTRL_RIE_SHIFT) & LPUART_CTRL_RIE_MASK)

/* Transmission Complete Interrupt Enable (TCIE) */
#define LPUART_CTRL_TCIE_SHIFT          (22U)
#define LPUART_CTRL_TCIE_MASK           (0x00400000UL)
#define LPUART_CTRL_TCIE_WIDTH          (1U)
#define LPUART_CTRL_TCIE(x)             (((uint32_t)(x) << LPUART_CTRL_TCIE_SHIFT) & LPUART_CTRL_TCIE_MASK)

/* Transmit Interrupt Enable (TIE) */
#define LPUART_CTRL_TIE_SHIFT           (23U)
#define LPUART_CTRL_TIE_MASK            (0x00800000UL)
#define LPUART_CTRL_TIE_WIDTH           (1U)
#define LPUART_CTRL_TIE(x)              (((uint32_t)(x) << LPUART_CTRL_TIE_SHIFT) & LPUART_CTRL_TIE_MASK)

/* Parity Error Interrupt Enable (PEIE) */
#define LPUART_CTRL_PEIE_SHIFT          (24U)
#define LPUART_CTRL_PEIE_MASK           (0x01000000UL)
#define LPUART_CTRL_PEIE_WIDTH          (1U)
#define LPUART_CTRL_PEIE(x)             (((uint32_t)(x) << LPUART_CTRL_PEIE_SHIFT) & LPUART_CTRL_PEIE_MASK)

/* Framing Error Interrupt Enable (FEIE) */
#define LPUART_CTRL_FEIE_SHIFT          (25U)
#define LPUART_CTRL_FEIE_MASK           (0x02000000UL)
#define LPUART_CTRL_FEIE_WIDTH          (1U)
#define LPUART_CTRL_FEIE(x)             (((uint32_t)(x) << LPUART_CTRL_FEIE_SHIFT) & LPUART_CTRL_FEIE_MASK)

/* Noise Error Interrupt Enable (NEIE) */
#define LPUART_CTRL_NEIE_SHIFT          (26U)
#define LPUART_CTRL_NEIE_MASK           (0x04000000UL)
#define LPUART_CTRL_NEIE_WIDTH          (1U)
#define LPUART_CTRL_NEIE(x)             (((uint32_t)(x) << LPUART_CTRL_NEIE_SHIFT) & LPUART_CTRL_NEIE_MASK)

/* Overrun Interrupt Enable (ORIE) */
#define LPUART_CTRL_ORIE_SHIFT          (27U)
#define LPUART_CTRL_ORIE_MASK           (0x08000000UL)
#define LPUART_CTRL_ORIE_WIDTH          (1U)
#define LPUART_CTRL_ORIE(x)             (((uint32_t)(x) << LPUART_CTRL_ORIE_SHIFT) & LPUART_CTRL_ORIE_MASK)

/* Transmit Data Inversion (TXINV) */
#define LPUART_CTRL_TXINV_SHIFT         (28U)
#define LPUART_CTRL_TXINV_MASK          (0x10000000UL)
#define LPUART_CTRL_TXINV_WIDTH         (1U)
#define LPUART_CTRL_TXINV(x)            (((uint32_t)(x) << LPUART_CTRL_TXINV_SHIFT) & LPUART_CTRL_TXINV_MASK)

/* TXD Pin Direction in Single-Wire Mode (TXDIR) */
#define LPUART_CTRL_TXDIR_SHIFT         (29U)
#define LPUART_CTRL_TXDIR_MASK          (0x20000000UL)
#define LPUART_CTRL_TXDIR_WIDTH         (1U)
#define LPUART_CTRL_TXDIR(x)            (((uint32_t)(x) << LPUART_CTRL_TXDIR_SHIFT) & LPUART_CTRL_TXDIR_MASK)

/* 9th Data Bit for Transmitter (R9T8) */
#define LPUART_CTRL_R9T8_SHIFT          (30U)
#define LPUART_CTRL_R9T8_MASK           (0x40000000UL)
#define LPUART_CTRL_R9T8_WIDTH          (1U)
#define LPUART_CTRL_R9T8(x)             (((uint32_t)(x) << LPUART_CTRL_R9T8_SHIFT) & LPUART_CTRL_R9T8_MASK)

/* 9th Data Bit for Receiver (R8T9) */
#define LPUART_CTRL_R8T9_SHIFT          (31U)
#define LPUART_CTRL_R8T9_MASK           (0x80000000UL)
#define LPUART_CTRL_R8T9_WIDTH          (1U)
#define LPUART_CTRL_R8T9(x)             (((uint32_t)(x) << LPUART_CTRL_R8T9_SHIFT) & LPUART_CTRL_R8T9_MASK)

/*******************************************************************************
 * LPUART Data Register (DATA) Bit Definitions
 ******************************************************************************/

/* Read/Write Data (R0T0 - R9T9) */
#define LPUART_DATA_RT_SHIFT            (0U)
#define LPUART_DATA_RT_MASK             (0x000003FFUL)
#define LPUART_DATA_RT_WIDTH            (10U)

/*******************************************************************************
 * LPUART Helper Macros
 ******************************************************************************/

/** @brief Enable LPUART transmitter */
#define LPUART_ENABLE_TX(base)          ((base)->CTRL |= LPUART_CTRL_TE_MASK)

/** @brief Disable LPUART transmitter */
#define LPUART_DISABLE_TX(base)         ((base)->CTRL &= ~LPUART_CTRL_TE_MASK)

/** @brief Enable LPUART receiver */
#define LPUART_ENABLE_RX(base)          ((base)->CTRL |= LPUART_CTRL_RE_MASK)

/** @brief Disable LPUART receiver */
#define LPUART_DISABLE_RX(base)         ((base)->CTRL &= ~LPUART_CTRL_RE_MASK)

/** @brief Software reset LPUART */
#define LPUART_SW_RESET(base)           ((base)->GLOBAL |= LPUART_GLOBAL_RST_MASK)

/** @brief Check if transmit data register is empty */
#define LPUART_IS_TX_READY(base)        (((base)->STAT & LPUART_STAT_TDRE_MASK) != 0U)

/** @brief Check if receive data register is full */
#define LPUART_IS_RX_READY(base)        (((base)->STAT & LPUART_STAT_RDRF_MASK) != 0U)

/** @brief Check if transmission is complete */
#define LPUART_IS_TX_COMPLETE(base)     (((base)->STAT & LPUART_STAT_TC_MASK) != 0U)

/** @brief Check if receiver is active */
#define LPUART_IS_RX_ACTIVE(base)       (((base)->STAT & LPUART_STAT_RAF_MASK) != 0U)

/** @brief Check for parity error */
#define LPUART_HAS_PARITY_ERROR(base)   (((base)->STAT & LPUART_STAT_PF_MASK) != 0U)

/** @brief Check for framing error */
#define LPUART_HAS_FRAME_ERROR(base)    (((base)->STAT & LPUART_STAT_FE_MASK) != 0U)

/** @brief Check for noise error */
#define LPUART_HAS_NOISE_ERROR(base)    (((base)->STAT & LPUART_STAT_NF_MASK) != 0U)

/** @brief Check for overrun error */
#define LPUART_HAS_OVERRUN_ERROR(base)  (((base)->STAT & LPUART_STAT_OR_MASK) != 0U)

/** @brief Write transmit data */
#define LPUART_WRITE_DATA(base, data)   ((base)->DATA = (data))

/** @brief Read receive data */
#define LPUART_READ_DATA(base)          ((base)->DATA & LPUART_DATA_RT_MASK)

/** @brief Clear status flag */
#define LPUART_CLEAR_STATUS_FLAG(base, flags)  ((base)->STAT = (flags))

/** @brief Enable transmit interrupt */
#define LPUART_ENABLE_TX_INT(base)      ((base)->CTRL |= LPUART_CTRL_TIE_MASK)

/** @brief Disable transmit interrupt */
#define LPUART_DISABLE_TX_INT(base)     ((base)->CTRL &= ~LPUART_CTRL_TIE_MASK)

/** @brief Enable receive interrupt */
#define LPUART_ENABLE_RX_INT(base)      ((base)->CTRL |= LPUART_CTRL_RIE_MASK)

/** @brief Disable receive interrupt */
#define LPUART_DISABLE_RX_INT(base)     ((base)->CTRL &= ~LPUART_CTRL_RIE_MASK)

/*******************************************************************************
 * PCC Index for LPUART Clock Control
 ******************************************************************************/

/** @brief PCC index for LPUART0 */
#define PCC_LPUART0_INDEX               (106U)

/** @brief PCC index for LPUART1 */
#define PCC_LPUART1_INDEX               (107U)

/** @brief PCC index for LPUART2 */
#define PCC_LPUART2_INDEX               (108U)

#endif /* UART_REG_H */
