# UART DMA Functions - Hướng dẫn sử dụng

## Tổng quan

Module UART DMA cung cấp các API để truyền và nhận dữ liệu qua UART sử dụng DMA (Direct Memory Access), giúp giảm tải CPU và tăng hiệu suất hệ thống.

### Ưu điểm của UART DMA:
- **Giảm tải CPU**: CPU không cần can thiệp trong quá trình transfer dữ liệu
- **Hiệu suất cao**: Transfer được thực hiện song song với CPU
- **Non-blocking**: Cho phép CPU thực hiện công việc khác trong khi transfer
- **Hiệu quả với dữ liệu lớn**: Đặc biệt hiệu quả khi transfer buffer lớn

## Cấu trúc API

### 1. Khởi tạo và cấu hình

#### `UART_ConfigTxDMA()`
Cấu hình UART để sử dụng DMA cho transmission.

```c
UART_Status_t UART_ConfigTxDMA(LPUART_RegType *base, uint8_t dmaChannel);
```

**Parameters:**
- `base`: Con trỏ tới UART peripheral (LPUART0, LPUART1, LPUART2)
- `dmaChannel`: Số kênh DMA sử dụng (0-15)

**Returns:**
- `UART_STATUS_SUCCESS`: Cấu hình thành công
- `UART_STATUS_ERROR`: Lỗi tham số

**Example:**
```c
// Cấu hình LPUART1 TX với DMA channel 0
UART_ConfigTxDMA(LPUART1, 0);
```

---

#### `UART_ConfigRxDMA()`
Cấu hình UART để sử dụng DMA cho reception.

```c
UART_Status_t UART_ConfigRxDMA(LPUART_RegType *base, uint8_t dmaChannel);
```

**Parameters:**
- `base`: Con trỏ tới UART peripheral
- `dmaChannel`: Số kênh DMA sử dụng (0-15)

**Returns:**
- `UART_STATUS_SUCCESS`: Cấu hình thành công
- `UART_STATUS_ERROR`: Lỗi tham số

**Example:**
```c
// Cấu hình LPUART1 RX với DMA channel 1
UART_ConfigRxDMA(LPUART1, 1);
```

---

### 2. Transfer Functions (Non-blocking)

#### `UART_SendDMA()`
Gửi dữ liệu qua UART sử dụng DMA (non-blocking).

```c
UART_Status_t UART_SendDMA(LPUART_RegType *base, uint8_t dmaChannel,
                           const uint8_t *txBuff, uint32_t txSize);
```

**Parameters:**
- `base`: Con trỏ tới UART peripheral
- `dmaChannel`: Số kênh DMA
- `txBuff`: Buffer chứa dữ liệu cần gửi
- `txSize`: Số byte cần gửi

**Returns:**
- `UART_STATUS_SUCCESS`: DMA transfer bắt đầu thành công
- `UART_STATUS_ERROR`: Lỗi tham số hoặc cấu hình

**Đặc điểm:**
- Non-blocking: Hàm return ngay sau khi khởi động DMA
- Cần kiểm tra completion status sau đó

**Example:**
```c
uint8_t txData[] = "Hello DMA!";

// Start DMA transmission
UART_SendDMA(LPUART1, 0, txData, sizeof(txData)-1);

// Check completion
while (!DMA_IsChannelDone(0)) {
    // Do other work here
}
DMA_ClearDone(0);
```

---

#### `UART_ReceiveDMA()`
Nhận dữ liệu qua UART sử dụng DMA (non-blocking).

```c
UART_Status_t UART_ReceiveDMA(LPUART_RegType *base, uint8_t dmaChannel,
                              uint8_t *rxBuff, uint32_t rxSize);
```

**Parameters:**
- `base`: Con trỏ tới UART peripheral
- `dmaChannel`: Số kênh DMA
- `rxBuff`: Buffer để lưu dữ liệu nhận được
- `rxSize`: Số byte cần nhận

**Returns:**
- `UART_STATUS_SUCCESS`: DMA transfer bắt đầu thành công
- `UART_STATUS_ERROR`: Lỗi tham số hoặc cấu hình

**Đặc điểm:**
- Non-blocking: Hàm return ngay sau khi khởi động DMA
- DMA sẽ tự động nhận đủ `rxSize` bytes

**Example:**
```c
uint8_t rxData[100];

// Start DMA reception
UART_ReceiveDMA(LPUART1, 1, rxData, sizeof(rxData));

// Check completion
while (!DMA_IsChannelDone(1)) {
    // Do other work here
}
DMA_ClearDone(1);

// Process received data
printf("Received: %s\n", rxData);
```

---

