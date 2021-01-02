# design doc 6: ui

本文档主要介绍编译器UI部分的设计。

## Menu Bar

### file

```c
void on_actionopen_triggered();
void on_actionsave_triggered();
void on_actionnew_triggered();
void on_actionexit_triggered();
```

### help

```c
void on_actionabout_triggered();
void on_actiondocument_triggered();  //需添加doc文件-->mainwindow.cpp
```

### build

```c
void on_actioncompile_triggered();  //不确定是否和Tab1的compile button相同功能
```

## Tab1

+ 可读入代码文件，output框会显示消息save/open file sucessfully。

```c
compile button
    //void on_compileButton_clicked();
```

## Tab2

+ 输出多叉树图片，并输出语义分析生成的代码和相应的字节码指令。
+ parse tree  //需要输出多叉树图片
+ output        //需要输出代码生成的结果
+ instruction //需要输出字节码指令
