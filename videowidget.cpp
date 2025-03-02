#include "videowidget.h"

// openGLWidget_video = new QOpenGLWidget(ipwindow);
// openGLWidget_video->setObjectName("openGLWidget_video");
// openGLWidget_video->setGeometry(QRect(410, 150, 381, 311));

MyVideoWidget::MyVideoWidget(QWidget* parent)
    :QOpenGLWidget(parent), shaderProgram(nullptr)
{
}

MyVideoWidget::~MyVideoWidget()
{
    delete shaderProgram;
}
void MyVideoWidget::initializeGL()
{
    // 初始化 OpenGL 环境
    openglfunction = new QOpenGLFunctions_3_3_Core();
    openglfunction->initializeOpenGLFunctions();
    // initializeOpenGLFunctions();  // 初始化 OpenGL 函数
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // 设置背景色为黑色

    // 初始化着色器程序
    shaderProgram = new QOpenGLShaderProgram(this);
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex_shader.glsl");
    shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment_shader.glsl");
    shaderProgram->link();
    shaderProgram->bind();

    glGenTextures(1, &textureId);  // 生成一个纹理 ID
}
void MyVideoWidget::resizeGL(int width, int height)
{
    // 调整 OpenGL 视口
    glViewport(0, 0, width, height);  // 设置视口为整个窗口大小
}
void MyVideoWidget::paintGL()
{
    // 清除颜色缓冲区
    glClear(GL_COLOR_BUFFER_BIT);

    // 如果接收到视频帧，渲染它
    if (!videoFrameData.isEmpty()) 
    {
        // 这里假设接收到的视频帧数据为 RGB 格式
        // 你可以根据实际情况调整纹理的格式和尺寸

        glBindTexture(GL_TEXTURE_2D, textureId);  // 绑定纹理

        // 上传视频帧数据到纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, videoFrameData.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // 设置纹理过滤
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        shaderProgram->bind();
        // 绘制操作：绘制一个矩形，使用纹理显示视频帧
        // 这里可以使用 VAO/VBO 绘制矩形来显示纹理
        // glDrawArrays(...);
        shaderProgram->release();
    }
}

void MyVideoWidget::initialize_gl()
{
    initializeGL();
}
void MyVideoWidget::resize_gl(int width, int height)
{
    resizeGL(width, height);
}
void MyVideoWidget::paint_gl()
{
    paintGL();
}