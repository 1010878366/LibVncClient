#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include "vncviewer.h"
#include "ui_mainwindow.h"
#include "mainframe.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectButtonClicked();
    void onDisConnectButtonClicked();
    void on_brn_MainPreview_clicked();

    void on_tableWidget_IP_cellDoubleClicked(int row, int column);

    void on_btn_add_ip_clicked();

    void on_btn_remove_ip_clicked();

    void on_btn_rename_ip_clicked();
    void switchToOneScreen();
    void switchToFourScreens();

private:
    Ui::MainWindow *ui;
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QPushButton *connectButton;
    VncViewer *viewer;

    int m_nCurrentConnectionRow = -1; // 当前连接的行索引，-1表示未连接
};

#endif // MAINWINDOW_H
