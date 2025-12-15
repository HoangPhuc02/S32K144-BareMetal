# UART Driver Documentation

## Tổng Quan

Driver LPUART (Low Power UART) cho S32K144 cung cấp interface để giao tiếp serial với các thiết bị ngoại vi, PC, hoặc các module khác. Driver hỗ trợ nhiều cấu hình baud rate, parity, stop bits, và data bits.

## Cấu Trúc Thư Mục

```
Core/Drivers/uart/
├── uart_reg.h      # Register-level definitions
├── uart.h          # High-level API header
└── uart.c          # Implementation
```

## Kiến Trúc Driver

### 1. Register Level (`uart_reg.h`)

File này chứa các định nghĩa thanh ghi cấp thấp:

```c
typedef struct {
    __I  uint32_t VERID;        // Version ID Register
    __I  uint32_t PARAM;        // Parameter Register
    __IO uint32_t GLOBAL;       // Global Register
    __IO uint32_t PINCFG;       // Pin Configuration
    __IO uint32_t BAUD;         // Baud Rate Register
    __IO uint32_t STAT;         // Status Register
    __IO uint32_t CTRL;         // Control Register
    __IO uint32_t DATA;         // Data Register
    __IO uint32_t MATCH;        // Match Address
    __IO uint32_t MODIR;        // Modem IrDA
    __IO uint32_t FIFO;         // FIFO Register
    __IO uint32_t WATER;        // Watermark Register
} LPUART_RegType;
```

**Base Addresses:**
- LPUART0: `0x4006A000`
- LPUART1: `0x4006B000`
- LPUART2: `0x4006C000`

**PCC Clock Indices:**
- PCC_LPUART0_INDEX: `106`
- PCC_LPUART1_INDEX: `107`
- PCC_LPUART2_INDEX: `108`

### 2. Driver API Level (`uart.h`, `uart.c`)

Cung cấp các hàm high-level để sử dụng UART.

## Cấu Hình và Khởi Tạo

### 1. Cấu Trúc Cấu Hình

```c
typedef struct {
    uint32_t          baudRate;     // Baud rate (9600, 115200, etc.)
    UART_Parity_t     parity;       // Parity mode
    UART_StopBits_t   stopBits;     // Number of stop bits
    UART_DataBits_t   dataBits;     // Number of data bits
    bool              enableTx;     // Enable transmitter
    bool              enableRx;     // Enable receiver
} UART_Config_t;
```

### 2. Các Enum Định Nghĩa

```c
// Parity
typedef enum {
    UART_PARITY_DISABLED = 0,       // No parity
    UART_PARITY_EVEN     = 2,       // Even parity
    UART_PARITY_ODD      = 3        // Odd parity
} UART_Parity_t;

// Stop bits
typedef enum {
    UART_ONE_STOP_BIT = 0,          // 1 stop bit
    UART_TWO_STOP_BIT = 1           // 2 stop bits
} UART_StopBits_t;

// Data bits
typedef enum {
    UART_8_DATA_BITS  = 0,          // 8 data bits (common)
    UART_9_DATA_BITS  = 1,          // 9 data bits
    UART_10_DATA_BITS = 2           // 10 data bits
} UART_DataBits_t;
```

### 3. Khởi Tạo UART Cơ Bản

```c
#include "uart.h"
#include "port.h"

// Bước 1: Enable clock cho UART và PORT
UART_EnableClock(0);             // Enable LPUART0 clock
PORT_EnableClock(PORT_B);        // Enable PORTB for pins

// Bước 2: Cấu hình pins (PTB0=RX, PTB1=TX)
PORT_SetPinMux(PORTB, 0, PORT_MUX_ALT2);  // LPUART0_RX
PORT_SetPinMux(PORTB, 1, PORT_MUX_ALT2);  // LPUART0_TX

// Bước 3: Cấu hình UART
UART_Config_t config;
UART_GetDefaultConfig(&config);  // Get default: 115200-8-N-1

// Hoặc cấu hình tùy chỉnh
config.baudRate = 9600;
config.parity = UART_PARITY_DISABLED;
config.stopBits = UART_ONE_STOP_BIT;
config.dataBits = UART_8_DATA_BITS;
config.enableTx = true;
config.enableRx = true;

// Bước 4: Khởi tạo
uint32_t srcClock = 8000000;     // 8 MHz FIRC clock
UART_Status_t status = UART_Init(LPUART0, &config, srcClock);

if (status != UART_STATUS_SUCCESS) {
    // Xử lý lỗi
}
```

