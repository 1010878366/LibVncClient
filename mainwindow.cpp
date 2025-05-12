#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mainframe.h"

#include <QVBoxLayout>
#include <QMessageBox>

#define VIEWERCOUNT 4


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const int viewerCount = VIEWERCOUNT;
    viewers.resize(viewerCount);

    for (int i = 0; i < viewerCount; ++i)
    {
        viewers[i] = new VncViewer(this);
    }

    bindViewersToLabels();  // 单独封装绑定逻辑

    connect(ui->btn_connect, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(ui->btn_disconnect, &QPushButton::clicked, this, &MainWindow::onDisConnectButtonClicked);
    //connect(ui->comboBox_screenSwitch, SIGNAL(currentIndexChanged(int)), this, SLOT(onScreenSwitchChanged(int)));
    connect(ui->comboBox_screenSwitch, QOverload<int>::of(&QComboBox::currentIndexChanged),this, &MainWindow::onScreenSwitchChanged);

    switchToOneScreen();

    setWindowTitle("LibVncClient V1.0.4.5");
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnectButtonClicked()
{
    QString ip = ui->LineEdit_IP->text();
    int port = 5900;

    if (ip.isEmpty())
    {
        QMessageBox::warning(this, "输入错误", "请输入正确的IP地址！");
        return;
    }

    int row = ui->tableWidget_IP->currentRow();
    if (row < 0 || row >= viewers.size())
    {
        QMessageBox::warning(this, "错误", "请选择一个有效的连接行！");
        return;
    }

    VncViewer* targetViewer = viewers[row];
    targetViewer->setServerInfo(ip.toStdString(), port);
    targetViewer->start();
}


void MainWindow::onDisConnectButtonClicked()
{
    for (auto viewer : viewers)
    {
        viewer->disconnectFormServer();
    }
}

void MainWindow::on_tableWidget_IP_cellDoubleClicked(int row, int column)
{
    if (row == m_nCurrentConnectionRow)
        return;

    if (row < 0 || row >= ui->tableWidget_IP->rowCount())
        return;

    QString ip = ui->tableWidget_IP->item(row, 0)->text();
    int port = 5900;

    if (ip.isEmpty()) {
        QMessageBox::warning(this, "输入错误！", "IP 不能为空！");
        return;
    }

    if (row >= 0 && row < viewers.size())
    {
        VncViewer* targetViewer = viewers[row];
        targetViewer->setServerInfo(ip.toStdString(), port);
        targetViewer->start();
        m_nCurrentConnectionRow = row;
    }
    else
    {
        QMessageBox::warning(this, "超出范围", "没有足够的 VncViewer 来显示第 " + QString::number(row + 1) + " 个 IP！");
    }
}

void MainWindow::onScreenSwitchChanged(int index)
{
    switch (index)
    {
        case 0: // 1画面
            switchToOneScreen();
            break;
        case 1: // 4画面
            switchToFourScreens();
            break;
        default:
            break;
    }
}

void MainWindow::switchToOneScreen()
{
    ui->label_screen1->show();
    ui->label_screen2->hide();
    ui->label_screen3->hide();
    ui->label_screen4->hide();
}

void MainWindow::switchToFourScreens()
{
    ui->label_screen1->show();
    ui->label_screen2->show();
    ui->label_screen3->show();
    ui->label_screen4->show();
}

void MainWindow::on_btn_add_ip_clicked()
{
    int row = ui->tableWidget_IP->rowCount();
    ui->tableWidget_IP->insertRow(row);
    ui->tableWidget_IP->setItem(row, 0, new QTableWidgetItem(""));
    ui->tableWidget_IP->setItem(row, 1, new QTableWidgetItem(""));
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

    auto reply = QMessageBox::question(
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

    ui->tableWidget_IP->editItem(ui->tableWidget_IP->item(row, 0));
    ui->tableWidget_IP->editItem(ui->tableWidget_IP->item(row, 1));
}

void MainWindow::on_brn_MainPreview_clicked()
{
    MainFrame *frame = new MainFrame(this);
    frame->show();
}

void MainWindow::bindViewersToLabels()
{
    QVector<QWidget*> labels =
    {
        ui->label_screen1, ui->label_screen2, ui->label_screen3, ui->label_screen4
        // 扩展时继续加：ui->label_screen5 等
    };

    for (int i = 0; i < viewers.size() && i < labels.size(); ++i)
    {
        // 为每个 label 添加鼠标事件监听
        labels[i]->setAttribute(Qt::WA_Hover, true);  // 启用 hover 事件
        labels[i]->installEventFilter(this); // 安装事件过滤器

        QVBoxLayout* layout = new QVBoxLayout(labels[i]);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(viewers[i]);
        viewers[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        // 检查是哪个 label 被点击
        for (int i = 0; i < viewers.size(); ++i)
        {
            if (watched == ui->label_screen1 + i) // 检查当前控件
            {
                QString ip = ui->tableWidget_IP->item(i, 0)->text();
                if (ip.isEmpty()) {
                    QMessageBox::warning(this, "错误", "IP 不能为空！");
                    return true;
                }

                // 进行远程连接的操作
                VncViewer* targetViewer = viewers[i];
                targetViewer->setServerInfo(ip.toStdString(), 5900);  // 端口默认 5900
                targetViewer->start();
                break;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
