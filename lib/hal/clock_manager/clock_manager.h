#ifndef CLOCK_MANAGER_H
#define CLOCK_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Clock Source Types
 ******************************************************************************/
/**
 * @brief Logical clock tree names used by Clock Manager
 * @details Enumerates both SCG system clocks and PCC peripheral functional clocks.
 *          Use these values with ClockManager_GetFrequency() to retrieve the
 *          current frequency in Hz without dealing with low-level registers.
 */
typedef enum {
    CLOCK_NAME_CORE = 0,        /* Core/CPU clock */
    CLOCK_NAME_BUS,             /* Bus/Peripheral clock */
    CLOCK_NAME_SLOW,            /* Slow clock */
    CLOCK_NAME_SOSC,            /* System Oscillator clock */
    CLOCK_NAME_SIRC,            /* Slow IRC clock */
    CLOCK_NAME_FIRC,            /* Fast IRC clock */
    CLOCK_NAME_SPLL,            /* System PLL clock */
    CLOCK_NAME_FLASH,           /* Flash clock */
    CLOCK_NAME_FLEXCAN0,        /* FlexCAN0 functional clock */
    CLOCK_NAME_FLEXCAN1,        /* FlexCAN1 functional clock */
    CLOCK_NAME_FLEXCAN2,        /* FlexCAN2 functional clock */
    CLOCK_NAME_LPUART0,         /* LPUART0 functional clock */
    CLOCK_NAME_LPUART1,         /* LPUART1 functional clock */
    CLOCK_NAME_LPUART2,         /* LPUART2 functional clock */
    CLOCK_NAME_LPSPI0,          /* LPSPI0 functional clock */
    CLOCK_NAME_LPSPI1,          /* LPSPI1 functional clock */
    CLOCK_NAME_LPSPI2,          /* LPSPI2 functional clock */
    CLOCK_NAME_LPI2C0,          /* LPI2C0 functional clock */
    CLOCK_NAME_FTM0,            /* FTM0 functional clock */
    CLOCK_NAME_FTM1,            /* FTM1 functional clock */
    CLOCK_NAME_FTM2,            /* FTM2 functional clock */
    CLOCK_NAME_FTM3,            /* FTM3 functional clock */
    CLOCK_NAME_ADC0,            /* ADC0 functional clock */
    CLOCK_NAME_ADC1,            /* ADC1 functional clock */
    CLOCK_NAME_COUNT
} clock_name_t;

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/**
 * @brief Initialize clock manager cache using current SCG/PCC configuration
 */
void ClockManager_Init(void);

/**
 * @brief Get frequency of a specific clock name
 * @param clockName Logical clock to query (see clock_name_t)
 * @return Frequency in Hz, 0 if invalid or disabled
 * @note Cache is refreshed automatically on the first call after reset or
 *       whenever ClockManager_Update() is invoked explicitly.
 */
uint32_t ClockManager_GetFrequency(clock_name_t clockName);

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
 * @brief Update cached clock frequencies after SCG/PCC configuration changes
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
uint32_t ClockManager_CalcUartDivider(clock_name_t clockSource,
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
uint32_t ClockManager_CalcCanTiming(clock_name_t clockSource,
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
uint32_t ClockManager_CalcSpiDivider(clock_name_t clockSource,
                                      uint32_t desiredBaudrate,
                                      uint8_t *prescaler,
                                      uint8_t *scaler);

#endif /* CLOCK_MANAGER_H */
