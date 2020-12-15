# 编译原理课程项目：Toy

## 计划开发工期

|进度|计划工期|（基本功能）实际完成工期|
|:---:|:---:|:---:|
|scanner|2.5天|1.5天|
|parser|2.5天|4天|
|代码生成|3天||
|虚拟机实现|3天||
|GUI界面|7天||

## scanner

+ [ ] 缓冲区(double buffering)
+ [x] lexer
+ [x] 错误处理(直接跳过error token)

## parser

+ [x] LL(1) table generator
+ [x] LL parser
+ [x] syntax tree output
+ [x] 错误处理(panic mode)

## semantic analysis

+ [x] 符号表
+ [x] 语义分析

## code generation

+ [ ] 三地址码生成
+ [ ] 代码优化

## virtual machine

+ [ ] bytecode生成
+ [ ] 虚拟机实现

## visualization

+ [ ] 多叉树生成算法
+ [ ] UI界面

## 其它部分

- [x] c单元测试
- [ ] qt可视化显示，[UI参考1](https://mashplant.online/minidecaf-frontend/), [UI参考2](https://github.com/yunwei37/MIPS-sc-zju)
- [ ] scanner generator
- [ ] LL(1) parser generator
- [ ] LR(1) parser generator

## 额外的支持

- [ ] 编程语言其它支持：for，布尔类型，内置的字符串的支持、内置语句(例如：print)
- [ ] 函数
- [ ] OOP特性

## reference

- *Engineering a compiler*
