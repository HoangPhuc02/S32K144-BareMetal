/**
 * @file    lpit_example.c
 * @brief   LPIT (Low Power Interrupt Timer) Examples for S32K144
 * @details
 * File này chứa các ví dụ sử dụng LPIT driver:
 * 1. Basic Timer với Interrupt
 * 2. LED Blink với LPIT
 * 3. Multiple Channels
 * 4. Delay Function
 * 5. Chain Mode (64-bit Timer)
 * 6. Periodic Task Scheduling
 * 
 * @author  PhucPH32
 * @date    28/11/2025
 * @version 1.0
 */

#include "lpit.h"
#include "gpio.h"
#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* LED Configuration */
#define LED_RED_PORT    PORTD
#define LED_RED_PIN     15U
#define LED_GREEN_PORT  PORTD
#define LED_GREEN_PIN   16U
#define LED_BLUE_PORT   PORTD
#define LED_BLUE_PIN    0U

/* LPIT Clock Frequency (SIRC = 8 MHz) */
#define LPIT_CLOCK_FREQ     8000000U

/*******************************************************************************
 * Example 1: Basic Timer với Interrupt
 ******************************************************************************/

/**
 * @brief Callback function cho LPIT timer
 */
void BasicTimerCallback(uint8_t channel, void *userData)
{
    static uint32_t counter = 0;
    counter++;
    
    printf("Timer interrupt! Counter = %lu\n", counter);
    
    /* Blink LED mỗi 500ms */
    if (counter % 2 == 0) {
        GPIO_SetPin(LED_RED_PORT, LED_RED_PIN);
    } else {
        GPIO_ClearPin(LED_RED_PORT, LED_RED_PIN);
    }
}

/**
 * @example Example 1: Basic Timer với 1ms Interrupt
 * @details
 * Cấu hình LPIT channel 0 để tạo interrupt mỗi 1ms.
 * LED đỏ sẽ blink mỗi 500ms (toggle mỗi 2 interrupts).
 */
void LPIT_Example1_BasicTimer(void)
{
    lpit_channel_config_t config;
    
    printf("=== LPIT Example 1: Basic Timer ===\n");
    
    /* Khởi tạo GPIO cho LED */
    GPIO_Init(LED_RED_PORT, LED_RED_PIN, GPIO_OUTPUT);
    
    /* Khởi tạo LPIT với SIRC (8 MHz) */
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    /* Cấu hình channel 0 cho 1ms interrupt */
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = LPIT_CalculatePeriod(LPIT_CLOCK_FREQ, 1000);  /* 1kHz = 1ms */
    config.enableInterrupt = true;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    
    LPIT_ConfigChannel(&config);
    
    /* Đăng ký callback */
    LPIT_InstallCallback(0, BasicTimerCallback, NULL);
    
    /* Enable NVIC interrupt cho LPIT channel 0 */
    // NVIC_EnableIRQ(LPIT0_Ch0_IRQn);  /* Uncomment nếu cần */
    
    /* Start timer */
    LPIT_StartChannel(0);
    
    printf("Timer started! LED should blink every 500ms\n");
    
    /* Main loop - chờ interrupts */
    while (1) {
        /* CPU có thể vào sleep mode ở đây để tiết kiệm năng lượng */
        __asm("WFI");  /* Wait For Interrupt */
    }
}

/*******************************************************************************
 * Example 2: LED Blink với Different Rates
 ******************************************************************************/

void RedLedCallback(uint8_t channel, void *userData)
{
    GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN);
}

void GreenLedCallback(uint8_t channel, void *userData)
{
    GPIO_TogglePin(LED_GREEN_PORT, LED_GREEN_PIN);
}

void BlueLedCallback(uint8_t channel, void *userData)
{
    GPIO_TogglePin(LED_BLUE_PORT, LED_BLUE_PIN);
}

