#include "clock_manager.h"
#include <string.h>
#include "scg.h"
#include "pcc.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static uint32_t s_clockFrequencies[CLOCK_NAME_COUNT] = {0};
static bool s_clockCacheValid = false;

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static void ClockManager_UpdateScgFrequencies(void)
{
    scg_clock_frequencies_t freqs;

    if (!SCG_GetClockFrequencies(&freqs)) {
        memset(&freqs, 0, sizeof(freqs));
    }

    s_clockFrequencies[CLOCK_NAME_CORE] = freqs.coreClk;
    s_clockFrequencies[CLOCK_NAME_BUS]  = freqs.busClk;
    s_clockFrequencies[CLOCK_NAME_SLOW] = freqs.slowClk;
    s_clockFrequencies[CLOCK_NAME_SOSC] = freqs.soscClk;
    s_clockFrequencies[CLOCK_NAME_SIRC] = freqs.sircClk;
    s_clockFrequencies[CLOCK_NAME_FIRC] = freqs.fircClk;
    s_clockFrequencies[CLOCK_NAME_SPLL] = freqs.spllClk;
    s_clockFrequencies[CLOCK_NAME_FLASH] = freqs.busClk; /* Flash clock derives from bus clock */
}

static void ClockManager_UpdatePeripheralFrequencies(void)
{
    s_clockFrequencies[CLOCK_NAME_FLEXCAN0] = PCC_GetFlexCanClockFreq(0U);
    s_clockFrequencies[CLOCK_NAME_FLEXCAN1] = PCC_GetFlexCanClockFreq(1U);
    s_clockFrequencies[CLOCK_NAME_FLEXCAN2] = PCC_GetFlexCanClockFreq(2U);

    s_clockFrequencies[CLOCK_NAME_LPUART0] = PCC_GetLpuartClockFreq(0U);
    s_clockFrequencies[CLOCK_NAME_LPUART1] = PCC_GetLpuartClockFreq(1U);
    s_clockFrequencies[CLOCK_NAME_LPUART2] = PCC_GetLpuartClockFreq(2U);

    s_clockFrequencies[CLOCK_NAME_LPSPI0] = PCC_GetLpspiClockFreq(0U);
    s_clockFrequencies[CLOCK_NAME_LPSPI1] = PCC_GetLpspiClockFreq(1U);
    s_clockFrequencies[CLOCK_NAME_LPSPI2] = PCC_GetLpspiClockFreq(2U);

    s_clockFrequencies[CLOCK_NAME_LPI2C0] = PCC_GetLpi2cClockFreq(0U);

    s_clockFrequencies[CLOCK_NAME_FTM0] = PCC_GetFtmClockFreq(0U);
    s_clockFrequencies[CLOCK_NAME_FTM1] = PCC_GetFtmClockFreq(1U);
    s_clockFrequencies[CLOCK_NAME_FTM2] = PCC_GetFtmClockFreq(2U);
    s_clockFrequencies[CLOCK_NAME_FTM3] = PCC_GetFtmClockFreq(3U);

    s_clockFrequencies[CLOCK_NAME_ADC0] = PCC_GetAdcClockFreq(0U);
    s_clockFrequencies[CLOCK_NAME_ADC1] = PCC_GetAdcClockFreq(1U);
}

