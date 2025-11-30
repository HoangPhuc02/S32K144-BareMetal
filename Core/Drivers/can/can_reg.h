/**
 * @file    can_reg.h
 * @brief   FlexCAN Register Definitions for S32K144
 * @details This file contains register definitions and bit field macros for the FlexCAN module.
 *          FlexCAN is a full CAN controller that supports CAN 2.0A/B protocol.
 * 
 * The S32K144 has 3 FlexCAN modules:
 * - FlexCAN0: 32 Message Buffers, Base address 0x40024000
 * - FlexCAN1: 32 Message Buffers, Base address 0x40025000
 * - FlexCAN2: 32 Message Buffers, Base address 0x4002B000
 * 
 * Key Features:
 * - CAN 2.0A (Standard 11-bit ID) and 2.0B (Extended 29-bit ID) support
 * - Bit rates up to 1 Mbps
 * - 32 configurable Message Buffers for TX/RX operations
 * - Hardware message filtering with RX FIFO or Individual Mask registers
 * - Self-reception disable mode for testing
 * - Listen-Only mode for bus monitoring
 * - Programmable TX arbitration scheme
 * - Programmable error counters and error states
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 * 
 * @note    Refer to S32K1xx Reference Manual Chapter 52 (FlexCAN) for detailed information
 * @warning Clock must be enabled and pins configured before using FlexCAN module
 * 
 * @par Change Log:
 * - Version 1.0 (30/11/2025): Initial FlexCAN driver implementation
 */

#ifndef CAN_REG_H
#define CAN_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "def_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Base addresses of FlexCAN modules */
#define CAN0_BASE           (0x40024000UL)  /**< FlexCAN0 base address */
#define CAN1_BASE           (0x40025000UL)  /**< FlexCAN1 base address */
#define CAN2_BASE           (0x4002B000UL)  /**< FlexCAN2 base address */

/** @brief Number of Message Buffers per FlexCAN instance */
#define CAN_MB_COUNT        (32U)

/** @brief Number of RX FIFO ID Filter elements (when RX FIFO is enabled) */
#define CAN_RX_FIFO_ID_FILTERS  (128U)

/*******************************************************************************
 * FlexCAN Register Bit Fields
 ******************************************************************************/

/**
 * @name Module Configuration Register (MCR) Bit Fields
 * @brief MCR controls the overall operation of the FlexCAN module
 * @{
 */
#define CAN_MCR_MDIS_MASK       (0x80000000UL)  /**< Module Disable: 1=Disable, 0=Enable */
#define CAN_MCR_MDIS_SHIFT      (31U)
#define CAN_MCR_FRZ_MASK        (0x40000000UL)  /**< Freeze Enable: 1=Freeze mode enabled */
#define CAN_MCR_FRZ_SHIFT       (30U)
#define CAN_MCR_RFEN_MASK       (0x20000000UL)  /**< RX FIFO Enable: 1=RX FIFO enabled */
#define CAN_MCR_RFEN_SHIFT      (29U)
#define CAN_MCR_HALT_MASK       (0x10000000UL)  /**< Halt FlexCAN: 1=Enters Freeze mode */
#define CAN_MCR_HALT_SHIFT      (28U)
#define CAN_MCR_NOTRDY_MASK     (0x08000000UL)  /**< Not Ready: 1=FlexCAN in Disable/Freeze */
#define CAN_MCR_NOTRDY_SHIFT    (27U)
#define CAN_MCR_SOFTRST_MASK    (0x02000000UL)  /**< Soft Reset: 1=Resets FlexCAN */
#define CAN_MCR_SOFTRST_SHIFT   (25U)
#define CAN_MCR_FRZACK_MASK     (0x01000000UL)  /**< Freeze Acknowledge: 1=In Freeze mode */
#define CAN_MCR_FRZACK_SHIFT    (24U)
#define CAN_MCR_SUPV_MASK       (0x00800000UL)  /**< Supervisor Mode: 1=Supervisor access */
#define CAN_MCR_SUPV_SHIFT      (23U)
#define CAN_MCR_SRXDIS_MASK     (0x00020000UL)  /**< Self Reception Disable: 1=Disabled */
#define CAN_MCR_SRXDIS_SHIFT    (17U)
#define CAN_MCR_IRMQ_MASK       (0x00010000UL)  /**< Individual RX Masking: 1=Enabled */
#define CAN_MCR_IRMQ_SHIFT      (16U)
#define CAN_MCR_MAXMB_MASK      (0x0000007FUL)  /**< Maximum Message Buffers (0-31) */
#define CAN_MCR_MAXMB_SHIFT     (0U)
/** @} */

