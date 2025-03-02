#include "my_windows.h"

UartWindow::UartWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    timer_send = new QTimer();
    timer_send->setInterval(ui.spinBox_ms->text().toInt());
    
    slot_serialport_find();
    slot_connect();
}

UartWindow::~UartWindow()
{
    delete timer_send;
    delete serialport;
}
void UartWindow::slot_connect()
{
    connect(ui.pushButton_FindUart,&QPushButton::clicked,this,&UartWindow::slot_serialport_find);
    connect(ui.pushButton_OpenUart,&QPushButton::clicked,this,&UartWindow::slot_serialport_open);
    connect(ui.pushButton_send,&QPushButton::clicked,this,&UartWindow::slot_send);
    connect(serialport,&QSerialPort::readyRead,this,&UartWindow::slot_receive);//绑定有可读数据信号与接收槽函数
    connect(timer_send,&QTimer::timeout,this,&UartWindow::callback_timer);//绑定定时器和回调函数
    connect(ui.checkBox_AutoSend,&QCheckBox::toggled,this,&UartWindow::slot_timer_control);
    connect(ui.pushButton_RXclear,&QPushButton::clicked,this,&UartWindow::slot_rx_clear);
    connect(ui.pushButton_TXclear,&QPushButton::clicked,this,&UartWindow::slot_tx_clear);

}
void UartWindow::slot_serialport_find()
{
    serialport = new QSerialPort(this);
    ui.comboBox_com->clear();
    // std::cout<< "push FindUart" <<std::endl;
    foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts()) 
    {
        // qDebug() << "发现串口：" << portInfo.portName();
        ui.comboBox_com->addItem(portInfo.portName());
    }
    if (0 == (ui.comboBox_com->count()))
    {
        QMessageBox::critical(this, "Remind", "Can't find available COM port!\nSerial Port may be ouccupied");
    }
}
void friendfunc_FindUART(UartWindow* object)
{
    object->slot_serialport_find();
}
void UartWindow::slot_serialport_open()
{
    //串口初始化
    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity checkBits;

    // 获取串口波特率
    // baudRate = ui->baundrateCb->currentText().toInt();直接字符串转换为 int 的方法
    unsigned int baudnum = ui.comboBox_baud->currentText().toInt();
    switch (baudnum)
    {
        case 1200:
            baudRate=QSerialPort::Baud1200;
            break;
        case 2400:
            baudRate=QSerialPort::Baud2400;
            break;
        case 4800:
            baudRate=QSerialPort::Baud4800;
            break;
        case 9600:
            baudRate=QSerialPort::Baud9600;
            break;
        case 19200:
            baudRate=QSerialPort::Baud19200;
            break;
        case 38400:
            baudRate=QSerialPort::Baud38400;
            break;
        case 57600:
            baudRate=QSerialPort::Baud57600;
            break;
        case 115200:
            baudRate=QSerialPort::Baud115200;
            break;
        default:
            QMessageBox::critical(this, "Remind", "Wrong Baud Rate!");
            break;
    }

    // 获取串口数据位
    if(ui.comboBox_databit->currentText()=="5")
    {
        dataBits=QSerialPort::Data5;
    }
    else if(ui.comboBox_databit->currentText()=="6")
    {
        dataBits=QSerialPort::Data6;
    }
    else if(ui.comboBox_databit->currentText()=="7")
    {
        dataBits=QSerialPort::Data7;
    }
    else if(ui.comboBox_databit->currentText()=="8")
    {
        dataBits=QSerialPort::Data8;
    }

    // 获取串口停止位
    if(ui.comboBox_stopbit->currentText()=="1")
    {
        stopBits=QSerialPort::OneStop;
    }
    else if(ui.comboBox_stopbit->currentText()=="1.5")
    {
        stopBits=QSerialPort::OneAndHalfStop;
    }
    else if(ui.comboBox_stopbit->currentText()=="2")
    {
        stopBits=QSerialPort::TwoStop;
    }
        

    // 获取串口奇偶校验位
    if(ui.comboBox_checkbit->currentText() == "None")
    {
        checkBits = QSerialPort::NoParity;
    }
    else if(ui.comboBox_checkbit->currentText() == "Odd")
    {
        checkBits = QSerialPort::OddParity;
    }
    else if(ui.comboBox_checkbit->currentText() == "Even")
    {
        checkBits = QSerialPort::EvenParity;
    }

    // 初始化串口属性，设置 端口号、波特率、数据位、停止位、奇偶校验位数
    serialport->setPortName(ui.comboBox_com->currentText());
    serialport->setBaudRate(baudRate);
    serialport->setDataBits(dataBits);
    serialport->setStopBits(stopBits);
    serialport->setParity(checkBits);
    // QMessageBox::critical(this, "错误提示", "串口打开失败！！！\r\n该串口可能被占用\r\n请选择正确的串口");
    // 根据初始化好的串口属性，打开串口
    // 如果打开成功，反转打开按钮显示和功能。打开失败，无变化，并且弹出错误对话框。
    if(ui.pushButton_OpenUart->text() == "Open UART")
    {
        if(serialport->open(QIODevice::ReadWrite) == true)
        {
            ui.pushButton_OpenUart->setText("Close UART");
            ui.label_StatusIcon->setPixmap(QPixmap(":/qrc/succeed.png"));
            ui.label_StatusText->setText("Connecting");
            // 让端口号下拉框不可选，避免误操作（选择功能不可用，控件背景为灰色）
            ui.comboBox_com->setEnabled(false);
        }
        else
        {
            QMessageBox::critical(this, "Remind", "Fail to open the Port!\nSerial Port may be ouccupied\nPlease choose the right Port");
        }
    }
    else if(ui.pushButton_OpenUart->text() == "Close UART")
    {
        serialport->close();
        ui.pushButton_OpenUart->setText("Open UART");
        ui.label_StatusIcon->setPixmap(QPixmap(":/qrc/failed.png"));
        ui.label_StatusText->setText("Disconnecting");
        // 端口号下拉框恢复可选，避免误操作
        ui.comboBox_com->setEnabled(true);
    }



    //     //statusBar 状态栏显示端口状态
    //     QString sm = "%1 OPENED, %2, 8, NONE, 1";
    //     QString status = sm.arg(serialPort->portName()).arg(serialPort->baudRate());
    //     lblPortState->setText(status);
    //     lblPortState->setStyleSheet("color:green");
    //     //statusBar 状态栏显示端口状态
    //     QString sm = "%1 CLOSED";
    //     QString status = sm.arg(serialPort->portName());
    //     lblPortState->setText(status);
    //     lblPortState->setStyleSheet("color:red");
    //

}

