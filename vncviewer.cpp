
#include "vncviewer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QCloseEvent>
#include <iostream>

VncViewer::VncViewer(QWidget *parent)
    : QWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);  //开启鼠标移动追踪

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(1, 1);  // 防止最小尺寸限制

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

    cl->appData.compressLevel = 3;  //压缩等级（0~9），越高压缩越强，占用 CPU 越多但节省带宽
    cl->appData.qualityLevel = 6;   //图像质量（0~9），值越小越模糊但带宽更小
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

void VncViewer::finishedFramebufferUpdateStatic(rfbClient *cl)
{
    VncViewer *viewer = static_cast<VncViewer*>(rfbClientGetClientData(cl, nullptr));
    if (viewer)
        viewer->finishedFramebufferUpdate(cl);
}

void VncViewer::finishedFramebufferUpdate(rfbClient *cl)
{
    //将 VNC 接收到的帧缓冲区内容（frameBuffer）构造成 QImage，然后触发重绘事件
    m_image = QImage(cl->frameBuffer, cl->width, cl->height, QImage::Format_RGB16);
    update();
}

void VncViewer::paintEvent(QPaintEvent *event)
{
//    Q_UNUSED(event);
//    QPainter painter(this);
//    painter.drawImage(rect(), m_image);



    Q_UNUSED(event);
    QPainter painter(this);

    if (!m_image.isNull()) {
        // 将图像缩放到 widget 的大小，用 IgnoreAspectRatio 拉伸填满
        //QImage scaledImage = m_image.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); //保持比例
        QImage scaledImage = m_image.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

        // 将图像居中绘制
        int x = (width() - scaledImage.width()) / 2;
        int y = (height() - scaledImage.height()) / 2;

        painter.drawImage(x, y, scaledImage);
    }
}

void VncViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        int x = event->localPos().x() / width() * cl->width;
        int y = event->localPos().y() / height() * cl->height;
        int buttons = (event->buttons() & Qt::LeftButton) ? 1 : 0;

        SendPointerEvent(cl, x, y, buttons); // 发送鼠标移动位置
        SendFramebufferUpdateRequest(cl, false, 0, 0, cl->width, cl->height); // 请求更新画面
    }
}

void VncViewer::mousePressEvent(QMouseEvent *event)
{
    if (m_startFlag) {
        int buttonMask = 0;
        if (event->button() == Qt::LeftButton)
            buttonMask = 1;        // Bit 0 左键
        else if (event->button() == Qt::MiddleButton)
            buttonMask = 2;        // Bit 1 中键
        else if (event->button() == Qt::RightButton)
            buttonMask = 4;        // Bit 2 右键

        SendPointerEvent(cl,
                         event->localPos().x() / width() * cl->width,
                         event->localPos().y() / height() * cl->height,
                         buttonMask);
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

void VncViewer::wheelEvent(QWheelEvent *event)
{
    if (!m_startFlag)
        return;

    int buttonMask = 0;
    if (event->angleDelta().y() > 0)
        buttonMask = 8;  // 滚轮上滚
    else if (event->angleDelta().y() < 0)
        buttonMask = 16; // 滚轮下滚

    //int x = event->position().x() / width() * cl->width;
    //int y = event->position().y() / height() * cl->height;
    int x = event->pos().x() / width() * cl->width;
    int y = event->pos().y() / height() * cl->height;

    // 发送滚轮按下
    SendPointerEvent(cl, x, y, buttonMask);
    // 发送滚轮释放
    SendPointerEvent(cl, x, y, 0);
}

void VncViewer::keyPressEvent(QKeyEvent *event)
{
    if (!m_startFlag || !cl)
        return;

    int qtKey = event->key();

    if (qtKey == Qt::Key_CapsLock) {
        m_capsLockOn = !m_capsLockOn; // 切换内部状态
    }

    int rfbKey = qtKeyToRfbKey(qtKey);
    if (rfbKey != 0)
        SendKeyEvent(cl, rfbKey, true);
}


void VncViewer::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_startFlag || !cl)
        return;

    int qtKey = event->key();
    int rfbKey = qtKeyToRfbKey(qtKey);
    if (rfbKey != 0)
        SendKeyEvent(cl, rfbKey, false);  // false 表示松开
}


