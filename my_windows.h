#ifndef __MY_WINDOWS_H
#define __MY_WINDOWS_H
#include <iostream>
#include <QApplication>
#include <QMainWindow>
#include "ui/ui_uart.h"
#include "ui/ui_ip.h"
#include "ui/ui_bluetooth.h"
#include <QSerialPort>
#include <QString>
#include <QSerialPortInfo>
#include <QMessageBox>//消息弹窗
#include <QTimer>//定时器
#include <QMovie>//在控件中显示gif
#include <QProcess>
#include <QDateTime>//获取时间戳
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>

#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothSocket>
#include <QtBluetooth/QBluetoothUuid>
#include <QtBluetooth/QBluetoothAddress>
#include <QtBluetooth/QLowEnergyController>



#include <QDir>

#include <QImageReader>//使用qt自带库替代opencv解码
#include <QBuffer>
#include <QPainter>
// #include "videowidget.h"//opengl

#include <opencv2/opencv.hpp>

class UartWindow : public QMainWindow
{
    Q_OBJECT
private:
    /* data */
    Ui::uartwindow ui;
    QSerialPort* serialport = nullptr;//串口指针
    QTimer* timer_send = nullptr;
    void slot_connect(void);
    void slot_serialport_find(void);
    void slot_serialport_open(void);
    void slot_send(void);
    void slot_receive(void);
    void slot_timer_control(bool isChecked);
    void slot_rx_clear(void);
    void slot_tx_clear(void);
    void callback_timer(void);
public:
    UartWindow(QWidget *parent = nullptr);
    ~UartWindow();
    friend void friendfunc_FindUART(UartWindow* object );
};

class IPWindow : public QMainWindow
{
    Q_OBJECT
private:
    /* data */
    Ui::ipwindow ui;
    QUdpSocket* udpsocket = nullptr;
    QTcpSocket* tcpsocket = nullptr;
    QTcpServer* tcpserver = nullptr;
    int image_width,image_height;
    QImage video_image;
    //用于保存视频流
    cv::VideoWriter videowriter;  

    bool isDisplayText = true;
    bool isDisplayVideo = false;
    bool isSaved = true;
    void slot_connect(void);
    void slot_update_pc_ip(void);
    void slot_ipconnect(void);
    void slot_receive(void);
    void slot_send(void);
    void slot_clear(void);
    void slot_TextDisplay(bool isChecked);
    void slot_VideoDisplay(bool isChecked);
    void slot_TcpNewConnection();
    void slot_TcpDisConnection();
    void slot_isSaved(bool isChecked);
    void display_image(QByteArray data);

    void paintEvent(QPaintEvent *event) override;//重载绘图函数

    void save_video(cv::Mat frame);
    void stop_save_video();
    void Visible_init();
    void Enable_set(bool isEnable);



public:
    IPWindow(QWidget *parent = nullptr);
    ~IPWindow();
};


class BlWindow : public QMainWindow
{
    Q_OBJECT
private:
    /* data */
    Ui::blwindow ui;
    QBluetoothDeviceDiscoveryAgent* discoveryAgent;  // 蓝牙扫描对象
    //Bluetooth
    QBluetoothSocket* bluetoothSocket;
    //BLE
    QLowEnergyController* BLEcontroller; 
    bool isBluetoothMode;
    void slot_connect(void);
    void slot_deviceDiscovered(const QBluetoothDeviceInfo &device);
    void slot_scanFinished(); 
    void slot_scan();
    void slot_buildConnect();
    void slot_send();
    void slot_receive();
    void slot_changeModeBt();
    void slot_changeModeBLE();
    
public:
    BlWindow(QWidget *parent = nullptr);
    ~BlWindow();
};



#endif