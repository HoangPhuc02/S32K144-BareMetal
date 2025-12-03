/**
 * @file    nvic.h
 * @brief   NVIC Driver API for S32K144
 * @details
 * NVIC driver cung cấp các API để quản lý interrupts trong ARM Cortex-M4.
 * 
 * Tính năng:
 * - Enable/Disable interrupts
 * - Set interrupt priority
 * - Pending/Clear pending interrupts
 * - Check interrupt active status
 * - Priority grouping configuration
 * - System reset
 * 
 * @author  PhucPH32
 * @date    30/11/2025
 * @version 1.0
 * 
 * @par Example:
 * @code
 * // Enable và set priority cho LPIT0 Channel 0 interrupt
 * NVIC_SetPriority(LPIT0_Ch0_IRQn, 3);
 * NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
 * 
 * // Trong ISR
 * void LPIT0_Ch0_IRQHandler(void)
 * {
 *     // Handle interrupt
 *     // Clear interrupt flag...
 * }
 * 
 * // Disable interrupt
 * NVIC_DisableIRQ(LPIT0_Ch0_IRQn);
 * @endcode
 */

#ifndef NVIC_H
#define NVIC_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "nvic_reg.h"
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Status return type */
typedef enum {
    NVIC_STATUS_SUCCESS = 0x00U,         /**< Operation successful */
    NVIC_STATUS_ERROR = 0x01U,           /**< General error */
    NVIC_STATUS_INVALID_PARAM = 0x04U    /**< Invalid parameter */
} nvic_status_t;

/** @brief Maximum priority value (lowest priority) */
#define NVIC_PRIORITY_MIN       (15U)

/** @brief Minimum priority value (highest priority) */
#define NVIC_PRIORITY_MAX       (0U)

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Enable interrupt
 * @details Enable một interrupt source trong NVIC
 * 
 * @param[in] IRQn Interrupt number (từ IRQn_Type enum)
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Interrupt enabled
 *         - NVIC_STATUS_INVALID_PARAM: Invalid IRQ number
 * 
 * @note Phải set priority trước khi enable interrupt
 * @warning Interrupt sẽ trigger ngay nếu pending flag đã set
 * 
 * @code
 * // Enable LPIT0 Channel 0 interrupt
 * NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
 * @endcode
 */
nvic_status_t NVIC_EnableIRQ(IRQn_Type IRQn);

/**
 * @brief Disable interrupt
 * @details Disable một interrupt source trong NVIC
 * 
 * @param[in] IRQn Interrupt number
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Interrupt disabled
 *         - NVIC_STATUS_INVALID_PARAM: Invalid IRQ number
 * 
 * @note Interrupt đang active sẽ được hoàn thành trước khi disable
 * 
 * @code
 * // Disable LPIT0 Channel 0 interrupt
 * NVIC_DisableIRQ(LPIT0_Ch0_IRQn);
 * @endcode
 */
nvic_status_t NVIC_DisableIRQ(IRQn_Type IRQn);

/**
 * @brief Get interrupt enable status
 * @details Kiểm tra xem interrupt có được enable không
 * 
 * @param[in] IRQn Interrupt number
 * @param[out] isEnabled Pointer to store enable status
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Status retrieved
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 */
nvic_status_t NVIC_GetEnableIRQ(IRQn_Type IRQn, bool *isEnabled);

/**
 * @brief Set interrupt pending
 * @details Set pending flag cho một interrupt
 * 
 * @param[in] IRQn Interrupt number
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Pending set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid IRQ number
 * 
 * @note Nếu interrupt enabled, ISR sẽ được gọi ngay
 * 
 * @code
 * // Trigger software interrupt
 * NVIC_SetPendingIRQ(SWI_IRQn);
 * @endcode
 */
nvic_status_t NVIC_SetPendingIRQ(IRQn_Type IRQn);

