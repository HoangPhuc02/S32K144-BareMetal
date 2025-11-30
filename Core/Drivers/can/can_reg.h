/**
 * @file    can_reg.h
 * @brief   FlexCAN Register Definitions for S32K144
 * @details
 * Định nghĩa các thanh ghi và bit field của module FlexCAN.
 * FlexCAN là CAN controller hỗ trợ CAN 2.0A/B protocol.
 * 
 * S32K144 có 3 FlexCAN modules:
 * - FlexCAN0: 32 Message Buffers
 * - FlexCAN1: 32 Message Buffers  
 * - FlexCAN2: 32 Message Buffers
 * 
 * Tính năng chính:
 * - Hỗ trợ CAN 2.0A (Standard ID 11-bit) và 2.0B (Extended ID 29-bit)
 * - Bitrate lên đến 1 Mbps
 * - 32 Message Buffers cho TX/RX
 * - Hardware filtering với RX FIFO hoặc Individual Mask
 * - Self-reception disable mode
 * - Listen-Only mode
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 * 
 * @note    Tham khảo S32K1xx Reference Manual Chapter 52 (FlexCAN)
 * @warning Phải enable clock cho FlexCAN và configure pins trước khi sử dụng
 * 
 * @par Change Log:
 * - Version 1.0 (30/11/2025): Khởi tạo driver FlexCAN
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

/** @brief Địa chỉ base của FlexCAN modules */
#define CAN0_BASE           (0x40024000UL)
#define CAN1_BASE           (0x40025000UL)
#define CAN2_BASE           (0x4002B000UL)

/** @brief Số lượng Message Buffers */
#define CAN_MB_COUNT        (32U)

/** @brief Số lượng RX FIFO ID Filters */
#define CAN_RX_FIFO_ID_FILTERS  (128U)

/*******************************************************************************
 * FlexCAN Register Bit Fields
 ******************************************************************************/

/* Module Configuration Register (MCR) */
#define CAN_MCR_MDIS_MASK       (0x80000000UL)
#define CAN_MCR_MDIS_SHIFT      (31U)
#define CAN_MCR_FRZ_MASK        (0x40000000UL)
#define CAN_MCR_FRZ_SHIFT       (30U)
#define CAN_MCR_RFEN_MASK       (0x20000000UL)
#define CAN_MCR_RFEN_SHIFT      (29U)
#define CAN_MCR_HALT_MASK       (0x10000000UL)
#define CAN_MCR_HALT_SHIFT      (28U)
#define CAN_MCR_NOTRDY_MASK     (0x08000000UL)
#define CAN_MCR_NOTRDY_SHIFT    (27U)
#define CAN_MCR_SOFTRST_MASK    (0x02000000UL)
#define CAN_MCR_SOFTRST_SHIFT   (25U)
#define CAN_MCR_FRZACK_MASK     (0x01000000UL)
#define CAN_MCR_FRZACK_SHIFT    (24U)
#define CAN_MCR_SUPV_MASK       (0x00800000UL)
#define CAN_MCR_SUPV_SHIFT      (23U)
#define CAN_MCR_SRXDIS_MASK     (0x00020000UL)
#define CAN_MCR_SRXDIS_SHIFT    (17U)
#define CAN_MCR_IRMQ_MASK       (0x00010000UL)
#define CAN_MCR_IRMQ_SHIFT      (16U)
#define CAN_MCR_MAXMB_MASK      (0x0000007FUL)
#define CAN_MCR_MAXMB_SHIFT     (0U)

/* Control 1 Register (CTRL1) */
#define CAN_CTRL1_PRESDIV_MASK  (0xFF000000UL)
#define CAN_CTRL1_PRESDIV_SHIFT (24U)
#define CAN_CTRL1_RJW_MASK      (0x00C00000UL)
#define CAN_CTRL1_RJW_SHIFT     (22U)
#define CAN_CTRL1_PSEG1_MASK    (0x00380000UL)
#define CAN_CTRL1_PSEG1_SHIFT   (19U)
#define CAN_CTRL1_PSEG2_MASK    (0x00070000UL)
#define CAN_CTRL1_PSEG2_SHIFT   (16U)
#define CAN_CTRL1_BOFFMSK_MASK  (0x00008000UL)
#define CAN_CTRL1_BOFFMSK_SHIFT (15U)
#define CAN_CTRL1_ERRMSK_MASK   (0x00004000UL)
#define CAN_CTRL1_ERRMSK_SHIFT  (14U)
#define CAN_CTRL1_LPB_MASK      (0x00001000UL)
#define CAN_CTRL1_LPB_SHIFT     (12U)
#define CAN_CTRL1_LOM_MASK      (0x00000008UL)
#define CAN_CTRL1_LOM_SHIFT     (3U)
#define CAN_CTRL1_PROPSEG_MASK  (0x00000007UL)
#define CAN_CTRL1_PROPSEG_SHIFT (0U)

