#include "mainpreview.h"
#include "ui_mainpreview.h"

MainPreview::MainPreview(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainPreview)
{
    ui->setupUi(this);





    setWindowTitle("主预览");

}

MainPreview::~MainPreview()
{
    delete ui;
}
