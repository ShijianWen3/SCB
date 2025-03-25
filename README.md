## 简单的Qt6上位机程序

**以 <u>集成各种通信方式 </u>为目的的桌面端调试上位机，起源于一个大创项目。后续为了用于智能车龙芯赛道的图像调试，为TCP/UDP加上了图像传输及处理功能。**

------



### 目前支持：UART TCP/UDP Bluetooth/BLE

#### 1.UART：实现收发信息

#### 2.TCP/UDP：实现收发信息，同时针对图像编码数据进行解码，对接收帧进行图像处理(OpenCV)

#### 3.Bluetooth/BLE：实现收发信息

------



### To DO List：

- [ ] **优化图像处理线程，避免阻塞UI主线程**

- [ ] **将蓝牙与BLE实现方式从Qt6自带库替换为移植性更强的开源库**

------

#### ***·如何使用图传：***

##### 1.在下位机添加此程序：

```c++

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <opencv2/opencv.hpp>
int main(int argc, char* argv[])
{
    const char* ip_address;
    if (argc <= 1)
    {
        ip_address = "192.168.41.185";//默认IP
    }
    else
    {
        ip_address = argv[1];
    }
    int sock = socket(AF_INET, SOCK_DGRAM, 0); // 创建 UDP 套接字
    if (sock < 0) 
    {
        std::cerr << "Error: Unable to create socket." << std::endl;
        return;
    }
	cv::VideoCapture cap(0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8800); // 目标端口
    serverAddr.sin_addr.s_addr = inet_addr(ip_address); // PC 端 IP
    cv::Mat frame2send;
	while(1)
	{
		cap >> frame2send;
		std::vector<uchar> buffer;
		std::vector<int> encode_params = {cv::IMWRITE_JPEG_QUALITY, 30};
		// 将帧编码为 JPEG 格式
		cv::imencode(".jpg", frame2send, buffer,encode_params);
		// 发送数据
		sendto(sock, buffer.data(), buffer.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
	}
	return 0;
}
```

#####   2.编译运行：

`./main.exe <your PC IP>`**假设你编译生成的可执行文件为main.exe**

***在上位机TCP/UDP界面可以查看本机IP***