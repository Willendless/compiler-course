#include "mainwindow.h"
#include <QApplication>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    printf("begin\n");
    fflush(stdout);
    w.show();
    return a.exec();
}