/**
 * @name Control 1 Register (CTRL1) Bit Fields
 * @brief CTRL1 configures bit timing and operating modes
 * @{
 */
#define CAN_CTRL1_PRESDIV_MASK  (0xFF000000UL)  /**< Prescaler Division Factor (0-255) */
#define CAN_CTRL1_PRESDIV_SHIFT (24U)
#define CAN_CTRL1_RJW_MASK      (0x00C00000UL)  /**< Resync Jump Width (0-3) */
#define CAN_CTRL1_RJW_SHIFT     (22U)
#define CAN_CTRL1_PSEG1_MASK    (0x00380000UL)  /**< Phase Segment 1 (0-7) */
#define CAN_CTRL1_PSEG1_SHIFT   (19U)
#define CAN_CTRL1_PSEG2_MASK    (0x00070000UL)  /**< Phase Segment 2 (0-7) */
#define CAN_CTRL1_PSEG2_SHIFT   (16U)
#define CAN_CTRL1_BOFFMSK_MASK  (0x00008000UL)  /**< Bus Off Interrupt Mask */
#define CAN_CTRL1_BOFFMSK_SHIFT (15U)
#define CAN_CTRL1_ERRMSK_MASK   (0x00004000UL)  /**< Error Interrupt Mask */
#define CAN_CTRL1_ERRMSK_SHIFT  (14U)
#define CAN_CTRL1_LPB_MASK      (0x00001000UL)  /**< Loopback Mode: 1=Enabled */
#define CAN_CTRL1_LPB_SHIFT     (12U)
#define CAN_CTRL1_LOM_MASK      (0x00000008UL)  /**< Listen-Only Mode: 1=Enabled */
#define CAN_CTRL1_LOM_SHIFT     (3U)
#define CAN_CTRL1_PROPSEG_MASK  (0x00000007UL)  /**< Propagation Segment (0-7) */
#define CAN_CTRL1_PROPSEG_SHIFT (0U)
/** @} */

/**
 * @name Error and Status 1 Register (ESR1) Bit Fields
 * @brief ESR1 reports error status and bus activity
 * @{
 */
#define CAN_ESR1_SYNCH_MASK     (0x00040000UL)  /**< CAN Synchronization Status */
#define CAN_ESR1_SYNCH_SHIFT    (18U)
#define CAN_ESR1_TWRNINT_MASK   (0x00020000UL)  /**< TX Warning Interrupt Flag */
#define CAN_ESR1_TWRNINT_SHIFT  (17U)
#define CAN_ESR1_RWRNINT_MASK   (0x00010000UL)  /**< RX Warning Interrupt Flag */
#define CAN_ESR1_RWRNINT_SHIFT  (16U)
#define CAN_ESR1_BIT1ERR_MASK   (0x00008000UL)  /**< Bit1 Error (recessive expected) */
#define CAN_ESR1_BIT1ERR_SHIFT  (15U)
#define CAN_ESR1_BIT0ERR_MASK   (0x00004000UL)  /**< Bit0 Error (dominant expected) */
#define CAN_ESR1_BIT0ERR_SHIFT  (14U)
#define CAN_ESR1_ACKERR_MASK    (0x00002000UL)  /**< Acknowledge Error */
#define CAN_ESR1_ACKERR_SHIFT   (13U)
#define CAN_ESR1_CRCERR_MASK    (0x00001000UL)  /**< CRC Error */
#define CAN_ESR1_CRCERR_SHIFT   (12U)
#define CAN_ESR1_FRMERR_MASK    (0x00000800UL)  /**< Form Error */
#define CAN_ESR1_FRMERR_SHIFT   (11U)
#define CAN_ESR1_STFERR_MASK    (0x00000400UL)  /**< Stuffing Error */
#define CAN_ESR1_STFERR_SHIFT   (10U)
#define CAN_ESR1_TXWRN_MASK     (0x00000200UL)  /**< TX Error Counter >= 96 */
#define CAN_ESR1_TXWRN_SHIFT    (9U)
#define CAN_ESR1_RXWRN_MASK     (0x00000100UL)  /**< RX Error Counter >= 96 */
#define CAN_ESR1_RXWRN_SHIFT    (8U)
#define CAN_ESR1_IDLE_MASK      (0x00000080UL)  /**< CAN Bus Idle State */
#define CAN_ESR1_IDLE_SHIFT     (7U)
#define CAN_ESR1_TX_MASK        (0x00000040UL)  /**< FlexCAN in Transmission */
#define CAN_ESR1_TX_SHIFT       (6U)
#define CAN_ESR1_FLTCONF_MASK   (0x00000030UL)  /**< Fault Confinement State */
#define CAN_ESR1_FLTCONF_SHIFT  (4U)
#define CAN_ESR1_RX_MASK        (0x00000008UL)  /**< FlexCAN in Reception */
#define CAN_ESR1_RX_SHIFT       (3U)
#define CAN_ESR1_BOFFINT_MASK   (0x00000004UL)  /**< Bus Off Interrupt */
#define CAN_ESR1_BOFFINT_SHIFT  (2U)
#define CAN_ESR1_ERRINT_MASK    (0x00000002UL)  /**< Error Interrupt */
#define CAN_ESR1_ERRINT_SHIFT   (1U)
/** @} */

