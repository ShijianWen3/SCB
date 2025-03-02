cmake_minimum_required(VERSION 3.30)

project(SCB LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "D:/Qt/6.8.2/msvc2022_64")
# 启用详细的编译日志
set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON) 
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_C_COMPILER_LAUNCHER ccache gcc)
# set(CMAKE_CXX_COMPILER_LAUNCHER ccache g++)#使用ccache加速编译

set(CMAKE_CXX_FLAGS "-mwindows")#.exe运行时不显示终端

# set(OpenCV_DIR D:/opencv/opencv-4.9.0-msvc/install)
set(OpenCV_DIR D:/opencv/opencv/build/x64/vc16/lib)


find_package(Qt6 COMPONENTS Widgets Gui SerialPort OpenGLWidgets Network OpenGL Bluetooth REQUIRED)
find_package(OpenCV REQUIRED)
file(GLOB ui_files ./src/ui/*.ui)

file(GLOB qrc_files ./src/*.qrc)
file(GLOB src_files ./src/*.cpp)
file(GLOB inc_files ./src/*.h)
# set(QRC_SOURCE "./src/qrc")

set(ICON_RESOURCE "icon.rc")#生成的.exe文件的图标来源




add_executable(${PROJECT_NAME} ${inc_files} ${src_files} ${ui_FILES} ${qrc_files} ${ICON_RESOURCE})


# target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Gui Qt6::SerialPort Qt6::OpenGLWidgets Qt6::Network opengl32 ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Gui Qt6::SerialPort Qt6::OpenGLWidgets Qt6::Network opengl32 Qt6::Bluetooth ${OpenCV_LIBS})
