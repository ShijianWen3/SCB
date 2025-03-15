# 简单的Qt6上位机程序
以集成各种通信方式的桌面端调试上位机为目的的上位机, 起源于一个大创项目.
后续为了智能车龙芯组别的调试,又加上了图传.

## 目前支持:UART TCP/UDP Bluetooth/BLE

TCP/UDP支持图传
-------------
### 如何使用:

----

***1.在下位机中添加此程序:*** 

#### framesend.cpp

```cpp

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
***2.编译运行:***
`./main.exe 192.168.0.0`后跟你要发送的ip

*在上位机TCP/UDP界面可以查看本机IP*


