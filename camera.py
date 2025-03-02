import socket
import cv2
import numpy as np

# 配置 UDP 套接字
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, 0)
sock.bind(('192.168.41.185', 8800))  # 绑定到本地 IP 和端口

print("Listening for incoming video frames...")

# 初始化视频写入器
fourcc = cv2.VideoWriter_fourcc(*'XVID')  # 使用 XVID 编码
filename = 'output.avi'  # 保存的视频文件名
fps = 30  # 假设帧率为 30
frame_size = None  # 将从第一帧中获取尺寸
out = None

while True:
    try:
        # 接收数据（设置最大数据包大小）
        data, addr = sock.recvfrom(65536)  # UDP 数据包大小一般小于 64KB

        # 解码 JPEG 数据为图像
        frame = cv2.imdecode(np.frombuffer(data, np.uint8), cv2.IMREAD_COLOR)

        if frame is None:
            print("Failed to decode frame.")
            continue

        # 初始化 VideoWriter（仅在第一帧时执行）
        if out is None:
            frame_size = (frame.shape[1], frame.shape[0])  # 获取帧的宽和高
            out = cv2.VideoWriter(filename, fourcc, fps, frame_size)

        # 写入帧到视频文件
        out.write(frame)

        # 显示接收到的图像
        cv2.imshow('Received Frame', frame)

        # 按 'q' 键退出
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    except socket.timeout:
        print("No data received within timeout.")
        break
    except Exception as e:
        print(f"An error occurred: {e}")
        break

# 释放资源
sock.close()
if out:
    out.release()
cv2.destroyAllWindows()
