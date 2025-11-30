/**
 * @file    nvic_reg.h
 * @brief   NVIC Register Definitions for ARM Cortex-M4
 * @details
 * Định nghĩa các thanh ghi NVIC (Nested Vectored Interrupt Controller).
 * NVIC quản lý tất cả interrupts trong ARM Cortex-M4.
 * 
 * S32K144 có:
 * - 240 interrupt sources
 * - 16 priority levels (4-bit priority)
 * - Nested interrupt support
 * - Interrupt enable/disable
 * - Pending interrupt management
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 * 
 * @note    Tham khảo ARM Cortex-M4 Generic User Guide
 * @warning Phải cẩn thận khi thay đổi priority và enable/disable interrupt
 */

#ifndef NVIC_REG_H
#define NVIC_REG_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "def_reg.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief NVIC base address */
#define NVIC_BASE           (0xE000E100UL)

/** @brief SCB (System Control Block) base address */
#define SCB_BASE            (0xE000ED00UL)

/** @brief Maximum number of interrupt sources */
#define NVIC_NUM_INTERRUPTS (240U)

/** @brief Number of priority bits implemented */
#define NVIC_PRIORITY_BITS  (4U)

/*******************************************************************************
 * S32K144 Interrupt Numbers (IRQn)
 ******************************************************************************/

/**
 * @brief S32K144 Interrupt Number Definition
 */
