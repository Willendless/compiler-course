# 编译原理课程项目：Toy

## 计划开发工期

|进度|计划工期|（基本功能）实际完成工期|
|:---:|:---:|:---:|
|scanner|2.5天|1.5天|
|parser|2.5天|4天|
|代码生成|3天|3天|
|虚拟机实现|3天|1天|
|GUI界面|7天|7天|

## scanner

+ [ ] 缓冲区(double buffering)
+ [x] lexer(scanning on demand)
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

+ [x] parser tree -> ast tree
+ [x] ast tree -> 三地址码生成

## virtual machine

+ [x] 虚拟机实现

## visualization

+ [ ] 多叉树生成算法
+ [x] UI界面

## 其它部分

- [x] c单元测试
- [x] qt可视化显示，[UI参考1](https://mashplant.online/minidecaf-frontend/), [UI参考2](https://github.com/yunwei37/MIPS-sc-zju)
- [ ] IR代码优化

## reference

- *Engineering a compiler*
- *编译原理实验手册1，2，3，4  南京大学计算机科学与技术系 许畅等*
- *Compilers: Principles, Techniques, and Tools*
- *qt UI参考：https://github.com/yunwei37/MIPS-sc-zju*