/**
 * @example Example 2: Multiple LEDs với Different Blink Rates
 * @details
 * Sử dụng 3 kênh LPIT để blink 3 LEDs với tốc độ khác nhau:
 * - LED Red: 1 Hz (500ms on, 500ms off)
 * - LED Green: 2 Hz (250ms on, 250ms off)
 * - LED Blue: 4 Hz (125ms on, 125ms off)
 */
void LPIT_Example2_MultipleLEDs(void)
{
    lpit_channel_config_t config;
    
    printf("=== LPIT Example 2: Multiple LEDs ===\n");
    
    /* Khởi tạo GPIO cho các LED */
    GPIO_Init(LED_RED_PORT, LED_RED_PIN, GPIO_OUTPUT);
    GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_OUTPUT);
    GPIO_Init(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_OUTPUT);
    
    /* Khởi tạo LPIT */
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    /* Cấu hình Channel 0: Red LED - 1 Hz (toggle every 500ms) */
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = LPIT_CalculatePeriod(LPIT_CLOCK_FREQ, 2);  /* 2 Hz toggle = 1 Hz blink */
    config.enableInterrupt = true;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(0, RedLedCallback, NULL);
    
    /* Cấu hình Channel 1: Green LED - 2 Hz (toggle every 250ms) */
    config.channel = 1;
    config.period = LPIT_CalculatePeriod(LPIT_CLOCK_FREQ, 4);  /* 4 Hz toggle = 2 Hz blink */
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(1, GreenLedCallback, NULL);
    
    /* Cấu hình Channel 2: Blue LED - 4 Hz (toggle every 125ms) */
    config.channel = 2;
    config.period = LPIT_CalculatePeriod(LPIT_CLOCK_FREQ, 8);  /* 8 Hz toggle = 4 Hz blink */
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(2, BlueLedCallback, NULL);
    
    /* Enable NVIC interrupts */
    // NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
    // NVIC_EnableIRQ(LPIT0_Ch1_IRQn);
    // NVIC_EnableIRQ(LPIT0_Ch2_IRQn);
    
    /* Start tất cả timers */
    LPIT_StartChannel(0);
    LPIT_StartChannel(1);
    LPIT_StartChannel(2);
    
    printf("All LEDs blinking at different rates!\n");
    printf("Red: 1 Hz | Green: 2 Hz | Blue: 4 Hz\n");
    
    while (1) {
        __asm("WFI");
    }
}

/*******************************************************************************
 * Example 3: Blocking Delay Function
 ******************************************************************************/

/**
 * @example Example 3: Delay Function (Blocking)
 * @details
 * Sử dụng LPIT_DelayUs() để tạo delay chính xác.
 * Ví dụ này blink LED bằng cách dùng delay function.
 */
void LPIT_Example3_DelayFunction(void)
{
    printf("=== LPIT Example 3: Delay Function ===\n");
    
    /* Khởi tạo GPIO */
    GPIO_Init(LED_RED_PORT, LED_RED_PIN, GPIO_OUTPUT);
    
    /* Khởi tạo LPIT */
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    printf("LED blinking using LPIT_DelayUs()\n");
    
    while (1) {
        /* Turn ON LED */
        GPIO_SetPin(LED_RED_PORT, LED_RED_PIN);
        printf("LED ON\n");
        
        /* Delay 1 second (1,000,000 us) */
        LPIT_DelayUs(0, 1000000);
        
        /* Turn OFF LED */
        GPIO_ClearPin(LED_RED_PORT, LED_RED_PIN);
        printf("LED OFF\n");
        
        /* Delay 1 second */
        LPIT_DelayUs(0, 1000000);
    }
}

/*******************************************************************************
 * Example 4: Precise Timing Measurement
 ******************************************************************************/

/**
 * @example Example 4: Timing Measurement
 * @details
 * Sử dụng LPIT để đo thời gian thực thi của một đoạn code.
 */
