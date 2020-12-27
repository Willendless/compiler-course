#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QTextEdit"
#include "highlighter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // menu-file
    void on_actionopen_triggered();
    void on_actionsave_triggered();
    void on_actionnew_triggered();
    void on_actionexit_triggered();
    // menu-build
    void on_actioncompile_triggered();
    // menu-help
    void on_actionabout_triggered();
    void on_actiondocument_triggered();
    // bottom-Buttons
    void on_compileButton_clicked();
    void on_runButton_clicked();
    void on_resetButton_clicked();
    // tab-switched
    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    Highlighter *highlighter;

};

#endif // MAINWINDOW_H