void UartWindow::slot_send()
{
    QByteArray array;//发送缓冲区

    //Hex发送复选框
    if(ui.checkBox_HexSend->checkState() == Qt::Checked)
    {
        array = QByteArray::fromHex(ui.textEdit_send->toPlainText().toUtf8());
    }
    else
    {
        //array = data.toLatin1();    //ASCII
        array = ui.textEdit_send->toPlainText().toUtf8();
    }
    //追加'\n'复选框
    if(ui.checkBox_Addn->checkState() == Qt::Checked)
    {
        array.append('\n');
    }
    // 如发送成功，会返回发送的字节长度。失败，返回-1。
    int len = serialport->write(array);
    // std::cout<<"sent length:"<< len << std::endl;
    // 发送字节计数并显示
    if (-1 == len)
    {
        QMessageBox::critical(this, "Remind", "Fail to send!\nPlease check the settings");
        //关闭timer
        ui.checkBox_AutoSend->setCheckState(Qt::Unchecked);
        timer_send->stop();
        ui.spinBox_ms->setEnabled(true);
        
    }
    //
    if (!(timer_send->isActive()))
    {
        if (ui.checkBox_AutoSend->checkState() == Qt::Checked)
        {

            if (ui.spinBox_ms->text().toInt() >= 10)
            {
                timer_send->setInterval(ui.spinBox_ms->text().toInt());
                timer_send->start();
                ui.spinBox_ms->setEnabled(false);
            }
            else
            {
                ui.checkBox_AutoSend->setCheckState(Qt::Unchecked);
                QMessageBox::critical(this, "Remind", "The minimum period is 10ms\nPlease make sure the period >=10");
            }
        }
        
    }
    
    
    
}
void UartWindow::slot_receive()
{
    QByteArray recBuf = serialport->readAll();//接收缓冲区
    QString str_rev,str_time,str_display;

    // 接收字节计数
    // recvNum += recBuf.size();

    if(ui.checkBox_HexDisplay->checkState() == Qt::Checked)
    {
        // 16进制显示，并转换为大写
        str_rev = recBuf.toHex().toUpper();
        // 添加空格
        QString str_temp;
        for(int i = 0; i<str_rev.length (); i+=2)
        {
            str_temp += str_rev.mid (i,2);
            str_temp += " ";
        }
        str_rev = str_temp;
    }
    else
    {
        str_rev = QString(recBuf);   
    }
    //是否添加时间戳, 添加时间戳就自动换行
    if(ui.checkBox_TimeStamp->checkState() == Qt::Checked)
    {
        QDateTime nowtime = QDateTime::currentDateTime();
        str_time = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
        str_display = str_time + str_rev;
        str_display.append('\n');
    }
    else
    {
        str_display = str_rev;
        //是否自动换行
        if(ui.checkBox_LinkBreak->checkState() == Qt::Checked)
        {
            str_display.append('\n');
        }
    }
    // qDebug()<< str_display << "\n";
    ui.plainTextEdit_receive->insertPlainText(str_display);
    ui.plainTextEdit_receive->moveCursor(QTextCursor::End);
}

void UartWindow::slot_timer_control(bool isChecked)
{
    if (isChecked)
    {
        if (ui.spinBox_ms->text().toInt() >= 10)
        {
            timer_send->setInterval(ui.spinBox_ms->text().toInt());
            timer_send->start();
            ui.spinBox_ms->setEnabled(false);
        }
        else
        {
            ui.checkBox_AutoSend->setCheckState(Qt::Unchecked);
            QMessageBox::critical(this, "Remind", "The minimum period is 10ms\nPlease make sure the period >=10");
        }
    }
    else
    {
        timer_send->stop();
        ui.spinBox_ms->setEnabled(true);
    }
    
}

void UartWindow::callback_timer()
{
    slot_send();
}

void UartWindow::slot_rx_clear()
{
    ui.plainTextEdit_receive->clear();
}
void UartWindow::slot_tx_clear()
{
    ui.textEdit_send->clear();
}