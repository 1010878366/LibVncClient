#include "vncviewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCloseEvent>
#include <iostream>

VncViewer::VncViewer(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}

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
    cl = rfbGetClient(8, 3, 4);     //创建rfbClient实例

    //配置图像颜色格式
    cl->format.depth = 16;
    cl->format.bitsPerPixel = 16;
    //颜色通道排列（RGB565格式）
    cl->format.redShift = 11;
    cl->format.greenShift = 5;
    cl->format.blueShift = 0;
    cl->format.redMax = 0x1f;
    cl->format.greenMax = 0x3f;
    cl->format.blueMax = 0x1f;


    cl->appData.compressLevel = 9;  //压缩等级（0~9），越高压缩越强，占用 CPU 越多但节省带宽
    cl->appData.qualityLevel = 1;   //图像质量（0~9），值越小越模糊但带宽更小
    cl->appData.encodingsString = "tight ultra";    //VNC编码方式，tight 是高效压缩编码，ultra 是其优化版
    cl->FinishedFrameBufferUpdate = finishedFramebufferUpdateStatic;    //设置一个回调，当有新画面数据到来时调用
    cl->serverHost = strdup(serverIp.c_str());  //目标服务器IP
    cl->serverPort = serverPort;                //目标服务器端口号
    cl->appData.useRemoteCursor = TRUE;         //使用远程服务器的鼠标样式

    rfbClientSetClientData(cl, nullptr, this);  //给客户端绑定自定义指针

    //这里后续改为判断10秒内是否连接成功。现在是20秒，时间太长
    if (!rfbInitClient(cl, 0, nullptr))         //建立连接
    {
        std::string tip = "[INFO] 无法连接到 IP: " + serverIp + "端口号: " + std::to_string(serverPort) + " 的目标 VNC 服务器。";
        QMessageBox::critical(this, "连接失败", QString::fromStdString(tip));
        return;
    }

    //启动接收线程，后台接收远程画面
    m_startFlag = true;
    resize(cl->width, cl->height);
    m_vncThread = new std::thread([this]()
    {
        while (m_startFlag)
        {
            SendFramebufferUpdateRequest(cl, false, 0, 0, cl->width, cl->height);  // 主动请求远程屏幕更新。加上这行，可以实时看到鼠标移动
            int i = WaitForMessage(cl, 100);    //最多阻塞100毫秒
            if (i < 0)
            {
                //std::cout << "[INFO] disconnected" << std::endl;
                QMessageBox::critical(this,"失败","[INFO] 连接失败！");
                rfbClientCleanup(cl);
                break;
            }
            if (i && !HandleRFBServerMessage(cl))
            {
                //std::cout << "[INFO] disconnected" << std::endl;
                QMessageBox::critical(this,"失败","[INFO] 连接失败！");
                rfbClientCleanup(cl);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));    //限制帧率，避免CPU跑满
        }
    });
}

//作为 LibVNCClient 调用的回调入口，当有新的图像帧到达（即服务器发送屏幕更新）时，它会被触发
void VncViewer::finishedFramebufferUpdateStatic(rfbClient *cl)
{
    VncViewer *viewer = static_cast<VncViewer*>(rfbClientGetClientData(cl, nullptr));
    if (viewer)
        viewer->finishedFramebufferUpdate(cl);
}

//真正处理图像更新
void VncViewer::finishedFramebufferUpdate(rfbClient *cl)
{
    //将 VNC 接收到的帧缓冲区内容（frameBuffer）构造成 QImage，然后触发重绘事件
    m_image = QImage(cl->frameBuffer, cl->width, cl->height, QImage::Format_RGB16);
    update();
}

//绘制远程屏幕图像
void VncViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawImage(rect(), m_image);
}

//鼠标移动时，发送远程鼠标移动事件
void VncViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_startFlag)
    {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         (event->buttons() & Qt::LeftButton) ? 1 : 0);
    }
}

//鼠标点击按下时，向远程发送点击事件
void VncViewer::mousePressEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         1);
    }
}

//鼠标点击释放时，发送“松开鼠标”事件
void VncViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         0);
    }
}

void VncViewer::keyPressEvent(QKeyEvent *event)
{
    if (!m_startFlag || !cl) return;

    int qtKey = event->key();
    int rfbKey = qtKeyToRfbKey(qtKey);
    if (rfbKey > 0)
        SendKeyEvent(cl, rfbKey, true);  // true 表示按下
}

void VncViewer::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_startFlag || !cl) return;

    int qtKey = event->key();
    int rfbKey = qtKeyToRfbKey(qtKey);
    if (rfbKey > 0)
        SendKeyEvent(cl, rfbKey, false);  // false 表示松开
}

int VncViewer::qtKeyToRfbKey(int qtKey)
{
    // 简单映射常用键位，可扩展更多
    if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z)
        return qtKey;
    if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9)
        return qtKey;
    if (qtKey == Qt::Key_Return)
        return XK_Return;
    if (qtKey == Qt::Key_Backspace)
        return XK_BackSpace;
    if (qtKey == Qt::Key_Space)
        return XK_space;
    if (qtKey == Qt::Key_Escape)
        return XK_Escape;
    if (qtKey == Qt::Key_Tab)
        return XK_Tab;
    if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F12)
        return XK_F1 + (qtKey - Qt::Key_F1);

    return 0; // 不支持的键
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
