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

extern "C"

{
#include "toy/compiler.h"
}


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
    // 获取系统当前目录
    QString curPath=QDir::currentPath();
    // 对话框标题
    QString dlgTitle="选择一个文件";
    // 文件过滤器
    QString filter="all(*.*);;source(*.toy)";
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
    QMessageBox::about(this, tr("关于"),
                tr("<p> <b>toy compiler</b></p> " \
                   "<p>version 1.0</p>" \
                   "<p>author: 李嘉睿, 陈可欣, 孔露萍</p>" \
                   ""));
}

void MainWindow::on_compileButton_clicked()
{
    // get the source from edit plane
    QString context = ui->codeEdit->toPlainText();
    const char *source = qPrintable(context);
    // scan, parse and semantic analysis
    memset(compile_output, 0, sizeof(compile_output));
    compile(source);
    // output result to bottom plane
    ui->compileOutput->setText(compile_output);
}

void MainWindow::on_actioncompile_triggered()
{
    on_compileButton_clicked();
}

void MainWindow::on_actiondocument_triggered()
{
    //open docx
}

