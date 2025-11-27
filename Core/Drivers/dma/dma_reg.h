/**
 * @file    dma_reg.h
 * @brief   DMA (Direct Memory Access) Register Definitions for S32K144
 * @details
 * Định nghĩa các thanh ghi và bit field của module DMA và DMAMUX.
 * DMA cho phép truyền dữ liệu giữa bộ nhớ và thiết bị ngoại vi mà không cần CPU.
 * 
 * S32K144 có:
 * - 16 kênh DMA (DMA channels 0-15)
 * - DMAMUX để chọn nguồn request cho mỗi kênh
 * - Transfer Control Descriptor (TCD) cho mỗi kênh
 * 
 * @author  PhucPH32
 * @date    27/11/2025
 * @version 1.0
 * 
 * @note    Tham khảo S32K1xx Reference Manual Chapter 21 (eDMA) và Chapter 22 (DMAMUX)
 * @warning Phải enable clock cho DMA và DMAMUX trước khi sử dụng
 * 
 * @par Change Log:
 * - Version 1.0 (27/11/2025): Khởi tạo driver DMA
 */

#ifndef DMA_REG_H
#define DMA_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "def_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Địa chỉ base của DMA module */
#define DMA_BASE            (0x40008000UL)

/** @brief Địa chỉ base của DMAMUX module */
#define DMAMUX_BASE         (0x40021000UL)

/** @brief Số lượng kênh DMA */
#define DMA_CHANNEL_COUNT   (16U)

/*******************************************************************************
 * DMA Register Structure
 ******************************************************************************/

/**
 * @brief DMA Control Register (DMA_CR)
 * @details Thanh ghi điều khiển chung cho DMA module
 */
typedef struct {
    __IO uint32_t EDBG       : 1;   /**< Bit 0: Enable Debug - DMA hoạt động khi ở chế độ debug */
    __IO uint32_t ERCA       : 1;   /**< Bit 1: Enable Round Robin Channel Arbitration */
    __I  uint32_t RESERVED1  : 2;   /**< Bits 2-3: Reserved */
    __IO uint32_t HOE        : 1;   /**< Bit 4: Halt On Error */
    __IO uint32_t HALT       : 1;   /**< Bit 5: Halt DMA Operations */
    __IO uint32_t CLM        : 1;   /**< Bit 6: Continuous Link Mode */
    __IO uint32_t EMLM       : 1;   /**< Bit 7: Enable Minor Loop Mapping */
    __I  uint32_t RESERVED2  : 8;   /**< Bits 8-15: Reserved */
    __IO uint32_t ECX        : 1;   /**< Bit 16: Error Cancel Transfer */
    __IO uint32_t CX         : 1;   /**< Bit 17: Cancel Transfer */
    __I  uint32_t RESERVED3  : 14;  /**< Bits 18-31: Reserved */
} DMA_CR_Type;

/**
 * @brief DMA Error Status Register (DMA_ES)
 * @details Thanh ghi trạng thái lỗi
 */
typedef struct {
    __I  uint32_t DBE        : 1;   /**< Bit 0: Destination Bus Error */
    __I  uint32_t SBE        : 1;   /**< Bit 1: Source Bus Error */
    __I  uint32_t SGE        : 1;   /**< Bit 2: Scatter/Gather Configuration Error */
    __I  uint32_t NCE        : 1;   /**< Bit 3: NBYTES/CITER Configuration Error */
    __I  uint32_t DOE        : 1;   /**< Bit 4: Destination Offset Error */
    __I  uint32_t DAE        : 1;   /**< Bit 5: Destination Address Error */
    __I  uint32_t SOE        : 1;   /**< Bit 6: Source Offset Error */
    __I  uint32_t SAE        : 1;   /**< Bit 7: Source Address Error */
    __I  uint32_t ERRCHN     : 4;   /**< Bits 8-11: Error Channel Number */
    __I  uint32_t RESERVED1  : 2;   /**< Bits 12-13: Reserved */
    __I  uint32_t CPE        : 1;   /**< Bit 14: Channel Priority Error */
    __I  uint32_t RESERVED2  : 1;   /**< Bit 15: Reserved */
    __I  uint32_t ECX        : 1;   /**< Bit 16: Transfer Cancelled */
    __I  uint32_t RESERVED3  : 14;  /**< Bits 17-30: Reserved */
    __I  uint32_t VLD        : 1;   /**< Bit 31: Logical OR of all ERR status bits */
} DMA_ES_Type;

