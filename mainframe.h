#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>
#include"vncviewer.h"

namespace Ui {
class MainFrame;
}

class MainFrame : public QWidget
{
    Q_OBJECT

public:
    explicit MainFrame(QWidget *parent = nullptr);
    ~MainFrame();

private slots:
    void on_btn_connect_clicked();

    void on_btn_disconnect_clicked();

private:
    Ui::MainFrame *ui;

    VncViewer *viewer;
};

#endif // MAINFRAME_H
