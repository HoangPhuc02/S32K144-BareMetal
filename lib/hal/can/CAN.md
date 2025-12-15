# FlexCAN Driver cho S32K144

## Tổng quan

Driver FlexCAN cung cấp các API để sử dụng CAN communication trên S32K144. Hỗ trợ CAN 2.0A (Standard ID) và CAN 2.0B (Extended ID).

## Tính năng

- ✅ CAN 2.0A/B protocol support
- ✅ Standard ID (11-bit) và Extended ID (29-bit)
- ✅ Baudrate có thể cấu hình (lên đến 1 Mbps)
- ✅ 32 Message Buffers (8 TX, 16 RX)
- ✅ Hardware filtering với Individual Mask
- ✅ Callback cho TX/RX events
- ✅ Error handling và monitoring
- ✅ Loopback và Listen-Only modes
- ✅ Blocking và Non-blocking APIs

## Cấu trúc Driver

```
can/
├── can_reg.h      # Register definitions
├── can.h          # API declarations
├── can.c          # Implementation
└── CAN.md         # Documentation (file này)
```

## Sử dụng cơ bản

### 1. Khởi tạo CAN

```c
#include "can.h"

can_config_t canConfig = {
    .instance = 0,                          // CAN0
    .clockSource = CAN_CLK_SRC_FIRCDIV2,    // 48MHz/2 = 24MHz
    .baudRate = 500000,                     // 500 Kbps
    .mode = CAN_MODE_NORMAL,
    .enableSelfReception = false,
    .useRxFifo = false                      // Dùng Individual MBs
};

status_t status = CAN_Init(&canConfig);
if (status != STATUS_SUCCESS) {
    // Handle error
}
```

### 2. Cấu hình RX Filter

```c
// Configure MB16 to receive messages with ID 0x123
can_rx_filter_t rxFilter = {
    .id = 0x123,
    .mask = 0x7FF,          // Match all 11 bits
    .idType = CAN_ID_STD
};

CAN_ConfigRxFilter(0, 16, &rxFilter);
```

### 3. Gửi Message (Non-blocking)

```c
can_message_t txMsg = {
    .id = 0x123,
    .idType = CAN_ID_STD,
    .frameType = CAN_FRAME_DATA,
    .dataLength = 8,
    .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
};

// Gửi qua MB8 (TX Message Buffer)
status = CAN_Send(0, 8, &txMsg);
if (status == STATUS_SUCCESS) {
    // Message queued for transmission
}
```

### 4. Gửi Message (Blocking)

```c
// Gửi và chờ transmission complete (timeout 100ms)
status = CAN_SendBlocking(0, 8, &txMsg, 100);
if (status == STATUS_SUCCESS) {
    // Message transmitted successfully
}
```

### 5. Nhận Message (Polling)

```c
can_message_t rxMsg;

// Nhận từ MB16
status = CAN_Receive(0, 16, &rxMsg);
if (status == STATUS_SUCCESS) {
    // Process received message
    uint32_t id = rxMsg.id;
    uint8_t length = rxMsg.dataLength;
    // ...
}
```

### 6. Nhận Message (Blocking)

```c
can_message_t rxMsg;

// Chờ nhận message (timeout 1000ms)
status = CAN_ReceiveBlocking(0, 16, &rxMsg, 1000);
if (status == STATUS_SUCCESS) {
    // Message received
}
```

## Sử dụng Interrupt với Callback

### 1. Install TX Callback

```c
void TxCallback(uint8_t instance, uint8_t mbIndex, void *userData)
{
    // Message transmitted successfully
    // Clear flag đã được driver xử lý
}

// Đăng ký callback cho MB8
CAN_InstallTxCallback(0, 8, TxCallback, NULL);

// Gửi message - callback sẽ được gọi khi TX complete
CAN_Send(0, 8, &txMsg);
```

### 2. Install RX Callback

```c
void RxCallback(uint8_t instance, uint8_t mbIndex, void *userData)
{
    can_message_t rxMsg;
    
    // Đọc message
    if (CAN_Receive(instance, mbIndex, &rxMsg) == STATUS_SUCCESS) {
        // Process received message
    }
}

// Đăng ký callback cho MB16
CAN_ConfigRxFilter(0, 16, &rxFilter);
CAN_InstallRxCallback(0, 16, RxCallback, NULL);
```

### 3. Install Error Callback