/**
 * @brief Transfer Control Descriptor (TCD)
 * @details
 * Mỗi kênh DMA có một TCD riêng để cấu hình transfer:
 * - Source/Destination address
 * - Transfer size và offset
 * - Loop count (major/minor)
 * - Control flags (interrupts, linking, etc.)
 */
typedef struct {
    __IO uint32_t SADDR;            /**< Source Address - Địa chỉ nguồn */
    __IO uint16_t SOFF;             /**< Source Offset - Bước nhảy sau mỗi minor loop */
    __IO uint16_t ATTR;             /**< Transfer Attributes - Kích thước dữ liệu source/dest */
    union {
        __IO uint32_t NBYTES_MLNO;  /**< Minor Byte Count (no link) */
        __IO uint32_t NBYTES_MLOFFNO; /**< Minor Byte Count (with offset, no link) */
        __IO uint32_t NBYTES_MLOFFYES; /**< Minor Byte Count (with offset and link) */
    };
    __IO uint32_t SLAST;            /**< Last Source Address Adjustment - Điều chỉnh địa chỉ sau major loop */
    __IO uint32_t DADDR;            /**< Destination Address - Địa chỉ đích */
    __IO uint16_t DOFF;             /**< Destination Offset - Bước nhảy đích sau mỗi minor loop */
    union {
        __IO uint16_t CITER_ELINKNO;  /**< Current Major Loop Count (no link) */
        __IO uint16_t CITER_ELINKYES; /**< Current Major Loop Count (with link) */
    };
    __IO uint32_t DLAST_SGA;        /**< Last Destination Address Adjustment / Scatter Gather Address */
    __IO uint16_t CSR;              /**< Control and Status - Các cờ điều khiển và trạng thái */
    union {
        __IO uint16_t BITER_ELINKNO;  /**< Beginning Major Loop Count (no link) */
        __IO uint16_t BITER_ELINKYES; /**< Beginning Major Loop Count (with link) */
    };
} DMA_TCD_Type;

/**
 * @brief DMA Module Structure
 * @details Cấu trúc đầy đủ của DMA module bao gồm các thanh ghi điều khiển và 16 TCD
 */
typedef struct {
    __IO uint32_t CR;               /**< 0x0000: Control Register */
    __I  uint32_t ES;               /**< 0x0004: Error Status Register */
    __I  uint32_t RESERVED0;        /**< 0x0008: Reserved */
    __IO uint32_t ERQ;              /**< 0x000C: Enable Request Register */
    __I  uint32_t RESERVED1;        /**< 0x0010: Reserved */
    __IO uint32_t EEI;              /**< 0x0014: Enable Error Interrupt Register */
    __O  uint8_t  CEEI;             /**< 0x0018: Clear Enable Error Interrupt Register */
    __O  uint8_t  SEEI;             /**< 0x0019: Set Enable Error Interrupt Register */
    __O  uint8_t  CERQ;             /**< 0x001A: Clear Enable Request Register */
    __O  uint8_t  SERQ;             /**< 0x001B: Set Enable Request Register */
    __O  uint8_t  CDNE;             /**< 0x001C: Clear DONE Status Bit Register */
    __O  uint8_t  SSRT;             /**< 0x001D: Set START Bit Register */
    __O  uint8_t  CERR;             /**< 0x001E: Clear Error Register */
    __O  uint8_t  CINT;             /**< 0x001F: Clear Interrupt Request Register */
    __I  uint32_t RESERVED2;        /**< 0x0020: Reserved */
    __IO uint32_t INT;              /**< 0x0024: Interrupt Request Register */
    __I  uint32_t RESERVED3;        /**< 0x0028: Reserved */
    __IO uint32_t ERR;              /**< 0x002C: Error Register */
    __I  uint32_t RESERVED4;        /**< 0x0030: Reserved */
    __I  uint32_t HRS;              /**< 0x0034: Hardware Request Status Register */
    __I  uint32_t RESERVED5[3];     /**< 0x0038-0x0040: Reserved */
    __IO uint32_t EARS;             /**< 0x0044: Enable Asynchronous Request in Stop Register */
    __I  uint32_t RESERVED6[46];    /**< 0x0048-0x00FC: Reserved */
    __IO uint8_t  DCHPRI[16];       /**< 0x0100-0x010F: Channel Priority Registers */
    __I  uint32_t RESERVED7[956];   /**< 0x0110-0x0FFC: Reserved */
    DMA_TCD_Type  TCD[16];          /**< 0x1000-0x11FF: Transfer Control Descriptors (16 channels) */
} DMA_Type;

