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

    setWindowTitle("LibVncClient V1.0.4.2");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectButtonClicked()
{
    QString ip = ui->LineEdit_IP->text();
    int port = /*ui->LineEdit_Port->text().toInt()*/5900;

    if (ip.isEmpty() || port <= 0)
    {
        QMessageBox::warning(this, "输入错误！", "请输入正确的IP！");
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


void MainWindow::on_tableWidget_IP_cellDoubleClicked(int row, int column)
{
    if (row == m_nCurrentConnectionRow) {
        return; // 避免重复连接同一行
    }

    // 如果已经连接到其他行，先断开
    if (m_nCurrentConnectionRow >= 0) {
        viewer->disconnectFormServer();
        m_nCurrentConnectionRow = -1;
    }

    QString ip = ui->tableWidget_IP->item(row, 0)->text();  // 第0列是 IP
    int port = 5900; // 固定端口

    if (ip.isEmpty()) {
        QMessageBox::warning(this, "输入错误！", "IP 不能为空！");
        return;
    }

    viewer->setServerInfo(ip.toStdString(), port);
    viewer->start();
    m_nCurrentConnectionRow = row; // 更新当前连接行
}


void MainWindow::on_btn_add_ip_clicked()
{
    int row = ui->tableWidget_IP->rowCount();
    ui->tableWidget_IP->insertRow(row);

    // 插入两个空项，用户自己编辑
    ui->tableWidget_IP->setItem(row, 0, new QTableWidgetItem(""));
    ui->tableWidget_IP->setItem(row, 1, new QTableWidgetItem(""));

    // 选中新增行，并自动开始编辑第一列（IP）
    ui->tableWidget_IP->setCurrentCell(row, 0);
    ui->tableWidget_IP->editItem(ui->tableWidget_IP->item(row, 0));

}

void MainWindow::on_btn_remove_ip_clicked()
{
    int row = ui->tableWidget_IP->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选中要删除的行！");
        return;
    }

    QString ip = ui->tableWidget_IP->item(row, 0)->text();
    QString name = ui->tableWidget_IP->item(row, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除",
        QString("确定要删除以下项吗？\nIP: %1\n名称: %2").arg(ip, name),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        ui->tableWidget_IP->removeRow(row);
    }
}

void MainWindow::on_btn_rename_ip_clicked()
{
    int row = ui->tableWidget_IP->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选中要重命名的行！");
        return;
    }

    // 开始编辑 IP 和 名称
    ui->tableWidget_IP->editItem(ui->tableWidget_IP->item(row, 0)); // IP
    ui->tableWidget_IP->editItem(ui->tableWidget_IP->item(row, 1)); // 名称
}
