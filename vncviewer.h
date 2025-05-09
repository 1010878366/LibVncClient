#ifndef VNCVIEWER_H
#define VNCVIEWER_H

#include <QWidget>
#include <QApplication>
#include <thread>
#include <QImage>
#include "rfb/rfbclient.h"
#include "rfb/keysym.h"
#include <QMessageBox>

extern "C" {
#include <rfb/keysym.h>
}

class VncViewer : public QWidget
{
    Q_OBJECT
public:
    explicit VncViewer(QWidget *parent = nullptr);
    ~VncViewer();

    void setServerInfo(const std::string &ip, int port);
    void start();
    void disconnectFormServer();
    void clearDisplay();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;       //鼠标移动
    void mousePressEvent(QMouseEvent *event) override;      //鼠标按下
    void mouseReleaseEvent(QMouseEvent *event) override;    //鼠标松开
    void wheelEvent(QWheelEvent *event) override;           //滚轮移动
    void keyPressEvent(QKeyEvent *event) override;          //键盘按下
    void keyReleaseEvent(QKeyEvent *event) override;        //键盘松开
    void closeEvent(QCloseEvent *event) override;

private:
    bool m_startFlag = false;   //是否连接成功
    QImage m_image;             //存储从 VNC 服务器接收到的画面数据
    rfbClient *cl = nullptr;    //指向 rfbClient 的指针，rfbClient 是 VNC 协议的客户端数据结构，负责与 VNC 服务器进行通信和数据交互
    std::thread *m_vncThread = nullptr;
    std::string serverIp;
    int serverPort;
    bool m_capsLockOn = false;  //大写锁定按键是否按下

    static void finishedFramebufferUpdateStatic(rfbClient *cl);     //接收到 VNC 服务器的画面
    void finishedFramebufferUpdate(rfbClient *cl);      //处理接收到的画面数据并将其存储到 m_image 中，然后触发界面更新。
    int qtKeyToRfbKey(int qtKey);               //将 Qt 的键盘事件映射为 VNC 协议所使用的键盘事件代码。这是键盘事件处理的辅助函数。
};

#endif // VNCVIEWER_H
