/**
 * @file    lpit.c
 * @brief   LPIT (Low Power Interrupt Timer) Driver Implementation for S32K144
 * @details
 * Implementation của LPIT driver cho phép:
 * - Tạo periodic interrupts với độ chính xác cao
 * - Timing và delay functions
 * - Hỗ trợ 4 kênh độc lập
 * - Chain mode để tạo timer 64-bit
 * - Hoạt động trong low-power modes
 * 
 * LPIT Operation:
 * - Timer đếm xuống từ TVAL về 0
 * - Khi đếm về 0, interrupt flag được set và timer reload TVAL
 * - Có thể chain 2 kênh để tạo timer 64-bit
 * 
 * @author  PhucPH32
 * @date    28/11/2025
 * @version 1.0
 * 
 * @note    Clock source phải được cấu hình trước khi khởi tạo
 * @warning Không thay đổi TVAL khi timer đang active
 * 
 * @par Change Log:
 * - Version 1.0 (28/11/2025): Khởi tạo driver LPIT
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "lpit.h"
#include <stddef.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/** @brief Clock frequency tracking */
static uint32_t s_lpitClockFreq = 8000000U;  /* Default: 8 MHz (SIRC) */

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

/** @brief Callback function pointers cho mỗi kênh */
static lpit_callback_t s_lpitCallbacks[LPIT_MAX_CHANNELS] = {NULL};

/** @brief User data pointers cho mỗi kênh */
static void *s_lpitUserData[LPIT_MAX_CHANNELS] = {NULL};

/** @brief Cờ đánh dấu LPIT đã được khởi tạo */
static bool s_lpitInitialized = false;

/*******************************************************************************
 * Private Function Prototypes
 ******************************************************************************/

/**
 * @brief Validate channel number
 * @param channel Số kênh LPIT
 * @return true nếu channel hợp lệ (0-3)
 */
static inline bool LPIT_IsValidChannel(uint8_t channel);

/**
 * @brief Enable clock cho LPIT module
 * @param clockSource Clock source selection
 * @return STATUS_SUCCESS nếu thành công
 */
static status_t LPIT_EnableClock(lpit_clock_source_t clockSource);

/**
 * @brief Get clock frequency theo clock source
 * @param clockSource Clock source
 * @return Clock frequency in Hz
 */
static uint32_t LPIT_GetClockFrequency(lpit_clock_source_t clockSource);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief Kiểm tra channel có hợp lệ không
 */
static inline bool LPIT_IsValidChannel(uint8_t channel)
{
    return (channel < LPIT_MAX_CHANNELS);
}

/**
 * @brief Enable clock cho LPIT
 */