### 4. Baud Rate Phổ Biến

| Baud Rate | Typical Usage |
|-----------|---------------|
| 9600      | Low-speed sensors, GPS |
| 19200     | Industrial protocols |
| 38400     | Moderate-speed communication |
| 57600     | High-speed data |
| 115200    | Debug console, fast data |
| 230400    | Very high-speed (check error rate) |

## Gửi Dữ Liệu (Transmit)

### 1. Gửi 1 Byte

```c
/**
 * Gửi 1 byte đơn giản
 */
void SendSingleByte(void)
{
    uint8_t data = 'A';
    
    UART_Status_t status = UART_SendByte(LPUART0, data);
    
    if (status != UART_STATUS_SUCCESS) {
        // Xử lý lỗi
    }
}
```

### 2. Gửi Chuỗi

```c
/**
 * Gửi một chuỗi ký tự
 */
void SendString(const char *str)
{
    UART_SendBlocking(LPUART0, (const uint8_t *)str, strlen(str));
}

// Sử dụng
SendString("Hello World!\r\n");
```

### 3. Gửi Buffer

```c
/**
 * Gửi một buffer dữ liệu
 */
void SendBuffer(const uint8_t *buffer, uint32_t length)
{
    UART_Status_t status = UART_SendBlocking(LPUART0, buffer, length);
    
    if (status == UART_STATUS_TIMEOUT) {
        // Xử lý timeout
    }
}

// Ví dụ
uint8_t txData[] = {0x01, 0x02, 0x03, 0x04};
SendBuffer(txData, sizeof(txData));
```

### 4. Printf-like Function

```c
#include <stdarg.h>
#include <stdio.h>

/**
 * UART printf implementation
 */
void UART_Printf(const char *format, ...)
{
    char buffer[128];
    va_list args;
    
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (len > 0) {
        UART_SendBlocking(LPUART0, (uint8_t *)buffer, len);
    }
}

// Sử dụng
int value = 123;
UART_Printf("Value = %d\r\n", value);
UART_Printf("Temperature: %.2f C\r\n", 25.5f);
```

## Nhận Dữ Liệu (Receive)

### 1. Nhận 1 Byte

```c
/**
 * Nhận 1 byte đơn giản
 */
uint8_t ReceiveSingleByte(void)
{
    uint8_t data;
    UART_Status_t status = UART_ReceiveByte(LPUART0, &data);
    
    if (status == UART_STATUS_SUCCESS) {
        return data;
    }
    
    return 0;
}
```

### 2. Nhận Buffer

```c
/**
 * Nhận một lượng dữ liệu cố định
 */
void ReceiveBuffer(uint8_t *buffer, uint32_t length)
{
    UART_Status_t status = UART_ReceiveBlocking(LPUART0, buffer, length);
    
    if (status == UART_STATUS_PARITY_ERROR) {
        // Xử lý lỗi parity
    } else if (status == UART_STATUS_FRAME_ERROR) {
        // Xử lý lỗi frame
    }
}

// Ví dụ
uint8_t rxBuffer[16];
ReceiveBuffer(rxBuffer, 16);
```

### 3. Nhận Cho Đến Ký Tự Kết Thúc

```c
/**
 * Nhận dữ liệu cho đến khi gặp newline
 */
uint32_t ReceiveLine(char *buffer, uint32_t maxLen)
{
    uint32_t count = 0;
    uint8_t ch;
    
    while (count < (maxLen - 1)) {
        if (UART_ReceiveByte(LPUART0, &ch) == UART_STATUS_SUCCESS) {
            if (ch == '\n' || ch == '\r') {
                break;
            }
            buffer[count++] = ch;
        }
    }
    
    buffer[count] = '\0';  // Null terminate
    return count;
}

// Sử dụng
char cmdBuffer[64];
uint32_t len = ReceiveLine(cmdBuffer, sizeof(cmdBuffer));
```

### 4. Echo - Nhận và Gửi Lại

```c
/**
 * Echo back received characters
 */
void UART_EchoTask(void)
{
    uint8_t ch;
    
    if (UART_IsRxReady(LPUART0)) {
        if (UART_ReceiveByte(LPUART0, &ch) == UART_STATUS_SUCCESS) {
            UART_SendByte(LPUART0, ch);  // Echo
        }
    }
}
```

## Ví Dụ Thực Tế

### 1. Debug Console

