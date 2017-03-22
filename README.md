## Khả năng phần cứng
1. Bộ điều khiển xe
  - Chạy thẳng với tốc độ tối đa 50km/h.
  -	Rẽ trái/phải với góc rẽ tốc đa 20o.
  - Chạy ngược (lùi xe).
2. Kinect
  - Luồng ảnh màu (3 kênh) độ với phân giải 640x480 và thời gian nhận khung hình ~33 ms (~30 FPS).
  -	Luồng ảnh trường sâu (1 kênh) với độ phân giải 640x480 và thời gian nhận khung hình ~2000 ms (~0.5 FPS).
3. Cảm biến sóng
  -	Sự hiện diện của vật thể đối điện (ước lượng gián tiếp qua sóng nhận được từ vật).

## Chu trình vận hành

1. Thu thập thông tin 
2. Tính toán thuộc tính khung hình
3. Phân tích trạng thái đường đi
4. Điều khiển phương tiện (quay lại **1**)

### 1. Thu thập thông tin
Khai thác thông tin từ phần cứng (I). Các thông tin hiện có:
-	Ảnh màu:
  - Độ phân giải: 640x480.
  - Thời gian nhận khung hình: 33 ms (~30 FPS).
-	Ảnh trường sâu:
  - Độ phân giải: 640x480.
  - Thời gian nhận khung hình: 2000 ms (~0.5 FPS).

### 2. Tính toán thuộc tính khung hình
Dựa trên dữ liệu cung cấp từ **[1]**, tính toán các thuộc tính của một khung hình hỗ trợ việc Phân tích trạng trái đường đi **[3]**.
- Ảnh màu: áp dụng Edge detector và Hough transformation thu được các đoạn thẳng (line) (dựa trên gradient) với các thuộc tính sau:
  - Màu sắc:
    - 3 kênh màu: Đỏ, Lục và Lam.
    - Mỗi kênh màu có giá trị nguyên từ 0 đến 255.
  - Độ dài:
    - Giá trị thực từ MIN_LINE_LENGTH đến SQRL(VIDEO_FRAME_WIDTH^2 + VIDEO_FRAME_HEIGHT^2).
  - Góc xoay:
    - Giá trị thực từ 0 đến 180.
  -	Vị trí:
    - Cặp giá trị thực: X và Y.
    - X thuộc [0, VIDEO_FRAME_WIDTH]
    - Y thuộc [0, VIDEO_FRAME_HEIGHT]
- Ảnh trường sâu: (chưa sử dụng)

### 3.	Phân tích trạng thái đường đi
Dựa trên các thuộc tính của một khung hình cung cấp từ [2], phân tích trạng thái đường đi, cung cấp cho bộ phận Điều khiển phương tiện [4].

### 4.	Điều khiển phương tiện
Dựa trên trạng thái đường đi cung cấp từ [3] cùng khả năng phần cứng (I), đưa ra quyết định di chuyển vừa đảm bảo an toàn, vừa đảm bảo khả năng thu thập thông tin tiếp theo cho bộ phần Thu thập thông tin [1].


