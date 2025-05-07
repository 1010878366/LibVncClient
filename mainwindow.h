#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include "vncviewer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectButtonClicked();

private:
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QPushButton *connectButton;
    VncViewer *viewer;
};

#endif // MAINWINDOW_H