```c
/**
 * Setup debug console trên UART0
 */
void DebugConsole_Init(void)
{
    // Enable clocks
    UART_EnableClock(0);
    PORT_EnableClock(PORT_B);
    
    // Configure pins: PTB0=RX, PTB1=TX
    PORT_SetPinMux(PORTB, 0, PORT_MUX_ALT2);
    PORT_SetPinMux(PORTB, 1, PORT_MUX_ALT2);
    
    // Configure UART: 115200-8-N-1
    UART_Config_t config;
    UART_GetDefaultConfig(&config);
    UART_Init(LPUART0, &config, 8000000);
    
    UART_Printf("\r\n=== Debug Console Ready ===\r\n");
}

/**
 * Main loop - process commands
 */
void DebugConsole_Task(void)
{
    static char cmdBuffer[64];
    static uint32_t cmdIndex = 0;
    uint8_t ch;
    
    if (UART_IsRxReady(LPUART0)) {
        UART_ReceiveByte(LPUART0, &ch);
        
        if (ch == '\r' || ch == '\n') {
            cmdBuffer[cmdIndex] = '\0';
            
            // Process command
            if (strcmp(cmdBuffer, "help") == 0) {
                UART_Printf("Available commands: help, reset, status\r\n");
            } else if (strcmp(cmdBuffer, "reset") == 0) {
                UART_Printf("Resetting...\r\n");
                // Perform reset
            } else if (strcmp(cmdBuffer, "status") == 0) {
                UART_Printf("System OK\r\n");
            }
            
            cmdIndex = 0;
            UART_Printf("> ");  // Prompt
        } else if (cmdIndex < (sizeof(cmdBuffer) - 1)) {
            cmdBuffer[cmdIndex++] = ch;
            UART_SendByte(LPUART0, ch);  // Echo
        }
    }
}
```

### 2. GPS Module (NMEA)

```c
/**
 * Parse GPS NMEA sentences
 */
void GPS_ParseNMEA(void)
{
    static char nmeaBuffer[128];
    static uint32_t nmeaIndex = 0;
    uint8_t ch;
    
    // UART1 for GPS: 9600-8-N-1
    if (UART_IsRxReady(LPUART1)) {
        UART_ReceiveByte(LPUART1, &ch);
        
        if (ch == '$') {
            // Start of NMEA sentence
            nmeaIndex = 0;
            nmeaBuffer[nmeaIndex++] = ch;
        } else if (ch == '\n') {
            // End of sentence
            nmeaBuffer[nmeaIndex] = '\0';
            
            // Parse sentence
            if (strncmp(nmeaBuffer, "$GPGGA", 6) == 0) {
                // Parse GGA sentence (position)
                ParseGGA(nmeaBuffer);
            } else if (strncmp(nmeaBuffer, "$GPRMC", 6) == 0) {
                // Parse RMC sentence (recommended minimum)
                ParseRMC(nmeaBuffer);
            }
            
            nmeaIndex = 0;
        } else if (nmeaIndex < (sizeof(nmeaBuffer) - 1)) {
            nmeaBuffer[nmeaIndex++] = ch;
        }
    }
}
```

### 3. Modbus RTU Slave

```c
/**
 * Modbus RTU packet structure
 */
typedef struct {
    uint8_t slaveAddr;
    uint8_t function;
    uint16_t startAddr;
    uint16_t count;
    uint16_t crc;
} ModbusRTU_t;

/**
 * Receive Modbus packet
 */
bool Modbus_ReceivePacket(ModbusRTU_t *packet)
{
    uint8_t buffer[8];
    
    // Wait for packet (with timeout)
    UART_Status_t status = UART_ReceiveBlocking(LPUART2, buffer, 8);
    
    if (status == UART_STATUS_SUCCESS) {
        // Parse packet
        packet->slaveAddr = buffer[0];
        packet->function = buffer[1];
        packet->startAddr = (buffer[2] << 8) | buffer[3];
        packet->count = (buffer[4] << 8) | buffer[5];
        packet->crc = (buffer[6] << 8) | buffer[7];
        
        // Verify CRC
        if (Modbus_VerifyCRC(buffer, 8)) {
            return true;
        }
    }
    
    return false;
}

/**
 * Send Modbus response
 */
void Modbus_SendResponse(const uint8_t *data, uint32_t length)
{
    UART_SendBlocking(LPUART2, data, length);
    
    // RS485: Toggle DE/RE pins if needed
    // GPIO_WritePin(PORTA, 5, 0);  // DE low
}
```

### 4. AT Command Interface

