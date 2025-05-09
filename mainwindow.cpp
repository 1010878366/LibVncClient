#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      viewer(new VncViewer(this)),
      ui(new Ui::MainWindow)
{ 
    ui->setupUi(this);

    viewer = new VncViewer(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->widget_view);
    layout->addWidget(viewer);
    layout->setContentsMargins(0, 0, 0, 0);  // 去除边距
    viewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->widget_view->setLayout(layout);

    connect(ui->btn_connect, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(ui->btn_disconnect,&QPushButton::clicked,this,&MainWindow::onDisConnectButtonClicked);

    setWindowTitle("LibVncClient V1.0.4 - UI");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectButtonClicked()
{
    QString ip = ui->ipLineEdit->text();
    int port = ui->portLineEdit->text().toInt();

    if (ip.isEmpty() || port <= 0)
    {
        QMessageBox::warning(this, "Input Error", "Please enter a valid IP and port.");
        return;
    }

    viewer->setServerInfo(ip.toStdString(), port);
    viewer->start();
}

void MainWindow::onDisConnectButtonClicked()
{
    viewer->disconnectFormServer();
}

void MainWindow::on_brn_MainPreview_clicked()
{
    MainFrame *frame = new MainFrame(this);
    frame->show();
}