typedef enum {
    /* Cortex-M4 Processor Exceptions */
    NonMaskableInt_IRQn         = -14,  /**< Non Maskable Interrupt */
    HardFault_IRQn              = -13,  /**< Cortex-M4 Hard Fault Interrupt */
    MemoryManagement_IRQn       = -12,  /**< Cortex-M4 Memory Management Interrupt */
    BusFault_IRQn               = -11,  /**< Cortex-M4 Bus Fault Interrupt */
    UsageFault_IRQn             = -10,  /**< Cortex-M4 Usage Fault Interrupt */
    SVCall_IRQn                 = -5,   /**< Cortex-M4 SV Call Interrupt */
    DebugMonitor_IRQn           = -4,   /**< Cortex-M4 Debug Monitor Interrupt */
    PendSV_IRQn                 = -2,   /**< Cortex-M4 Pend SV Interrupt */
    SysTick_IRQn                = -1,   /**< Cortex-M4 System Tick Interrupt */
    
    /* S32K144 Peripheral Interrupts */
    DMA0_IRQn                   = 0,    /**< DMA channel 0 transfer complete */
    DMA1_IRQn                   = 1,    /**< DMA channel 1 transfer complete */
    DMA2_IRQn                   = 2,    /**< DMA channel 2 transfer complete */
    DMA3_IRQn                   = 3,    /**< DMA channel 3 transfer complete */
    DMA4_IRQn                   = 4,    /**< DMA channel 4 transfer complete */
    DMA5_IRQn                   = 5,    /**< DMA channel 5 transfer complete */
    DMA6_IRQn                   = 6,    /**< DMA channel 6 transfer complete */
    DMA7_IRQn                   = 7,    /**< DMA channel 7 transfer complete */
    DMA8_IRQn                   = 8,    /**< DMA channel 8 transfer complete */
    DMA9_IRQn                   = 9,    /**< DMA channel 9 transfer complete */
    DMA10_IRQn                  = 10,   /**< DMA channel 10 transfer complete */
    DMA11_IRQn                  = 11,   /**< DMA channel 11 transfer complete */
    DMA12_IRQn                  = 12,   /**< DMA channel 12 transfer complete */
    DMA13_IRQn                  = 13,   /**< DMA channel 13 transfer complete */
    DMA14_IRQn                  = 14,   /**< DMA channel 14 transfer complete */
    DMA15_IRQn                  = 15,   /**< DMA channel 15 transfer complete */
    DMA_Error_IRQn              = 16,   /**< DMA error interrupt */
    MCM_IRQn                    = 17,   /**< MCM interrupt */
    FTFC_IRQn                   = 18,   /**< FTFC command complete */
    Read_Collision_IRQn         = 19,   /**< FTFC read collision */
    LVD_LVW_IRQn                = 20,   /**< PMC Low-voltage detect, low-voltage warning */
    FTFC_Fault_IRQn             = 21,   /**< FTFC double bit fault detect */
    WDOG_EWM_IRQn               = 22,   /**< WDOG or EWM interrupt */
    RCM_IRQn                    = 23,   /**< RCM Asynchronous Interrupt */
    LPI2C0_Master_IRQn          = 24,   /**< LPI2C0 Master Interrupt */
    LPI2C0_Slave_IRQn           = 25,   /**< LPI2C0 Slave Interrupt */
    LPSPI0_IRQn                 = 26,   /**< LPSPI0 Interrupt */
    LPSPI1_IRQn                 = 27,   /**< LPSPI1 Interrupt */
    LPSPI2_IRQn                 = 28,   /**< LPSPI2 Interrupt */
    LPUART0_RxTx_IRQn           = 31,   /**< LPUART0 Transmit / Receive Interrupt */
    LPUART1_RxTx_IRQn           = 33,   /**< LPUART1 Transmit / Receive Interrupt */
    LPUART2_RxTx_IRQn           = 35,   /**< LPUART2 Transmit / Receive Interrupt */
    ADC0_IRQn                   = 39,   /**< ADC0 interrupt */
    ADC1_IRQn                   = 40,   /**< ADC1 interrupt */
    CMP0_IRQn                   = 41,   /**< CMP0 interrupt */
    ERM_single_fault_IRQn       = 44,   /**< ERM single bit error correction */
    ERM_double_fault_IRQn       = 45,   /**< ERM double bit error non-correctable */
    RTC_IRQn                    = 46,   /**< RTC alarm interrupt */
    RTC_Seconds_IRQn            = 47,   /**< RTC seconds interrupt */
    LPIT0_Ch0_IRQn              = 48,   /**< LPIT0 channel 0 overflow interrupt */
    LPIT0_Ch1_IRQn              = 49,   /**< LPIT0 channel 1 overflow interrupt */
    LPIT0_Ch2_IRQn              = 50,   /**< LPIT0 channel 2 overflow interrupt */
    LPIT0_Ch3_IRQn              = 51,   /**< LPIT0 channel 3 overflow interrupt */
    PDB0_IRQn                   = 52,   /**< PDB0 interrupt */
    SCG_IRQn                    = 57,   /**< SCG bus interrupt request */
    LPTMR0_IRQn                 = 58,   /**< LPTIMER interrupt request */
    PORTA_IRQn                  = 59,   /**< Port A pin detect interrupt */
    PORTB_IRQn                  = 60,   /**< Port B pin detect interrupt */
    PORTC_IRQn                  = 61,   /**< Port C pin detect interrupt */
    PORTD_IRQn                  = 62,   /**< Port D pin detect interrupt */
    PORTE_IRQn                  = 63,   /**< Port E pin detect interrupt */
    SWI_IRQn                    = 64,   /**< Software interrupt */
    PDB1_IRQn                   = 68,   /**< PDB1 interrupt */
    FLEXIO_IRQn                 = 69,   /**< FlexIO interrupt */
    CAN0_ORed_IRQn              = 78,   /**< CAN0 OR'ed Bus in Off State */
    CAN0_Error_IRQn             = 79,   /**< CAN0 Interrupt indicating errors */
    CAN0_Wake_Up_IRQn           = 80,   /**< CAN0 Interrupt asserted when bus activity */
    CAN0_ORed_0_15_MB_IRQn      = 81,   /**< CAN0 OR'ed Message buffer (0-15) */
    CAN0_ORed_16_31_MB_IRQn     = 82,   /**< CAN0 OR'ed Message buffer (16-31) */
    CAN1_ORed_IRQn              = 85,   /**< CAN1 OR'ed Bus in Off State */
    CAN1_Error_IRQn             = 86,   /**< CAN1 Interrupt indicating errors */
    CAN1_ORed_0_15_MB_IRQn      = 88,   /**< CAN1 OR'ed Message buffer (0-15) */
    CAN2_ORed_IRQn              = 92,   /**< CAN2 OR'ed Bus in Off State */
    CAN2_Error_IRQn             = 93,   /**< CAN2 Interrupt indicating errors */
    CAN2_ORed_0_15_MB_IRQn      = 95,   /**< CAN2 OR'ed Message buffer (0-15) */
    FTM0_Ch0_Ch1_IRQn           = 99,   /**< FTM0 Channel 0 and 1 interrupt */
    FTM0_Ch2_Ch3_IRQn           = 100,  /**< FTM0 Channel 2 and 3 interrupt */
    FTM0_Ch4_Ch5_IRQn           = 101,  /**< FTM0 Channel 4 and 5 interrupt */
    FTM0_Ch6_Ch7_IRQn           = 102,  /**< FTM0 Channel 6 and 7 interrupt */
    FTM0_Fault_IRQn             = 103,  /**< FTM0 Fault interrupt */
    FTM0_Ovf_Reload_IRQn        = 104,  /**< FTM0 Counter overflow and Reload interrupt */
    FTM1_Ch0_Ch1_IRQn           = 105,  /**< FTM1 Channel 0 and 1 interrupt */
    FTM1_Ch2_Ch3_IRQn           = 106,  /**< FTM1 Channel 2 and 3 interrupt */
    FTM1_Ch4_Ch5_IRQn           = 107,  /**< FTM1 Channel 4 and 5 interrupt */
    FTM1_Ch6_Ch7_IRQn           = 108,  /**< FTM1 Channel 6 and 7 interrupt */
    FTM1_Fault_IRQn             = 109,  /**< FTM1 Fault interrupt */
    FTM1_Ovf_Reload_IRQn        = 110,  /**< FTM1 Counter overflow and Reload interrupt */
    FTM2_Ch0_Ch1_IRQn           = 111,  /**< FTM2 Channel 0 and 1 interrupt */
    FTM2_Ch2_Ch3_IRQn           = 112,  /**< FTM2 Channel 2 and 3 interrupt */
    FTM2_Ch4_Ch5_IRQn           = 113,  /**< FTM2 Channel 4 and 5 interrupt */
    FTM2_Ch6_Ch7_IRQn           = 114,  /**< FTM2 Channel 6 and 7 interrupt */
    FTM2_Fault_IRQn             = 115,  /**< FTM2 Fault interrupt */
    FTM2_Ovf_Reload_IRQn        = 116,  /**< FTM2 Counter overflow and Reload interrupt */
    FTM3_Ch0_Ch1_IRQn           = 117,  /**< FTM3 Channel 0 and 1 interrupt */
    FTM3_Ch2_Ch3_IRQn           = 118,  /**< FTM3 Channel 2 and 3 interrupt */
    FTM3_Ch4_Ch5_IRQn           = 119,  /**< FTM3 Channel 4 and 5 interrupt */
    FTM3_Ch6_Ch7_IRQn           = 120,  /**< FTM3 Channel 6 and 7 interrupt */
    FTM3_Fault_IRQn             = 121,  /**< FTM3 Fault interrupt */
    FTM3_Ovf_Reload_IRQn        = 122   /**< FTM3 Counter overflow and Reload interrupt */
} IRQn_Type;

