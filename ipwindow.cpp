#include "my_windows.h"

IPWindow::IPWindow(QWidget* parent)
    :QMainWindow(parent)
{
    ui.setupUi(this);
    Visible_init();
    udpsocket = new QUdpSocket(this);
    tcpserver = new QTcpServer(this);
    slot_connect();
    slot_update_pc_ip();

}
IPWindow::~IPWindow()
{
    delete udpsocket;
    delete tcpserver;
    delete tcpsocket;
}
void IPWindow::Visible_init()
{
    ui.label_width->setVisible(false);
    ui.label_height->setVisible(false);
    ui.spinBox_width->setVisible(false);
    ui.spinBox_height->setVisible(false);
    ui.checkBox_Save->setVisible(false);
    ui.comboBox_SaveType->setVisible(false);
}
void IPWindow::slot_connect()
{
    connect(ui.pushButton_update_pc_ip,&QPushButton::clicked,this,&IPWindow::slot_update_pc_ip);
    connect(ui.pushButton_connect,&QPushButton::clicked,this,&IPWindow::slot_ipconnect);
    connect(udpsocket,&QUdpSocket::readyRead,this,&IPWindow::slot_receive);
    connect(ui.pushButton_send,&QPushButton::clicked,this,&IPWindow::slot_send);
    connect(ui.pushButton_clear,&QPushButton::clicked,this,&IPWindow::slot_clear);
    connect(tcpserver,&QTcpServer::newConnection,this,&IPWindow::slot_TcpNewConnection);
    connect(ui.checkBox_RX_display,&QCheckBox::toggled,this,&IPWindow::slot_TextDisplay);
    connect(ui.checkBox_RX_VideoDisplay,&QCheckBox::toggled,this,&IPWindow::slot_VideoDisplay);
    connect(ui.checkBox_Save,&QCheckBox::toggled,this,&IPWindow::slot_isSaved);
    
}
void IPWindow::slot_update_pc_ip()
{
    QProcess process;
    process.start("ipconfig");
    if (process.waitForFinished()) 
    {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split("\n");
    
        // 遍历输出的每一行，查找包含 "无线局域网适配器 WLAN" 的行
        bool isWlanAdapter = false;
        for (const QString& line : lines) 
        {
            if (line.contains("WLAN")) 
            {
                // 找到 WLAN 适配器，开始提取 IPv4 地址
                isWlanAdapter = true;
            }
    
            if (isWlanAdapter && line.contains("IPv4")) 
            {
                // 使用正则表达式提取 IP 地址
                QRegularExpression regex(R"(\d{1,3}(\.\d{1,3}){3})");
                QRegularExpressionMatch match = regex.match(line);
    
                if (match.hasMatch()) 
                {
                    QString ipAddress = match.captured(0);  // 获取匹配到的 IP 地址
                    ui.label_pc_ip->setText(ipAddress);  // 设置到界面的标签上
                    return;  // 找到后退出
                }
            }
        }
    }
    
    // ui.label_pc_ip->setText("None");
    std::cout<< "failed to find ipv4 address" << std::endl;
}

