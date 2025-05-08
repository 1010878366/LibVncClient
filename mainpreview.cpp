#include "mainpreview.h"
#include "ui_mainpreview.h"

MainPreview::MainPreview(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainPreview)
{
    ui->setupUi(this);

    viewer = new VncViewer(this);

    connect(ui->ipListWidget, &QListWidget::itemDoubleClicked,
            this, &MainPreview::onIpItemDoubleClicked);
    ui->portLineEdit->setText("5900");

    QVBoxLayout *layout = new QVBoxLayout(ui->videoPanel_1);
    layout->setContentsMargins(0, 0, 0, 0); // 去除边距
    layout->addWidget(viewer);


    setWindowTitle("主预览");

}

MainPreview::~MainPreview()
{
    delete ui;
}

void MainPreview::on_btn_connect_clicked()
{

}

void MainPreview::onIpItemDoubleClicked(QListWidgetItem *item)
{
    QString ip = item->text();
    int port = ui->portLineEdit->text().toInt(); // 读取端口行编辑框
    if (port <= 0)
        port = 5900; // 默认端口

    if (ip.isEmpty())
    {
        QMessageBox::warning(this, "Input Error", "IP address is empty.");
        return;
    }

    viewer->setServerInfo(ip.toStdString(), port);

    viewer->start();
}
