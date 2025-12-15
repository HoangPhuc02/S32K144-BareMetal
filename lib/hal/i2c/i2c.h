/**
 * @file    i2c.h
 * @brief   LPI2C Driver API Header for S32K144
 * @details High-level I2C driver interface providing functions for I2C master/slave operations
 * 
 * @author  PhucPH32
 * @date    23/11/2025
 * @version 1.0
 * 
 * @par Change Log:
 * - Version 1.0 (Nov 24, 2025): Initial version
 */

#ifndef I2C_H
#define I2C_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "i2c_reg.h"

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/

/**
 * @brief I2C transfer direction
 */
typedef enum {
    I2C_WRITE = 0U,         /**< Write operation */
    I2C_READ  = 1U          /**< Read operation */
} I2C_Direction_t;

/**
 * @brief I2C master configuration structure
 */
typedef struct {
    uint32_t baudRate;              /**< Baud rate in Hz (e.g., 100000 for 100kHz, 400000 for 400kHz) */
    uint8_t  prescaler;             /**< Clock prescaler value (0-7) */
    bool     enableMaster;          /**< Enable master mode */
    bool     enableDebug;           /**< Enable debug mode */
} I2C_MasterConfig_t;

/**
 * @brief I2C slave configuration structure
 */
typedef struct {
    uint16_t slaveAddress;          /**< 7-bit or 10-bit slave address */
    bool     enable10BitAddress;    /**< Enable 10-bit addressing */
    bool     enableGeneralCall;     /**< Enable general call address matching */
    bool     enableSlave;           /**< Enable slave mode */
} I2C_SlaveConfig_t;

/**
 * @brief I2C status codes
 */
typedef enum {
    I2C_STATUS_SUCCESS = 0U,        /**< Operation successful */
    I2C_STATUS_BUSY,                /**< Bus is busy */
    I2C_STATUS_NACK,                /**< NACK received */
    I2C_STATUS_ARBITRATION_LOST,    /**< Arbitration lost */
    I2C_STATUS_TIMEOUT,             /**< Operation timeout */
    I2C_STATUS_ERROR                /**< General error */
} I2C_Status_t;

/*******************************************************************************
 * Function Prototypes - Master Mode
 ******************************************************************************/

/**
 * @brief Initialize I2C master mode
 * 
 * @param[in] base      Pointer to I2C peripheral base address
 * @param[in] config    Pointer to master configuration structure
 * @param[in] srcClock  Source clock frequency in Hz
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_MasterInit(LPI2C_RegType *base, const I2C_MasterConfig_t *config, uint32_t srcClock);

/**
 * @brief Deinitialize I2C master
 * 
 * @param[in] base  Pointer to I2C peripheral base address
 */
void I2C_MasterDeinit(LPI2C_RegType *base);

/**
 * @brief Start I2C master transfer
 * 
 * @param[in] base          Pointer to I2C peripheral base address
 * @param[in] slaveAddress  7-bit slave address
 * @param[in] direction     Transfer direction (read/write)
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_MasterStart(LPI2C_RegType *base, uint8_t slaveAddress, I2C_Direction_t direction);

/**
 * @brief Send data in master mode
 * 
 * @param[in] base      Pointer to I2C peripheral base address
 * @param[in] txBuff    Pointer to transmit buffer
 * @param[in] txSize    Number of bytes to transmit
 * @param[in] sendStop  Send stop condition after transmission
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_MasterSend(LPI2C_RegType *base, const uint8_t *txBuff, uint32_t txSize, bool sendStop);

/**
 * @brief Receive data in master mode
 * 
 * @param[in]  base      Pointer to I2C peripheral base address
 * @param[out] rxBuff    Pointer to receive buffer
 * @param[in]  rxSize    Number of bytes to receive
 * @param[in]  sendStop  Send stop condition after reception
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_MasterReceive(LPI2C_RegType *base, uint8_t *rxBuff, uint32_t rxSize, bool sendStop);

/**
 * @brief Send stop condition
 * 
 * @param[in] base  Pointer to I2C peripheral base address
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_MasterStop(LPI2C_RegType *base);

/**
 * @brief Get master status
 * 
 * @param[in] base  Pointer to I2C peripheral base address
 * 
 * @return Current status flags
 */
uint32_t I2C_MasterGetStatus(LPI2C_RegType *base);

/**
 * @brief Clear master status flags
 * 
 * @param[in] base   Pointer to I2C peripheral base address
 * @param[in] flags  Status flags to clear
 */
void I2C_MasterClearStatus(LPI2C_RegType *base, uint32_t flags);

/*******************************************************************************
 * Function Prototypes - Slave Mode
 ******************************************************************************/

/**
 * @brief Initialize I2C slave mode
 * 
 * @param[in] base    Pointer to I2C peripheral base address
 * @param[in] config  Pointer to slave configuration structure
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_SlaveInit(LPI2C_RegType *base, const I2C_SlaveConfig_t *config);

/**
 * @brief Deinitialize I2C slave
 * 
 * @param[in] base  Pointer to I2C peripheral base address
 */
void I2C_SlaveDeinit(LPI2C_RegType *base);

/**
 * @brief Send data in slave mode
 * 
 * @param[in] base      Pointer to I2C peripheral base address
 * @param[in] txBuff    Pointer to transmit buffer
 * @param[in] txSize    Number of bytes to transmit
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_SlaveSend(LPI2C_RegType *base, const uint8_t *txBuff, uint32_t txSize);

/**
 * @brief Receive data in slave mode
 * 
 * @param[in]  base    Pointer to I2C peripheral base address
 * @param[out] rxBuff  Pointer to receive buffer
 * @param[in]  rxSize  Number of bytes to receive
 * 
 * @return I2C_STATUS_SUCCESS if successful
 */
I2C_Status_t I2C_SlaveReceive(LPI2C_RegType *base, uint8_t *rxBuff, uint32_t rxSize);

/*******************************************************************************
 * Function Prototypes - Utility
 ******************************************************************************/

/**
 * @brief Check if I2C bus is busy
 * 
 * @param[in] base  Pointer to I2C peripheral base address
 * 
 * @return true if bus is busy, false otherwise
 */
bool I2C_IsBusBusy(LPI2C_RegType *base);

/**
 * @brief Enable I2C clock
 * 
 * @param[in] instance  I2C instance number (0, 1, 2...)
 */
void I2C_EnableClock(uint8_t instance);

/**
 * @brief Disable I2C clock
 * 
 * @param[in] instance  I2C instance number (0, 1, 2...)
 */
void I2C_DisableClock(uint8_t instance);

#endif /* I2C_H */
