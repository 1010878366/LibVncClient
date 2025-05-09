// VncDisplayWidget.h
#pragma once
#include <QWidget>
#include <QImage>
#include <QMutex>

class VncDisplayWidget : public QWidget {
    Q_OBJECT
public:
    explicit VncDisplayWidget(QWidget *parent = nullptr);

    void updateFrame(const QImage &frame);  // 外部调用此函数更新画面

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QImage m_frame;
    QMutex m_mutex;
};
