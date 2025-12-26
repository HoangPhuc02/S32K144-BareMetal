#include "clock_manager.h"
#include "S32K144.h"

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static uint32_t s_clockFrequencies[CLOCK_SRC_COUNT] = {0};

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

static uint32_t GetSircFreq(void)
{
    /* SIRC: 8 MHz default */
    return 8000000U;
}

static uint32_t GetFircFreq(void)
{
    /* FIRC: 48 MHz default */
    return 48000000U;
}

static uint32_t GetSoscFreq(void)
{
    /* SOSC: Depends on external crystal, typically 8 MHz */
    return 8000000U;
}

static uint32_t GetSpllFreq(void)
{
    uint32_t srcFreq;
    uint32_t prediv, mult;
    
    /* Get SPLL source (SOSC or FIRC) */
    if ((SCG->SPLLCFG & SCG_SPLLCFG_SOURCE_MASK) == 0)
    {
        srcFreq = GetSoscFreq();
    }
    else
    {
        srcFreq = GetFircFreq();
    }
    
    /* Get PREDIV and MULT */
    prediv = ((SCG->SPLLCFG & SCG_SPLLCFG_PREDIV_MASK) >> SCG_SPLLCFG_PREDIV_SHIFT) + 1U;
    mult = ((SCG->SPLLCFG & SCG_SPLLCFG_MULT_MASK) >> SCG_SPLLCFG_MULT_SHIFT) + 16U;
    
    /* SPLL_CLK = (SRC_CLK / PREDIV) * MULT / 2 */
    return (srcFreq / prediv) * mult / 2U;
}

static uint32_t GetSystemClockSource(void)
{
    uint32_t scs = (SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT;
    
    switch (scs)
    {
        case 1: return GetSoscFreq();
        case 2: return GetSircFreq();
        case 3: return GetFircFreq();
        case 6: return GetSpllFreq();
        default: return 0;
    }
}

static void UpdateCoreClock(void)
{
    uint32_t divCore = ((SCG->CSR & SCG_CSR_DIVCORE_MASK) >> SCG_CSR_DIVCORE_SHIFT) + 1U;
    s_clockFrequencies[CLOCK_SRC_CORE] = GetSystemClockSource() / divCore;
}

static void UpdateBusClock(void)
{
    uint32_t divBus = ((SCG->CSR & SCG_CSR_DIVBUS_MASK) >> SCG_CSR_DIVBUS_SHIFT) + 1U;
    s_clockFrequencies[CLOCK_SRC_BUS] = s_clockFrequencies[CLOCK_SRC_CORE] / divBus;
}

static uint32_t GetPccClockFreq(uint32_t pccIndex)
{
    uint32_t pccReg = ((volatile uint32_t *)PCC)[pccIndex];
    uint32_t pcs;
    
    /* Check if clock is enabled */
    if ((pccReg & PCC_CGC_MASK) == 0)
    {
        return 0;
    }
    
    /* Get peripheral clock source */
    pcs = (pccReg & PCC_PCS_MASK) >> PCC_PCS_SHIFT;
    
    switch (pcs)
    {
        case 1: return GetSoscFreq();
        case 2: return GetSircFreq();
        case 3: return GetFircFreq();
        case 6: return GetSpllFreq();
        default: return 0;
    }
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
    /* Update system clocks */
    UpdateCoreClock();
    UpdateBusClock();
    
    /* Update peripheral clocks - use PCC indices for S32K144 */
    s_clockFrequencies[CLOCK_SRC_FLASH] = s_clockFrequencies[CLOCK_SRC_BUS];
    s_clockFrequencies[CLOCK_SRC_FLEXCAN] = GetPccClockFreq(PCC_FlexCAN0_INDEX);
    s_clockFrequencies[CLOCK_SRC_LPUART0] = GetPccClockFreq(PCC_LPUART0_INDEX);
    s_clockFrequencies[CLOCK_SRC_LPUART1] = GetPccClockFreq(PCC_LPUART1_INDEX);
    s_clockFrequencies[CLOCK_SRC_LPUART2] = GetPccClockFreq(PCC_LPUART2_INDEX);
    s_clockFrequencies[CLOCK_SRC_LPSPI0] = GetPccClockFreq(PCC_LPSPI0_INDEX);
    s_clockFrequencies[CLOCK_SRC_LPSPI1] = GetPccClockFreq(PCC_LPSPI1_INDEX);
    s_clockFrequencies[CLOCK_SRC_LPI2C0] = GetPccClockFreq(PCC_LPI2C0_INDEX);
    s_clockFrequencies[CLOCK_SRC_FTM0] = GetPccClockFreq(PCC_FTM0_INDEX);
    s_clockFrequencies[CLOCK_SRC_FTM1] = GetPccClockFreq(PCC_FTM1_INDEX);
    s_clockFrequencies[CLOCK_SRC_FTM2] = GetPccClockFreq(PCC_FTM2_INDEX);
    s_clockFrequencies[CLOCK_SRC_FTM3] = GetPccClockFreq(PCC_FTM3_INDEX);
}

uint32_t ClockManager_GetFrequency(ClockSource_t source)
{
    if (source >= CLOCK_SRC_COUNT)
    {
        return 0;
    }
    return s_clockFrequencies[source];
}

uint32_t ClockManager_GetCoreFreq(void)
{
    return s_clockFrequencies[CLOCK_SRC_CORE];
}

uint32_t ClockManager_GetBusFreq(void)
{
    return s_clockFrequencies[CLOCK_SRC_BUS];
}

uint32_t ClockManager_CalcUartDivider(ClockSource_t clockSource,
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

uint32_t ClockManager_CalcCanTiming(ClockSource_t clockSource,
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

uint32_t ClockManager_CalcSpiDivider(ClockSource_t clockSource,
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