/*******************************************************************************
 * DMAMUX Register Structure
 ******************************************************************************/

/**
 * @brief DMAMUX Channel Configuration Register
 * @details
 * Mỗi kênh DMA có một thanh ghi CHCFG trong DMAMUX để:
 * - Enable/disable kênh
 * - Chọn nguồn request (peripheral hoặc always-on)
 * - Enable periodic trigger
 */
typedef struct {
    __IO uint8_t SOURCE      : 6;   /**< Bits 0-5: DMA Channel Source (Slot) */
    __IO uint8_t TRIG        : 1;   /**< Bit 6: DMA Channel Trigger Enable */
    __IO uint8_t ENBL        : 1;   /**< Bit 7: DMA Channel Enable */
} DMAMUX_CHCFG_Type;

/**
 * @brief DMAMUX Module Structure
 * @details 16 thanh ghi CHCFG, mỗi thanh ghi cho một kênh DMA
 */
typedef struct {
    __IO uint8_t CHCFG[16];         /**< Channel Configuration Registers (0-15) */
} DMAMUX_Type;

/*******************************************************************************
 * Register Access Macros
 ******************************************************************************/

/** @brief DMA module pointer */
#define DMA                 ((DMA_Type *)DMA_BASE)

/** @brief DMAMUX module pointer */
#define DMAMUX              ((DMAMUX_Type *)DMAMUX_BASE)

/*******************************************************************************
 * DMA Control Register (CR) Bit Definitions
 ******************************************************************************/

/** @brief Enable Debug - DMA hoạt động khi CPU ở chế độ debug */
#define DMA_CR_EDBG_MASK    (0x00000001UL)
#define DMA_CR_EDBG_SHIFT   (0U)

/** @brief Enable Round Robin Channel Arbitration - Ưu tiên các kênh theo vòng tròn */
#define DMA_CR_ERCA_MASK    (0x00000002UL)
#define DMA_CR_ERCA_SHIFT   (1U)

/** @brief Halt On Error - Dừng DMA khi có lỗi */
#define DMA_CR_HOE_MASK     (0x00000010UL)
#define DMA_CR_HOE_SHIFT    (4U)

/** @brief Halt DMA Operations - Dừng tất cả hoạt động DMA */
#define DMA_CR_HALT_MASK    (0x00000020UL)
#define DMA_CR_HALT_SHIFT   (5U)

/** @brief Continuous Link Mode - Chế độ liên kết liên tục */
#define DMA_CR_CLM_MASK     (0x00000040UL)
#define DMA_CR_CLM_SHIFT    (6U)

/** @brief Enable Minor Loop Mapping - Cho phép offset trong minor loop */
#define DMA_CR_EMLM_MASK    (0x00000080UL)
#define DMA_CR_EMLM_SHIFT   (7U)

/*******************************************************************************
 * TCD Register Bit Definitions
 ******************************************************************************/

/** @brief TCD ATTR - Transfer Attributes */
#define DMA_TCD_ATTR_DSIZE_SHIFT    (0U)    /**< Destination data transfer size */
#define DMA_TCD_ATTR_DSIZE_MASK     (0x0007U)
#define DMA_TCD_ATTR_DMOD_SHIFT     (3U)    /**< Destination Address Modulo */
#define DMA_TCD_ATTR_DMOD_MASK      (0x00F8U)
#define DMA_TCD_ATTR_SSIZE_SHIFT    (8U)    /**< Source data transfer size */
#define DMA_TCD_ATTR_SSIZE_MASK     (0x0700U)
#define DMA_TCD_ATTR_SMOD_SHIFT     (11U)   /**< Source Address Modulo */
#define DMA_TCD_ATTR_SMOD_MASK      (0xF800U)

