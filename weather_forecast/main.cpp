#include "mainwindow.h"
#include <QApplication>

// главная функция
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
