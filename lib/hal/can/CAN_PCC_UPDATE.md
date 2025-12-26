# CAN Driver - PCC Integration Update

## Tổng quan

Đã cập nhật driver CAN để tích hợp với module PCC (Peripheral Clock Control), tách biệt việc quản lý clock khỏi driver CAN và thêm chức năng chuyển đổi chế độ hoạt động động.

## Các thay đổi chính

### 1. Di chuyển quản lý Clock sang PCC Module

**Trước đây:** Driver CAN truy cập trực tiếp thanh ghi PCC để enable/disable clock
```c
// Old code in can.c
PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;
```

**Hiện tại:** Driver CAN sử dụng API của PCC module
```c
// New code in can.c
PCC_EnableCANClock(instance, clockSource);
```

**Lợi ích:**
- Tách biệt trách nhiệm: PCC module quản lý tất cả peripheral clocks
- Code dễ bảo trì và test
- Tái sử dụng code cho các peripheral khác
- Giảm dependency trực tiếp với hardware registers

### 2. Thêm CAN Clock Source Types vào PCC

Đã thêm enum `can_clk_src_t` vào [pcc.h](../pcc/pcc.h):

```c
typedef enum {
    CAN_CLK_SRC_SOSCDIV2 = 0U,  /**< System Oscillator DIV2 clock (typically 4 MHz) */
    CAN_CLK_SRC_BUSCLOCK = 1U   /**< Bus clock (typically 40 MHz) */
} can_clk_src_t;
```

**Di chuyển từ:** [can_reg.h](can_reg.h) (đã xóa)
**Lý do:** Clock source selection là phần của PCC configuration, không phải CAN register definition

### 3. API mới trong PCC Module

#### PCC_EnableCANClock()
```c
bool PCC_EnableCANClock(uint8_t instance, can_clk_src_t clockSource);
```
- Enable clock cho CAN peripheral thông qua PCC
- Tự động chọn clock source phù hợp
- **Tham số:**
  - `instance`: CAN instance (0, 1, hoặc 2)
  - `clockSource`: `CAN_CLK_SRC_SOSCDIV2` hoặc `CAN_CLK_SRC_BUSCLOCK`
- **Trả về:** `true` nếu thành công, `false` nếu thất bại

**Ví dụ:**
```c
// Enable CAN0 with bus clock (40 MHz)
PCC_EnableCANClock(0, CAN_CLK_SRC_BUSCLOCK);
```

#### PCC_DisableCANClock()
```c
bool PCC_DisableCANClock(uint8_t instance);
```
- Disable clock cho CAN peripheral
- **Tham số:** `instance`: CAN instance (0, 1, hoặc 2)
- **Trả về:** `true` nếu thành công, `false` nếu thất bại

### 4. Hàm chuyển đổi chế độ hoạt động CAN

#### CAN_SetOperatingMode()
```c
status_t CAN_SetOperatingMode(uint8_t instance, can_mode_t mode);
```

**Mô tả:**
- Chuyển đổi chế độ hoạt động của CAN controller động khi đang chạy
- Hỗ trợ 3 chế độ: Normal, Loopback, Listen-Only

**Tham số:**
- `instance`: CAN instance (0-2)
- `mode`: Chế độ hoạt động
  - `CAN_MODE_NORMAL`: Hoạt động bình thường
  - `CAN_MODE_LOOPBACK`: Chế độ loopback nội bộ (self-test)
  - `CAN_MODE_LISTEN_ONLY`: Chế độ giám sát bus (không gửi ACK)

**Trả về:**
- `STATUS_SUCCESS`: Chuyển đổi thành công
- `STATUS_INVALID_PARAM`: Tham số không hợp lệ
- `STATUS_NOT_INITIALIZED`: CAN chưa được khởi tạo
- `STATUS_TIMEOUT`: Không thể vào/thoát freeze mode

**Lưu ý:**
- Hàm tạm thời vào freeze mode để thay đổi cấu hình
- Nên hoàn thành tất cả transmission trước khi chuyển đổi
- An toàn khi gọi trong runtime

**Ví dụ sử dụng:**

```c
// 1. Khởi tạo CAN ở chế độ Normal
can_config_t config = {
    .instance = 0,
    .clockSource = CAN_CLK_SRC_BUSCLOCK,
    .baudRate = 500000,
    .mode = CAN_MODE_NORMAL,
    .enableSelfReception = false,
    .useRxFifo = false
};
CAN_Init(&config);

// 2. Chuyển sang chế độ Loopback để tự test
CAN_SetOperatingMode(0, CAN_MODE_LOOPBACK);

// Test transmission và reception
can_message_t txMsg = {
    .id = 0x123,
    .idType = CAN_ID_STD,
    .frameType = CAN_FRAME_DATA,
    .dataLength = 8,
    .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
};
CAN_Send(0, 8, &txMsg);

can_message_t rxMsg;
if (CAN_Receive(0, 16, &rxMsg) == STATUS_SUCCESS) {
    // Message received in loopback mode
}

// 3. Chuyển sang chế độ Listen-Only để giám sát bus
CAN_SetOperatingMode(0, CAN_MODE_LISTEN_ONLY);
// Node sẽ nhận messages nhưng không gửi ACK

// 4. Quay lại chế độ Normal
CAN_SetOperatingMode(0, CAN_MODE_NORMAL);
```

