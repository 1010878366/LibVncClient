#include "newfrom.h"
#include "ui_newfrom.h"

NewFrom::NewFrom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewFrom)
{
    ui->setupUi(this);
}

NewFrom::~NewFrom()
{
    delete ui;
}
