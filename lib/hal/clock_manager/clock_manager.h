#ifndef CLOCK_MANAGER_H
#define CLOCK_MANAGER_H

#include <stdint.h>

/*******************************************************************************
 * Clock Source Types
 ******************************************************************************/
typedef enum {
    CLOCK_SRC_CORE,         /* Core/CPU clock */
    CLOCK_SRC_BUS,          /* Bus clock (peripheral clock) */
    CLOCK_SRC_FLASH,        /* Flash clock */
    CLOCK_SRC_FLEXCAN,      /* FlexCAN clock */
    CLOCK_SRC_LPUART0,      /* LPUART0 clock */
    CLOCK_SRC_LPUART1,      /* LPUART1 clock */
    CLOCK_SRC_LPUART2,      /* LPUART2 clock */
    CLOCK_SRC_LPSPI0,       /* LPSPI0 clock */
    CLOCK_SRC_LPSPI1,       /* LPSPI1 clock */
    CLOCK_SRC_LPI2C0,       /* LPI2C0 clock */
    CLOCK_SRC_FTM0,         /* FTM0 clock */
    CLOCK_SRC_FTM1,         /* FTM1 clock */
    CLOCK_SRC_FTM2,         /* FTM2 clock */
    CLOCK_SRC_FTM3,         /* FTM3 clock */
    CLOCK_SRC_COUNT
} ClockSource_t;

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initialize clock manager with current system configuration
 */
void ClockManager_Init(void);

/**
 * @brief Get frequency of a specific clock source
 * @param source Clock source to query
 * @return Frequency in Hz, 0 if invalid source
 */
uint32_t ClockManager_GetFrequency(ClockSource_t source);

/**
 * @brief Get core clock frequency
 * @return Core clock frequency in Hz
 */
uint32_t ClockManager_GetCoreFreq(void);

/**
 * @brief Get bus clock frequency (commonly used by peripherals)
 * @return Bus clock frequency in Hz
 */
uint32_t ClockManager_GetBusFreq(void);

/**
 * @brief Update clock frequencies after clock configuration change
 * @note Call this after modifying PCC or SCG registers
 */
void ClockManager_Update(void);

/*******************************************************************************
 * Baudrate Calculation Helpers
 ******************************************************************************/

/**
 * @brief Calculate UART divider for desired baudrate
 * @param clockSource Clock source for the UART module
 * @param desiredBaudrate Target baudrate
 * @param osr Oversampling ratio (4-32)
 * @param[out] sbr Calculated SBR value
 * @return Actual baudrate achieved
 */
uint32_t ClockManager_CalcUartDivider(ClockSource_t clockSource,
                                       uint32_t desiredBaudrate,
                                       uint8_t osr,
                                       uint16_t *sbr);

/**
 * @brief Calculate CAN timing parameters
 * @param clockSource Clock source for CAN module
 * @param desiredBaudrate Target baudrate (typically 500000 or 1000000)
 * @param[out] presc Prescaler value
 * @param[out] propseg Propagation segment
 * @param[out] pseg1 Phase segment 1
 * @param[out] pseg2 Phase segment 2
 * @return Actual baudrate achieved
 */
uint32_t ClockManager_CalcCanTiming(ClockSource_t clockSource,
                                     uint32_t desiredBaudrate,
                                     uint8_t *presc,
                                     uint8_t *propseg,
                                     uint8_t *pseg1,
                                     uint8_t *pseg2);

/**
 * @brief Calculate SPI divider for desired baudrate
 * @param clockSource Clock source for SPI module
 * @param desiredBaudrate Target baudrate
 * @param[out] prescaler Prescaler value
 * @param[out] scaler Scaler value
 * @return Actual baudrate achieved
 */
uint32_t ClockManager_CalcSpiDivider(ClockSource_t clockSource,
                                      uint32_t desiredBaudrate,
                                      uint8_t *prescaler,
                                      uint8_t *scaler);

#endif /* CLOCK_MANAGER_H */
