// VncDisplayWidget.cpp
#include "VncDisplayWidget.h"
#include <QPainter>

VncDisplayWidget::VncDisplayWidget(QWidget *parent)
    : QWidget(parent) {

}

void VncDisplayWidget::updateFrame(const QImage &frame) {
    QMutexLocker locker(&m_mutex);
    m_frame = frame;
    update(); // 触发重绘
}

void VncDisplayWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QMutexLocker locker(&m_mutex);
    if (!m_frame.isNull()) {
        // 自适应缩放铺满显示
        painter.drawImage(rect(), m_frame);
    }
}

void VncDisplayWidget::resizeEvent(QResizeEvent *) {
    update(); // 窗口尺寸变化时触发重绘
}
