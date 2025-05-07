#ifndef VNCVIEWER_H
#define VNCVIEWER_H

#include <QWidget>
#include <thread>
#include <QImage>
#include "rfb/rfbclient.h"

class VncViewer : public QWidget
{
    Q_OBJECT
public:
    explicit VncViewer(QWidget *parent = nullptr);
    ~VncViewer();

    void setServerInfo(const std::string &ip, int port);
    void start();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool m_startFlag = false;
    QImage m_image;
    rfbClient *cl = nullptr;
    std::thread *m_vncThread = nullptr;
    std::string serverIp;
    int serverPort;

    static void finishedFramebufferUpdateStatic(rfbClient *cl);
    void finishedFramebufferUpdate(rfbClient *cl);
};

#endif // VNCVIEWER_H