/* Error and Status 1 Register (ESR1) */
#define CAN_ESR1_SYNCH_MASK     (0x00040000UL)
#define CAN_ESR1_SYNCH_SHIFT    (18U)
#define CAN_ESR1_TWRNINT_MASK   (0x00020000UL)
#define CAN_ESR1_TWRNINT_SHIFT  (17U)
#define CAN_ESR1_RWRNINT_MASK   (0x00010000UL)
#define CAN_ESR1_RWRNINT_SHIFT  (16U)
#define CAN_ESR1_BIT1ERR_MASK   (0x00008000UL)
#define CAN_ESR1_BIT1ERR_SHIFT  (15U)
#define CAN_ESR1_BIT0ERR_MASK   (0x00004000UL)
#define CAN_ESR1_BIT0ERR_SHIFT  (14U)
#define CAN_ESR1_ACKERR_MASK    (0x00002000UL)
#define CAN_ESR1_ACKERR_SHIFT   (13U)
#define CAN_ESR1_CRCERR_MASK    (0x00001000UL)
#define CAN_ESR1_CRCERR_SHIFT   (12U)
#define CAN_ESR1_FRMERR_MASK    (0x00000800UL)
#define CAN_ESR1_FRMERR_SHIFT   (11U)
#define CAN_ESR1_STFERR_MASK    (0x00000400UL)
#define CAN_ESR1_STFERR_SHIFT   (10U)
#define CAN_ESR1_TXWRN_MASK     (0x00000200UL)
#define CAN_ESR1_TXWRN_SHIFT    (9U)
#define CAN_ESR1_RXWRN_MASK     (0x00000100UL)
#define CAN_ESR1_RXWRN_SHIFT    (8U)
#define CAN_ESR1_IDLE_MASK      (0x00000080UL)
#define CAN_ESR1_IDLE_SHIFT     (7U)
#define CAN_ESR1_TX_MASK        (0x00000040UL)
#define CAN_ESR1_TX_SHIFT       (6U)
#define CAN_ESR1_FLTCONF_MASK   (0x00000030UL)
#define CAN_ESR1_FLTCONF_SHIFT  (4U)
#define CAN_ESR1_RX_MASK        (0x00000008UL)
#define CAN_ESR1_RX_SHIFT       (3U)
#define CAN_ESR1_BOFFINT_MASK   (0x00000004UL)
#define CAN_ESR1_BOFFINT_SHIFT  (2U)
#define CAN_ESR1_ERRINT_MASK    (0x00000002UL)
#define CAN_ESR1_ERRINT_SHIFT   (1U)

/* Interrupt Masks Register (IMASK1) */
#define CAN_IMASK1_BUF31TO0M_MASK   (0xFFFFFFFFUL)
#define CAN_IMASK1_BUF31TO0M_SHIFT  (0U)

/* Interrupt Flags Register (IFLAG1) */
#define CAN_IFLAG1_BUF31TO0I_MASK   (0xFFFFFFFFUL)
#define CAN_IFLAG1_BUF31TO0I_SHIFT  (0U)

/* Control 2 Register (CTRL2) */
#define CAN_CTRL2_RFFN_MASK     (0x0F000000UL)
#define CAN_CTRL2_RFFN_SHIFT    (24U)

/* Message Buffer Code for TX */
#define CAN_MB_CS_CODE_TX_INACTIVE  (0x8UL)
#define CAN_MB_CS_CODE_TX_ABORT     (0x9UL)
#define CAN_MB_CS_CODE_TX_ONCE      (0xCUL)
#define CAN_MB_CS_CODE_TX_REMOTE    (0xAUL)

/* Message Buffer Code for RX */
#define CAN_MB_CS_CODE_RX_INACTIVE  (0x0UL)
#define CAN_MB_CS_CODE_RX_EMPTY     (0x4UL)
#define CAN_MB_CS_CODE_RX_FULL      (0x2UL)
#define CAN_MB_CS_CODE_RX_OVERRUN   (0x6UL)