void LPIT_Example4_TimingMeasurement(void)
{
    lpit_channel_config_t config;
    uint32_t startValue, endValue, elapsedTicks;
    float elapsedMs;
    
    printf("=== LPIT Example 4: Timing Measurement ===\n");
    
    /* Khởi tạo LPIT */
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    /* Cấu hình channel 0 với giá trị max để đếm xuống */
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = 0xFFFFFFFFU;  /* Max value */
    config.enableInterrupt = false;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    
    LPIT_ConfigChannel(&config);
    LPIT_StartChannel(0);
    
    /* Lấy giá trị start */
    LPIT_GetCurrentValue(0, &startValue);
    
    /* Code cần đo thời gian */
    for (volatile uint32_t i = 0; i < 100000; i++) {
        __asm("NOP");
    }
    
    /* Lấy giá trị end */
    LPIT_GetCurrentValue(0, &endValue);
    
    /* Tính elapsed time (timer đếm xuống) */
    elapsedTicks = startValue - endValue;
    elapsedMs = (float)elapsedTicks / (float)(LPIT_CLOCK_FREQ / 1000);
    
    printf("Start value: %lu\n", startValue);
    printf("End value: %lu\n", endValue);
    printf("Elapsed ticks: %lu\n", elapsedTicks);
    printf("Elapsed time: %.3f ms\n", elapsedMs);
    
    LPIT_StopChannel(0);
}

/*******************************************************************************
 * Example 5: Chain Mode (64-bit Timer)
 ******************************************************************************/

/**
 * @example Example 5: Chain Mode for 64-bit Timer
 * @details
 * Chain 2 kênh LPIT để tạo timer 64-bit.
 * Channel 1 chain với Channel 0 để extend timer range.
 */
void LPIT_Example5_ChainMode(void)
{
    lpit_channel_config_t config;
    
    printf("=== LPIT Example 5: Chain Mode ===\n");
    
    /* Khởi tạo LPIT */
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    /* Cấu hình Channel 0 (Lower 32-bit) */
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = 0xFFFFFFFFU;  /* Max value */
    config.enableInterrupt = false;
    config.chainChannel = false;  /* Base timer */
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    LPIT_ConfigChannel(&config);
    
    /* Cấu hình Channel 1 (Upper 32-bit) - Chained to Channel 0 */
    config.channel = 1;
    config.period = 0xFFFFFFFFU;
    config.chainChannel = true;   /* Chain to previous channel (0) */
    config.enableInterrupt = true; /* Interrupt khi 64-bit timer expire */
    LPIT_ConfigChannel(&config);
    
    /* Install callback cho channel 1 */
    LPIT_InstallCallback(1, BasicTimerCallback, NULL);
    
    /* Start timers (start channel 0 trước) */
    LPIT_StartChannel(0);
    LPIT_StartChannel(1);
    
    printf("64-bit timer started (chained channels 0 and 1)\n");
    printf("This creates a very long period timer!\n");
    
    while (1) {
        /* Check current values */
        uint32_t val0, val1;
        LPIT_GetCurrentValue(0, &val0);
        LPIT_GetCurrentValue(1, &val1);
        
        printf("Timer values: CH0=%lu, CH1=%lu\n", val0, val1);
        
        /* Delay 1 second */
        LPIT_DelayUs(2, 1000000);  /* Use channel 2 for delay */
    }
}

/*******************************************************************************
 * Example 6: Periodic Task Scheduling
 ******************************************************************************/

/* Task counters */
static volatile uint32_t s_task1Counter = 0;
static volatile uint32_t s_task2Counter = 0;
static volatile uint32_t s_task3Counter = 0;

void Task1_10ms(uint8_t channel, void *userData)
{
    s_task1Counter++;
    /* Task chạy mỗi 10ms */
    // Đọc sensor, xử lý signal, etc.
}

void Task2_100ms(uint8_t channel, void *userData)
{
    s_task2Counter++;
    /* Task chạy mỗi 100ms */
    // Update display, gửi data, etc.
}

void Task3_1000ms(uint8_t channel, void *userData)
{
    s_task3Counter++;
    /* Task chạy mỗi 1 second */
    printf("Task counters - T1:%lu, T2:%lu, T3:%lu\n", 
           s_task1Counter, s_task2Counter, s_task3Counter);
    GPIO_TogglePin(LED_RED_PORT, LED_RED_PIN);
}

