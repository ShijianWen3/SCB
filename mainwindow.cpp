#include "mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);  // 使用 uic 生成的 ui 类
    slot_connect();
}
MainWindow:: ~MainWindow()
{
    ;
}
void MainWindow::slot_connect(void)
{
    connect(ui.radioButton_uart,&QRadioButton::toggled,this,&MainWindow::slot_communicate_uart);
    connect(ui.radioButton_ip,&QRadioButton::toggled,this,&MainWindow::slot_communicate_ip);
    connect(ui.radioButton_bl,&QRadioButton::toggled,this,&MainWindow::slot_communicate_bl);

}
void MainWindow::slot_communicate_uart(bool checked)
{
    if (checked)//true时被选中
    {
        uartwindow = new UartWindow();//动态实例化,延长生命周期
        uartwindow->show();//进入下一个窗口
        // friendfunc_FindUART(uartwindow);
        this->close();
    }
    
}
void MainWindow::slot_communicate_ip(bool checked)
{
    if (checked)
    {
        ipwindow = new IPWindow();
        ipwindow->show();
        this->close();
    }
    
}
void MainWindow::slot_communicate_bl(bool checked)
{
    if (checked)
    {
        blwindow = new BlWindow();
        blwindow->show();
        this->close();
    }
    
}