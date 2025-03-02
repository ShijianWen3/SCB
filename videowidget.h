/*
一种更标准的openglwidget渲染方式
*/
#ifndef __VIDEOWIDGET_H
#define __VIDEOWIDGET_H
#include <QOpenGLFunctions_3_3_Core>
#include <QtOpenGLWidgets>
#include <QOpenGLShaderProgram>
class MyVideoWidget:public QOpenGLWidget
{
    Q_OBJECT
private:
    QOpenGLShaderProgram *shaderProgram; // OpenGL 着色器程序
    QOpenGLFunctions_3_3_Core* openglfunction;
    GLuint textureId;// OpenGL 纹理 ID
    QByteArray videoFrameData;// 存储接收到的视频帧数据

    
protected:
    void initializeGL() override;// 初始化 OpenGL 环境
    void resizeGL(int width, int height) override;//// 调整 OpenGL 视口
    void paintGL() override;
public:
    MyVideoWidget(QWidget* parent = nullptr);
    ~MyVideoWidget();
    void initialize_gl(void);
    void resize_gl(int width, int height);
    void paint_gl(void);
};









#endif