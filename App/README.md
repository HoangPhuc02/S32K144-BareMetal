# Application Project - Two Board CAN Communication System

## Tổng Quan Dự Án

Dự án này triển khai hệ thống giao tiếp giữa hai board S32K144 thông qua CAN bus, với khả năng đọc ADC và truyền dữ liệu lên PC qua UART.

### Mô Tả Hệ Thống

```
┌─────────────────┐         CAN Bus         ┌─────────────────┐
│                 │      (500 Kbps)         │                 │
│    BOARD 1      │◄──────────────────────►│    BOARD 2      │
│  (ADC Reader)   │                         │   (Gateway)     │
│                 │                         │                 │
└─────────────────┘                         └────────┬────────┘
      ▲                                              │
      │                                              │
   [Buttons]                                     UART (9600)
   [ADC Input]                                       │
                                                     ▼
                                                    [PC]
```

## Cấu Trúc Thư Mục

```
App/
├── req.md                  # File requirement gốc
├── app_config.h            # Cấu hình chung cho cả 2 board
├── README.md              # File này
├── board1/                # Board 1 - ADC Reader
│   ├── board1_main.c     # Code chính Board 1
│   └── board1_main.h     # Header Board 1
└── board2/                # Board 2 - CAN/UART Gateway
    ├── board2_main.c     # Code chính Board 2
    └── board2_main.h     # Header Board 2
```

## Chức Năng Chi Tiết

### Board 1 - ADC Reader

**Phần cứng:**
- 2 nút nhấn (PTD3, PTD5)
- 1 LED status (PTD0)
- ADC channel 12

**Chức năng:**
1. **Nút 1 (PTD3)**: Bắt đầu đọc ADC
   - Kích hoạt LPIT timer
   - Đọc ADC mỗi 1 giây
   - Gửi giá trị qua CAN
   - LED bật để báo trạng thái

2. **Nút 2 (PTD5)**: Dừng đọc ADC
   - Dừng LPIT timer
   - Ngừng gửi CAN
   - LED tắt

**Luồng hoạt động:**
```
User nhấn Nút 1
    ↓
Kích hoạt LPIT (1s timer)
    ↓
┌──────────────────────────┐
│ LPIT ISR (mỗi 1 giây)   │
│  1. Đọc ADC              │
│  2. Chuyển đổi sang     │
│     ASCII digits         │
│  3. Gửi qua CAN          │
└──────────────────────────┘
    ↓
User nhấn Nút 2 → Dừng timer
```

### Board 2 - CAN/UART Gateway

**Phần cứng:**
- 1 LED status (PTD0)
- CAN interface
- UART interface

**Chức năng:**
1. Nhận message CAN từ Board 1
2. Parse dữ liệu ADC từ CAN frame
3. Format và gửi lên PC qua UART
4. LED nhấp nháy mỗi khi có message

**Luồng hoạt động:**
```
CAN message từ Board 1
    ↓
CAN RX Callback
    ↓
Lưu message vào buffer
    ↓
Main loop xử lý
    ↓
Parse ADC value từ ASCII
    ↓
Format: "ADC Value: 456 (0x1C8)\r\n"
    ↓
Gửi qua UART đến PC
```

## Protocol Truyền Thông

### CAN Frame Format (ID: 0x100)

```
Byte:   0    1    2    3    4    5    6    7
Data:  [0]  [0]  [0]  [0]  [T]  [H]  [D]  [U]
```

**Giải thích:**
- Byte 0-3: Dự trữ (luôn = 0)
- Byte 4 (T): Chữ số hàng nghìn (ASCII '0'-'9')
- Byte 5 (H): Chữ số hàng trăm (ASCII '0'-'9')
- Byte 6 (D): Chữ số hàng chục (ASCII '0'-'9')
- Byte 7 (U): Chữ số hàng đơn vị (ASCII '0'-'9')