/** @brief TCD CSR - Control and Status Register */
#define DMA_TCD_CSR_START_MASK      (0x0001U) /**< Channel Start */
#define DMA_TCD_CSR_INTMAJOR_MASK   (0x0002U) /**< Enable interrupt when major loop complete */
#define DMA_TCD_CSR_INTHALF_MASK    (0x0004U) /**< Enable interrupt when major half complete */
#define DMA_TCD_CSR_DREQ_MASK       (0x0008U) /**< Disable Request after major loop complete */
#define DMA_TCD_CSR_ESG_MASK        (0x0010U) /**< Enable Scatter/Gather Processing */
#define DMA_TCD_CSR_MAJORELINK_MASK (0x0020U) /**< Enable channel-to-channel linking on major loop complete */
#define DMA_TCD_CSR_ACTIVE_MASK     (0x0040U) /**< Channel Active (read-only) */
#define DMA_TCD_CSR_DONE_MASK       (0x0080U) /**< Channel Done */
#define DMA_TCD_CSR_MAJORLINKCH_SHIFT (8U)    /**< Link Channel Number */
#define DMA_TCD_CSR_MAJORLINKCH_MASK  (0x0F00U)
#define DMA_TCD_CSR_BWC_SHIFT       (14U)     /**< Bandwidth Control */
#define DMA_TCD_CSR_BWC_MASK        (0xC000U)

/*******************************************************************************
 * DMAMUX Register Bit Definitions
 ******************************************************************************/

/** @brief DMAMUX Channel Configuration */
#define DMAMUX_CHCFG_SOURCE_SHIFT   (0U)    /**< DMA Channel Source */
#define DMAMUX_CHCFG_SOURCE_MASK    (0x3FU)
#define DMAMUX_CHCFG_TRIG_SHIFT     (6U)    /**< DMA Channel Trigger Enable */
#define DMAMUX_CHCFG_TRIG_MASK      (0x40U)
#define DMAMUX_CHCFG_ENBL_SHIFT     (7U)    /**< DMA Channel Enable */
#define DMAMUX_CHCFG_ENBL_MASK      (0x80U)

/*******************************************************************************
 * DMAMUX Source Definitions (S32K144)
 ******************************************************************************/