### 5. Mô tả các chế độ hoạt động

#### CAN_MODE_NORMAL
- Hoạt động CAN tiêu chuẩn
- Gửi và nhận messages bình thường
- Gửi ACK cho messages nhận được
- Sử dụng cho communication thực tế

#### CAN_MODE_LOOPBACK
- Messages TX được nhận nội bộ, không truyền ra bus
- Hữu ích cho self-test không ảnh hưởng bus
- Kiểm tra cấu hình TX/RX buffers
- Debug và testing

#### CAN_MODE_LISTEN_ONLY
- Node giám sát bus nhưng không gửi bất kỳ frames nào (kể cả ACK)
- Không ảnh hưởng đến bus communication
- Sử dụng cho:
  - Bus analysis và monitoring
  - Logging CAN traffic
  - Diagnostic tools
  - Hot-plug monitoring (không ảnh hưởng bus hoạt động)

## Cấu trúc File thay đổi

### Files đã sửa đổi:

1. **[pcc.h](../pcc/pcc.h)**
   - Thêm `can_clk_src_t` enum
   - Thêm `PCC_EnableCANClock()` function declaration
   - Thêm `PCC_DisableCANClock()` function declaration

2. **[pcc.c](../pcc/pcc.c)**
   - Implement `PCC_EnableCANClock()`
   - Implement `PCC_DisableCANClock()`

3. **[can.h](can.h)**
   - Thêm include `"pcc.h"`
   - Thêm `CAN_SetOperatingMode()` function declaration

4. **[can.c](can.c)**
   - Thay `#include "pcc_reg.h"` bằng `#include "pcc.h"`
   - Sửa `CAN_EnableClock()` để sử dụng `PCC_EnableCANClock()`
   - Sửa `CAN_Deinit()` để sử dụng `PCC_DisableCANClock()`
   - Thêm implementation `CAN_SetOperatingMode()`

5. **[can_reg.h](can_reg.h)**
   - Xóa định nghĩa `can_clk_src_t` (đã di chuyển sang pcc.h)

## Sơ đồ Dependencies

```
┌─────────────┐
│   can.h     │
│             │──includes──┐
└─────────────┘            │
                           ▼
                    ┌─────────────┐
                    │   pcc.h     │
                    │             │
                    │ - can_clk_src_t
                    │ - PCC APIs
                    └─────────────┘
                           ▲
                           │
┌─────────────┐            │
│   can.c     │──uses──────┘
│             │
└─────────────┘
```

## Migration Guide cho Code hiện có

### Không cần thay đổi code application

Code application sử dụng CAN driver **không cần thay đổi** vì:
- Public API của CAN driver giữ nguyên
- `CAN_Init()` vẫn hoạt động như cũ
- Clock configuration tự động được xử lý bên trong

### Nếu code trực tiếp truy cập PCC cho CAN

**Trước:**
```c
PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;
```

**Sau:**
```c
PCC_EnableCANClock(0, CAN_CLK_SRC_BUSCLOCK);
```

## Testing

### Kiểm tra cơ bản

```c
// Test 1: Initialize CAN with different clock sources
can_config_t config1 = {
    .instance = 0,
    .clockSource = CAN_CLK_SRC_SOSCDIV2,  // 4 MHz
    .baudRate = 250000,
    .mode = CAN_MODE_NORMAL,
    .enableSelfReception = false,
    .useRxFifo = false
};
CAN_Init(&config1);

// Test 2: Switch to loopback mode
CAN_SetOperatingMode(0, CAN_MODE_LOOPBACK);

// Test 3: Send and receive in loopback
can_message_t msg = {...};
CAN_Send(0, 8, &msg);
CAN_Receive(0, 16, &rxMsg);

// Test 4: Switch to listen-only
CAN_SetOperatingMode(0, CAN_MODE_LISTEN_ONLY);

// Test 5: Back to normal
CAN_SetOperatingMode(0, CAN_MODE_NORMAL);
```

## Backward Compatibility

- ✅ Public API của CAN driver không thay đổi
- ✅ Existing applications vẫn hoạt động bình thường
- ✅ Clock configuration vẫn tự động
- ⚠️ Code truy cập trực tiếp PCC registers cần update

## Future Enhancements

1. Thêm runtime clock source switching
2. Thêm power management APIs (sleep/wakeup modes)
3. Support thêm các clock sources (SIRC, FIRC, SPLL)
4. Thêm clock frequency validation

## Tác giả & Ngày cập nhật

- **Cập nhật bởi:** GitHub Copilot
- **Ngày:** 26/12/2025
- **Phiên bản:** 1.1
- **Tài liệu gốc:** PhucPH32

## Tham khảo

- S32K1xx Reference Manual, Chapter 29: Peripheral Clock Control (PCC)
- S32K1xx Reference Manual, Chapter 52: FlexCAN
- [CAN.md](CAN.md) - CAN Driver Documentation
- [pcc/README.md](../pcc/README.md) - PCC Module Documentation