void IPWindow::slot_ipconnect()
{
    QString pc_ip = ui.label_pc_ip->text();
    unsigned int pc_port = ui.lineEdit_pc_ip_port->text().toInt();
    QHostAddress pc_addr(pc_ip);

    // std::cout<<"Port:"<<pc_port<<std::endl;
    // std::cout<<"IP:"<<pc_ip.toStdString()<<std::endl;

    if (ui.pushButton_connect->text() == "Connect")
    {
        QMovie* gif_wifi;
        gif_wifi = new QMovie(":/qrc/wifi_connecting.gif");
        ui.label_gif->setMovie(gif_wifi);
        gif_wifi->start();
        ui.pushButton_connect->setText("Disconnect");
        Enable_set(false);
        if (ui.comboBox_method->currentText() == "UDP")
        {   
            //绑定udp
            udpsocket->bind(pc_addr,pc_port);
        }
        else if (ui.comboBox_method->currentText() == "TCP")
        {
            //避免重复启动
            if (tcpserver->isListening())
            {
                ;
            }
            else
            {
                //启动tcp服务器,开始监听
                if (tcpserver->listen(pc_addr, pc_port)) 
                {
                    ;//等待client发送连接请求
                }
                else 
                {
                    QMessageBox::critical(this, "Remind", "TCP Server Failed to Start Listen");
                }
            }
        }
    }
    else if (ui.pushButton_connect->text() == "Disconnect")
    {
        QPixmap pixmap;
        pixmap = *(new QPixmap(":/qrc/failed.png"));
        ui.label_gif->setPixmap(pixmap);
        ui.pushButton_connect->setText("Connect");
        Enable_set(true);
        if (ui.comboBox_method->currentText() == "UDP")
        {
            udpsocket->close();
        }
        else if (ui.comboBox_method->currentText() == "TCP")
        {
            if (nullptr != tcpsocket)
            {
                tcpsocket->disconnectFromHost();  // 断开连接
            }
            tcpserver->close();
            
        }

        stop_save_video();

    }
    
    
}
void IPWindow::slot_receive()
{
    QString str_receive,str_display;
    QByteArray data;
    QHostAddress src_ip;
    quint16 src_port;
    if (ui.comboBox_method->currentText() == "UDP")
    {
        while (udpsocket->hasPendingDatagrams()) 
        {
            data.resize(int(udpsocket->pendingDatagramSize()));
            udpsocket->readDatagram(data.data(), data.size(), &(src_ip), &(src_port));
        }
        
        
    }
    else if (ui.comboBox_method->currentText() == "TCP")
    {
        data = tcpsocket->readAll();
        src_ip = tcpsocket->peerAddress();
        src_port = tcpsocket->peerPort();
        
    }
    
    
    //是否显示Text
    if (isDisplayText)
    {
        str_receive = QString::fromUtf8(data);
        str_display = "[" + src_ip.toString() + ":" + QString::number(src_port) + "]";
        str_display = str_display + str_receive + "\n";
        ui.plainTextEdit_receive->insertPlainText(str_display);
        ui.plainTextEdit_receive->moveCursor(QTextCursor::End);
        // std::cout<<"RX:"<<str_display.toStdString()<<std::endl;
    }
    //是否显示视频流
    if (isDisplayVideo)
    {
        display_image(data);
        // draw_image(data);
    }
    
    
}

void IPWindow::slot_send()
{
    QByteArray data;
    data = ui.textEdit_send->toPlainText().toUtf8();
    if (ui.comboBox_method->currentText() == "UDP")
    {
        
        QHostAddress device_addr(ui.lineEdit_dev_ip->text());
        unsigned int device_port = ui.lineEdit_dev_ip_port->text().toInt();
        udpsocket->writeDatagram(data, device_addr, device_port);
    }
    else if (ui.comboBox_method->currentText() == "TCP")
    {
        // data.fill('A', 1024);  // 发送 1KB 数据
        if (tcpsocket->state() == QAbstractSocket::ConnectedState)
        {
            tcpsocket->write(data);
            
            tcpsocket->flush();  // 强制将缓冲区的数据立即发送  
            // tcpsocket->waitForBytesWritten(1000);
        }
        else
        {
            std::cout<< "error in tcp send" <<std::endl;
        }
        
        
        
    }
    
}

void IPWindow::slot_clear()
{
    ui.textEdit_send->clear();
    ui.plainTextEdit_receive->clear();
    ui.label_video->clear();
}

void IPWindow::slot_TextDisplay(bool isChecked)
{
    if (isChecked)
    {
        isDisplayText = true;
    }
    else
    {
        isDisplayText = false;
    }
}
void IPWindow::slot_VideoDisplay(bool isChecked)
{
    if (isChecked)
    {
        isDisplayVideo = true;
        ui.label_width->setVisible(true);
        ui.label_height->setVisible(true);
        ui.spinBox_width->setVisible(true);
        ui.spinBox_height->setVisible(true);
        ui.checkBox_Save->setVisible(true);
        ui.comboBox_SaveType->setVisible(true);
        image_width = ui.spinBox_width->text().toInt();
        image_height = ui.spinBox_height->text().toInt();
    }
    else
    {
        isDisplayVideo = false;
        ui.label_width->setVisible(false);
        ui.label_height->setVisible(false);
        ui.spinBox_width->setVisible(false);
        ui.spinBox_height->setVisible(false);
        ui.checkBox_Save->setVisible(false);
        ui.comboBox_SaveType->setVisible(false);
        stop_save_video();
        
    }
    
    
}

