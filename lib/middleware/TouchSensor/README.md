Phương pháp phổ biến nhất mà các MCU sử dụng để phát hiện chạm trong sơ đồ này là phương pháp **Đo Hằng số Thời gian RC (RC Time Constant Measurement)** hoặc **Chuyển Điện Tích (Charge Transfer)**.

Dưới đây là cách MCU tương tác với các chân **PTC0, PTC1, PTC2, PTC3** để phát hiện chạm, tập trung vào một cặp cảm ứng (ví dụ: PTC1 và PTC3):

---

## 1. Vai trò của các chân I/O

Trong cặp mạch cảm ứng (ví dụ: Mạch 1: **PTC1** và **PTC3**):

* **PTC3 (hoặc PTC0):** Thường là chân **Kích thích/Điều khiển (Drive/Source Pin)**. Nó được cấu hình là ngõ ra (Output) để cấp tín hiệu.
* **PTC1 (hoặc PTC2):** Thường là chân **Đo lường/Cảm nhận (Sense/Receiver Pin)**. Nó được cấu hình là ngõ vào (Input) để đo điện áp.

## 2. Nguyên lý hoạt động (Phương pháp Đo Hằng số Thời gian RC)

Mạch cảm ứng hoạt động dựa trên việc đo sự thay đổi của **hằng số thời gian $\tau$** (Tau) của một mạch RC, trong đó $R$ là điện trở và $C$ là tổng điện dung của mạch.

$$\tau = R \times C_{\text{tổng}}$$

### Bước 1: Thiết lập và Sạc Mạch (Charge)

1.  **MCU cấu hình PTC3 là ngõ ra (Output) mức CAO (High/VCC)**, bắt đầu sạc tụ điện $C_{\text{tổng}}$.2.  **MCU cấu hình PTC1 là ngõ vào (Input) với bộ đếm thờigian (Timer) được kích hoạt.**

$C_{\text{tổng}}$ ở đây bao gồm $C533 (10\text{pF})$ **nối đất** và **Điện dung Điện cực** ($C_{\text{Electrode}}$) qua SW7, cùng với bất kỳ điện dung ký sinh nào khác.

### Bước 2: Đo Thời gian (Timing)

* **Mạch không chạm (Baseline):**
    * Điện dung $C_{\text{tổng}}$ chỉ bao gồm $C533$ và điện dung ký sinh.
    * Thời gian để điện áp tại PTC1 tăng từ mức THẤP (Low) lên **ngưỡng logic CAO** của MCU ($V_{\text{IH}}$) sẽ là **thời gian cơ sở** ($T_{\text{cơ sở}}$).
* **Mạch có chạm (Touch):**
    * Khi ngón tay chạm vào điện cực (**Electrode**), **điện dung của ngón tay** ($C_{\text{ngón tay}}$) sẽ được **cộng thêm** vào $C_{\text{tổng}}$.
    * Vì $C_{\text{tổng}}$ tăng, **hằng số thời gian $\tau$ tăng** theo.
    * Thời gian để sạc điện áp tại PTC1 đạt đến ngưỡng $V_{\text{IH}}$ sẽ **lâu hơn** ($T_{\text{chạm}} > T_{\text{cơ sở}}$).

### Bước 3: Phát hiện Chạm (Detection)

* MCU sử dụng bộ định thời (Timer) để đo chính xác **số chu kỳ xung nhịp** (Clock Cycles) cần thiết để chân PTC1 chuyển từ mức THẤP sang mức CAO.
* Nếu **số chu kỳ đo được ($N$) lớn hơn đáng kể** so với giá trị cơ sở đã được hiệu chuẩn ($N_{\text{cơ sở}}$), MCU sẽ xác định là **có chạm**.

$$\text{Nếu } N > N_{\text{ngưỡng}} \quad \rightarrow \quad \text{CHẠM (Touch)}$$

---

## 3. Vai trò của các thành phần thụ động

* **R533 (0 $\Omega$) và R172 (0 $\Omega$):** Các điện trở $0\Omega$ này có thể được sử dụng như cầu chì bảo vệ hoặc các điểm kiểm tra/ngắt mạch tiện lợi trên PCB.
* **R525 (4.7K $\Omega$):** Điện trở này, cùng với $C_{\text{tổng}}$, tạo ra hằng số thời gian $\tau$ được MCU đo lường. Giá trị của nó được chọn để giữ $\tau$ trong phạm vi đo của Timer của MCU.
* **C533 (10pF):** Tụ điện nối đất này cung cấp **điện dung cơ sở (Baseline Capacitance)** cho mạch, giúp ổn định phép đo và lọc nhiễu.

---

### Tóm tắt Tương tác MCU:

MCU luân phiên thực hiện hai vai trò trên mỗi cặp chân:

1.  **Kích thích (PTCx $\rightarrow$ Output):** Cấp tín hiệu (High hoặc Low) để sạc/xả mạch RC.
2.  **Cảm nhận (PTCy $\rightarrow$ Input):** Đo lường thời gian cần thiết để chân này chuyển trạng thái (đạt ngưỡng $V_{\text{IH}}$).
