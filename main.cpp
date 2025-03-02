#include "mainwindow.h"
#include "my_windows.h"

int main(int argc, char *argv[])
{
    // QSurfaceFormat format;
    // format.setDepthBufferSize(24);
    // format.setStencilBufferSize(8);
    // format.setVersion(3, 3);  // 设置 OpenGL 版本
    // format.setProfile(QSurfaceFormat::CoreProfile);
    // QSurfaceFormat::setDefaultFormat(format);


    QApplication app(argc, argv);
    QIcon icon(":/logo_app.png");
    app.setWindowIcon(icon);
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}