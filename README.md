## Khả năng phần cứng
1. Bộ điều khiển xe
- Chạy thẳng với tốc độ tối đa _MAX_VELOCITY_ km/h.
- Rẽ trái/phải với góc rẽ tốc đa _MAX_STEERING_ANGLE_.
- Chạy ngược (lùi xe).

2. Kinect
- Luồng ảnh màu (3 kênh) độ với phân giải _COLOR_FRAME_WIDTH_ x _COLOR_FRAME_HEIGHT_ và thời gian nhận khung hình _COLOR_FRAME_RATE_ ms (_COLOR_FPS_ FPS).
- Luồng ảnh trường sâu (1 kênh) với độ phân giải DEPTH_FRAME_WIDTH x DEPTH_FRAME_HEIGHT và thời gian nhận khung hình _DEPTH_FRAME_RATE_ ms (_DEPTH_FPS_ FPS).

3. Cảm biến sóng
  -	Sự hiện diện của vật thể đối điện (ước lượng gián tiếp qua sóng nhận được từ vật).

## Chu trình vận hành


<div id="container" style="text-align:center;">
    <img src="http://imgur.com/FLhTyiD.png"/>
</div>

1. Thu thập thông tin [1]
2. Tính toán thuộc tính khung hình [2]
3. Phân tích trạng thái đường đi [3]
4. Điều khiển phương tiện [4] (quay lại **[1]**)

### 1. Thu thập thông tin [1]
Khai thác thông tin từ phần cứng **(I)**. Các thông tin hiện có:
-	Ảnh màu: nguồn từ Kinect
  •	Độ phân giải: _COLOR_FRAME_WIDTH_ x _COLOR_FRAME_HEIGHT_.
  •	Thời gian nhận khung hình: _COLOR_FRAME_RATE_ ms (_COLOR_FPS FPS_).
-	Ảnh trường sâu: nguồn từ Kinect
  •	Độ phân giải: _DEPTH_FRAME_WIDTH_ x _DEPTH_FRAME_HEIGHT_.
  •	Thời gian nhận khung hình: _DEPTH_FRAME_RATE_ ms (_DEPTH_FPS_ FPS).

### 2. Tính toán thuộc tính khung hình [2]
Dựa trên dữ liệu cung cấp từ **[1]**, tính toán các thuộc tính của một khung hình hỗ trợ việc Phân tích trạng trái đường đi [3].
-	Ảnh màu: áp dụng _Edge detector_ và _Hough transformation_ thu được các đoạn thẳng _(line)_ (dựa trên gradient) với các thuộc tính sau:
  •	Màu sắc: lấy từ pixel tương ứng trên _Ảnh màu_
    o	3 kênh màu: _Đỏ_, _Lục_ và _Lam_.
    o	Mỗi kênh màu có giá trị nguyên từ 0 đến 255.
  •	Độ dài: Khoảng cách từ điểm đầu đến điểm cuối
    o	Giá trị thực từ _MIN_LINE_LENGTH_ đến _SQRL(VIDEO_FRAME_WIDTH^2 + VIDEO_FRAME_HEIGHT^2)_.
  •	Góc xoay: góc lệch so với _Trục hoành_ (tính theo chiều dương).
    o	Giá trị thực từ 0 đến 180.
  •	Vị trí: vị trí điểm đầu, điểm cuối trên Khung hình
    o	Cặp giá trị thực: X và Y.
    o	X thuộc [0, _VIDEO_FRAME_WIDTH_]
    o	Y thuộc [0, _VIDEO_FRAME_HEIGHT_]
-	Ảnh trường sâu: _(chưa sử dụng)_

### 3.	Phân tích trạng thái đường đi [3]
Dựa trên các thuộc tính của một khung hình cung cấp từ [2], phân tích trạng thái đường đi, cung cấp cho bộ phận Điều khiển phương tiện **[4]**.
-	Vị trí của làn đường trái/phải so với xe: chuẩn hóa về [-1.0, 1.0]
  •	Vị trí lề trái khung hình (0) tương đương -1.0.
  •	Vị trí lề phải khung hình (_VIDEO_FRAME_WIDTH_-1) tương đương 1.0.
-	Góc xoay của làn đường so với xe.
  •	Giá trị thực tính theo đơn vị độ.
-	Vị trí, độ rộng của các vật cản (nếu có) so với xe.
  •	Giá trị thực tính theo đơn vị mét.
-	Độ tin cậy của trạng thái.
  •	Giá trị thực thuộc (0, 1)
  •	Giá trị càng lớn thì trạng thái đường đi càng chính xác (ngược lại)

Mọi thông số về trạng thái đường đi đều lấy Vị trí xe làm Gốc tọa độ O(0,0), lấy Góc xoay của xe làm Góc 0 độ, có thể mô phỏng như sau:

<div id="container" style="text-align:center;">
    <img src="http://imgur.com/bQCxxQs.png"/>
</div>

### 4.	Điều khiển phương tiện [4]
Dựa trên trạng thái đường đi cung cấp từ **[3]** cùng khả năng phần cứng **(I)**, đưa ra quyết định di chuyển vừa đảm bảo an toàn, vừa đảm bảo khả năng thu thập thông tin tiếp theo cho bộ phần Thu thập thông tin **[1]**.
