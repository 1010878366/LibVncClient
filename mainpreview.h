#ifndef MAINPREVIEW_H
#define MAINPREVIEW_H

#include <QMainWindow>
#include"vncviewer.h"


namespace Ui {
class MainPreview;
}

class MainPreview : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPreview(QWidget *parent = nullptr);
    ~MainPreview();



private slots:


private:
    Ui::MainPreview *ui;


};

#endif // MAINPREVIEW_H
