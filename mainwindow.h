#ifndef __MAINWINDOW_H
#define __MAINWINDOW_H
#include <QApplication>
#include <QMainWindow>
#include "ui/ui_mainwindow.h"  // 引入 uic 生成的头文件
#include "my_windows.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private:
        Ui::mainwindow ui;  // 生成的 UI 类实例
        UartWindow* uartwindow;// 初始化为指针以动态实例化
        IPWindow* ipwindow;
        BlWindow* blwindow;
        
        void slot_connect(void);
        void slot_communicate_uart(bool checked);//checked 接收信号发送的bool参数,用于区分选中和取消选中
        void slot_communicate_ip(bool checked);
        void slot_communicate_bl(bool checked);
        
};

#endif