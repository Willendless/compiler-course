#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QApplication a(argc, argv);
    MainWindow w;

    printf("begin\n");
    fflush(stdout);
    w.setWindowTitle("Toy compiler");
    w.show();
    return a.exec();
}
