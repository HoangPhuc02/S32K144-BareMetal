/**
 * @file    can_mb_access.h
 * @brief   CAN Message Buffer Access Helpers for S32K144
 * @details Provides helper functions and macros to access Message Buffers in RAMn array
 * 
 * @author  PhucPH32
 * @date    December 2, 2025
 * @version 1.0
 */

#ifndef CAN_MB_ACCESS_H
#define CAN_MB_ACCESS_H

#include "S32K144.h"
#include <stdint.h>

/*******************************************************************************
 * Message Buffer Field Definitions
 ******************************************************************************/

/* CS field masks and shifts */
#define CAN_CS_CODE_SHIFT                (24U)
#define CAN_CS_CODE_MASK                 (0x0F000000U)
#define CAN_CS_IDE_MASK                  (0x00200000U)
#define CAN_CS_SRR_MASK                  (0x00400000U)
#define CAN_CS_RTR_MASK                  (0x00100000U)
#define CAN_CS_DLC_SHIFT                 (16U)
#define CAN_CS_DLC_MASK                  (0x000F0000U)

/* MB CODE values */
#define CAN_CS_CODE_TX_INACTIVE          (0x08U)
#define CAN_CS_CODE_TX_ABORT             (0x09U)
#define CAN_CS_CODE_TX_ONCE              (0x0CU)
#define CAN_CS_CODE_RX_INACTIVE          (0x00U)
#define CAN_CS_CODE_RX_EMPTY             (0x04U)
#define CAN_CS_CODE_RX_FULL              (0x02U)

/* ID field masks and shifts */
#define CAN_ID_STD_SHIFT                 (18U)
#define CAN_ID_STD_MASK                  (0x1FFC0000U)
#define CAN_ID_EXT_SHIFT                 (0U)
#define CAN_ID_EXT_MASK                  (0x1FFFFFFFU)

/*******************************************************************************
 * Inline Helper Functions
 ******************************************************************************/

/**
 * @brief Get Message Buffer offset in RAMn array
 * @param mbIdx Message Buffer index (0-31)
 * @return Offset in 32-bit words
 */
static inline uint32_t CAN_GetMbOffset(uint8_t mbIdx)
{
    return (uint32_t)mbIdx * 4U;
}

/**
 * @brief Read Message Buffer CS register
 */
static inline uint32_t CAN_ReadMbCs(CAN_Type *base, uint8_t mbIdx)
{
    return base->RAMn[CAN_GetMbOffset(mbIdx) + 0U];
}

/**
 * @brief Write Message Buffer CS register
 */
static inline void CAN_WriteMbCs(CAN_Type *base, uint8_t mbIdx, uint32_t cs)
{
    base->RAMn[CAN_GetMbOffset(mbIdx) + 0U] = cs;
}

/**
 * @brief Read Message Buffer ID register
 */
static inline uint32_t CAN_ReadMbId(CAN_Type *base, uint8_t mbIdx)
{
    return base->RAMn[CAN_GetMbOffset(mbIdx) + 1U];
}

/**
 * @brief Write Message Buffer ID register
 */
static inline void CAN_WriteMbId(CAN_Type *base, uint8_t mbIdx, uint32_t id)
{
    base->RAMn[CAN_GetMbOffset(mbIdx) + 1U] = id;
}

/**
 * @brief Read Message Buffer DATA word (2 words for 8 bytes)
 */
static inline uint32_t CAN_ReadMbData(CAN_Type *base, uint8_t mbIdx, uint8_t wordIdx)
{
    return base->RAMn[CAN_GetMbOffset(mbIdx) + 2U + wordIdx];
}

/**
 * @brief Write Message Buffer DATA word
 */
static inline void CAN_WriteMbData(CAN_Type *base, uint8_t mbIdx, uint8_t wordIdx, uint32_t data)
{
    base->RAMn[CAN_GetMbOffset(mbIdx) + 2U + wordIdx] = data;
}

/**
 * @brief Copy data to Message Buffer
 * @param base CAN peripheral base
 * @param mbIdx Message Buffer index
 * @param data Source data array
 * @param length Data length in bytes (max 8)
 */
static inline void CAN_CopyDataToMb(CAN_Type *base, uint8_t mbIdx, const uint8_t *data, uint8_t length)
{
    uint32_t dataWord0 = 0U;
    uint32_t dataWord1 = 0U;
    
    /* Pack bytes into 32-bit words (big-endian format) */
    for (uint8_t i = 0; i < length && i < 4U; i++) {
        dataWord0 |= ((uint32_t)data[i] << (24U - i * 8U));
    }
    
    for (uint8_t i = 4; i < length && i < 8U; i++) {
        dataWord1 |= ((uint32_t)data[i] << (56U - i * 8U));
    }
    
    CAN_WriteMbData(base, mbIdx, 0U, dataWord0);
    CAN_WriteMbData(base, mbIdx, 1U, dataWord1);
}

/**
 * @brief Copy data from Message Buffer
 * @param base CAN peripheral base
 * @param mbIdx Message Buffer index
 * @param data Destination data array
 * @param length Data length in bytes (max 8)
 */
static inline void CAN_CopyDataFromMb(CAN_Type *base, uint8_t mbIdx, uint8_t *data, uint8_t length)
{
    uint32_t dataWord0 = CAN_ReadMbData(base, mbIdx, 0U);
    uint32_t dataWord1 = CAN_ReadMbData(base, mbIdx, 1U);
    
    /* Unpack 32-bit words into bytes (big-endian format) */
    for (uint8_t i = 0; i < length && i < 4U; i++) {
        data[i] = (uint8_t)(dataWord0 >> (24U - i * 8U));
    }
    
    for (uint8_t i = 4; i < length && i < 8U; i++) {
        data[i] = (uint8_t)(dataWord1 >> (56U - i * 8U));
    }
}

/**
 * @brief Clear Message Buffer (set CS, ID, DATA to 0)
 */
static inline void CAN_ClearMb(CAN_Type *base, uint8_t mbIdx)
{
    uint32_t offset = CAN_GetMbOffset(mbIdx);
    base->RAMn[offset + 0U] = 0U;  /* CS */
    base->RAMn[offset + 1U] = 0U;  /* ID */
    base->RAMn[offset + 2U] = 0U;  /* DATA word 0 */
    base->RAMn[offset + 3U] = 0U;  /* DATA word 1 */
}

#endif /* CAN_MB_ACCESS_H */
