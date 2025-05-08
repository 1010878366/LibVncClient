QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


QT += widgets
CONFIG -= app_bundle
TEMPLATE = app
# 相对路径到项目中的 include 文件夹
INCLUDEPATH += $$PWD/include
# 相对路径到 lib 目录
LIBS += -L$$PWD/lib -lvncclient -lvncserver
# 需要用到 WinSock 函数，需要加上 ws2_32 库
win32:LIBS += -lws2_32

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainpreview.cpp \
    mainwindow.cpp \
    vncviewer.cpp

HEADERS += \
    mainpreview.h \
    mainwindow.h \
    vncviewer.h

FORMS += \
    mainpreview.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=
