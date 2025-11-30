Tạo thư mục trong app tôi đang muốn thiết kế một dự án hãy giúp tui lên ý tưởng và cơ bản cho các api cần ghi ý tưởng là : 
- Gồm có hai board giao tiếp qua can với baud là 500Kbs
- Board 2 giao tiếp với PC thông qua uart với baud 9600
- Board 1 sẽ có hai nút nhấn:
    - Nhấn nút 1 thì board 1 sẽ đọc giá trị adc và dùng lpit để đảm bảo đọc adc sau mỗi 1 giây và gửi sang board 2 thông qua can và truyền từ board 2 lên pc thông qua uart
    - Nhất nút 2 thì board 1 sẽ ngừng đọc và gửi giá trị adc sang board 2 đồng thời ngừng gửi từ board 2 sang pc

- Frame truyền ADC thông qua can
	byte0	byte1	byte2	byte3	byte4	byte5	byte6	byte7
	ADC value							
								
Example:								
	ADC value = 1							
	byte0	byte1	byte2	byte3	byte4	byte5	byte6	byte7
	0	    0	    0	    0	    0	    0	    0       1
								
	ADC value = 456							
	byte0	byte1	byte2	byte3	byte4	byte5	byte6	byte7
	0	    0	    0	    0	    0	    4	    5	    6

- Sử dụng lại các api trong driver và comment chuẩn chỉ