```c
void ErrorCallback(uint8_t instance, uint32_t errorFlags, void *userData)
{
    // Handle CAN errors
    if (errorFlags & CAN_ESR1_BOFFINT_MASK) {
        // Bus Off error
    }
    if (errorFlags & CAN_ESR1_ERRINT_MASK) {
        // Error interrupt
    }
}

CAN_InstallErrorCallback(0, ErrorCallback, NULL);
```

### 4. Interrupt Handler

```c
void CAN0_ORed_0_15_MB_IRQHandler(void)
{
    CAN_Type *base = CAN0;
    uint32_t flags = base->IFLAG1;
    
    // Check each MB
    for (uint8_t mb = 0; mb < 16; mb++) {
        if (flags & (1UL << mb)) {
            // Call appropriate callback
            if (mb >= 8 && mb < 16) {
                // TX callback
                if (s_txCallbacks[0][mb] != NULL) {
                    s_txCallbacks[0][mb](0, mb, s_txUserData[0][mb]);
                }
            }
            
            // Clear flag
            base->IFLAG1 = (1UL << mb);
        }
    }
}

void CAN0_ORed_16_31_MB_IRQHandler(void)
{
    CAN_Type *base = CAN0;
    uint32_t flags = base->IFLAG1;
    
    // Check MBs 16-31
    for (uint8_t mb = 16; mb < 32; mb++) {
        if (flags & (1UL << mb)) {
            // RX callback
            if (s_rxCallbacks[0][mb] != NULL) {
                s_rxCallbacks[0][mb](0, mb, s_rxUserData[0][mb]);
            }
            
            // Clear flag
            base->IFLAG1 = (1UL << mb);
        }
    }
}
```

## Extended ID (29-bit)

```c
// Gửi Extended ID message
can_message_t extMsg = {
    .id = 0x12345678,           // 29-bit ID
    .idType = CAN_ID_EXT,
    .frameType = CAN_FRAME_DATA,
    .dataLength = 4,
    .data = {0xAA, 0xBB, 0xCC, 0xDD}
};

CAN_Send(0, 8, &extMsg);

// Configure RX filter cho Extended ID
can_rx_filter_t extFilter = {
    .id = 0x12345678,
    .mask = 0x1FFFFFFF,         // Match all 29 bits
    .idType = CAN_ID_EXT
};

CAN_ConfigRxFilter(0, 16, &extFilter);
```

## Remote Frame

```c
// Gửi Remote Frame request
can_message_t remoteMsg = {
    .id = 0x456,
    .idType = CAN_ID_STD,
    .frameType = CAN_FRAME_REMOTE,
    .dataLength = 8              // DLC của data cần request
};

CAN_Send(0, 8, &remoteMsg);
```

## Loopback Mode (Testing)

```c
// Khởi tạo CAN ở Loopback mode
can_config_t loopbackConfig = {
    .instance = 0,
    .clockSource = CAN_CLK_SRC_FIRCDIV2,
    .baudRate = 500000,
    .mode = CAN_MODE_LOOPBACK,      // Loopback mode
    .enableSelfReception = true,    // Enable self-reception
    .useRxFifo = false
};

CAN_Init(&loopbackConfig);

// Trong loopback mode, message gửi đi sẽ tự nhận lại
```

## Listen-Only Mode (Monitoring)

```c
// Khởi tạo CAN ở Listen-Only mode
can_config_t listenConfig = {
    .instance = 0,
    .clockSource = CAN_CLK_SRC_FIRCDIV2,
    .baudRate = 500000,
    .mode = CAN_MODE_LISTEN_ONLY,   // Listen-only mode
    .enableSelfReception = false,
    .useRxFifo = false
};

CAN_Init(&listenConfig);

// Trong listen-only mode, chỉ nhận, không gửi ACK
```

## Error Handling

### Kiểm tra Error State

```c
can_error_state_t errorState;

CAN_GetErrorState(0, &errorState);

switch (errorState) {
    case CAN_ERROR_ACTIVE:
        // Normal operation
        break;
    case CAN_ERROR_PASSIVE:
        // Error count > 127
        break;
    case CAN_ERROR_BUS_OFF:
        // Error count > 255, bus off!
        break;
}
```

### Đọc Error Counters

```c
uint8_t txErrors, rxErrors;

CAN_GetErrorCounters(0, &txErrors, &rxErrors);

if (txErrors > 96 || rxErrors > 96) {
    // Warning level
}
```

