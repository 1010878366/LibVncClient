#ifndef NEWFROM_H
#define NEWFROM_H

#include <QWidget>

namespace Ui {
class NewFrom;
}

class NewFrom : public QWidget
{
    Q_OBJECT

public:
    explicit NewFrom(QWidget *parent = nullptr);
    ~NewFrom();

private:
    Ui::NewFrom *ui;
};

#endif // NEWFROM_H