/**
 * @example Example 6: Periodic Task Scheduling
 * @details
 * Sử dụng LPIT để schedule nhiều tasks với periods khác nhau:
 * - Task 1: Every 10ms (100 Hz)
 * - Task 2: Every 100ms (10 Hz)
 * - Task 3: Every 1000ms (1 Hz)
 */
void LPIT_Example6_TaskScheduling(void)
{
    lpit_channel_config_t config;
    
    printf("=== LPIT Example 6: Task Scheduling ===\n");
    
    /* Khởi tạo GPIO */
    GPIO_Init(LED_RED_PORT, LED_RED_PIN, GPIO_OUTPUT);
    
    /* Khởi tạo LPIT */
    LPIT_Init(LPIT_CLK_SRC_SIRC);
    
    /* Channel 0: Task1 - 10ms period (100 Hz) */
    config.channel = 0;
    config.mode = LPIT_MODE_32BIT_PERIODIC;
    config.period = LPIT_CalculatePeriod(LPIT_CLOCK_FREQ, 100);
    config.enableInterrupt = true;
    config.chainChannel = false;
    config.startOnTrigger = false;
    config.stopOnInterrupt = false;
    config.reloadOnTrigger = false;
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(0, Task1_10ms, NULL);
    
    /* Channel 1: Task2 - 100ms period (10 Hz) */
    config.channel = 1;
    config.period = LPIT_CalculatePeriod(LPIT_CLOCK_FREQ, 10);
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(1, Task2_100ms, NULL);
    
    /* Channel 2: Task3 - 1000ms period (1 Hz) */
    config.channel = 2;
    config.period = LPIT_CalculatePeriod(LPIT_CLOCK_FREQ, 1);
    LPIT_ConfigChannel(&config);
    LPIT_InstallCallback(2, Task3_1000ms, NULL);
    
    /* Enable NVIC interrupts */
    // NVIC_EnableIRQ(LPIT0_Ch0_IRQn);
    // NVIC_EnableIRQ(LPIT0_Ch1_IRQn);
    // NVIC_EnableIRQ(LPIT0_Ch2_IRQn);
    
    /* Start tất cả tasks */
    LPIT_StartChannel(0);
    LPIT_StartChannel(1);
    LPIT_StartChannel(2);
    
    printf("Task scheduling started!\n");
    printf("Task1: 10ms | Task2: 100ms | Task3: 1000ms\n");
    
    while (1) {
        __asm("WFI");
    }
}

/*******************************************************************************
 * ISR Examples (phải thêm vào vector table)
 ******************************************************************************/

/**
 * @brief LPIT Channel 0 ISR
 */
void LPIT0_Ch0_IRQHandler(void)
{
    LPIT_IRQHandler(0);
}

/**
 * @brief LPIT Channel 1 ISR
 */
void LPIT0_Ch1_IRQHandler(void)
{
    LPIT_IRQHandler(1);
}

/**
 * @brief LPIT Channel 2 ISR
 */
void LPIT0_Ch2_IRQHandler(void)
{
    LPIT_IRQHandler(2);
}

/**
 * @brief LPIT Channel 3 ISR
 */
void LPIT0_Ch3_IRQHandler(void)
{
    LPIT_IRQHandler(3);
}

/*******************************************************************************
 * Main Function - Run Examples
 ******************************************************************************/

int main(void)
{
    printf("\n");
    printf("==================================\n");
    printf("   LPIT Examples for S32K144      \n");
    printf("==================================\n\n");
    
    /* Chọn example để chạy */
    
    // LPIT_Example1_BasicTimer();
    // LPIT_Example2_MultipleLEDs();
    // LPIT_Example3_DelayFunction();
    // LPIT_Example4_TimingMeasurement();
    // LPIT_Example5_ChainMode();
    LPIT_Example6_TaskScheduling();
    
    return 0;
}
