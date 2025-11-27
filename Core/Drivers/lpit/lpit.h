/**
 * @file    lpit.h
 * @brief   LPIT (Low Power Interrupt Timer) Driver for S32K144
 * @details
 * Driver LPIT cung cấp các API để:
 * - Cấu hình và điều khiển 4 kênh timer độc lập
 * - Tạo periodic interrupts với độ chính xác cao
 * - Hỗ trợ chain mode để tạo timer 64-bit
 * - Timing, delay, PWM generation
 * - Hoạt động trong low-power modes
 * 
 * LPIT Features:
 * - 4 kênh timer 32-bit độc lập
 * - Có thể chain 2 kênh thành timer 64-bit
 * - Periodic counter mode
 * - Dual 16-bit periodic counter mode
 * - Hoạt động trong Doze và Debug modes
 * 
 * @author  PhucPH32
 * @date    28/11/2025
 * @version 1.0
 * 
 * @note    
 * - LPIT clock phải được cấu hình trước khi sử dụng
 * - Timer đếm xuống từ TVAL về 0
 * - Mỗi kênh có thể trigger interrupt riêng
 * 
 * @warning 
 * - Phải enable clock cho LPIT trước khi sử dụng
 * - Không thay đổi TVAL khi timer đang chạy
 * 
 * @par Change Log:
 * - Version 1.0 (28/11/2025): Khởi tạo driver LPIT
 */