### 3. Transfer Functions (Blocking)

#### `UART_SendDMABlocking()`
Gửi dữ liệu qua UART sử dụng DMA (blocking).

```c
UART_Status_t UART_SendDMABlocking(LPUART_RegType *base, uint8_t dmaChannel,
                                   const uint8_t *txBuff, uint32_t txSize);
```

**Parameters:**
- `base`: Con trỏ tới UART peripheral
- `dmaChannel`: Số kênh DMA
- `txBuff`: Buffer chứa dữ liệu cần gửi
- `txSize`: Số byte cần gửi

**Returns:**
- `UART_STATUS_SUCCESS`: Transfer hoàn tất thành công
- `UART_STATUS_TIMEOUT`: Timeout
- `UART_STATUS_ERROR`: Lỗi tham số

**Đặc điểm:**
- Blocking: Hàm chờ cho đến khi transfer hoàn tất
- Tự động clear done flag
- Có timeout protection

**Example:**
```c
uint8_t message[] = "Blocking transfer\r\n";

// Send and wait for completion
if (UART_SendDMABlocking(LPUART1, 0, message, sizeof(message)-1) == UART_STATUS_SUCCESS) {
    printf("Transfer complete!\n");
}
```

---

#### `UART_ReceiveDMABlocking()`
Nhận dữ liệu qua UART sử dụng DMA (blocking).

```c
UART_Status_t UART_ReceiveDMABlocking(LPUART_RegType *base, uint8_t dmaChannel,
                                      uint8_t *rxBuff, uint32_t rxSize);
```

**Parameters:**
- `base`: Con trỏ tới UART peripheral
- `dmaChannel`: Số kênh DMA
- `rxBuff`: Buffer để lưu dữ liệu nhận được
- `rxSize`: Số byte cần nhận

**Returns:**
- `UART_STATUS_SUCCESS`: Transfer hoàn tất thành công
- `UART_STATUS_TIMEOUT`: Timeout
- `UART_STATUS_ERROR`: Lỗi tham số

**Example:**
```c
uint8_t rxBuffer[50];

// Receive and wait for completion
if (UART_ReceiveDMABlocking(LPUART1, 1, rxBuffer, 50) == UART_STATUS_SUCCESS) {
    printf("Received: %s\n", rxBuffer);
}
```

---

### 4. Control Functions

#### `UART_DisableTxDMA()`
Disable UART TX DMA request.

```c
void UART_DisableTxDMA(LPUART_RegType *base);
```

**Example:**
```c
UART_DisableTxDMA(LPUART1);
```

---

#### `UART_DisableRxDMA()`
Disable UART RX DMA request.

```c
void UART_DisableRxDMA(LPUART_RegType *base);
```

**Example:**
```c
UART_DisableRxDMA(LPUART1);
```

---

## Complete Usage Example

### Basic Setup

```c
#include "uart.h"
#include "dma.h"

#define TX_DMA_CHANNEL  0
#define RX_DMA_CHANNEL  1

void UART_DMA_Setup(void)
{
    UART_Config_t uartConfig;
    
    /* 1. Initialize DMA module */
    DMA_Init();
    
    /* 2. Enable UART clock */
    UART_EnableClock(1);  // LPUART1
    
    /* 3. Configure UART pins (PTC6/PTC7 for LPUART1) */
    PORT_SetPinMux(PORTC, 6, PORT_MUX_ALT2);  // RX
    PORT_SetPinMux(PORTC, 7, PORT_MUX_ALT2);  // TX
    
    /* 4. Initialize UART */
    UART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate = 115200;
    UART_Init(LPUART1, &uartConfig, 40000000);
    
    /* 5. Configure UART for DMA */
    UART_ConfigTxDMA(LPUART1, TX_DMA_CHANNEL);
    UART_ConfigRxDMA(LPUART1, RX_DMA_CHANNEL);
}
```

### Example 1: Simple TX with DMA

```c
void Example_SimpleTX(void)
{
    const char *message = "Hello from UART DMA!\r\n";
    uint32_t length = strlen(message);
    
    // Send using blocking DMA
    UART_SendDMABlocking(LPUART1, TX_DMA_CHANNEL, 
                        (const uint8_t *)message, length);
}
```

### Example 2: Non-blocking TX with Callback

```c
volatile bool txDone = false;

void DMA_TxCallback(uint8_t channel, void *userData)
{
    txDone = true;
}

void Example_NonBlockingTX(void)
{
    const char *message = "Non-blocking transfer\r\n";
    uint32_t length = strlen(message);
    
    // Install callback
    DMA_InstallCallback(TX_DMA_CHANNEL, DMA_TxCallback, NULL);
    DMA_EnableChannelInterrupt(TX_DMA_CHANNEL);
    
    // Start transfer
    txDone = false;
    UART_SendDMA(LPUART1, TX_DMA_CHANNEL, 
                (const uint8_t *)message, length);
    
    // Do other work while transfer is in progress
    while (!txDone) {
        // Process other tasks
        DoOtherWork();
    }
}
```