int VncViewer::qtKeyToRfbKey(int qtKey)
{
    using namespace Qt;
    bool shiftPressed = QApplication::keyboardModifiers() & Qt::ShiftModifier;
    bool needUpper = shiftPressed ^ m_capsLockOn;   // 判断是否需要大写：只有 Shift 和 Caps 互斥时为大写

    switch (qtKey)
    {
        // 控制键
        case Key_Backspace: return XK_BackSpace;
        case Key_Tab:       return XK_Tab;
        case Key_Clear:     return XK_Clear;
        case Key_Return:    return XK_Return;
        case Key_Enter:     return XK_KP_Enter;  // 小键盘Enter
        case Key_Shift:     return XK_Shift_L;
        case Key_Control:   return XK_Control_L;
        case Key_Alt:       return XK_Alt_L;
        case Key_Meta:      return XK_Meta_L;
        case Key_Escape:    return XK_Escape;
        case Key_Space:     return XK_space;
        //case Key_CapsLock:  return XK_Caps_Lock;
        case Key_CapsLock:
            m_capsLockOn = !m_capsLockOn;   // 切换 CapsLock 状态
            return XK_Caps_Lock;

        // 光标键
        case Key_Left:      return XK_Left;
        case Key_Up:        return XK_Up;
        case Key_Right:     return XK_Right;
        case Key_Down:      return XK_Down;
        case Key_Insert:    return XK_Insert;
        case Key_Delete:    return XK_Delete;
        case Key_Home:      return XK_Home;
        case Key_End:       return XK_End;
        case Key_PageUp:    return XK_Page_Up;
        case Key_PageDown:  return XK_Page_Down;

        // 功能键
        case Key_F1: return XK_F1;
        case Key_F2: return XK_F2;
        case Key_F3: return XK_F3;
        case Key_F4: return XK_F4;
        case Key_F5: return XK_F5;
        case Key_F6: return XK_F6;
        case Key_F7: return XK_F7;
        case Key_F8: return XK_F8;
        case Key_F9: return XK_F9;
        case Key_F10: return XK_F10;
        case Key_F11: return XK_F11;
        case Key_F12: return XK_F12;

        // 数字
        case Key_1: return XK_1;
        case Key_2: return XK_2;
        case Key_3: return XK_3;
        case Key_4: return XK_4;
        case Key_5: return XK_5;
        case Key_6: return XK_6;
        case Key_7: return XK_7;
        case Key_8: return XK_8;
        case Key_9: return XK_9;
        case Key_0: return XK_0;

        // 字母键
        case Key_A: return needUpper ? XK_A : XK_a;
        case Key_B: return needUpper ? XK_B : XK_b;
        case Key_C: return needUpper ? XK_C : XK_c;
        case Key_D: return needUpper ? XK_D : XK_d;
        case Key_E: return needUpper ? XK_E : XK_e;
        case Key_F: return needUpper ? XK_F : XK_f;
        case Key_G: return needUpper ? XK_G : XK_g;
        case Key_H: return needUpper ? XK_H : XK_h;
        case Key_I: return needUpper ? XK_I : XK_i;
        case Key_J: return needUpper ? XK_J : XK_j;
        case Key_K: return needUpper ? XK_K : XK_k;
        case Key_L: return needUpper ? XK_L : XK_l;
        case Key_M: return needUpper ? XK_M : XK_m;
        case Key_N: return needUpper ? XK_N : XK_n;
        case Key_O: return needUpper ? XK_O : XK_o;
        case Key_P: return needUpper ? XK_P : XK_p;
        case Key_Q: return needUpper ? XK_Q : XK_q;
        case Key_R: return needUpper ? XK_R : XK_r;
        case Key_S: return needUpper ? XK_S : XK_s;
        case Key_T: return needUpper ? XK_T : XK_t;
        case Key_U: return needUpper ? XK_U : XK_u;
        case Key_V: return needUpper ? XK_V : XK_v;
        case Key_W: return needUpper ? XK_W : XK_w;
        case Key_X: return needUpper ? XK_X : XK_x;
        case Key_Y: return needUpper ? XK_Y : XK_y;
        case Key_Z: return needUpper ? XK_Z : XK_z;

        // 符号
        case Key_ParenLeft:     return XK_parenleft;
        case Key_ParenRight:    return XK_parenright;
        case Key_Plus:          return XK_plus;
        case Key_Minus:         return XK_minus;
        case Key_Asterisk:      return XK_asterisk;
        case Key_Slash:         return XK_slash;
        case Key_Equal:         return XK_equal;
        case Key_Period:        return XK_period;
        case Key_Comma:         return XK_comma;
        case Key_Semicolon:     return XK_semicolon;
        case Key_Colon:         return XK_colon;
        case Key_QuoteDbl:      return XK_quotedbl;
        case Key_Apostrophe:    return XK_apostrophe;
        case Key_BracketLeft:   return XK_bracketleft;
        case Key_BracketRight:  return XK_bracketright;
        case Key_Backslash:     return XK_backslash;
        case Key_Exclam:        return XK_exclam;
        case Key_Question:      return XK_question;
        case Key_At:            return XK_at;
        case Key_NumberSign:    return XK_numbersign;
        case Key_Dollar:        return XK_dollar;
        case Key_Percent:       return XK_percent;
        case Key_Ampersand:     return XK_ampersand;
        case Key_Underscore:    return XK_underscore;
        case Key_Less:          return XK_less;
        case Key_Greater:       return XK_greater;
        case Key_AsciiTilde:    return XK_asciitilde;
        case Key_QuoteLeft:     return XK_grave;

        default:
            return 0; // 不支持的按键
    }
}

void VncViewer::closeEvent(QCloseEvent *event)
{
    disconnectFormServer();
    event->accept();
    QWidget::closeEvent(event);
}

void VncViewer::disconnectFormServer()
{
    m_startFlag = false;

    if (m_vncThread && m_vncThread->joinable())
    {
        m_vncThread->join();
        delete m_vncThread;
        m_vncThread = nullptr;
    }

    if (cl)
    {
        rfbClientCleanup(cl);
        cl = nullptr;
    }
    clearDisplay();
}

void VncViewer::clearDisplay()
{
    m_image = QImage(); // 清空图像
    update(); // 触发重绘
}