/** @brief DMAMUX Sources - Các nguồn request cho DMA */
typedef enum {
    DMAMUX_SRC_DISABLED         = 0U,   /**< DMA channel disabled */
    DMAMUX_SRC_LPUART0_RX       = 2U,   /**< LPUART0 Receive */
    DMAMUX_SRC_LPUART0_TX       = 3U,   /**< LPUART0 Transmit */
    DMAMUX_SRC_LPUART1_RX       = 4U,   /**< LPUART1 Receive */
    DMAMUX_SRC_LPUART1_TX       = 5U,   /**< LPUART1 Transmit */
    DMAMUX_SRC_LPUART2_RX       = 6U,   /**< LPUART2 Receive */
    DMAMUX_SRC_LPUART2_TX       = 7U,   /**< LPUART2 Transmit */
    DMAMUX_SRC_FLEXIO_SHFT0     = 10U,  /**< FlexIO Shifter 0 */
    DMAMUX_SRC_FLEXIO_SHFT1     = 11U,  /**< FlexIO Shifter 1 */
    DMAMUX_SRC_FLEXIO_SHFT2     = 12U,  /**< FlexIO Shifter 2 */
    DMAMUX_SRC_FLEXIO_SHFT3     = 13U,  /**< FlexIO Shifter 3 */
    DMAMUX_SRC_LPSPI0_RX        = 14U,  /**< LPSPI0 Receive */
    DMAMUX_SRC_LPSPI0_TX        = 15U,  /**< LPSPI0 Transmit */
    DMAMUX_SRC_LPSPI1_RX        = 16U,  /**< LPSPI1 Receive */
    DMAMUX_SRC_LPSPI1_TX        = 17U,  /**< LPSPI1 Transmit */
    DMAMUX_SRC_LPSPI2_RX        = 18U,  /**< LPSPI2 Receive */
    DMAMUX_SRC_LPSPI2_TX        = 19U,  /**< LPSPI2 Transmit */
    DMAMUX_SRC_FTM0_CH0         = 20U,  /**< FTM0 Channel 0 */
    DMAMUX_SRC_FTM0_CH1         = 21U,  /**< FTM0 Channel 1 */
    DMAMUX_SRC_FTM0_CH2         = 22U,  /**< FTM0 Channel 2 */
    DMAMUX_SRC_FTM0_CH3         = 23U,  /**< FTM0 Channel 3 */
    DMAMUX_SRC_FTM0_CH4         = 24U,  /**< FTM0 Channel 4 */
    DMAMUX_SRC_FTM0_CH5         = 25U,  /**< FTM0 Channel 5 */
    DMAMUX_SRC_FTM0_CH6         = 26U,  /**< FTM0 Channel 6 */
    DMAMUX_SRC_FTM0_CH7         = 27U,  /**< FTM0 Channel 7 */
    DMAMUX_SRC_FTM1_CH0         = 28U,  /**< FTM1 Channel 0 */
    DMAMUX_SRC_FTM1_CH1         = 29U,  /**< FTM1 Channel 1 */
    DMAMUX_SRC_FTM2_CH0         = 30U,  /**< FTM2 Channel 0 */
    DMAMUX_SRC_FTM2_CH1         = 31U,  /**< FTM2 Channel 1 */
    DMAMUX_SRC_FTM3_CH0         = 32U,  /**< FTM3 Channel 0 */
    DMAMUX_SRC_FTM3_CH1         = 33U,  /**< FTM3 Channel 1 */
    DMAMUX_SRC_FTM3_CH2         = 34U,  /**< FTM3 Channel 2 */
    DMAMUX_SRC_FTM3_CH3         = 35U,  /**< FTM3 Channel 3 */
    DMAMUX_SRC_FTM3_CH4         = 36U,  /**< FTM3 Channel 4 */
    DMAMUX_SRC_FTM3_CH5         = 37U,  /**< FTM3 Channel 5 */
    DMAMUX_SRC_FTM3_CH6         = 38U,  /**< FTM3 Channel 6 */
    DMAMUX_SRC_FTM3_CH7         = 39U,  /**< FTM3 Channel 7 */
    DMAMUX_SRC_ADC0             = 42U,  /**< ADC0 Conversion Complete */
    DMAMUX_SRC_ADC1             = 43U,  /**< ADC1 Conversion Complete */
    DMAMUX_SRC_LPI2C0_RX        = 44U,  /**< LPI2C0 Receive */
    DMAMUX_SRC_LPI2C0_TX        = 45U,  /**< LPI2C0 Transmit */
    DMAMUX_SRC_PDB0             = 46U,  /**< PDB0 Pulse-Out */
    DMAMUX_SRC_PDB1             = 47U,  /**< PDB1 Pulse-Out */
    DMAMUX_SRC_CMP0             = 48U,  /**< CMP0 Output */
    DMAMUX_SRC_PORTA            = 49U,  /**< PORTA Pin */
    DMAMUX_SRC_PORTB            = 50U,  /**< PORTB Pin */
    DMAMUX_SRC_PORTC            = 51U,  /**< PORTC Pin */
    DMAMUX_SRC_PORTD            = 52U,  /**< PORTD Pin */
    DMAMUX_SRC_PORTE            = 53U,  /**< PORTE Pin */
    DMAMUX_SRC_FLEXCAN0         = 54U,  /**< FlexCAN0 */
    DMAMUX_SRC_FLEXCAN1         = 55U,  /**< FlexCAN1 */
    DMAMUX_SRC_FLEXCAN2         = 56U,  /**< FlexCAN2 */
    DMAMUX_SRC_LPTMR0           = 59U,  /**< LPTMR0 */
    DMAMUX_SRC_ALWAYS_ON_60     = 60U,  /**< Always enabled slot 60 */
    DMAMUX_SRC_ALWAYS_ON_61     = 61U,  /**< Always enabled slot 61 */
    DMAMUX_SRC_ALWAYS_ON_62     = 62U,  /**< Always enabled slot 62 */
    DMAMUX_SRC_ALWAYS_ON_63     = 63U   /**< Always enabled slot 63 */
} dmamux_source_t;

#endif /* DMA_REG_H */