static status_t LPIT_EnableClock(lpit_clock_source_t clockSource)
{
    uint32_t pccReg;
    
    /* Disable clock trước khi cấu hình */
    PCC_LPIT0 &= ~PCC_CGC_MASK;
    
    /* Cấu hình clock source */
    pccReg = PCC_LPIT0;
    pccReg &= ~PCC_PCS_MASK;  /* Clear PCS bits */
    pccReg |= ((uint32_t)clockSource << PCC_PCS_SHIFT) & PCC_PCS_MASK;
    PCC_LPIT0 = pccReg;
    
    /* Enable clock */
    PCC_LPIT0 |= PCC_CGC_MASK;
    
    /* Lưu clock frequency */
    s_lpitClockFreq = LPIT_GetClockFrequency(clockSource);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Get clock frequency
 */
static uint32_t LPIT_GetClockFrequency(lpit_clock_source_t clockSource)
{
    uint32_t freq = 8000000U;  /* Default: 8 MHz */
    
    switch (clockSource) {
        case LPIT_CLK_SRC_SOSC:
            freq = 8000000U;   /* System Oscillator: 8 MHz */
            break;
        case LPIT_CLK_SRC_SIRC:
            freq = 8000000U;   /* Slow IRC: 8 MHz */
            break;
        case LPIT_CLK_SRC_FIRC:
            freq = 48000000U;  /* Fast IRC: 48 MHz */
            break;
        case LPIT_CLK_SRC_SPLL:
            freq = 80000000U;  /* System PLL: 80 MHz (typical) */
            break;
        default:
            freq = 8000000U;
            break;
    }
    
    return freq;
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Khởi tạo LPIT module
 */
status_t LPIT_Init(lpit_clock_source_t clockSource)
{
    uint8_t i;
    
    /* Enable clock cho LPIT */
    LPIT_EnableClock(clockSource);
    
    /* Disable module clock để cấu hình */
    LPIT0->MCR &= ~LPIT_MCR_M_CEN_MASK;
    
    /* Software reset */
    LPIT0->MCR |= LPIT_MCR_SW_RST_MASK;
    LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;
    
    /* Enable Debug mode - LPIT hoạt động khi CPU ở debug mode */
    LPIT0->MCR |= LPIT_MCR_DBG_EN_MASK;
    
    /* Disable Doze mode - LPIT dừng trong Doze mode để tiết kiệm năng lượng */
    LPIT0->MCR &= ~LPIT_MCR_DOZE_EN_MASK;
    
    /* Clear tất cả interrupt flags */
    LPIT0->MSR = 0x0FU;  /* Write 1 to clear */
    
    /* Disable tất cả interrupts */
    LPIT0->MIER = 0x00U;
    
    /* Disable và reset tất cả các kênh */
    for (i = 0U; i < LPIT_MAX_CHANNELS; i++) {
        LPIT0->CHANNEL[i].TCTRL = 0U;
        LPIT0->CHANNEL[i].TVAL = 0U;
        s_lpitCallbacks[i] = NULL;
        s_lpitUserData[i] = NULL;
    }
    
    /* Enable module clock */
    LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;
    
    s_lpitInitialized = true;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Deinitialize LPIT module
 */
status_t LPIT_Deinit(void)
{
    uint8_t i;
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    /* Disable tất cả các kênh */
    for (i = 0U; i < LPIT_MAX_CHANNELS; i++) {
        LPIT_StopChannel(i);
        LPIT0->CHANNEL[i].TCTRL = 0U;
        LPIT0->CHANNEL[i].TVAL = 0U;
    }
    
    /* Disable module clock */
    LPIT0->MCR &= ~LPIT_MCR_M_CEN_MASK;
    
    /* Disable clock gating */
    PCC_LPIT0 &= ~PCC_CGC_MASK;
    
    s_lpitInitialized = false;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Cấu hình một kênh LPIT
 */
status_t LPIT_ConfigChannel(const lpit_channel_config_t *config)
{
    uint32_t tctrl;
    uint8_t channel;
    
    /* Validate parameters */
    if (config == NULL) {
        return STATUS_ERROR;
    }
    
    channel = config->channel;
    
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    /* Disable channel trước khi cấu hình */
    LPIT_StopChannel(channel);
    
    /* Set timer value (period) */
    LPIT0->CHANNEL[channel].TVAL = config->period;
    
    /* Cấu hình Timer Control Register */
    tctrl = 0U;
    
    /* Set timer mode */
    tctrl |= ((uint32_t)config->mode << LPIT_TCTRL_MODE_SHIFT) & LPIT_TCTRL_MODE_MASK;
    
    /* Chain channel nếu cần */
    if (config->chainChannel) {
        tctrl |= LPIT_TCTRL_CHAIN_MASK;
    }
    
    /* Start on trigger */
    if (config->startOnTrigger) {
        tctrl |= LPIT_TCTRL_TSOT_MASK;
    }
    
    /* Stop on interrupt */
    if (config->stopOnInterrupt) {
        tctrl |= LPIT_TCTRL_TSOI_MASK;
    }
    
    /* Reload on trigger */
    if (config->reloadOnTrigger) {
        tctrl |= LPIT_TCTRL_TROT_MASK;
    }
    
    /* Write to TCTRL register */
    LPIT0->CHANNEL[channel].TCTRL = tctrl;
    
    /* Enable/disable interrupt */
    if (config->enableInterrupt) {
        LPIT_EnableInterrupt(channel);
    } else {
        LPIT_DisableInterrupt(channel);
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Start một kênh timer
 */
status_t LPIT_StartChannel(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    /* Clear interrupt flag nếu có */
    LPIT_ClearInterruptFlag(channel);
    
    /* Enable timer */
    LPIT0->CHANNEL[channel].TCTRL |= LPIT_TCTRL_T_EN_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Stop một kênh timer
 */
status_t LPIT_StopChannel(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Disable timer */
    LPIT0->CHANNEL[channel].TCTRL &= ~LPIT_TCTRL_T_EN_MASK;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Thiết lập period cho một kênh
 */
status_t LPIT_SetPeriod(uint8_t channel, uint32_t period)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Set timer value */
    LPIT0->CHANNEL[channel].TVAL = period;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Lấy giá trị hiện tại của timer
 */
status_t LPIT_GetCurrentValue(uint8_t channel, uint32_t *value)
{
    if (!LPIT_IsValidChannel(channel) || (value == NULL)) {
        return STATUS_ERROR;
    }
    
    /* Read current timer value */
    *value = LPIT0->CHANNEL[channel].CVAL;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Kiểm tra interrupt flag của kênh
 */
bool LPIT_GetInterruptFlag(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return false;
    }
    
    return ((LPIT0->MSR & (1U << channel)) != 0U);
}

/**
 * @brief Clear interrupt flag của kênh
 */
status_t LPIT_ClearInterruptFlag(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Write 1 to clear */
    LPIT0->MSR = (1U << channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Enable interrupt cho kênh
 */
status_t LPIT_EnableInterrupt(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Set interrupt enable bit */
    LPIT0->MIER |= (1U << channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Disable interrupt cho kênh
 */
status_t LPIT_DisableInterrupt(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    /* Clear interrupt enable bit */
    LPIT0->MIER &= ~(1U << channel);
    
    return STATUS_SUCCESS;
}

/**
 * @brief Đăng ký callback function cho kênh
 */
status_t LPIT_InstallCallback(uint8_t channel, lpit_callback_t callback, void *userData)
{
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (callback == NULL) {
        return STATUS_ERROR;
    }
    
    s_lpitCallbacks[channel] = callback;
    s_lpitUserData[channel] = userData;
    
    return STATUS_SUCCESS;
}

/**
 * @brief Xử lý LPIT interrupt (gọi từ ISR)
 */
void LPIT_IRQHandler(uint8_t channel)
{
    if (!LPIT_IsValidChannel(channel)) {
        return;
    }
    
    /* Clear interrupt flag */
    LPIT_ClearInterruptFlag(channel);
    
    /* Gọi callback nếu có */
    if (s_lpitCallbacks[channel] != NULL) {
        s_lpitCallbacks[channel](channel, s_lpitUserData[channel]);
    }
}

/**
 * @brief Tạo delay bằng LPIT (blocking)
 */
status_t LPIT_DelayUs(uint8_t channel, uint32_t delayUs)
{
    uint32_t ticks;
    uint32_t timeout = 0xFFFFFFFFU;
    lpit_channel_config_t config;
    
    if (!LPIT_IsValidChannel(channel)) {
        return STATUS_ERROR;
    }
    
    if (!s_lpitInitialized) {
        return STATUS_ERROR;
    }
    
    if (delayUs == 0U) {
        return STATUS_SUCCESS;
    }
    
    /* Tính số ticks cần thiết */
    /* ticks = (delayUs * clockFreq) / 1000000 */
    ticks = (delayUs * (s_lpitClockFreq / 1000000U));
    
    if (ticks == 0U) {
        ticks = 1U;
    }
    
    /* Cấu hình timer cho one-shot mode */
    config.channel = channel;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = ticks - 1U;
    config.enableInterrupt = false;  /* Polling mode */
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = true;   /* Stop after one timeout */
    config.reloadOnTrigger = false;
    
    LPIT_ConfigChannel(&config);
    
    /* Clear flag trước khi start */
    LPIT_ClearInterruptFlag(channel);
    
    /* Start timer */
    LPIT_StartChannel(channel);
    
    /* Chờ cho đến khi timer expire (polling) */
    while (!LPIT_GetInterruptFlag(channel) && (timeout > 0U)) {
        timeout--;
    }
    
    /* Clear flag */
    LPIT_ClearInterruptFlag(channel);
    
    if (timeout == 0U) {
        return STATUS_TIMEOUT;
    }
    
    return STATUS_SUCCESS;
}

/**
 * @brief Tính period value từ clock frequency và desired frequency
 */
uint32_t LPIT_CalculatePeriod(uint32_t clockFreq, uint32_t desiredFreq)
{
    uint32_t period;
    
    if (desiredFreq == 0U) {
        return 0U;
    }
    
    /* Period = (clockFreq / desiredFreq) - 1 */
    period = (clockFreq / desiredFreq);
    
    if (period > 0U) {
        period -= 1U;
    }
    
    return period;
}