/* Message Buffer Control and Status */
#define CAN_MB_CS_CODE_MASK     (0x0F000000UL)
#define CAN_MB_CS_CODE_SHIFT    (24U)
#define CAN_MB_CS_SRR_MASK      (0x00400000UL)
#define CAN_MB_CS_SRR_SHIFT     (22U)
#define CAN_MB_CS_IDE_MASK      (0x00200000UL)
#define CAN_MB_CS_IDE_SHIFT     (21U)
#define CAN_MB_CS_RTR_MASK      (0x00100000UL)
#define CAN_MB_CS_RTR_SHIFT     (20U)
#define CAN_MB_CS_DLC_MASK      (0x000F0000UL)
#define CAN_MB_CS_DLC_SHIFT     (16U)
#define CAN_MB_CS_TIME_STAMP_MASK   (0x0000FFFFUL)
#define CAN_MB_CS_TIME_STAMP_SHIFT  (0U)

/* Message Buffer ID */
#define CAN_MB_ID_STD_MASK      (0x1FFC0000UL)
#define CAN_MB_ID_STD_SHIFT     (18U)
#define CAN_MB_ID_EXT_MASK      (0x1FFFFFFFUL)
#define CAN_MB_ID_EXT_SHIFT     (0U)

/* RX FIFO Control and Status */
#define CAN_RX_FIFO_CS_IDHIT_MASK   (0x000001FFUL)
#define CAN_RX_FIFO_CS_IDHIT_SHIFT  (0U)

/*******************************************************************************
 * FlexCAN Register Structures
 ******************************************************************************/

/**
 * @brief FlexCAN Message Buffer Structure
 * @details Mỗi Message Buffer 16 bytes gồm CS, ID, và 8 bytes data
 */
typedef struct {
    __IO uint32_t CS;       /**< Control and Status */
    __IO uint32_t ID;       /**< Message Buffer ID */
    __IO uint8_t  DATA[8];  /**< Data bytes 0-7 */
} CAN_MB_Type;

/**
 * @brief FlexCAN RX FIFO Structure
 * @details Structure cho RX FIFO message
 */
typedef struct {
    __IO uint32_t CS;       /**< Control and Status */
    __IO uint32_t ID;       /**< Message ID */
    __IO uint8_t  DATA[8];  /**< Data bytes 0-7 */
} CAN_RX_FIFO_Type;

/**
 * @brief FlexCAN Module Register Structure
 * @details Tất cả các thanh ghi của FlexCAN module
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

/** @brief FlexCAN0 base pointer */
#define CAN0                ((CAN_Type *)CAN0_BASE)

/** @brief FlexCAN1 base pointer */
#define CAN1                ((CAN_Type *)CAN1_BASE)

/** @brief FlexCAN2 base pointer */
#define CAN2                ((CAN_Type *)CAN2_BASE)

/*******************************************************************************
 * PCC Register for FlexCAN Clock Control
 ******************************************************************************/

/** @brief PCC base address */
#define PCC_BASE            (0x40065000UL)

/** @brief PCC FlexCAN0 offset */
#define PCC_FlexCAN0_INDEX  (36U)
#define PCC_FlexCAN1_INDEX  (37U)
#define PCC_FlexCAN2_INDEX  (43U)

/** @brief PCC register bit fields */
#define PCC_CGC_MASK        (0x40000000UL)
#define PCC_CGC_SHIFT       (30U)
#define PCC_PCS_MASK        (0x07000000UL)
#define PCC_PCS_SHIFT       (24U)

/** @brief PCC FlexCAN register addresses */
#define PCC_FlexCAN0        ((__IO uint32_t *)(PCC_BASE + (PCC_FlexCAN0_INDEX * 4U)))
#define PCC_FlexCAN1        ((__IO uint32_t *)(PCC_BASE + (PCC_FlexCAN1_INDEX * 4U)))
#define PCC_FlexCAN2        ((__IO uint32_t *)(PCC_BASE + (PCC_FlexCAN2_INDEX * 4U)))

/*******************************************************************************
 * Clock Source Selection
 ******************************************************************************/

/**
 * @brief FlexCAN Clock Source Selection
 */
typedef enum {
    CAN_CLK_SRC_SOSCDIV2 = 1U,  /**< SOSC DIV2 clock */
    CAN_CLK_SRC_SIRCDIV2 = 2U,  /**< SIRC DIV2 clock */
    CAN_CLK_SRC_FIRCDIV2 = 3U,  /**< FIRC DIV2 clock */
    CAN_CLK_SRC_SPLLDIV2 = 6U   /**< SPLL DIV2 clock */
} can_clk_src_t;

#endif /* CAN_REG_H */
