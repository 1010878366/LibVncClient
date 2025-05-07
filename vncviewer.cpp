#include "vncviewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCloseEvent>
#include <iostream>

VncViewer::VncViewer(QWidget *parent)
    : QWidget(parent)
{}

VncViewer::~VncViewer()
{
    m_startFlag = false;
    if (m_vncThread && m_vncThread->joinable()) {
        m_vncThread->join();
        delete m_vncThread;
    }
    if (cl) {
        rfbClientCleanup(cl);
    }
}

void VncViewer::setServerInfo(const std::string &ip, int port)
{
    serverIp = ip;
    serverPort = port;
}

void VncViewer::start()
{
    cl = rfbGetClient(8, 3, 4);
    cl->format.depth = 16;
    cl->format.bitsPerPixel = 16;
    cl->format.redShift = 11;
    cl->format.greenShift = 5;
    cl->format.blueShift = 0;
    cl->format.redMax = 0x1f;
    cl->format.greenMax = 0x3f;
    cl->format.blueMax = 0x1f;
    cl->appData.compressLevel = 9;
    cl->appData.qualityLevel = 1;
    cl->appData.encodingsString = "tight ultra";
    cl->FinishedFrameBufferUpdate = finishedFramebufferUpdateStatic;
    cl->serverHost = strdup(serverIp.c_str());
    cl->serverPort = serverPort;
    cl->appData.useRemoteCursor = TRUE;

    rfbClientSetClientData(cl, nullptr, this);

    if (!rfbInitClient(cl, 0, nullptr)) {
        std::cout << "[INFO] Failed to connect" << std::endl;
        return;
    }

    m_startFlag = true;
    resize(cl->width, cl->height);

    m_vncThread = new std::thread([this]() {
        while (m_startFlag) {
            int i = WaitForMessage(cl, 500);
            if (i < 0) {
                std::cout << "[INFO] disconnected" << std::endl;
                rfbClientCleanup(cl);
                break;
            }
            if (i && !HandleRFBServerMessage(cl)) {
                std::cout << "[INFO] disconnected" << std::endl;
                rfbClientCleanup(cl);
                break;
            }
        }
    });
}

void VncViewer::finishedFramebufferUpdateStatic(rfbClient *cl)
{
    VncViewer *viewer = static_cast<VncViewer*>(rfbClientGetClientData(cl, nullptr));
    if (viewer)
        viewer->finishedFramebufferUpdate(cl);
}

void VncViewer::finishedFramebufferUpdate(rfbClient *cl)
{
    m_image = QImage(cl->frameBuffer, cl->width, cl->height, QImage::Format_RGB16);
    update();
}

void VncViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(rect(), m_image);
}

void VncViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         (event->buttons() & Qt::LeftButton) ? 1 : 0);
    }
}

void VncViewer::mousePressEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         1);
    }
}

void VncViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         0);
    }
}

void VncViewer::closeEvent(QCloseEvent *event)
{
    m_startFlag = false;
    if (m_vncThread && m_vncThread->joinable()) {
        m_vncThread->join();
        delete m_vncThread;
        m_vncThread = nullptr;
    }
    if (cl) {
        rfbClientCleanup(cl);
        cl = nullptr;
    }
    QWidget::closeEvent(event);
}
