/**
 * @file    nvic.h
 * @brief   NVIC Driver API for S32K144
 * @details
 * NVIC driver provides APIs to manage interrupts in ARM Cortex-M4.
 * 
 * Features:
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
 * // Enable and set priority for LPIT0 Channel 0 interrupt
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
 * @details Enable an interrupt source in NVIC
 * 
 * @param[in] IRQn Interrupt number (from IRQn_Type enum)
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Interrupt enabled
 *         - NVIC_STATUS_INVALID_PARAM: Invalid IRQ number
 * 
 * @note Must set priority before enabling interrupt
 * @warning Interrupt will trigger immediately if pending flag is already set
 * 
 * @code
 * // Enable LPIT0 Channel 0 interrupt
 * NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
 * @endcode
 */
nvic_status_t NVIC_EnableIRQ(IRQn_Type IRQn);

/**
 * @brief Disable interrupt
 * @details Disable an interrupt source in NVIC
 * 
 * @param[in] IRQn Interrupt number
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Interrupt disabled
 *         - NVIC_STATUS_INVALID_PARAM: Invalid IRQ number
 * 
 * @note Active interrupt will be completed before disabling
 * 
 * @code
 * // Disable LPIT0 Channel 0 interrupt
 * NVIC_DisableIRQ(LPIT0_Ch0_IRQn);
 * @endcode
 */
nvic_status_t NVIC_DisableIRQ(IRQn_Type IRQn);

/**
 * @brief Get interrupt enable status
 * @details Check if interrupt is enabled
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
 * @details Set pending flag for an interrupt
 * 
 * @param[in] IRQn Interrupt number
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Pending set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid IRQ number
 * 
 * @note If interrupt is enabled, ISR will be called immediately
 * 
 * @code
 * // Trigger software interrupt
 * NVIC_SetPendingIRQ(SWI_IRQn);
 * @endcode
 */
nvic_status_t NVIC_SetPendingIRQ(IRQn_Type IRQn);

/**
 * @brief Clear interrupt pending
 * @details Clear pending flag of an interrupt
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
 * @details Check if interrupt is pending
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
 * @details Check if interrupt is currently active (executing)
 * 
 * @param[in] IRQn Interrupt number
 * @param[out] isActive Pointer to store active status
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Status retrieved
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 * 
 * @note Active = ISR is being executed
 */
nvic_status_t NVIC_GetActiveIRQ(IRQn_Type IRQn, bool *isActive);

/**
 * @brief Set interrupt priority
 * @details Configure priority for an interrupt
 * 
 * @param[in] IRQn Interrupt number
 * @param[in] priority Priority value (0-15, 0 = highest)
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid parameter
 * 
 * @note Lower priority value (smaller number) has higher precedence
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
 * @details Read priority of an interrupt
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
 * @details Configure how priority is divided into preempt and subpriority
 * 
 * @param[in] priorityGroup Priority grouping configuration
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Priority grouping set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid priority group
 * 
 * @note Priority grouping affects preemption:
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
 * @details Read current priority grouping
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
 * @details Encode preempt priority and sub priority into priority value
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
 * @note This function helps calculate priority value from preempt and sub priority
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
 * @details Decode priority value into preempt and sub priority
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
 * @details Perform software reset of entire system
 * 
 * @return This function does not return (system will reset)
 * 
 * @warning All state will be lost after reset
 * 
 * @code
 * // Perform system reset
 * NVIC_SystemReset();
 * @endcode
 */
void NVIC_SystemReset(void);

/**
 * @brief Disable all interrupts (global)
 * @details Disable all interrupts by setting PRIMASK
 * 
 * @return Previous PRIMASK value (to restore later)
 * 
 * @note Used for critical sections
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
 * @details Restore PRIMASK value to enable interrupts
 * 
 * @param[in] primask Previous PRIMASK value
 */
void NVIC_EnableGlobalIRQ(uint32_t primask);

/**
 * @brief Set vector table offset
 * @details Change address of vector table
 * 
 * @param[in] offset Vector table offset address (must be aligned to 128 bytes)
 * 
 * @return nvic_status_t
 *         - NVIC_STATUS_SUCCESS: Vector table offset set
 *         - NVIC_STATUS_INVALID_PARAM: Invalid offset (not aligned)
 * 
 * @note Commonly used when bootloader transfers to application
 */
nvic_status_t NVIC_SetVectorTable(uint32_t offset);

/**
 * @brief Get vector table offset
 * @details Read current address of vector table
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
 * @details Enter sleep mode and wait for interrupt
 * 
 * @note CPU will be woken up when interrupt occurs
 */
void NVIC_WaitForInterrupt(void);

/**
 * @brief Wait for event
 * @details Enter sleep mode and wait for event
 */
void NVIC_WaitForEvent(void);

/**
 * @brief Send event
 * @details Send event to wake up CPU in WFE state
 */
void NVIC_SendEvent(void);

/**
 * @brief Enable fault exceptions
 * @details Enable fault handlers (UsageFault, BusFault, MemManage)
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