**Ví dụ:**
```
ADC = 1    → [0][0][0][0]['0']['0']['0']['1']
ADC = 456  → [0][0][0][0]['0']['4']['5']['6']
ADC = 3210 → [0][0][0][0]['3']['2']['1']['0']
```

### UART Output Format

```
ADC Value: 456 (0x1C8)\r\n
```

## Cấu Hình Hệ Thống

### Clock Configuration
- **System Clock**: 80 MHz (SPLL)
- **Bus Clock**: 40 MHz
- **Flash Clock**: 20 MHz
- **SOSC**: 8 MHz (external crystal)

### CAN Configuration
- **Baudrate**: 500 Kbps
- **Clock Source**: Peripheral clock
- **Message ID**: 0x100 (Standard ID)
- **Data Length**: 8 bytes

### UART Configuration
- **Baudrate**: 9600 bps
- **Data Bits**: 8
- **Parity**: None
- **Stop Bits**: 1

### ADC Configuration
- **Resolution**: 12-bit (0-4095)
- **Clock Divider**: 2
- **Sample Time**: Long
- **Voltage Reference**: VREFH
- **Trigger**: Software

### LPIT Configuration
- **Channel**: 0
- **Period**: 1,000,000 μs (1 second)
- **Mode**: Periodic
- **Interrupt**: Enabled

## Interrupt Priority

| Peripheral | Priority | Mô tả |
|------------|----------|-------|
| LPIT Timer | 2 (High) | Đảm bảo đọc ADC đúng thời gian |
| CAN RX     | 2 (High) | Nhận message kịp thời |
| Buttons    | 3 (Normal) | Điều khiển người dùng |
| UART TX    | 3 (Normal) | Truyền dữ liệu đến PC |

## Sử Dụng Driver APIs

### Board 1 - Các Driver Sử Dụng

```c
/* Clock Driver */
CLOCK_Init(&clockConfig);

/* GPIO Driver */
GPIO_SetPinDirection(PTD, 3, GPIO_INPUT);   // Button
GPIO_WritePin(PTD, 0, 1);                   // LED

/* PORT Driver */
PORT_SetMux(PORTD, 3, PORT_MUX_GPIO);
PORT_SetInterruptConfig(PORTD, 3, PORT_INT_FALLING_EDGE);

/* ADC Driver */
ADC_Init(0, &adcConfig);
ADC_StartConversion(0, 12);
value = ADC_GetConversionResult(0);

/* LPIT Driver */
LPIT_Init();
LPIT_ConfigChannel(0, &lpitConfig);
LPIT_StartChannel(0);
LPIT_StopChannel(0);

/* CAN Driver */
CAN_Init(0, &canConfig);
CAN_Send(0, 8, &message);

/* NVIC Driver */
NVIC_EnableIRQ(PORTD_IRQn);
NVIC_SetPriority(PORTD_IRQn, 3);
```

### Board 2 - Các Driver Sử Dụng

```c
/* Clock Driver */
CLOCK_Init(&clockConfig);

/* GPIO Driver */
GPIO_TogglePin(PTD, 0);  // LED toggle

/* CAN Driver */
CAN_Init(0, &canConfig);
CAN_SetRxCallback(0, 16, callback);
CAN_Receive(0, 16, &rxMsg);

/* UART Driver */
UART_Init(1, &uartConfig);
UART_SendBlocking(1, data, length, timeout);

/* NVIC Driver */
NVIC_EnableGlobalIRQ(0);
```

## Biên Dịch và Nạp Code

### Cho Board 1

1. Mở project trong S32 Design Studio
2. Sửa file `src/main.c`:
   ```c
   #include "App/board1/board1_main.h"
   
   int main(void) {
       Board1_Main();
       return 0;
   }
   ```
3. Build project: `Project → Build Project`
4. Flash vào Board 1: `Run → Debug Configurations`

### Cho Board 2

1. Sửa file `src/main.c`:
   ```c
   #include "App/board2/board2_main.h"
   
   int main(void) {
       Board2_Main();
       return 0;
   }
   ```
2. Build và flash vào Board 2