/**
 * @name Interrupt Masks and Flags Registers
 * @{
 */
#define CAN_IMASK1_BUF31TO0M_MASK   (0xFFFFFFFFUL)  /**< Buffer MB0-MB31 Interrupt Masks */
#define CAN_IMASK1_BUF31TO0M_SHIFT  (0U)

#define CAN_IFLAG1_BUF31TO0I_MASK   (0xFFFFFFFFUL)  /**< Buffer MB0-MB31 Interrupt Flags */
#define CAN_IFLAG1_BUF31TO0I_SHIFT  (0U)
/** @} */

/**
 * @name Control 2 Register (CTRL2) Bit Fields
 * @{
 */
#define CAN_CTRL2_RFFN_MASK     (0x0F000000UL)  /**< Number of RX FIFO Filters (0-15) */
#define CAN_CTRL2_RFFN_SHIFT    (24U)
/** @} */

/**
 * @name Message Buffer TX Codes
 * @brief TX Message Buffer CODE field values
 * @{
 */
#define CAN_MB_CS_CODE_TX_INACTIVE  (0x8UL)  /**< MB not ready for transmission */
#define CAN_MB_CS_CODE_TX_ABORT     (0x9UL)  /**< MB is aborted */
#define CAN_MB_CS_CODE_TX_ONCE      (0xCUL)  /**< MB transmit once then becomes INACTIVE */
#define CAN_MB_CS_CODE_TX_REMOTE    (0xAUL)  /**< MB transmit remote frame */
/** @} */

/**
 * @name Message Buffer RX Codes
 * @brief RX Message Buffer CODE field values
 * @{
 */
#define CAN_MB_CS_CODE_RX_INACTIVE  (0x0UL)  /**< MB not active */
#define CAN_MB_CS_CODE_RX_EMPTY     (0x4UL)  /**< MB active and empty, ready to receive */
#define CAN_MB_CS_CODE_RX_FULL      (0x2UL)  /**< MB full, contains received message */
#define CAN_MB_CS_CODE_RX_OVERRUN   (0x6UL)  /**< MB was full and overrun occurred */
/** @} */

/**
 * @name Message Buffer Control and Status (CS) Field Bit Masks
 * @brief Bit fields for Message Buffer CS register
 * @{
 */
#define CAN_MB_CS_CODE_MASK     (0x0F000000UL)  /**< Message Buffer Code (4 bits) */
#define CAN_MB_CS_CODE_SHIFT    (24U)
#define CAN_MB_CS_SRR_MASK      (0x00400000UL)  /**< Substitute Remote Request (for Extended ID) */
#define CAN_MB_CS_SRR_SHIFT     (22U)
#define CAN_MB_CS_IDE_MASK      (0x00200000UL)  /**< ID Extended: 0=Standard, 1=Extended */
#define CAN_MB_CS_IDE_SHIFT     (21U)
#define CAN_MB_CS_RTR_MASK      (0x00100000UL)  /**< Remote Transmission Request */
#define CAN_MB_CS_RTR_SHIFT     (20U)
#define CAN_MB_CS_DLC_MASK      (0x000F0000UL)  /**< Data Length Code (0-8 bytes) */
#define CAN_MB_CS_DLC_SHIFT     (16U)
#define CAN_MB_CS_TIME_STAMP_MASK   (0x0000FFFFUL)  /**< Free-Running Counter Time Stamp */
#define CAN_MB_CS_TIME_STAMP_SHIFT  (0U)
/** @} */

