#include "my_windows.h"

BlWindow::BlWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);//实例化蓝牙对象
    slot_connect();
    isBluetoothMode = true;
    ui.pushButton_scan->setEnabled(false);
    ui.pushButton_connect->setEnabled(false);
    slot_scan();

}

BlWindow::~BlWindow()
{
}

void BlWindow::slot_connect()
{
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BlWindow::slot_deviceDiscovered);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BlWindow::slot_scanFinished);
    connect(ui.pushButton_scan, &QPushButton::clicked, this, &BlWindow::slot_scan);
    connect(ui.pushButton_connect, &QPushButton::clicked, this, &BlWindow::slot_buildConnect);
    connect(ui.pushButton_send, &QPushButton::clicked, this, &BlWindow::slot_send);
    connect(ui.pushButton_Bt, &QPushButton::clicked, this, &BlWindow::slot_changeModeBt);
    connect(ui.pushButton_BLE, &QPushButton::clicked, this, &BlWindow::slot_changeModeBLE);
}

// 发现设备
void BlWindow::slot_deviceDiscovered(const QBluetoothDeviceInfo &device) 
{
    if (device.coreConfigurations() & QBluetoothDeviceInfo::BaseRateCoreConfiguration) 
    {
        QString deviceInfo = device.name() + " (" + device.address().toString() + ")";
        // qDebug() << "发现蓝牙设备: " << deviceInfo;
        ui.comboBox_Bluetooth->addItem(deviceInfo);
    }
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        QString deviceInfo = device.name() + " (" + device.address().toString() + ")";
        // qDebug() << "发现 BLE 设备: " << deviceInfo;
        ui.comboBox_BLE->addItem(deviceInfo);
    }
    
}

// 扫描完成
void BlWindow::slot_scanFinished() 
{
    // qDebug() << "蓝牙扫描完成";
    
    ui.comboBox_BLE->showPopup();
    ui.comboBox_Bluetooth->showPopup();
    ui.pushButton_scan->setEnabled(true);
    ui.pushButton_connect->setEnabled(true);
}

void BlWindow::slot_scan()
{
    QMessageBox::information(this, "Remind", "Bluetooth is scanning \n please wait a moment");
    ui.comboBox_Bluetooth->clear();
    ui.comboBox_BLE->clear();
    discoveryAgent->start();
}


void BlWindow::slot_buildConnect()
{
    QString deviceInfo = ui.comboBox_Bluetooth->currentText();
    QString str_name, str_addr;
    if (deviceInfo.isEmpty()) 
    {
        qDebug() << "未选择 BLE 设备";
        return;
    }

    // 解析设备名和地址 (设备名 + "(地址)")
    int start = deviceInfo.indexOf("(");
    int end = deviceInfo.indexOf(")");
    if (start == -1 || end == -1 || end <= start + 1) 
    {
        qDebug() << "设备格式错误：" << deviceInfo;
        return;
    }
    str_name = deviceInfo.left(start).trimmed();
    str_addr = deviceInfo.mid(start + 1, end - start - 1).trimmed();

    QBluetoothAddress address(str_addr);

   
    if (isBluetoothMode)
    {
        bluetoothSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);
        bluetoothSocket->connectToService(address, QBluetoothUuid::ServiceClassUuid::SerialPort);
        connect(bluetoothSocket, &QBluetoothSocket::readyRead, this, &BlWindow::slot_receive);
    }
    else
    {
        // 手动构造 QBluetoothDeviceInfo
        QBluetoothDeviceInfo GoalDevice(address, str_name, 0);
        // 设置设备为 BLE（低功耗蓝牙）
        GoalDevice.setCoreConfigurations(QBluetoothDeviceInfo::LowEnergyCoreConfiguration);
        BLEcontroller = QLowEnergyController::createCentral(GoalDevice, this);
        BLEcontroller->connectToDevice();
    }
}



void BlWindow::slot_send()
{
    QByteArray data;
    data = ui.textEdit_send->toPlainText().toUtf8();
    if (isBluetoothMode)
    {
        bluetoothSocket->write(data);
    }
    else
    {

    }
    
}
void BlWindow::slot_receive()
{
    if (isBluetoothMode)
    {
        QByteArray data = bluetoothSocket->readAll();
    }
        
    QString str_rev,str_time,str_display;
    QDateTime nowtime = QDateTime::currentDateTime();
    str_time = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
    str_display = str_time + str_rev;
    str_display.append('\n');

    ui.plainTextEdit_receive->insertPlainText(str_display);
    ui.plainTextEdit_receive->moveCursor(QTextCursor::End);
}


void BlWindow::slot_changeModeBt()
{
    isBluetoothMode = true;
    ui.pushButton_connect->setIcon(QIcon(":/qrc/bluetooth.png"));
}
void BlWindow::slot_changeModeBLE()
{
    isBluetoothMode = false;
    ui.pushButton_connect->setIcon(QIcon(":/qrc/ble.png"));

}