### Example 3: Echo Server with DMA

```c
void Example_EchoServer(void)
{
    uint8_t rxBuffer[100];
    uint32_t rxLength = 10;  // Expect 10 bytes
    
    while (1) {
        // Receive data
        if (UART_ReceiveDMABlocking(LPUART1, RX_DMA_CHANNEL, 
                                    rxBuffer, rxLength) == UART_STATUS_SUCCESS) {
            
            // Echo back
            UART_SendDMABlocking(LPUART1, TX_DMA_CHANNEL,
                               rxBuffer, rxLength);
        }
    }
}
```

### Example 4: Large Data Transfer

```c
void Example_LargeTransfer(void)
{
    #define BUFFER_SIZE 1024
    uint8_t largeBuffer[BUFFER_SIZE];
    
    // Fill buffer with data
    for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
        largeBuffer[i] = (uint8_t)(i & 0xFF);
    }
    
    // Send large buffer using DMA
    UART_SendDMA(LPUART1, TX_DMA_CHANNEL, largeBuffer, BUFFER_SIZE);
    
    // Do other work while DMA is transferring
    while (!DMA_IsChannelDone(TX_DMA_CHANNEL)) {
        ProcessOtherTasks();
    }
    
    DMA_ClearDone(TX_DMA_CHANNEL);
}
```

---

## Best Practices

### 1. Buffer Management
```c
// ❌ BAD: Stack buffer may be out of scope
void BadExample(void)
{
    uint8_t txBuffer[100] = "Hello";
    UART_SendDMA(LPUART1, 0, txBuffer, 5);
    // txBuffer goes out of scope while DMA is still running!
}

// ✅ GOOD: Use global or static buffer
static uint8_t g_txBuffer[100];

void GoodExample(void)
{
    strcpy((char *)g_txBuffer, "Hello");
    UART_SendDMA(LPUART1, 0, g_txBuffer, 5);
}
```

### 2. DMA Channel Selection
```c
// Use separate channels for TX and RX
#define UART_TX_DMA_CHANNEL  0
#define UART_RX_DMA_CHANNEL  1

// Configure both
UART_ConfigTxDMA(LPUART1, UART_TX_DMA_CHANNEL);
UART_ConfigRxDMA(LPUART1, UART_RX_DMA_CHANNEL);
```

### 3. Error Handling
```c
UART_Status_t status = UART_SendDMABlocking(LPUART1, 0, buffer, size);

switch (status) {
    case UART_STATUS_SUCCESS:
        // Transfer completed successfully
        break;
        
    case UART_STATUS_TIMEOUT:
        // Handle timeout
        DMA_StopChannel(0);
        break;
        
    case UART_STATUS_ERROR:
        // Handle error
        break;
}
```

### 4. Callback Usage
```c
void DMA_Callback(uint8_t channel, void *userData)
{
    // Keep callback SHORT and FAST
    // Set flags, don't do heavy processing
    
    transferComplete = true;
    
    // Don't call blocking functions here!
}
```

---

## Performance Comparison

| Method | CPU Load | Transfer Speed | Complexity | Use Case |
|--------|----------|---------------|------------|----------|
| Polling | Very High | Slow | Low | Small data, simple apps |
| Interrupt | Medium | Medium | Medium | Moderate data, byte-by-byte |
| **DMA** | **Very Low** | **Fast** | Medium | **Large data, high performance** |

---

## Troubleshooting

### Problem: DMA transfer doesn't start
**Solution:**
1. Check DMA_Init() was called
2. Verify UART_ConfigTxDMA/RxDMA was called
3. Check UART is initialized and enabled
4. Verify buffer addresses are valid

### Problem: Transfer hangs
**Solution:**
1. Check if UART pins are configured correctly
2. Verify baud rate matches receiver
3. Check for hardware flow control issues
4. Use timeout in blocking functions

### Problem: Received data is corrupted
**Solution:**
1. Verify baud rate is correct
2. Check clock configuration
3. Ensure buffer is not being modified during transfer
4. Check for buffer overflow

---

## See Also

- `dma.h` - DMA driver API
- `uart.h` - UART driver API
- `uart_dma_example.c` - Complete working examples
- S32K144 Reference Manual - Chapter 21 (eDMA), Chapter 46 (LPUART)
