/**
 * @file    i2c.c
 * @brief   LPI2C Driver Implementation for S32K144
 * @details Implementation of I2C driver functions for master and slave operations
 * 
 * @author  PhucPH32
 * @date    November 24, 2025
 * @version 1.0
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "i2c.h"
#include "i2c_reg.h"
#include "pcc_reg.h"

/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/* Timeout counter for operations */
#define I2C_TIMEOUT_COUNT       (10000U)

/* I2C commands for master transmit data register */
#define I2C_CMD_START           (0x400U)    /* Generate START condition */
#define I2C_CMD_STOP            (0x200U)    /* Generate STOP condition */
#define I2C_CMD_TRANSMIT        (0x000U)    /* Transmit data */
#define I2C_CMD_RECEIVE         (0x100U)    /* Receive data */

/*******************************************************************************
 * Master Mode Functions
 ******************************************************************************/

/**
 * @brief Initialize I2C master mode
 */
I2C_Status_t I2C_MasterInit(LPI2C_RegType *base, const I2C_MasterConfig_t *config, uint32_t srcClock)
{
    uint32_t prescaler;
    uint32_t clkLo, clkHi, setHold, dataVd;
    uint32_t baudRateHz;

    if ((base == NULL) || (config == NULL)) {
        return I2C_STATUS_ERROR;
    }

    /* Disable master before configuration */
    LPI2C_DISABLE_MASTER(base);

    /* Reset master logic */
    LPI2C_SW_RESET(base);
    
    /* Clear reset bit */
    base->MCR &= ~LPI2C_MCR_RST_MASK;

    /* Configure prescaler */
    prescaler = config->prescaler & 0x07U;
    base->MCFGR1 = LPI2C_MCFGR1_PRESCALE(prescaler);

    /* Calculate baud rate parameters */
    baudRateHz = config->baudRate;
    
    /* Simple calculation for standard mode (100kHz) or fast mode (400kHz) */
    /* CLKLO = CLKHI = (srcClock / (2^prescaler * baudRate * 2)) - 2 */
    clkLo = clkHi = ((srcClock / (1U << prescaler)) / (baudRateHz * 2U)) - 2U;
    
    if (clkLo > 63U) clkLo = 63U;
    if (clkHi > 63U) clkHi = 63U;
    
    /* Setup and data valid time */
    setHold = clkLo / 2U;
    dataVd = clkLo / 4U;

    /* Configure clock parameters */
    base->MCCR0 = LPI2C_MCCR0_CLKLO(clkLo) | 
                  LPI2C_MCCR0_CLKHI(clkHi) |
                  LPI2C_MCCR0_SETHOLD(setHold) |
                  LPI2C_MCCR0_DATAVD(dataVd);

    /* Configure debug mode */
    if (config->enableDebug) {
        base->MCR |= LPI2C_MCR_DBGEN_MASK;
    }

    /* Enable master if requested */
    if (config->enableMaster) {
        LPI2C_ENABLE_MASTER(base);
    }

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Deinitialize I2C master
 */
void I2C_MasterDeinit(LPI2C_RegType *base)
{
    if (base != NULL) {
        /* Disable master */
        LPI2C_DISABLE_MASTER(base);
        
        /* Reset peripheral */
        LPI2C_SW_RESET(base);
    }
}

/**
 * @brief Start I2C master transfer
 */
I2C_Status_t I2C_MasterStart(LPI2C_RegType *base, uint8_t slaveAddress, I2C_Direction_t direction)
{
    uint32_t timeout = I2C_TIMEOUT_COUNT;
    uint16_t cmdData;

    if (base == NULL) {
        return I2C_STATUS_ERROR;
    }

    /* Check if bus is busy */
    if (LPI2C_IS_BUS_BUSY(base)) {
        return I2C_STATUS_BUSY;
    }

    /* Prepare address with R/W bit */
    cmdData = I2C_CMD_START | ((uint16_t)slaveAddress << 1) | (uint16_t)direction;

    /* Wait for transmit FIFO ready */
    while (!LPI2C_IS_TX_READY(base) && (timeout > 0U)) {
        timeout--;
    }

    if (timeout == 0U) {
        return I2C_STATUS_TIMEOUT;
    }

    /* Send START + Address + R/W */
    LPI2C_WRITE_DATA(base, cmdData);

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Send data in master mode
 */
I2C_Status_t I2C_MasterSend(LPI2C_RegType *base, const uint8_t *txBuff, uint32_t txSize, bool sendStop)
{
    uint32_t i;
    uint32_t timeout;

    if ((base == NULL) || (txBuff == NULL) || (txSize == 0U)) {
        return I2C_STATUS_ERROR;
    }

    /* Send each byte */
    for (i = 0U; i < txSize; i++) {
        timeout = I2C_TIMEOUT_COUNT;

        /* Wait for transmit FIFO ready */
        while (!LPI2C_IS_TX_READY(base) && (timeout > 0U)) {
            timeout--;
        }

        if (timeout == 0U) {
            return I2C_STATUS_TIMEOUT;
        }

        /* Write data */
        LPI2C_WRITE_DATA(base, I2C_CMD_TRANSMIT | txBuff[i]);

        /* Check for NACK */
        if ((base->MSR & LPI2C_MSR_NDF_MASK) != 0U) {
            base->MSR = LPI2C_MSR_NDF_MASK;  /* Clear flag */
            return I2C_STATUS_NACK;
        }
    }

    /* Send STOP condition if requested */
    if (sendStop) {
        return I2C_MasterStop(base);
    }

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Receive data in master mode
 */
I2C_Status_t I2C_MasterReceive(LPI2C_RegType *base, uint8_t *rxBuff, uint32_t rxSize, bool sendStop)
{
    uint32_t i;
    uint32_t timeout;

    if ((base == NULL) || (rxBuff == NULL) || (rxSize == 0U)) {
        return I2C_STATUS_ERROR;
    }

    /* Send receive commands */
    for (i = 0U; i < rxSize; i++) {
        timeout = I2C_TIMEOUT_COUNT;

        /* Wait for transmit FIFO ready */
        while (!LPI2C_IS_TX_READY(base) && (timeout > 0U)) {
            timeout--;
        }

        if (timeout == 0U) {
            return I2C_STATUS_TIMEOUT;
        }

        /* Send receive command */
        LPI2C_WRITE_DATA(base, I2C_CMD_RECEIVE);
    }

    /* Read received data */
    for (i = 0U; i < rxSize; i++) {
        timeout = I2C_TIMEOUT_COUNT;

        /* Wait for receive data ready */
        while (!LPI2C_IS_RX_READY(base) && (timeout > 0U)) {
            timeout--;
        }

        if (timeout == 0U) {
            return I2C_STATUS_TIMEOUT;
        }

        /* Read data */
        rxBuff[i] = (uint8_t)LPI2C_READ_DATA(base);
    }

    /* Send STOP condition if requested */
    if (sendStop) {
        return I2C_MasterStop(base);
    }

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Send stop condition
 */
I2C_Status_t I2C_MasterStop(LPI2C_RegType *base)
{
    uint32_t timeout = I2C_TIMEOUT_COUNT;

    if (base == NULL) {
        return I2C_STATUS_ERROR;
    }

    /* Wait for transmit FIFO ready */
    while (!LPI2C_IS_TX_READY(base) && (timeout > 0U)) {
        timeout--;
    }

    if (timeout == 0U) {
        return I2C_STATUS_TIMEOUT;
    }

    /* Send STOP command */
    LPI2C_WRITE_DATA(base, I2C_CMD_STOP);

    /* Wait for STOP detection */
    timeout = I2C_TIMEOUT_COUNT;
    while (LPI2C_IS_MASTER_BUSY(base) && (timeout > 0U)) {
        timeout--;
    }

    return (timeout > 0U) ? I2C_STATUS_SUCCESS : I2C_STATUS_TIMEOUT;
}

/**
 * @brief Get master status
 */
uint32_t I2C_MasterGetStatus(LPI2C_RegType *base)
{
    return (base != NULL) ? base->MSR : 0U;
}

/**
 * @brief Clear master status flags
 */
void I2C_MasterClearStatus(LPI2C_RegType *base, uint32_t flags)
{
    if (base != NULL) {
        base->MSR = flags;
    }
}

/*******************************************************************************
 * Slave Mode Functions
 ******************************************************************************/

/**
 * @brief Initialize I2C slave mode
 */
I2C_Status_t I2C_SlaveInit(LPI2C_RegType *base, const I2C_SlaveConfig_t *config)
{
    if ((base == NULL) || (config == NULL)) {
        return I2C_STATUS_ERROR;
    }

    /* Disable slave before configuration */
    base->SCR &= ~(1U << 0);  /* Clear SEN bit */

    /* Configure slave address */
    base->SAMR = (config->slaveAddress & 0x3FFU);

    /* Enable slave if requested */
    if (config->enableSlave) {
        base->SCR |= (1U << 0);  /* Set SEN bit */
    }

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Deinitialize I2C slave
 */
void I2C_SlaveDeinit(LPI2C_RegType *base)
{
    if (base != NULL) {
        /* Disable slave */
        base->SCR &= ~(1U << 0);
    }
}

/**
 * @brief Send data in slave mode
 */
I2C_Status_t I2C_SlaveSend(LPI2C_RegType *base, const uint8_t *txBuff, uint32_t txSize)
{
    uint32_t i;
    uint32_t timeout;

    if ((base == NULL) || (txBuff == NULL) || (txSize == 0U)) {
        return I2C_STATUS_ERROR;
    }

    for (i = 0U; i < txSize; i++) {
        timeout = I2C_TIMEOUT_COUNT;

        /* Wait for transmit ready */
        while (((base->SSR & (1U << 0)) == 0U) && (timeout > 0U)) {
            timeout--;
        }

        if (timeout == 0U) {
            return I2C_STATUS_TIMEOUT;
        }

        /* Write data */
        base->STDR = txBuff[i];
    }

    return I2C_STATUS_SUCCESS;
}

/**
 * @brief Receive data in slave mode
 */
I2C_Status_t I2C_SlaveReceive(LPI2C_RegType *base, uint8_t *rxBuff, uint32_t rxSize)
{
    uint32_t i;
    uint32_t timeout;

    if ((base == NULL) || (rxBuff == NULL) || (rxSize == 0U)) {
        return I2C_STATUS_ERROR;
    }

    for (i = 0U; i < rxSize; i++) {
        timeout = I2C_TIMEOUT_COUNT;

        /* Wait for receive ready */
        while (((base->SSR & (1U << 1)) == 0U) && (timeout > 0U)) {
            timeout--;
        }

        if (timeout == 0U) {
            return I2C_STATUS_TIMEOUT;
        }

        /* Read data */
        rxBuff[i] = (uint8_t)(base->SRDR & 0xFFU);
    }

    return I2C_STATUS_SUCCESS;
}

/*******************************************************************************
 * Utility Functions
 ******************************************************************************/

/**
 * @brief Check if I2C bus is busy
 */
bool I2C_IsBusBusy(LPI2C_RegType *base)
{
    return (base != NULL) ? LPI2C_IS_BUS_BUSY(base) : false;
}

/**
 * @brief Enable I2C clock
 */
void I2C_EnableClock(uint8_t instance)
{
    if (instance == 0U) {
        /* Enable LPI2C0 clock via PCC */
        PCC->PCCn[PCC_LPI2C0_INDEX] |= PCC_PCCn_CGC_MASK;
    }
}

/**
 * @brief Disable I2C clock
 */
void I2C_DisableClock(uint8_t instance)
{
    if (instance == 0U) {
        /* Disable LPI2C0 clock via PCC */
        PCC->PCCn[PCC_LPI2C0_INDEX] &= ~PCC_PCCn_CGC_MASK;
    }
}