/**
 * @brief Clear interrupt pending
 * @details Clear pending flag của một interrupt
 * 
 * @param[in] IRQn Interrupt number
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Pending cleared
 *         - NVIC_STATUS_INVALID_PARAM: Invalid IRQ number
 * 
 * @code
 * // Clear pending flag
 * NVIC_ClearPendingIRQ(LPIT0_Ch0_IRQn);
 * @endcode
 */
nvic_status_t NVIC_ClearPendingIRQ(IRQn_Type IRQn);

/**
 * @brief Get interrupt pending status
 * @details Kiểm tra xem interrupt có pending không
 * 
 * @param[in] IRQn Interrupt number
 * @param[out] isPending Pointer to store pending status
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Status retrieved
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 */
nvic_status_t NVIC_GetPendingIRQ(IRQn_Type IRQn, bool *isPending);

/**
 * @brief Get interrupt active status
 * @details Kiểm tra xem interrupt có đang active (executing) không
 * 
 * @param[in] IRQn Interrupt number
 * @param[out] isActive Pointer to store active status
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Status retrieved
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 * 
 * @note Active = ISR đang được execute
 */
nvic_status_t NVIC_GetActiveIRQ(IRQn_Type IRQn, bool *isActive);

/**
 * @brief Set interrupt priority
 * @details Cấu hình priority cho một interrupt
 * 
 * @param[in] IRQn Interrupt number
 * @param[in] priority Priority value (0-15, 0 = highest)
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 * 
 * @note Priority thấp hơn (số nhỏ hơn) có độ ưu tiên cao hơn
 *       Priority 0 = highest, Priority 15 = lowest
 * 
 * @code
 * // Set LPIT0 Ch0 priority to 3
 * NVIC_SetPriority(LPIT0_Ch0_IRQn, 3);
 * 
 * // Set DMA0 priority to 2 (higher than LPIT)
 * NVIC_SetPriority(DMA0_IRQn, 2);
 * @endcode
 */
nvic_status_t NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority);

/**
 * @brief Get interrupt priority
 * @details Đọc priority của một interrupt
 * 
 * @param[in] IRQn Interrupt number
 * @param[out] priority Pointer to store priority value
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority retrieved
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 */
nvic_status_t NVIC_GetPriority(IRQn_Type IRQn, uint8_t *priority);

/**
 * @brief Set priority grouping
 * @details Cấu hình cách chia priority thành preempt và subpriority
 * 
 * @param[in] priorityGroup Priority grouping configuration
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority grouping set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid priority group
 * 
 * @note Priority grouping ảnh hưởng đến preemption:
 *       - PRIORITYGROUP_0: 0 bit preempt, 4 bit sub (no preemption)
 *       - PRIORITYGROUP_1: 1 bit preempt, 3 bit sub
 *       - PRIORITYGROUP_2: 2 bit preempt, 2 bit sub
 *       - PRIORITYGROUP_3: 3 bit preempt, 1 bit sub
 *       - PRIORITYGROUP_4: 4 bit preempt, 0 bit sub (full preemption)
 * 
 * @code
 * // Set priority grouping to 2 bits preempt, 2 bits sub
 * NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
 * @endcode
 */
nvic_status_t NVIC_SetPriorityGrouping(nvic_priority_group_t priorityGroup);

/**
 * @brief Get priority grouping
 * @details Đọc priority grouping hiện tại
 * 
 * @param[out] priorityGroup Pointer to store priority grouping
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority grouping retrieved
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 */
nvic_status_t NVIC_GetPriorityGrouping(nvic_priority_group_t *priorityGroup);

