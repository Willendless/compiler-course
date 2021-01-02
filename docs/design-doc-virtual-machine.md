# design doc 5: virtual machine

本文档介绍toy语言三地址码执行虚拟机的设计。

## 虚拟机架构

+ 使用一个pc变量表示当前遍历到的三地址码数组的索引
+ 不考虑寄存器，所有变量均放在内存中，从内存中获取

## 数据结构

```c
typedef struct {
    int pc;
    DArray *code;
    DArray *var;
    DArray *temp_var;
} vm;
```

## 算法

整体来说遍历三地址码数组，根据三地址码类型dispatch。

## 预处理

遍历三地址码数组，记录label声明对应的pc。初始化var、tempvar数组。

### 指令处理

+ 四则运算指令：分别处理两个操作数，然后完成赋值
+ if条件指令：处理条件表达式，根据结果跳转或者继续执行
+ goto指令：强制设置pc的值

### 操作数处理

+ OPERAND_VAR: 对于变量，插入var数组中，模拟对内存操作。
+ OPERAND_TEMP：对于临时变量，插入temp_var数组中，模拟对内存操作。
+ OPERAND_CONST: 对于const变量，直接计算出值
+ OPERAND_LABEL: 对于label的跳转，直接设置pc

## 合理性

该虚拟机的实现作为对测试代码结果的快速实现，因此不考虑性能。