void IPWindow::slot_TcpNewConnection()
{
    tcpsocket = tcpserver->nextPendingConnection();
    //禁用 Nagle 算法,使得数据立即发送
    tcpsocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    QString client_ip = tcpsocket->peerAddress().toString();
    quint16 client_port = tcpsocket->peerPort();

    QString dev_ip = ui.lineEdit_dev_ip->text();
    quint16 dev_port = ui.lineEdit_dev_ip_port->text().toInt();

    if ((client_ip == dev_ip) && (client_port == dev_port))
    {
        connect(tcpsocket, &QTcpSocket::readyRead, this, &IPWindow::slot_receive);
        connect(tcpsocket,&QTcpSocket::disconnected,this, &IPWindow::slot_TcpDisConnection);
    }
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,"Remind","Find New Connection!\nSet Device IP And Port Automatically?",QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            ui.lineEdit_dev_ip->setText(client_ip);
            ui.lineEdit_dev_ip_port->setText(QString::number(client_port));
            connect(tcpsocket, &QTcpSocket::readyRead, this, &IPWindow::slot_receive);
            connect(tcpsocket,&QTcpSocket::disconnected,this, &IPWindow::slot_TcpDisConnection);
        }
        else if (reply == QMessageBox::No)
        {
            QMessageBox::critical(this, "Remind", "Please Set Right Device IP And Port");
        }
        
        
    }
    
}

void IPWindow::slot_TcpDisConnection()
{
    tcpsocket->deleteLater();
    tcpsocket = nullptr;
}

void IPWindow::display_image(QByteArray data)
{
    // 将 QByteArray 转换为 OpenCV Mat
    std::vector<uchar> buffer(data.begin(), data.end());
    cv::Mat mat = cv::imdecode(buffer, cv::IMREAD_COLOR);
    if (mat.empty()) {
        qDebug() << "Failed to decode image";
        return;
    }
    if (isSaved)
    {
        //保存视频
        save_video(mat);
    }
    
  
    // 转换 Mat 到 QImage
    QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_BGR888);
    // 转换 QImage 到 QPixmap 并显示
    ui.label_video->setPixmap(QPixmap::fromImage(image));
}


/*如果使用QPainter绘制帧*/
void IPWindow::paintEvent(QPaintEvent *event)
{
    if (!video_image.isNull())
    {
        QPainter video_painter(ui.label_video);
        video_painter.drawImage(0,0,video_image);
    }
    
}//重载绘图函数

void IPWindow::save_video(cv::Mat frame)
{
    double save_fps = 30.0;
    QString save_type = ui.comboBox_SaveType->currentText();
    // 确保 output 目录存在
    QDir dir("output");
    if (!dir.exists()) 
    {
        dir.mkpath(".");
    }

    std::string std_filepath;
    //'X', 'V', 'I', 'D' -> .avi
    //'M', 'J', 'P', 'G' -> .mp4
    if (save_type == ".avi")
    {
        QDateTime nowtime = QDateTime::currentDateTime();
        QString filename = nowtime.toString("yyyy-MM-dd hh-mm") + ".avi";
        QString filepath = "output/" + filename;  // 先用 / 方便跨平台
        filepath = QDir::toNativeSeparators(filepath);  // 转换为 Windows 适用路径
        std_filepath = filepath.toStdString();
        if (!videowriter.isOpened())
        {
            videowriter.open(std_filepath, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), save_fps, cv::Size(frame.cols, frame.rows), true);
        }
        
    }
    else if (save_type == ".mp4")
    {
        QDateTime nowtime = QDateTime::currentDateTime();
        QString filename = nowtime.toString("yyyy-MM-dd hh-mm") + ".mp4";
        QString filepath = "output/" + filename;  // 先用 / 方便跨平台
        filepath = QDir::toNativeSeparators(filepath);  // 转换为 Windows 适用路径
        std_filepath = filepath.toStdString();
        if (!videowriter.isOpened())
        {
            videowriter.open(std_filepath, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), save_fps, cv::Size(frame.cols, frame.rows), true);
        }
    }
    videowriter.write(frame);
}

void IPWindow::stop_save_video()
{
    if (videowriter.isOpened()) 
    {
        videowriter.release();
        qDebug() << "Video recording stopped.";
    }
}

void IPWindow::slot_isSaved(bool isChecked)
{
    if (isChecked)
    {
        isSaved = true;
        ui.comboBox_SaveType->setVisible(true);
    }
    else
    {
        isSaved = false;
        ui.comboBox_SaveType->setVisible(false);
        stop_save_video();
    }
}

void IPWindow::Enable_set(bool isEnable)
{
    ui.comboBox_method->setEnabled(isEnable);
    ui.lineEdit_dev_ip->setEnabled(isEnable);
    ui.lineEdit_dev_ip_port->setEnabled(isEnable);
    ui.lineEdit_pc_ip_port->setEnabled(isEnable);
    ui.comboBox_SaveType->setEnabled(isEnable);
    ui.spinBox_height->setEnabled(isEnable);
    ui.spinBox_width->setEnabled(isEnable);
}