```c
/**
 * Send AT command and wait for response
 */
bool AT_SendCommand(const char *cmd, char *response, uint32_t timeout_ms)
{
    // Send command
    UART_SendBlocking(LPUART0, (uint8_t *)cmd, strlen(cmd));
    UART_SendBlocking(LPUART0, (uint8_t *)"\r\n", 2);
    
    // Wait for response
    uint32_t startTime = GetTickCount();
    uint32_t idx = 0;
    
    while ((GetTickCount() - startTime) < timeout_ms) {
        uint8_t ch;
        if (UART_ReceiveByte(LPUART0, &ch) == UART_STATUS_SUCCESS) {
            if (ch == '\n') {
                response[idx] = '\0';
                return true;
            }
            if (idx < 127) {
                response[idx++] = ch;
            }
        }
    }
    
    return false;  // Timeout
}

// Sử dụng
char response[128];
if (AT_SendCommand("AT+CSQ", response, 1000)) {
    // Parse response
}
```

## Interrupt Mode

### 1. Enable Interrupts

```c
/**
 * Setup UART với interrupts
 */
void UART_SetupInterrupts(void)
{
    // Initialize UART
    UART_Config_t config;
    UART_GetDefaultConfig(&config);
    UART_Init(LPUART0, &config, 8000000);
    
    // Enable RX interrupt
    UART_EnableInterrupts(LPUART0, UART_INT_RX_READY);
    
    // Enable NVIC interrupt
    NVIC_EnableIRQ(LPUART0_RxTx_IRQn);
    NVIC_SetPriority(LPUART0_RxTx_IRQn, 3);
}
```

### 2. Interrupt Handler

```c
/**
 * UART0 interrupt handler
 */
void LPUART0_RxTx_IRQHandler(void)
{
    uint32_t status = UART_GetStatusFlags(LPUART0);
    
    // RX interrupt
    if (status & LPUART_STAT_RDRF_MASK) {
        uint8_t data = LPUART_READ_DATA(LPUART0);
        
        // Store in circular buffer
        RxBuffer[rxWriteIdx++] = data;
        if (rxWriteIdx >= RX_BUFFER_SIZE) {
            rxWriteIdx = 0;
        }
    }
    
    // TX interrupt
    if (status & LPUART_STAT_TDRE_MASK) {
        if (txCount > 0) {
            LPUART_WRITE_DATA(LPUART0, txBuffer[txIdx++]);
            txCount--;
        } else {
            // Disable TX interrupt
            UART_DisableInterrupts(LPUART0, UART_INT_TX_READY);
        }
    }
    
    // Error handling
    if (status & LPUART_STAT_OR_MASK) {
        // Overrun error
        UART_ClearStatusFlags(LPUART0, LPUART_STAT_OR_MASK);
    }
}
```

## Xử Lý Lỗi

### Các Mã Lỗi

```c
typedef enum {
    UART_STATUS_SUCCESS = 0,        // Thành công
    UART_STATUS_BUSY,               // UART đang bận
    UART_STATUS_TIMEOUT,            // Timeout
    UART_STATUS_PARITY_ERROR,       // Lỗi parity
    UART_STATUS_FRAME_ERROR,        // Lỗi frame
    UART_STATUS_NOISE_ERROR,        // Lỗi noise
    UART_STATUS_OVERRUN_ERROR,      // Lỗi overrun
    UART_STATUS_ERROR               // Lỗi chung
} UART_Status_t;
```

### Kiểm Tra và Xử Lý Lỗi

```c
/**
 * Check for UART errors
 */
void UART_CheckErrors(void)
{
    UART_Status_t error = UART_GetError(LPUART0);
    
    switch (error) {
        case UART_STATUS_PARITY_ERROR:
            UART_Printf("Parity error!\r\n");
            break;
            
        case UART_STATUS_FRAME_ERROR:
            UART_Printf("Frame error! Check baud rate.\r\n");
            break;
            
        case UART_STATUS_NOISE_ERROR:
            UART_Printf("Noise detected on line.\r\n");
            break;
            
        case UART_STATUS_OVERRUN_ERROR:
            UART_Printf("Data overrun! Processing too slow.\r\n");
            break;
            
        default:
            break;
    }
}

/**
 * Clear all errors
 */
void UART_ClearAllErrors(void)
{
    uint32_t errorFlags = LPUART_STAT_PF_MASK | 
                          LPUART_STAT_FE_MASK |
                          LPUART_STAT_NF_MASK |
                          LPUART_STAT_OR_MASK;
    
    UART_ClearStatusFlags(LPUART0, errorFlags);
}
```