static void ClockManager_RefreshCache(void)
{
    ClockManager_UpdateScgFrequencies();
    ClockManager_UpdatePeripheralFrequencies();
    s_clockCacheValid = true;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

void ClockManager_Init(void)
{
    ClockManager_Update();
}

void ClockManager_Update(void)
{
    ClockManager_RefreshCache();
}

uint32_t ClockManager_GetFrequency(clock_name_t clockName)
{
    if (clockName >= CLOCK_NAME_COUNT) {
        return 0U;
    }

    if (!s_clockCacheValid) {
        ClockManager_RefreshCache();
    }

    return s_clockFrequencies[clockName];
}

uint32_t ClockManager_GetCoreFreq(void)
{
    return ClockManager_GetFrequency(CLOCK_NAME_CORE);
}

uint32_t ClockManager_GetBusFreq(void)
{
    return ClockManager_GetFrequency(CLOCK_NAME_BUS);
}

uint32_t ClockManager_CalcUartDivider(clock_name_t clockSource,
                                       uint32_t desiredBaudrate,
                                       uint8_t osr,
                                       uint16_t *sbr)
{
    uint32_t clockFreq = ClockManager_GetFrequency(clockSource);
    uint32_t calculatedSbr;
    uint32_t actualBaudrate;
    
    if (clockFreq == 0 || desiredBaudrate == 0 || osr < 4)
    {
        *sbr = 0;
        return 0;
    }
    
    /* SBR = ClockFreq / (Baudrate * (OSR + 1)) */
    calculatedSbr = (clockFreq + (desiredBaudrate * (osr + 1U) / 2U)) / 
                    (desiredBaudrate * (osr + 1U));
    
    if (calculatedSbr > 0x1FFF)
    {
        calculatedSbr = 0x1FFF;
    }
    
    *sbr = (uint16_t)calculatedSbr;
    
    /* Calculate actual baudrate */
    actualBaudrate = clockFreq / (calculatedSbr * (osr + 1U));
    
    return actualBaudrate;
}

uint32_t ClockManager_CalcCanTiming(clock_name_t clockSource,
                                     uint32_t desiredBaudrate,
                                     uint8_t *presc,
                                     uint8_t *propseg,
                                     uint8_t *pseg1,
                                     uint8_t *pseg2)
{
    uint32_t clockFreq = ClockManager_GetFrequency(clockSource);
    uint32_t tq;           /* Time quantum count per bit */
    uint32_t prescaler;
    uint32_t actualBaudrate;
    
    if (clockFreq == 0 || desiredBaudrate == 0)
    {
        return 0;
    }
    
    /* Target: 16 time quanta per bit for good sample point (~75%) */
    tq = 16;
    
    /* Prescaler = ClockFreq / (Baudrate * TQ) */
    prescaler = clockFreq / (desiredBaudrate * tq);
    
    if (prescaler == 0) prescaler = 1;
    if (prescaler > 256) prescaler = 256;
    
    /* Typical timing for 75% sample point with 16 TQ:
     * SYNC = 1, PROPSEG = 5, PSEG1 = 5, PSEG2 = 4
     * Sample point at (1 + 5 + 5) / 16 = 68.75%
     */
    *presc = (uint8_t)(prescaler - 1);
    *propseg = 4;  /* PROPSEG + 1 = 5 TQ */
    *pseg1 = 4;    /* PSEG1 + 1 = 5 TQ */
    *pseg2 = 3;    /* PSEG2 + 1 = 4 TQ */
    
    /* Calculate actual baudrate */
    actualBaudrate = clockFreq / (prescaler * tq);
    
    return actualBaudrate;
}

uint32_t ClockManager_CalcSpiDivider(clock_name_t clockSource,
                                      uint32_t desiredBaudrate,
                                      uint8_t *prescaler,
                                      uint8_t *scaler)
{
    uint32_t clockFreq = ClockManager_GetFrequency(clockSource);
    uint32_t ratio;
    uint8_t bestPrescaler = 0;
    uint8_t bestScaler = 0;
    uint32_t bestError = 0xFFFFFFFF;
    uint32_t actualBaudrate;
    
    /* Prescaler values: 1, 2, 4, 8 (encoded as 0, 1, 2, 3) */
    static const uint8_t prescalerValues[] = {1, 2, 4, 8};
    
    if (clockFreq == 0 || desiredBaudrate == 0)
    {
        return 0;
    }
    
    ratio = clockFreq / desiredBaudrate;
    
    /* Find best combination */
    for (uint8_t p = 0; p < 4; p++)
    {
        uint32_t scalerVal = ratio / prescalerValues[p];
        
        if (scalerVal > 0 && scalerVal <= 256)
        {
            uint32_t actual = clockFreq / (prescalerValues[p] * scalerVal);
            uint32_t error = (actual > desiredBaudrate) ? 
                             (actual - desiredBaudrate) : (desiredBaudrate - actual);
            
            if (error < bestError)
            {
                bestError = error;
                bestPrescaler = p;
                bestScaler = (uint8_t)(scalerVal - 1);
            }
        }
    }
    
    *prescaler = bestPrescaler;
    *scaler = bestScaler;
    
    actualBaudrate = clockFreq / (prescalerValues[bestPrescaler] * (bestScaler + 1));
    
    return actualBaudrate;
}
