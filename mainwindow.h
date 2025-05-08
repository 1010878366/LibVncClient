#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include "vncviewer.h"
#include "ui_mainwindow.h"
#include "mainpreview.h"

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

    void on_pushButton_clicked();

    void on_btn_connect_clicked();

    void on_brn_MainPreview_clicked();

private:
    Ui::MainWindow *ui;
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QPushButton *connectButton;
    VncViewer *viewer;
};

#endif // MAINWINDOW_H