/**
 * @name Message Buffer ID Field Bit Masks
 * @brief Bit fields for Message Buffer ID register
 * @{
 */
#define CAN_MB_ID_STD_MASK      (0x1FFC0000UL)  /**< Standard ID (11-bit) field */
#define CAN_MB_ID_STD_SHIFT     (18U)
#define CAN_MB_ID_EXT_MASK      (0x1FFFFFFFUL)  /**< Extended ID (29-bit) field */
#define CAN_MB_ID_EXT_SHIFT     (0U)
/** @} */

/**
 * @name RX FIFO Control and Status Bit Masks
 * @{
 */
#define CAN_RX_FIFO_CS_IDHIT_MASK   (0x000001FFUL)  /**< ID Hit indicator for RX FIFO */
#define CAN_RX_FIFO_CS_IDHIT_SHIFT  (0U)
/** @} */

/*******************************************************************************
 * FlexCAN Register Structures
 ******************************************************************************/

/**
 * @brief FlexCAN Message Buffer Structure
 * @details Each Message Buffer occupies 16 bytes consisting of:
 *          - CS (4 bytes): Control and Status word containing CODE, IDE, RTR, DLC, TIME_STAMP
 *          - ID (4 bytes): Message Identifier (Standard 11-bit or Extended 29-bit)
 *          - DATA (8 bytes): Payload data bytes 0-7
 */
typedef struct {
    __IO uint32_t CS;       /**< Control and Status register */
    __IO uint32_t ID;       /**< Message Buffer Identifier register */
    __IO uint8_t  DATA[8];  /**< Data bytes 0-7 */
} CAN_MB_Type;

/**
 * @brief FlexCAN RX FIFO Structure
 * @details Structure for RX FIFO output message format.
 *          When RX FIFO is enabled, MBs 0-5 are occupied by FIFO
 */
typedef struct {
    __IO uint32_t CS;       /**< Control and Status register */
    __IO uint32_t ID;       /**< Message Identifier register */
    __IO uint8_t  DATA[8];  /**< Data bytes 0-7 */
} CAN_RX_FIFO_Type;

/**
 * @brief FlexCAN Module Register Structure
 * @details Complete register map for FlexCAN module including:
 *          - Configuration registers (MCR, CTRL1, CTRL2)
 *          - Status registers (ESR1, ESR2, ECR)
 *          - Interrupt control (IMASK1, IFLAG1)
 *          - 32 Message Buffers (MB[0]-MB[31])
 *          - Individual RX Mask registers (RXIMR[0]-RXIMR[31])
 */
typedef struct {
    __IO uint32_t MCR;              /**< Module Configuration Register, offset: 0x00 */
    __IO uint32_t CTRL1;            /**< Control 1 Register, offset: 0x04 */
    __IO uint32_t TIMER;            /**< Free Running Timer, offset: 0x08 */
    uint32_t RESERVED0;             /**< Reserved, offset: 0x0C */
    __IO uint32_t RXMGMASK;         /**< RX Mailboxes Global Mask, offset: 0x10 */
    __IO uint32_t RX14MASK;         /**< RX Buffer 14 Mask, offset: 0x14 */
    __IO uint32_t RX15MASK;         /**< RX Buffer 15 Mask, offset: 0x18 */
    __IO uint32_t ECR;              /**< Error Counter Register, offset: 0x1C */
    __IO uint32_t ESR1;             /**< Error and Status 1, offset: 0x20 */
    uint32_t RESERVED1;             /**< Reserved, offset: 0x24 */
    __IO uint32_t IMASK1;           /**< Interrupt Masks 1, offset: 0x28 */
    uint32_t RESERVED2;             /**< Reserved, offset: 0x2C */
    __IO uint32_t IFLAG1;           /**< Interrupt Flags 1, offset: 0x30 */
    __IO uint32_t CTRL2;            /**< Control 2 Register, offset: 0x34 */
    __I  uint32_t ESR2;             /**< Error and Status 2, offset: 0x38 */
    uint32_t RESERVED3[2];          /**< Reserved, offset: 0x3C-0x40 */
    __I  uint32_t CRCR;             /**< CRC Register, offset: 0x44 */
    __IO uint32_t RXFGMASK;         /**< RX FIFO Global Mask, offset: 0x48 */
    __I  uint32_t RXFIR;            /**< RX FIFO Information, offset: 0x4C */
    uint32_t RESERVED4[12];         /**< Reserved, offset: 0x50-0x7C */
    CAN_MB_Type MB[CAN_MB_COUNT];   /**< Message Buffers 0-31, offset: 0x80 */
    uint32_t RESERVED5[256];        /**< Reserved, offset: 0x280-0x67C */
    __IO uint32_t RXIMR[CAN_MB_COUNT];  /**< RX Individual Mask Registers, offset: 0x880 */
} CAN_Type;

