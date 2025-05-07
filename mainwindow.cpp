#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      viewer(new VncViewer(this))
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    ipLineEdit = new QLineEdit("192.168.7.11", this);
    portLineEdit = new QLineEdit("5900", this);
    connectButton = new QPushButton("Connect", this);

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(new QLabel("IP:", this));
    inputLayout->addWidget(ipLineEdit);
    inputLayout->addWidget(new QLabel("Port:", this));
    inputLayout->addWidget(portLineEdit);
    inputLayout->addWidget(connectButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(viewer);

    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);

    setWindowTitle("LibVncClient V1.0.1");
}

MainWindow::~MainWindow()
{
    // VncViewer 在父对象析构时自动释放
}

void MainWindow::onConnectButtonClicked()
{
    QString ip = ipLineEdit->text();
    int port = portLineEdit->text().toInt();

    if (ip.isEmpty() || port <= 0) {
        QMessageBox::warning(this, "Input Error", "Please enter a valid IP and port.");
        return;
    }

    viewer->setServerInfo(ip.toStdString(), port);
    viewer->start();
}
