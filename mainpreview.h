#ifndef MAINPREVIEW_H
#define MAINPREVIEW_H

#include <QMainWindow>
#include"vncviewer.h"
#include<QListWidgetItem>

namespace Ui {
class MainPreview;
}

class MainPreview : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPreview(QWidget *parent = nullptr);
    ~MainPreview();


    void onIpItemDoubleClicked(QListWidgetItem *item);

private slots:
    void on_btn_connect_clicked();


    void on_btn_disconnect_clicked();

    void on_btn_exe_clicked();

private:
    Ui::MainPreview *ui;

    VncViewer *viewer;
};

#endif // MAINPREVIEW_H