## Helper Macros

### Register Level Macros

```c
// Enable/Disable TX/RX
LPUART_ENABLE_TX(LPUART0);
LPUART_DISABLE_TX(LPUART0);
LPUART_ENABLE_RX(LPUART0);
LPUART_DISABLE_RX(LPUART0);

// Reset
LPUART_SW_RESET(LPUART0);

// Status Checks
if (LPUART_IS_TX_READY(LPUART0)) { }
if (LPUART_IS_RX_READY(LPUART0)) { }
if (LPUART_IS_TX_COMPLETE(LPUART0)) { }
if (LPUART_IS_RX_ACTIVE(LPUART0)) { }

// Error Checks
if (LPUART_HAS_PARITY_ERROR(LPUART0)) { }
if (LPUART_HAS_FRAME_ERROR(LPUART0)) { }
if (LPUART_HAS_NOISE_ERROR(LPUART0)) { }
if (LPUART_HAS_OVERRUN_ERROR(LPUART0)) { }

// Data Transfer
LPUART_WRITE_DATA(LPUART0, data);
data = LPUART_READ_DATA(LPUART0);

// Interrupt Control
LPUART_ENABLE_TX_INT(LPUART0);
LPUART_DISABLE_TX_INT(LPUART0);
LPUART_ENABLE_RX_INT(LPUART0);
LPUART_DISABLE_RX_INT(LPUART0);
```

## Pin Configuration

### S32K144 UART Pin Options

| Instance | RX Pin  | TX Pin  | Alternative Function |
|----------|---------|---------|----------------------|
| LPUART0  | PTB0    | PTB1    | ALT2 |
| LPUART0  | PTC6    | PTC7    | ALT2 |
| LPUART1  | PTC6    | PTC7    | ALT3 |
| LPUART1  | PTA2    | PTA3    | ALT6 |
| LPUART2  | PTA6    | PTA7    | ALT3 |
| LPUART2  | PTD8    | PTD9    | ALT3 |

### Cấu Hình RS485

Nếu sử dụng RS485, cần thêm DE/RE control:

```c
void UART_ConfigureRS485(void)
{
    // Configure UART pins
    PORT_SetPinMux(PORTB, 0, PORT_MUX_ALT2);  // RX
    PORT_SetPinMux(PORTB, 1, PORT_MUX_ALT2);  // TX
    
    // Configure DE/RE pin (GPIO)
    PORT_SetPinMux(PORTA, 5, PORT_MUX_GPIO);
    GPIO_SetPinDirection(PORTA, 5, GPIO_OUTPUT);
    GPIO_WritePin(PORTA, 5, 0);  // Default: Receive mode
}

void RS485_Send(const uint8_t *data, uint32_t length)
{
    GPIO_WritePin(PORTA, 5, 1);  // Enable transmit
    UART_SendBlocking(LPUART0, data, length);
    GPIO_WritePin(PORTA, 5, 0);  // Back to receive
}
```

## Baud Rate Calculation

### Công Thức

```
Baud Rate = Clock / ((OSR + 1) × SBR)
```

Trong đó:
- **OSR**: Oversampling Ratio (4-32)
- **SBR**: Baud Rate Modulo Divisor (1-8191)

### Ví Dụ Tính Toán

Clock = 8 MHz, Target = 115200 baud

```
115200 = 8000000 / ((15 + 1) × SBR)
SBR = 8000000 / (16 × 115200) = 4.34

Chọn SBR = 4:
Actual baud = 8000000 / (16 × 4) = 125000 baud
Error = (125000 - 115200) / 115200 = 8.5%  (too high!)

Thử OSR = 13:
SBR = 8000000 / (14 × 115200) = 4.96
Chọn SBR = 5:
Actual baud = 8000000 / (14 × 5) = 114285 baud
Error = 0.8%  (acceptable!)
```

Driver tự động tìm OSR và SBR tối ưu trong `UART_Init()`.

## Best Practices

### 1. Baud Rate Error

- Error < 2%: OK
- Error 2-3%: Có thể hoạt động
- Error > 3%: Có thể có lỗi

### 2. Flow Control

Hardware flow control (RTS/CTS) cho baud rate cao:

```c
// Enable hardware flow control
// Configure CTS/RTS pins
PORT_SetPinMux(PORTB, 2, PORT_MUX_ALT2);  // CTS
PORT_SetPinMux(PORTB, 3, PORT_MUX_ALT2);  // RTS
```

### 3. Buffer Management