## Baudrate Configuration

Driver tự động tính timing parameters. Một số baudrate phổ biến:

| Baudrate | CAN Clock | Timing |
|----------|-----------|---------|
| 1 Mbps   | 24 MHz    | Auto    |
| 500 Kbps | 24 MHz    | Auto    |
| 250 Kbps | 24 MHz    | Auto    |
| 125 Kbps | 24 MHz    | Auto    |

### Manual Timing Configuration (Advanced)

```c
can_timing_config_t customTiming;

// Calculate timing for 500 Kbps @ 24 MHz
CAN_CalculateTiming(24000000, 500000, &customTiming);

// Hoặc tự set:
// customTiming.preDiv = 2;
// customTiming.propSeg = 7;
// customTiming.phaseSeg1 = 4;
// customTiming.phaseSeg2 = 3;
// customTiming.rJumpWidth = 1;
```

## Message Buffer Allocation

```
MB0-7:   Reserved (hoặc RX FIFO)
MB8-15:  TX Message Buffers (8 MBs)
MB16-31: RX Message Buffers (16 MBs)
```

## API Reference

### Initialization
- `CAN_Init()` - Initialize CAN module
- `CAN_Deinit()` - Deinitialize CAN module

### Transmission
- `CAN_Send()` - Send message (non-blocking)
- `CAN_SendBlocking()` - Send message with timeout
- `CAN_AbortTransmission()` - Abort pending transmission

### Reception
- `CAN_Receive()` - Receive message (non-blocking)
- `CAN_ReceiveBlocking()` - Receive with timeout
- `CAN_ConfigRxFilter()` - Configure RX filter

### Callbacks
- `CAN_InstallTxCallback()` - Install TX callback
- `CAN_InstallRxCallback()` - Install RX callback
- `CAN_InstallErrorCallback()` - Install error callback

### Status & Utilities
- `CAN_GetErrorState()` - Get error state
- `CAN_GetErrorCounters()` - Get TX/RX error counters
- `CAN_IsMbBusy()` - Check if MB is busy
- `CAN_CalculateTiming()` - Calculate timing parameters

## Lưu ý quan trọng

1. **Clock Configuration**: Đảm bảo CAN clock được enable và configure đúng
2. **Pin Configuration**: Phải configure CAN TX/RX pins trước khi sử dụng
3. **Message Buffer**: TX dùng MB8-15, RX dùng MB16-31
4. **Interrupt**: Enable NVIC interrupt nếu dùng callback
5. **Timing**: Driver tự động tính timing cho baudrate phổ biến
6. **Bus Termination**: CAN bus cần termination resistor 120Ω ở 2 đầu

## Example Project Structure

```c
/* main.c */
#include "can.h"
#include "gpio.h"

int main(void)
{
    // Initialize system
    SystemInit();
    
    // Configure CAN pins (ví dụ: PTE4=CAN0_RX, PTE5=CAN0_TX)
    PORT_ConfigurePin(PORTE, 4, PORT_MUX_ALT5);  // CAN0_RX
    PORT_ConfigurePin(PORTE, 5, PORT_MUX_ALT5);  // CAN0_TX
    
    // Initialize CAN
    can_config_t config = {
        .instance = 0,
        .clockSource = CAN_CLK_SRC_FIRCDIV2,
        .baudRate = 500000,
        .mode = CAN_MODE_NORMAL,
        .enableSelfReception = false,
        .useRxFifo = false
    };
    CAN_Init(&config);
    
    // Configure RX filter
    can_rx_filter_t filter = {
        .id = 0x123,
        .mask = 0x7FF,
        .idType = CAN_ID_STD
    };
    CAN_ConfigRxFilter(0, 16, &filter);
    
    while (1) {
        // Main loop
    }
}
```

## Troubleshooting

### Message không gửi được
- Kiểm tra CAN bus connection
- Kiểm tra termination resistor
- Kiểm tra baudrate configuration
- Verify pin configuration

### Không nhận được message
- Kiểm tra RX filter configuration
- Verify mask setting
- Check Message Buffer index
- Verify CAN bus activity

### Bus Off error
- Kiểm tra physical layer
- Check baudrate matching với other nodes
- Verify termination resistor
- Check cable quality

---

**Tác giả**: PhucPH32  
**Ngày tạo**: 30/11/2025  
**Version**: 1.0