#ifndef LPIT_H
#define LPIT_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "lpit_reg.h"
#include "status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup LPIT_Definitions LPIT Definitions
 * @{
 */

/** @brief Số kênh LPIT tối đa */
#define LPIT_MAX_CHANNELS   (4U)

/**
 * @brief LPIT Timer Mode
 * @details Các chế độ hoạt động của timer
 */
typedef enum {
    LPIT_MODE_32BIT_PERIODIC    = 0U,   /**< 32-bit Periodic Counter */
    LPIT_MODE_DUAL_16BIT        = 1U,   /**< Dual 16-bit Periodic Counter */
    LPIT_MODE_TRIGGER_ACCUM     = 2U,   /**< Trigger Accumulator */
    LPIT_MODE_INPUT_CAPTURE     = 3U    /**< Input Capture */
} lpit_mode_t;

/**
 * @brief LPIT Channel Configuration
 * @details Cấu trúc cấu hình cho một kênh LPIT
 */
typedef struct {
    uint8_t channel;                /**< Số kênh LPIT (0-3) */
    lpit_mode_t mode;               /**< Timer mode */
    uint32_t period;                /**< Period value (timer reload value) */
    bool enableInterrupt;           /**< Enable interrupt khi timer expire */
    bool chainChannel;              /**< Chain với kênh trước (để tạo 64-bit timer) */
    bool startOnTrigger;            /**< Start timer khi có trigger */
    bool stopOnInterrupt;           /**< Stop timer khi có interrupt */
    bool reloadOnTrigger;           /**< Reload timer khi có trigger */
} lpit_channel_config_t;

/**
 * @brief LPIT Callback Function Type
 * @details Function pointer cho callback khi timer expire
 * @param channel Số kênh LPIT
 * @param userData Con trỏ tới user data
 */
typedef void (*lpit_callback_t)(uint8_t channel, void *userData);

/** @} */ /* End of LPIT_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup LPIT_Functions LPIT Functions
 * @{
 */

/**
 * @brief Khởi tạo LPIT module
 * @details
 * - Enable clock cho LPIT
 * - Reset LPIT module
 * - Enable module clock
 * - Cấu hình debug mode
 * 
 * @param[in] clockSource Clock source cho LPIT (SIRC, FIRC, SOSC, SPLL)
 * 
 * @return STATUS_SUCCESS nếu thành công
 * 
 * @note
 * - Clock source phải được cấu hình trong PCC
 * - SIRC (8 MHz) là lựa chọn tốt cho low-power applications
 * - FIRC (48 MHz) cho timing độ chính xác cao
 * 
 * @code
 * // Khởi tạo LPIT với SIRC (8 MHz)
 * LPIT_Init(LPIT_CLK_SRC_SIRC);
 * @endcode
 */
status_t LPIT_Init(lpit_clock_source_t clockSource);

/**
 * @brief Deinitialize LPIT module
 * @details
 * - Disable tất cả các kênh
 * - Disable module clock
 * - Disable clock gating
 * 
 * @return STATUS_SUCCESS nếu thành công
 */
status_t LPIT_Deinit(void);

/**
 * @brief Cấu hình một kênh LPIT
 * @details
 * Thiết lập tất cả các tham số cho một kênh timer:
 * - Timer mode
 * - Period (reload value)
 * - Interrupt enable
 * - Chain mode
 * - Trigger options
 * 
 * @param[in] config Con trỏ tới cấu trúc cấu hình
 * 
 * @return STATUS_SUCCESS nếu cấu hình thành công
 * @return STATUS_ERROR nếu tham số không hợp lệ
 * 
 * @note
 * - Timer phải được stop trước khi cấu hình
 * - Period = 0 sẽ không tạo interrupt
 * 
 * @code
 * // Cấu hình channel 0 cho 1ms interrupt với SIRC 8MHz
 * lpit_channel_config_t config = {
 *     .channel = 0,
 *     .mode = LPIT_MODE_32BIT_PERIODIC,
 *     .period = 8000 - 1,              // 8MHz / 8000 = 1ms
 *     .enableInterrupt = true,
 *     .chainChannel = false,
 *     .startOnTrigger = false,
 *     .stopOnInterrupt = false,
 *     .reloadOnTrigger = false
 * };
 * LPIT_ConfigChannel(&config);
 * @endcode
 */
status_t LPIT_ConfigChannel(const lpit_channel_config_t *config);

/**
 * @brief Start một kênh timer
 * @details Enable kênh timer để bắt đầu đếm
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * 
 * @return STATUS_SUCCESS nếu start thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 * 
 * @code
 * // Start channel 0
 * LPIT_StartChannel(0);
 * @endcode
 */
status_t LPIT_StartChannel(uint8_t channel);

/**
 * @brief Stop một kênh timer
 * @details Disable kênh timer để dừng đếm
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * 
 * @return STATUS_SUCCESS nếu stop thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t LPIT_StopChannel(uint8_t channel);

/**
 * @brief Thiết lập period cho một kênh
 * @details
 * Cập nhật timer reload value.
 * Giá trị này sẽ được load vào counter khi timer expire.
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * @param[in] period Period value (0 đến 0xFFFFFFFF)
 * 
 * @return STATUS_SUCCESS nếu thiết lập thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 * 
 * @note
 * - Period mới chỉ có hiệu lực sau khi timer reload
 * - Nên stop timer trước khi thay đổi period
 * 
 * @code
 * // Thay đổi period thành 2ms (SIRC 8MHz)
 * LPIT_SetPeriod(0, 16000 - 1);
 * @endcode
 */
status_t LPIT_SetPeriod(uint8_t channel, uint32_t period);

/**
 * @brief Lấy giá trị hiện tại của timer
 * @details Đọc giá trị đang đếm của timer
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * @param[out] value Con trỏ để lưu giá trị timer
 * 
 * @return STATUS_SUCCESS nếu đọc thành công
 * @return STATUS_ERROR nếu channel không hợp lệ hoặc value NULL
 * 
 * @note Timer đếm xuống từ TVAL về 0
 * 
 * @code
 * uint32_t currentValue;
 * LPIT_GetCurrentValue(0, &currentValue);
 * printf("Current timer value: %lu\n", currentValue);
 * @endcode
 */
status_t LPIT_GetCurrentValue(uint8_t channel, uint32_t *value);

/**
 * @brief Kiểm tra interrupt flag của kênh
 * @details Kiểm tra xem timer đã expire chưa
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * 
 * @return true nếu interrupt flag được set
 * @return false nếu chưa có interrupt hoặc channel không hợp lệ
 */
bool LPIT_GetInterruptFlag(uint8_t channel);

/**
 * @brief Clear interrupt flag của kênh
 * @details Xóa interrupt flag sau khi xử lý
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * 
 * @return STATUS_SUCCESS nếu clear thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 * 
 * @note Phải clear flag trong ISR để tránh re-trigger
 */
status_t LPIT_ClearInterruptFlag(uint8_t channel);

/**
 * @brief Enable interrupt cho kênh
 * @details Enable interrupt khi timer expire
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * 
 * @return STATUS_SUCCESS nếu enable thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t LPIT_EnableInterrupt(uint8_t channel);

/**
 * @brief Disable interrupt cho kênh
 * @details Disable interrupt khi timer expire
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * 
 * @return STATUS_SUCCESS nếu disable thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 */
status_t LPIT_DisableInterrupt(uint8_t channel);

/**
 * @brief Đăng ký callback function cho kênh
 * @details
 * Callback sẽ được gọi từ ISR khi timer expire
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * @param[in] callback Function pointer cho callback
 * @param[in] userData Con trỏ tới user data (có thể NULL)
 * 
 * @return STATUS_SUCCESS nếu đăng ký thành công
 * @return STATUS_ERROR nếu channel không hợp lệ hoặc callback NULL
 * 
 * @code
 * void myTimerCallback(uint8_t channel, void *userData) {
 *     // Toggle LED hoặc thực hiện tác vụ khác
 *     GPIO_TogglePin(LED_PORT, LED_PIN);
 * }
 * 
 * LPIT_InstallCallback(0, myTimerCallback, NULL);
 * @endcode
 */
status_t LPIT_InstallCallback(uint8_t channel, lpit_callback_t callback, void *userData);

/**
 * @brief Xử lý LPIT interrupt (gọi từ ISR)
 * @details
 * Hàm này clear interrupt flag và gọi callback đã đăng ký.
 * Phải được gọi trong LPIT ISR.
 * 
 * @param[in] channel Số kênh LPIT (0-3)
 * 
 * @note Hàm này tự động clear interrupt flag
 * 
 * @code
 * void LPIT0_Ch0_IRQHandler(void) {
 *     LPIT_IRQHandler(0);
 * }
 * @endcode
 */
void LPIT_IRQHandler(uint8_t channel);

/**
 * @brief Tạo delay bằng LPIT (blocking)
 * @details
 * Sử dụng LPIT để tạo delay chính xác.
 * Hàm này là blocking (chờ cho đến khi hết delay).
 * 
 * @param[in] channel Số kênh LPIT sử dụng (0-3)
 * @param[in] delayUs Thời gian delay tính bằng microseconds
 * 
 * @return STATUS_SUCCESS nếu delay thành công
 * @return STATUS_ERROR nếu channel không hợp lệ
 * 
 * @note
 * - Hàm này là blocking
 * - Không nên dùng trong interrupt context
 * - Clock source phải được biết để tính toán chính xác
 * 
 * @code
 * // Delay 1000us (1ms)
 * LPIT_DelayUs(0, 1000);
 * @endcode
 */
status_t LPIT_DelayUs(uint8_t channel, uint32_t delayUs);

/**
 * @brief Tính period value từ tần số và frequency
 * @details
 * Helper function để tính toán period value cho timer
 * 
 * @param[in] clockFreq Clock frequency (Hz)
 * @param[in] desiredFreq Desired frequency (Hz)
 * 
 * @return Period value để load vào TVAL register
 * 
 * @note Period = (clockFreq / desiredFreq) - 1
 * 
 * @code
 * // Tính period cho 1kHz với clock 8MHz
 * uint32_t period = LPIT_CalculatePeriod(8000000, 1000);
 * // period = 7999 (cho 1ms period)
 * @endcode
 */
uint32_t LPIT_CalculatePeriod(uint32_t clockFreq, uint32_t desiredFreq);

/** @} */ /* End of LPIT_Functions */

#endif /* LPIT_H */