Sử dụng circular buffer cho interrupt mode:

```c
#define RX_BUFFER_SIZE 256

typedef struct {
    uint8_t buffer[RX_BUFFER_SIZE];
    volatile uint32_t writeIdx;
    volatile uint32_t readIdx;
} CircularBuffer_t;

CircularBuffer_t rxBuffer;

void Buffer_Write(CircularBuffer_t *buf, uint8_t data) {
    buf->buffer[buf->writeIdx++] = data;
    if (buf->writeIdx >= RX_BUFFER_SIZE) {
        buf->writeIdx = 0;
    }
}

bool Buffer_Read(CircularBuffer_t *buf, uint8_t *data) {
    if (buf->readIdx != buf->writeIdx) {
        *data = buf->buffer[buf->readIdx++];
        if (buf->readIdx >= RX_BUFFER_SIZE) {
            buf->readIdx = 0;
        }
        return true;
    }
    return false;
}
```

### 4. Timeout Implementation

```c
#define UART_RX_TIMEOUT_MS  100

bool UART_ReceiveWithTimeout(uint8_t *data, uint32_t timeout_ms)
{
    uint32_t startTime = GetTickCount();
    
    while ((GetTickCount() - startTime) < timeout_ms) {
        if (UART_IsRxReady(LPUART0)) {
            return (UART_ReceiveByte(LPUART0, data) == UART_STATUS_SUCCESS);
        }
    }
    
    return false;  // Timeout
}
```

### 5. Error Recovery

```c
void UART_ErrorRecovery(void)
{
    // Disable TX/RX
    LPUART_DISABLE_TX(LPUART0);
    LPUART_DISABLE_RX(LPUART0);
    
    // Clear all errors
    UART_ClearAllErrors();
    
    // Software reset
    LPUART_SW_RESET(LPUART0);
    base->GLOBAL &= ~LPUART_GLOBAL_RST_MASK;
    
    // Re-enable TX/RX
    LPUART_ENABLE_TX(LPUART0);
    LPUART_ENABLE_RX(LPUART0);
}
```

## Debugging

### 1. Loopback Test

```c
/**
 * Test UART bằng cách kết nối TX->RX
 */
void UART_LoopbackTest(void)
{
    uint8_t txData = 0x55;
    uint8_t rxData;
    
    UART_SendByte(LPUART0, txData);
    
    // Wait and read
    for (volatile int i = 0; i < 1000; i++);
    
    if (UART_ReceiveByte(LPUART0, &rxData) == UART_STATUS_SUCCESS) {
        if (rxData == txData) {
            UART_Printf("Loopback test PASSED\r\n");
        } else {
            UART_Printf("Loopback test FAILED: sent 0x%02X, got 0x%02X\r\n", 
                       txData, rxData);
        }
    }
}
```

### 2. Oscilloscope Check

- **Idle state**: TX high (~3.3V)
- **Start bit**: TX low
- **Data bits**: LSB first
- **Stop bit**: TX high
- **Baud rate**: Measure bit period

### 3. Common Issues

| Vấn đề | Nguyên nhân | Giải pháp |
|--------|-------------|-----------|
| Garbage characters | Sai baud rate | Check clock, recalculate |
| Missing characters | Overrun | Use interrupts/DMA, increase buffer |
| Frame errors | Sai config | Check bits, parity, stop |
| No transmission | Pin config | Check PORT mux settings |
| Random errors | Noise, grounding | Better wiring, add caps |

## Tài Liệu Tham Khảo

1. **S32K1xx Reference Manual** - Chapter: LPUART
2. **Application Notes**:
   - AN4905: Getting Started with LPUART
   - AN4244: UART Error Detection
3. **Standards**:
   - RS-232 (±12V, single-ended)
   - RS-485 (differential, multi-drop)

## Performance Tips

### 1. DMA Transfer

Cho high-speed, large data:

```c
// Configure DMA for UART TX
// (Requires DMA driver - not covered here)
DMA_ConfigureChannel(DMA_CH0, &dmaConfig);
DMA_StartTransfer(DMA_CH0, txBuffer, &LPUART0->DATA, length);
```

### 2. Optimize Baud Rate

```c
// Use highest clock source
// Enable FLL/PLL if needed
SCG_SetSystemClock(80000000);  // 80 MHz

// Reconfigure UART with higher clock
UART_Init(LPUART0, &config, 80000000);
```

---

**Phiên bản**: 1.0  
**Ngày cập nhật**: 24/11/2025  
**Tác giả**: PhucPH32