/*******************************************************************************
 * NVIC Register Structure
 ******************************************************************************/

/**
 * @brief NVIC Register Structure
 */
typedef struct {
    __IO uint32_t ISER[8];      /**< Interrupt Set Enable Register, offset: 0x000 */
    uint32_t RESERVED0[24];     /**< Reserved */
    __IO uint32_t ICER[8];      /**< Interrupt Clear Enable Register, offset: 0x080 */
    uint32_t RESERVED1[24];     /**< Reserved */
    __IO uint32_t ISPR[8];      /**< Interrupt Set Pending Register, offset: 0x100 */
    uint32_t RESERVED2[24];     /**< Reserved */
    __IO uint32_t ICPR[8];      /**< Interrupt Clear Pending Register, offset: 0x180 */
    uint32_t RESERVED3[24];     /**< Reserved */
    __IO uint32_t IABR[8];      /**< Interrupt Active Bit Register, offset: 0x200 */
    uint32_t RESERVED4[56];     /**< Reserved */
    __IO uint8_t  IP[240];      /**< Interrupt Priority Register, offset: 0x300 */
} NVIC_Type;

/**
 * @brief SCB (System Control Block) Register Structure
 */
typedef struct {
    __I  uint32_t CPUID;        /**< CPUID Base Register, offset: 0x00 */
    __IO uint32_t ICSR;         /**< Interrupt Control and State Register, offset: 0x04 */
    __IO uint32_t VTOR;         /**< Vector Table Offset Register, offset: 0x08 */
    __IO uint32_t AIRCR;        /**< Application Interrupt and Reset Control Register, offset: 0x0C */
    __IO uint32_t SCR;          /**< System Control Register, offset: 0x10 */
    __IO uint32_t CCR;          /**< Configuration and Control Register, offset: 0x14 */
    __IO uint8_t  SHPR[12];     /**< System Handler Priority Register, offset: 0x18 */
    __IO uint32_t SHCSR;        /**< System Handler Control and State Register, offset: 0x24 */
    __IO uint32_t CFSR;         /**< Configurable Fault Status Register, offset: 0x28 */
    __IO uint32_t HFSR;         /**< HardFault Status Register, offset: 0x2C */
    __IO uint32_t DFSR;         /**< Debug Fault Status Register, offset: 0x30 */
    __IO uint32_t MMFAR;        /**< MemManage Fault Address Register, offset: 0x34 */
    __IO uint32_t BFAR;         /**< BusFault Address Register, offset: 0x38 */
    __IO uint32_t AFSR;         /**< Auxiliary Fault Status Register, offset: 0x3C */
} SCB_Type;