/*******************************************************************************
 * FlexCAN Instance Pointers
 ******************************************************************************/

/** @brief FlexCAN0 peripheral instance base pointer */
#define CAN0                ((CAN_Type *)CAN0_BASE)

/** @brief FlexCAN1 peripheral instance base pointer */
#define CAN1                ((CAN_Type *)CAN1_BASE)

/** @brief FlexCAN2 peripheral instance base pointer */
#define CAN2                ((CAN_Type *)CAN2_BASE)

/*******************************************************************************
 * PCC (Peripheral Clock Control) Registers for FlexCAN
 ******************************************************************************/

/** @brief PCC (Peripheral Clock Control) base address */
#define PCC_BASE            (0x40065000UL)

/**
 * @name PCC FlexCAN Register Indices
 * @brief Offset indices for FlexCAN modules in PCC register array
 * @{
 */
#define PCC_FlexCAN0_INDEX  (36U)  /**< PCC register index for FlexCAN0 */
#define PCC_FlexCAN1_INDEX  (37U)  /**< PCC register index for FlexCAN1 */
#define PCC_FlexCAN2_INDEX  (43U)  /**< PCC register index for FlexCAN2 */
/** @} */

/**
 * @name PCC Register Bit Fields
 * @{
 */
#define PCC_CGC_MASK        (0x40000000UL)  /**< Clock Gate Control: 0=Disabled, 1=Enabled */
#define PCC_CGC_SHIFT       (30U)
#define PCC_PCS_MASK        (0x07000000UL)  /**< Peripheral Clock Source Select (0-7) */
#define PCC_PCS_SHIFT       (24U)
/** @} */

/**
 * @name PCC FlexCAN Register Pointers
 * @brief Direct access pointers to PCC registers for each FlexCAN instance
 * @{
 */
#define PCC_FlexCAN0        ((__IO uint32_t *)(PCC_BASE + (PCC_FlexCAN0_INDEX * 4U)))
#define PCC_FlexCAN1        ((__IO uint32_t *)(PCC_BASE + (PCC_FlexCAN1_INDEX * 4U)))
#define PCC_FlexCAN2        ((__IO uint32_t *)(PCC_BASE + (PCC_FlexCAN2_INDEX * 4U)))
/** @} */

/*******************************************************************************
 * Clock Source Selection
 ******************************************************************************/

/**
 * @brief FlexCAN Clock Source Selection
 * @details Defines available clock sources for FlexCAN peripheral.
 *          The selected clock source determines the CAN bit timing.
 *          Common clock frequencies:
 *          - SOSC DIV2: 8 MHz / 2 = 4 MHz (if 8 MHz crystal)
 *          - SIRC DIV2: 8 MHz / 2 = 4 MHz (internal slow clock)
 *          - FIRC DIV2: 48 MHz / 2 = 24 MHz (internal fast clock)
 *          - SPLL DIV2: 80 MHz / 2 = 40 MHz (if PLL configured for 80 MHz)
 */
typedef enum {
    CAN_CLK_SRC_SOSCDIV2 = 1U,  /**< System Oscillator DIV2 clock */
    CAN_CLK_SRC_SIRCDIV2 = 2U,  /**< Slow Internal Reference Clock DIV2 */
    CAN_CLK_SRC_FIRCDIV2 = 3U,  /**< Fast Internal Reference Clock DIV2 */
    CAN_CLK_SRC_SPLLDIV2 = 6U   /**< System PLL DIV2 clock */
} can_clk_src_t;

#endif /* CAN_REG_H */