## Kết Nối Phần Cứng

### Board 1
```
PTD3  ─────┐ 
           ├─── Button 1 (Start) ─── GND
PTD5  ─────┤
           └─── Button 2 (Stop) ─── GND

PTD0  ────── LED ────── Resistor ────── GND

ADC_Ch12 ────── Potentiometer ────── GND
                    │
                  VREF

CAN_TX ──────────────────────────────── CAN_RX (Board 2)
CAN_RX ──────────────────────────────── CAN_TX (Board 2)
```

### Board 2
```
CAN_TX ──────────────────────────────── CAN_RX (Board 1)
CAN_RX ──────────────────────────────── CAN_TX (Board 1)

UART_TX ─────────────────────────────── USB-to-UART ─── PC
```

### CAN Bus Termination
- Cần resistor 120Ω ở mỗi đầu CAN bus
- Nối CANH và CANL qua resistor

## Testing và Debug

### Test Board 1

1. **Kiểm tra LED khởi động:**
   - LED nháy với tần số ~2Hz khi board chạy

2. **Test Button 1:**
   - Nhấn Button 1 → LED sáng liên tục
   - Check CAN message bằng CAN analyzer

3. **Test ADC:**
   - Thay đổi giá trị potentiometer
   - Verify CAN message thay đổi theo

4. **Test Button 2:**
   - Nhấn Button 2 → LED tắt
   - CAN message dừng gửi

### Test Board 2

1. **Kiểm tra UART:**
   - Mở terminal (PuTTY, TeraTerm) với 9600 baud
   - Thấy message: "=== Board 2 - CAN to UART Gateway ==="

2. **Test CAN reception:**
   - Khi Board 1 gửi, Board 2 nhận và hiển thị:
     ```
     ADC Value: 456 (0x1C8)
     ADC Value: 457 (0x1C9)
     ...
     ```

3. **Verify LED:**
   - LED nháy mỗi khi nhận CAN message

## Troubleshooting

### Board 1

| Vấn đề | Nguyên nhân | Giải pháp |
|--------|-------------|-----------|
| LED không nháy | Clock chưa khởi tạo | Check `Board1_InitClock()` |
| Button không hoạt động | Chưa enable interrupt | Check NVIC enable |
| ADC không đọc | PCC chưa enable | Check ADC clock |
| CAN không gửi | CAN chưa khởi tạo đúng | Check baudrate, pins |

### Board 2

| Vấn đề | Nguyên nhân | Giải pháp |
|--------|-------------|-----------|
| UART không xuất | Baudrate không đúng | Check 9600 baud |
| Không nhận CAN | Message ID không khớp | Check ID = 0x100 |
| Data sai | Parse lỗi | Check CAN frame format |
| LED không nháy | Callback chưa đúng | Check CAN_SetRxCallback |

## Mở Rộng

### Tính năng có thể thêm:

1. **Board 1:**
   - Đọc nhiều kênh ADC
   - Thêm filter cho ADC (moving average)
   - Button long press cho chế độ khác
   - Gửi timestamp cùng ADC value

2. **Board 2:**
   - Lưu log vào SD card
   - Thêm LCD hiển thị giá trị
   - Forward nhiều loại message khác
   - Thêm command từ PC điều khiển Board 1

3. **Protocol:**
   - Thêm checksum cho CAN message
   - Thêm message type khác (status, error)
   - Flow control giữa 2 board

## Tài Liệu Tham Khảo

- [CAN Driver Documentation](../Core/Drivers/can/CAN.md)
- [LPIT Driver Documentation](../Core/Drivers/lpit/LPIT.md)
- [ADC Driver Documentation](../Core/Drivers/adc/ADC.md)
- [DMA Driver Documentation](../Core/Drivers/dma/DMA.md)
- [Coding Convention](../coding_convention_short.md)

## Tác Giả

- **PhucPH32**
- **Ngày tạo:** 30/11/2025
- **Version:** 1.0

## License

Internal use only - S32K144 BareMetal Project
