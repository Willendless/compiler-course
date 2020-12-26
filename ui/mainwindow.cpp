#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qdir.h"
#include "qdialog.h"
#include "qfiledialog.h"
#include "ui_mainwindow.h"
#include "qmessagebox.h"
#include "qfontdialog.h"

#include <string>
#include <cstring>
//char compileBuffer[65536*16];

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    highlighter = new Highlighter(ui->codeEdit->document());
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionsave_triggered()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    QString dlgTitle="save"; //对话框标题
    QString filter="source(*.asm);;text(*.txt);;all(*.*)";
    QString aFileName=QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (aFileName.isEmpty())
        return;
     QFile aFile(aFileName);
     if (!aFile.open(QIODevice::WriteOnly | QIODevice::Text))
         return;
      QString str = ui->codeEdit->toPlainText();
      QByteArray  strBytes=str.toUtf8();
      aFile.write(strBytes,strBytes.length());
      aFile.close();
      ui->compileOutput->setText("save file successfully");
      return;
}

void MainWindow::on_actionopen_triggered()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    //获取应用程序的路径
    QString dlgTitle="选择一个文件"; //对话框标题
    QString filter="source(*.toy);;all(*.*)"; //文件过滤器
    QString aFileName=QFileDialog::getOpenFileName(this,dlgTitle,curPath,filter);
    if (aFileName.isEmpty())
        return;
    QFile   aFile(aFileName);
    if (!aFile.exists())
        return;
    if (!aFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    if (!aFileName.isEmpty())
        ui->codeEdit->setPlainText(aFile.readAll());
    aFile.close();
    ui->compileOutput->setText("open file successfully");
    return;
}

void MainWindow::on_actionnew_triggered()
{
    ui->codeEdit->clear();
}

void MainWindow::on_actionexit_triggered()
{
    exit(0);
}

void MainWindow::on_actionabout_triggered()
{
    QMessageBox::about(this, tr("About the compiler"),
                tr("<p> <b>compiler</b></p> " \
                   "<p>version 0.1</p>" \
                   ""));
}

void MainWindow::on_actiondocument_triggered()
{
    //open docx
}

