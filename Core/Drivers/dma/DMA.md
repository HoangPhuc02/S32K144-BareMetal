# DMA (Direct Memory Access) Driver

## 📋 Mục lục
- [Giới thiệu](#giới-thiệu)
- [Khái niệm DMA](#khái-niệm-dma)
- [Kiến trúc DMA trên S32K144](#kiến-trúc-dma-trên-s32k144)
- [Cấu trúc Driver](#cấu-trúc-driver)
- [Hướng dẫn sử dụng](#hướng-dẫn-sử-dụng)
- [Ví dụ](#ví-dụ)
- [API Reference](#api-reference)

## 🎯 Giới thiệu

DMA (Direct Memory Access) là một tính năng phần cứng cho phép truyền dữ liệu giữa bộ nhớ và các thiết bị ngoại vi **mà không cần CPU can thiệp**. Điều này giúp:
- ⚡ **Giảm tải CPU**: CPU có thể thực hiện các tác vụ khác trong khi DMA transfer dữ liệu
- 🚀 **Tăng hiệu suất**: Transfer dữ liệu nhanh hơn so với CPU copy
- 💾 **Tiết kiệm năng lượng**: CPU có thể vào chế độ sleep trong khi DMA hoạt động

## 📚 Khái niệm DMA

### 1. Transfer Control Descriptor (TCD)
Mỗi kênh DMA có một **TCD** (Transfer Control Descriptor) chứa tất cả thông tin cần thiết cho một transfer:
```
┌─────────────────────────────────┐
│      TCD (32 bytes)             │
├─────────────────────────────────┤
│ SADDR   - Source Address        │  ← Địa chỉ nguồn
│ SOFF    - Source Offset         │  ← Bước nhảy nguồn
│ ATTR    - Transfer Attributes   │  ← Kích thước transfer
│ NBYTES  - Minor Loop Bytes      │  ← Số bytes/minor loop
│ SLAST   - Source Last Adjust    │  ← Điều chỉnh cuối nguồn
│ DADDR   - Destination Address   │  ← Địa chỉ đích
│ DOFF    - Destination Offset    │  ← Bước nhảy đích
│ CITER   - Current Iteration     │  ← Số vòng lặp hiện tại
│ DLAST   - Dest Last Adjust      │  ← Điều chỉnh cuối đích
│ CSR     - Control/Status        │  ← Điều khiển & trạng thái
│ BITER   - Beginning Iteration   │  ← Số vòng lặp ban đầu
└─────────────────────────────────┘
```

### 2. Minor Loop và Major Loop

DMA transfer được chia thành 2 cấp độ vòng lặp:

#### **Minor Loop** (Inner Loop)
- Một lần transfer nhỏ, thường là 1, 2, 4, 16 hoặc 32 bytes
- Sau mỗi minor loop, địa chỉ source/dest được cộng thêm offset (SOFF/DOFF)

#### **Major Loop** (Outer Loop)
- Tập hợp nhiều minor loops
- Khi major loop hoàn thành, có thể trigger interrupt

```
Major Loop (Repeat BITER times)
┌─────────────────────────────────────────────────────┐
│                                                     │
│  Minor Loop 1: Transfer NBYTES bytes                │
│     Source: SADDR → SADDR + SOFF                   │
│     Dest:   DADDR → DADDR + DOFF                   │
│                                                     │
│  Minor Loop 2: Transfer NBYTES bytes                │
│     Source: SADDR + SOFF → SADDR + 2*SOFF          │
│     Dest:   DADDR + DOFF → DADDR + 2*DOFF          │
│                                                     │
│  ...                                                │
│                                                     │
│  Minor Loop N: Transfer NBYTES bytes                │
│                                                     │
└─────────────────────────────────────────────────────┘
    ↓
When CITER reaches 0: Major loop complete
    ↓
Trigger interrupt (if enabled)
```

### 3. Ví dụ cụ thể: Copy 1024 bytes

Muốn copy 1024 bytes từ `srcBuffer` sang `destBuffer`:

**Cấu hình:**
- Transfer size: 4 bytes (32-bit)
- Minor loop bytes: 4 bytes
- Major loop count: 256 (vì 1024 / 4 = 256)

**Quá trình:**
```
Major Loop 1:   srcBuffer[0:3]   → destBuffer[0:3]     (4 bytes)
Major Loop 2:   srcBuffer[4:7]   → destBuffer[4:7]     (4 bytes)
Major Loop 3:   srcBuffer[8:11]  → destBuffer[8:11]    (4 bytes)
...
Major Loop 256: srcBuffer[1020:1023] → destBuffer[1020:1023] (4 bytes)
                                                       ─────────
                                                  Total: 1024 bytes
```

## 🏗️ Kiến trúc DMA trên S32K144

S32K144 có **eDMA** (Enhanced DMA) với các đặc điểm:

### Hardware Components
```
┌──────────────────────────────────────────────────────┐
│                    S32K144 DMA                       │
├──────────────────────────────────────────────────────┤
│                                                      │
│  ┌────────────┐      ┌──────────────────────┐      │
│  │  DMAMUX    │──────│  DMA Controller      │      │
│  │            │      │                      │      │
│  │ 16 Slots   │      │  16 Channels         │      │
│  │            │      │  16 TCDs             │      │
│  └────────────┘      └──────────────────────┘      │
│       ↑                       ↑                     │
│       │                       │                     │
│  Peripheral               System Bus                │
│  Requests                (Memory Access)            │
│                                                      │
└──────────────────────────────────────────────────────┘
```

### 1. DMA Controller
- **16 kênh độc lập** (Channel 0-15)
- Mỗi kênh có một **TCD** riêng
- Hỗ trợ **Round-Robin arbitration** hoặc **Fixed priority**
- Có thể **link các kênh** với nhau (channel linking)

### 2. DMAMUX (DMA Multiplexer)
- Kết nối các **peripheral requests** với các **DMA channels**
- Mỗi kênh DMA có thể chọn 1 trong nhiều nguồn request:
  - UART TX/RX
  - SPI TX/RX
  - I2C TX/RX
  - ADC conversion complete
  - Timer overflow
  - Always-on (software trigger)

### 3. Các loại Transfer

#### Memory-to-Memory
```
┌──────────┐         DMA         ┌──────────┐
│  Source  │ ──────────────────→ │   Dest   │
│  Buffer  │                     │  Buffer  │
└──────────┘                     └──────────┘
```

#### Memory-to-Peripheral
```
┌──────────┐         DMA         ┌──────────┐
│  TX      │ ──────────────────→ │   UART   │
│  Buffer  │                     │   TX     │
└──────────┘                     └──────────┘
```

#### Peripheral-to-Memory
```
┌──────────┐         DMA         ┌──────────┐
│   ADC    │ ──────────────────→ │  RX      │
│   Data   │                     │  Buffer  │
└──────────┘                     └──────────┘
```

## 📂 Cấu trúc Driver

### Files
```
dma/
├── dma_reg.h       # Register definitions & bit masks
├── dma.h           # API declarations & data structures
└── dma.c           # Implementation
```

### Key Data Structures

#### `dma_channel_config_t`
Cấu trúc cấu hình đầy đủ cho một kênh DMA:
```c
typedef struct {
    uint8_t channel;                    // Số kênh (0-15)
    dmamux_source_t source;             // Nguồn request
    dma_transfer_type_t transferType;   // Loại transfer
    dma_transfer_size_t transferSize;   // Kích thước (1B, 2B, 4B...)
    dma_channel_priority_t priority;    // Ưu tiên kênh
    
    uint32_t sourceAddr;                // Địa chỉ nguồn
    int16_t sourceOffset;               // Offset nguồn
    int32_t sourceLastAddrAdjust;       // Điều chỉnh cuối nguồn
    
    uint32_t destAddr;                  // Địa chỉ đích
    int16_t destOffset;                 // Offset đích
    int32_t destLastAddrAdjust;         // Điều chỉnh cuối đích
    
    uint32_t minorLoopBytes;            // Bytes/minor loop
    uint16_t majorLoopCount;            // Số major loops
    
    bool enableInterrupt;               // Enable interrupt
    bool disableRequestAfterDone;       // Disable request sau khi xong
} dma_channel_config_t;
```

## 🚀 Hướng dẫn sử dụng

### Bước 1: Khởi tạo DMA Module
```c
// Khởi tạo DMA (enable clock, reset channels)
DMA_Init();
```

### Bước 2: Cấu hình kênh DMA
```c
dma_channel_config_t config;

config.channel = 0;                             // Chọn kênh 0
config.source = DMAMUX_SRC_ALWAYS_ON_60;        // Software trigger
config.transferType = DMA_TRANSFER_MEM_TO_MEM;
config.transferSize = DMA_TRANSFER_SIZE_4B;     // 4 bytes/transfer
config.priority = DMA_PRIORITY_NORMAL;

config.sourceAddr = (uint32_t)srcBuffer;
config.sourceOffset = 4;                        // +4 bytes mỗi transfer
config.sourceLastAddrAdjust = 0;

config.destAddr = (uint32_t)destBuffer;
config.destOffset = 4;                          // +4 bytes mỗi transfer
config.destLastAddrAdjust = 0;

config.minorLoopBytes = 4;                      // 4 bytes/minor loop
config.majorLoopCount = 256;                    // 256 major loops

config.enableInterrupt = true;
config.disableRequestAfterDone = true;

DMA_ConfigChannel(&config);
```

### Bước 3: Đăng ký Callback (optional)
```c
void myDmaCallback(uint8_t channel, void *userData) {
    printf("DMA channel %d completed!\n", channel);
}

DMA_InstallCallback(0, myDmaCallback, NULL);
```

### Bước 4: Start Transfer
```c
DMA_StartChannel(0);
```

### Bước 5: Chờ hoàn thành hoặc xử lý trong ISR
```c
// Polling mode
while (!DMA_IsChannelDone(0)) {
    // Chờ
}
DMA_ClearDone(0);

// Hoặc dùng interrupt (trong ISR)
void DMA0_IRQHandler(void) {
    DMA_IRQHandler(0);  // Tự động gọi callback
}
```

## 📝 Ví dụ

### Ví dụ 1: Memory Copy (Blocking)
```c
#include "dma.h"

int main(void) {
    uint32_t srcBuffer[256];
    uint32_t destBuffer[256];
    
    // Khởi tạo srcBuffer với dữ liệu
    for (int i = 0; i < 256; i++) {
        srcBuffer[i] = i;
    }
    
    // Khởi tạo DMA
    DMA_Init();
    
    // Copy 1024 bytes bằng DMA (blocking)
    DMA_MemCopy(0, srcBuffer, destBuffer, 1024);
    
    // Kiểm tra kết quả
    for (int i = 0; i < 256; i++) {
        if (destBuffer[i] != i) {
            printf("Error at index %d\n", i);
        }
    }
    
    printf("DMA copy completed successfully!\n");
    
    while (1);
}
```

### Ví dụ 2: Memory Copy với Interrupt
```c
#include "dma.h"

volatile bool g_dmaComplete = false;

void dmaCompleteCallback(uint8_t channel, void *userData) {
    g_dmaComplete = true;
    printf("DMA transfer complete on channel %d\n", channel);
}

int main(void) {
    uint8_t srcBuffer[512];
    uint8_t destBuffer[512];
    dma_channel_config_t config;
    
    // Khởi tạo DMA
    DMA_Init();
    
    // Cấu hình DMA channel 1
    config.channel = 1;
    config.source = DMAMUX_SRC_ALWAYS_ON_61;
    config.transferType = DMA_TRANSFER_MEM_TO_MEM;
    config.transferSize = DMA_TRANSFER_SIZE_1B;      // 1 byte/transfer
    config.priority = DMA_PRIORITY_HIGH;
    
    config.sourceAddr = (uint32_t)srcBuffer;
    config.sourceOffset = 1;                         // +1 byte
    config.sourceLastAddrAdjust = 0;
    
    config.destAddr = (uint32_t)destBuffer;
    config.destOffset = 1;                           // +1 byte
    config.destLastAddrAdjust = 0;
    
    config.minorLoopBytes = 1;                       // 1 byte/minor loop
    config.majorLoopCount = 512;                     // 512 major loops
    
    config.enableInterrupt = true;
    config.disableRequestAfterDone = true;
    
    DMA_ConfigChannel(&config);
    
    // Đăng ký callback
    DMA_InstallCallback(1, dmaCompleteCallback, NULL);
    
    // Enable DMA interrupt trong NVIC
    NVIC_EnableIRQ(DMA1_IRQn);
    
    // Start transfer
    DMA_StartChannel(1);
    
    // Chờ hoàn thành
    while (!g_dmaComplete) {
        // CPU có thể làm việc khác ở đây
    }
    
    printf("All done!\n");
    
    while (1);
}

// DMA ISR
void DMA1_IRQHandler(void) {
    DMA_IRQHandler(1);
}
```

### Ví dụ 3: UART TX với DMA
```c
#include "dma.h"
#include "uart.h"

#define UART0_TDR_ADDR  0x4006A008  // UART0 Transmit Data Register

int main(void) {
    char txBuffer[] = "Hello from DMA!\r\n";
    dma_channel_config_t config;
    
    // Khởi tạo UART và DMA
    UART_Init(UART0, 115200);
    DMA_Init();
    
    // Cấu hình DMA cho UART TX
    config.channel = 2;
    config.source = DMAMUX_SRC_LPUART0_TX;           // UART0 TX request
    config.transferType = DMA_TRANSFER_MEM_TO_PERIPH;
    config.transferSize = DMA_TRANSFER_SIZE_1B;      // 1 byte
    config.priority = DMA_PRIORITY_NORMAL;
    
    config.sourceAddr = (uint32_t)txBuffer;
    config.sourceOffset = 1;                         // +1 byte
    config.sourceLastAddrAdjust = 0;
    
    config.destAddr = UART0_TDR_ADDR;                // UART TX register
    config.destOffset = 0;                           // Không thay đổi
    config.destLastAddrAdjust = 0;
    
    config.minorLoopBytes = 1;
    config.majorLoopCount = sizeof(txBuffer) - 1;    // Không gửi NULL terminator
    
    config.enableInterrupt = true;
    config.disableRequestAfterDone = true;
    
    DMA_ConfigChannel(&config);
    
    // Start transfer (UART sẽ trigger DMA khi ready)
    DMA_StartChannel(2);
    
    // Chờ hoàn thành
    while (!DMA_IsChannelDone(2));
    
    printf("UART TX via DMA completed!\n");
    
    while (1);
}
```

### Ví dụ 4: ADC Scan với DMA
```c
#include "dma.h"
#include "adc.h"

#define ADC0_DATA_ADDR  0x4003B000  // ADC0 Data Result Register

int main(void) {
    uint16_t adcResults[16];  // Lưu 16 samples
    dma_channel_config_t config;
    
    // Khởi tạo ADC và DMA
    ADC_Init(ADC0);
    DMA_Init();
    
    // Cấu hình DMA cho ADC
    config.channel = 3;
    config.source = DMAMUX_SRC_ADC0;                 // ADC0 complete
    config.transferType = DMA_TRANSFER_PERIPH_TO_MEM;
    config.transferSize = DMA_TRANSFER_SIZE_2B;      // 2 bytes (16-bit)
    config.priority = DMA_PRIORITY_HIGH;
    
    config.sourceAddr = ADC0_DATA_ADDR;              // ADC data register
    config.sourceOffset = 0;                         // Không thay đổi
    config.sourceLastAddrAdjust = 0;
    
    config.destAddr = (uint32_t)adcResults;
    config.destOffset = 2;                           // +2 bytes (16-bit)
    config.destLastAddrAdjust = 0;
    
    config.minorLoopBytes = 2;                       // 2 bytes/transfer
    config.majorLoopCount = 16;                      // 16 samples
    
    config.enableInterrupt = true;
    config.disableRequestAfterDone = true;
    
    DMA_ConfigChannel(&config);
    DMA_StartChannel(3);
    
    // Trigger ADC conversions (hardware hoặc software)
    ADC_StartConversion(ADC0);
    
    // Chờ DMA hoàn thành
    while (!DMA_IsChannelDone(3));
    
    // Xử lý dữ liệu ADC
    for (int i = 0; i < 16; i++) {
        printf("ADC[%d] = %d\n", i, adcResults[i]);
    }
    
    while (1);
}
```

## 📖 API Reference

### Initialization Functions

#### `DMA_Init()`
Khởi tạo DMA module.
- **Return:** `STATUS_SUCCESS` nếu thành công
- **Note:** Phải gọi trước khi sử dụng bất kỳ API nào khác

#### `DMA_Deinit()`
Deinitialize DMA module.
- **Return:** `STATUS_SUCCESS` nếu thành công

### Configuration Functions

#### `DMA_ConfigChannel(const dma_channel_config_t *config)`
Cấu hình một kênh DMA.
- **Parameters:**
  - `config`: Con trỏ tới cấu trúc cấu hình
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`
- **Note:** Kênh phải được stop trước khi cấu hình lại

#### `DMA_SetChannelPriority(uint8_t channel, uint8_t priority)`
Thiết lập ưu tiên cho kênh.
- **Parameters:**
  - `channel`: Số kênh (0-15)
  - `priority`: Ưu tiên (0=thấp nhất, 15=cao nhất)
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

### Control Functions

#### `DMA_StartChannel(uint8_t channel)`
Bắt đầu transfer cho kênh.
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

#### `DMA_StopChannel(uint8_t channel)`
Dừng transfer cho kênh.
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

### Status Functions

#### `DMA_IsChannelActive(uint8_t channel)`
Kiểm tra kênh có đang active không.
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Return:** `true` nếu active, `false` nếu không

#### `DMA_IsChannelDone(uint8_t channel)`
Kiểm tra kênh đã hoàn thành chưa.
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Return:** `true` nếu hoàn thành, `false` nếu chưa

#### `DMA_ClearDone(uint8_t channel)`
Clear cờ DONE của kênh.
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

#### `DMA_GetRemainingMajorLoops(uint8_t channel, uint16_t *count)`
Lấy số major loops còn lại.
- **Parameters:**
  - `channel`: Số kênh (0-15)
  - `count`: Con trỏ để lưu kết quả
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

### Interrupt Functions

#### `DMA_EnableChannelInterrupt(uint8_t channel)`
Enable interrupt cho kênh.
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

#### `DMA_DisableChannelInterrupt(uint8_t channel)`
Disable interrupt cho kênh.
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

#### `DMA_InstallCallback(uint8_t channel, dma_callback_t callback, void *userData)`
Đăng ký callback function.
- **Parameters:**
  - `channel`: Số kênh (0-15)
  - `callback`: Function pointer
  - `userData`: User data (có thể NULL)
- **Return:** `STATUS_SUCCESS` hoặc `STATUS_ERROR`

#### `DMA_IRQHandler(uint8_t channel)`
Xử lý interrupt (gọi trong ISR).
- **Parameters:**
  - `channel`: Số kênh (0-15)
- **Note:** Hàm này clear interrupt flags và gọi callback

### Utility Functions

#### `DMA_MemCopy(uint8_t channel, const void *src, void *dest, uint32_t size)`
Copy memory bằng DMA (blocking).
- **Parameters:**
  - `channel`: Số kênh sử dụng
  - `src`: Địa chỉ nguồn
  - `dest`: Địa chỉ đích
  - `size`: Số bytes cần copy
- **Return:** `STATUS_SUCCESS`, `STATUS_ERROR`, hoặc `STATUS_TIMEOUT`
- **Note:** Hàm này là blocking (chờ cho đến khi hoàn thành)

## ⚠️ Lưu ý quan trọng

### 1. Address Alignment
Địa chỉ source và destination **phải aligned** đúng với transfer size:
- 1-byte transfer: Không yêu cầu alignment
- 2-byte transfer: Phải aligned to 2-byte boundary (địa chỉ chẵn)
- 4-byte transfer: Phải aligned to 4-byte boundary (địa chỉ chia hết cho 4)

### 2. Clock Requirements
Phải **enable clock** cho DMA và DMAMUX trước khi sử dụng:
```c
// Clock được enable tự động trong DMA_Init()
DMA_Init();
```

### 3. Interrupt Configuration
Nếu dùng interrupt, phải:
1. Enable interrupt trong DMA channel config
2. Đăng ký callback function
3. Enable interrupt trong NVIC
4. Implement ISR và gọi `DMA_IRQHandler()`

### 4. Memory Barriers
Khi dùng DMA, nên thêm memory barrier để đảm bảo cache coherency:
```c
__DSB();  // Data Synchronization Barrier
__ISB();  // Instruction Synchronization Barrier
```

### 5. Concurrent Access
Không được:
- Cấu hình kênh khi nó đang active
- Modify buffer khi DMA đang transfer
- Sử dụng cùng một buffer cho nhiều kênh DMA đồng thời

## 🔧 Troubleshooting

### DMA không hoạt động?
1. ✅ Kiểm tra clock đã enable chưa
2. ✅ Kiểm tra address alignment
3. ✅ Kiểm tra DMAMUX source có đúng không
4. ✅ Kiểm tra kênh đã được cấu hình chưa
5. ✅ Kiểm tra peripheral đã enable DMA request chưa

### Transfer không hoàn thành?
1. ✅ Kiểm tra major loop count có đúng không
2. ✅ Kiểm tra minor loop bytes
3. ✅ Kiểm tra source/dest offset
4. ✅ Kiểm tra xem có error flags không

### Interrupt không trigger?
1. ✅ Đã enable interrupt trong config chưa
2. ✅ Đã enable trong NVIC chưa
3. ✅ Đã implement ISR chưa
4. ✅ Kiểm tra priority của interrupt

## 📚 Tài liệu tham khảo

- **S32K1xx Reference Manual** - Chapter 21: eDMA
- **S32K1xx Reference Manual** - Chapter 22: DMAMUX
- **ARM Cortex-M4 Generic User Guide** - DMA Programming

---

**Author:** PhucPH32  
**Date:** 27/11/2025  
**Version:** 1.0