/*******************************************************************************
 * NVIC Instance Pointers
 ******************************************************************************/

/** @brief NVIC base pointer */
#define NVIC                ((NVIC_Type *)NVIC_BASE)

/** @brief SCB base pointer */
#define SCB                 ((SCB_Type *)SCB_BASE)

/*******************************************************************************
 * SCB Register Bit Fields
 ******************************************************************************/

/* AIRCR Register */
#define SCB_AIRCR_VECTKEY_MASK      (0xFFFF0000UL)
#define SCB_AIRCR_VECTKEY_SHIFT     (16U)
#define SCB_AIRCR_VECTKEY           (0x05FA0000UL)  /**< Write key for AIRCR */
#define SCB_AIRCR_PRIGROUP_MASK     (0x00000700UL)
#define SCB_AIRCR_PRIGROUP_SHIFT    (8U)

/* ICSR Register */
#define SCB_ICSR_PENDSVSET_MASK     (0x10000000UL)
#define SCB_ICSR_PENDSVCLR_MASK     (0x08000000UL)
#define SCB_ICSR_PENDSTSET_MASK     (0x04000000UL)
#define SCB_ICSR_PENDSTCLR_MASK     (0x02000000UL)

/*******************************************************************************
 * Priority Grouping
 ******************************************************************************/

/**
 * @brief Priority Grouping Configuration
 */
typedef enum {
    NVIC_PRIORITYGROUP_0 = 7U,  /**< 0 bits preempt priority, 4 bits subpriority */
    NVIC_PRIORITYGROUP_1 = 6U,  /**< 1 bit preempt priority, 3 bits subpriority */
    NVIC_PRIORITYGROUP_2 = 5U,  /**< 2 bits preempt priority, 2 bits subpriority */
    NVIC_PRIORITYGROUP_3 = 4U,  /**< 3 bits preempt priority, 1 bit subpriority */
    NVIC_PRIORITYGROUP_4 = 3U   /**< 4 bits preempt priority, 0 bits subpriority */
} nvic_priority_group_t;

#endif /* NVIC_REG_H */
