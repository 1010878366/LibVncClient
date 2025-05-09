#include <QApplication>
#include "mainwindow.h"
#include"mainframe.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    //MainFrame w;
    w.show();
    return a.exec();
}