/**
 * @brief Encode priority
 * @details Encode preempt priority và sub priority thành priority value
 * 
 * @param[in] priorityGroup Priority grouping
 * @param[in] preemptPriority Preemption priority
 * @param[in] subPriority Sub priority
 * @param[out] priority Pointer to store encoded priority
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority encoded
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 * 
 * @note Hàm này giúp tính toán priority value từ preempt và sub priority
 * 
 * @code
 * uint8_t priority;
 * // Preempt = 1, Sub = 2 with PRIORITYGROUP_2
 * NVIC_EncodePriority(NVIC_PRIORITYGROUP_2, 1, 2, &priority);
 * NVIC_SetPriority(LPIT0_Ch0_IRQn, priority);
 * @endcode
 */
nvic_status_t NVIC_EncodePriority(nvic_priority_group_t priorityGroup,
                              uint8_t preemptPriority,
                              uint8_t subPriority,
                              uint8_t *priority);

/**
 * @brief Decode priority
 * @details Decode priority value thành preempt và sub priority
 * 
 * @param[in] priorityGroup Priority grouping
 * @param[in] priority Priority value
 * @param[out] preemptPriority Pointer to store preemption priority
 * @param[out] subPriority Pointer to store sub priority
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority decoded
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 */
nvic_status_t NVIC_DecodePriority(nvic_priority_group_t priorityGroup,
                              uint8_t priority,
                              uint8_t *preemptPriority,
                              uint8_t *subPriority);

/**
 * @brief System reset
 * @details Thực hiện software reset toàn bộ hệ thống
 * 
 * @return Hàm này không return (system sẽ reset)
 * 
 * @warning Tất cả state sẽ bị mất sau khi reset
 * 
 * @code
 * // Perform system reset
 * NVIC_SystemReset();
 * @endcode
 */
void NVIC_SystemReset(void);

/**
 * @brief Disable all interrupts (global)
 * @details Disable tất cả interrupts bằng cách set PRIMASK
 * 
 * @return Previous PRIMASK value (để restore sau)
 * 
 * @note Sử dụng cho critical sections
 * 
 * @code
 * uint32_t primask = NVIC_DisableGlobalIRQ();
 * // Critical section
 * NVIC_EnableGlobalIRQ(primask);
 * @endcode
 */
uint32_t NVIC_DisableGlobalIRQ(void);

/**
 * @brief Enable all interrupts (global)
 * @details Restore PRIMASK value để enable interrupts
 * 
 * @param[in] primask Previous PRIMASK value
 */
void NVIC_EnableGlobalIRQ(uint32_t primask);

/**
 * @brief Set vector table offset
 * @details Thay đổi địa chỉ của vector table
 * 
 * @param[in] offset Vector table offset address (must be aligned to 128 bytes)
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Vector table offset set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid offset (not aligned)
 * 
 * @note Thường dùng khi bootloader chuyển sang application
 */
nvic_status_t NVIC_SetVectorTable(uint32_t offset);

/**
 * @brief Get vector table offset
 * @details Đọc địa chỉ hiện tại của vector table
 * 
 * @param[out] offset Pointer to store vector table offset
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Offset retrieved
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 */
nvic_status_t NVIC_GetVectorTable(uint32_t *offset);

/**
 * @brief Wait for interrupt
 * @details Enter sleep mode và chờ interrupt
 * 
 * @note CPU sẽ được wake up khi có interrupt
 */
void NVIC_WaitForInterrupt(void);

/**
 * @brief Wait for event
 * @details Enter sleep mode và chờ event
 */
void NVIC_WaitForEvent(void);

/**
 * @brief Send event
 * @details Gửi event để wake up CPU đang ở WFE
 */
void NVIC_SendEvent(void);

/**
 * @brief Enable fault exceptions
 * @details Enable các fault handlers (UsageFault, BusFault, MemManage)
 * 
 * @param[in] enableUsageFault Enable UsageFault handler
 * @param[in] enableBusFault Enable BusFault handler
 * @param[in] enableMemManageFault Enable MemManage fault handler
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Fault handlers enabled
 */
nvic_status_t NVIC_EnableFaultHandlers(bool enableUsageFault,
                                   bool enableBusFault,
                                   bool enableMemManageFault);

#endif /* NVIC_H */
