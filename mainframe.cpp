#include "mainframe.h"
#include "ui_mainframe.h"
#include<QMessageBox>

MainFrame::MainFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainFrame)
{
    ui->setupUi(this);

//    viewer = new VncViewer(this);

//    QVBoxLayout *layout = new QVBoxLayout(ui->widget_view_1);
//    layout->addWidget(viewer);
//    layout->setContentsMargins(0, 0, 0, 0);  // 去除边距
//    viewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    ui->widget_view_1->setLayout(layout);

//    connect(ui->btn_connect,&QPushButton::clicked,this,&MainFrame::on_btn_connect_clicked);
//    connect(ui->btn_disconnect,&QPushButton::clicked,this,&MainFrame::on_btn_disconnect_clicked);

    setWindowTitle("LibVncClient V1.0.3 - UI");
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::on_btn_connect_clicked()
{
//    QString ip = ui->ipLineEdit->text();
//    int port = ui->portLineEdit->text().toInt();

//    if (ip.isEmpty() || port <= 0)
//    {
//        QMessageBox::warning(this, "Input Error", "Please enter a valid IP and port.");
//        return;
//    }

//    viewer->setServerInfo(ip.toStdString(), port);
//    viewer->start();
}

void MainFrame::on_btn_disconnect_clicked()
{
    viewer->disconnectFormServer();
}
