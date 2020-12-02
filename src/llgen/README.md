# llgen

llgen程序用于辅助实现toy语言的LL(1) parser。

## llgen的编译

在当前目录下执行`make`

## llgen的使用

```
llgen -i [input file path] [-o [output file directory path]]
```

+ 输出文件默认目录为`$PWD`
+ 输入、输出文件格式见[格式](./docs/input_output_file_format.md)
+ 设计文档见[设计](./docs/design_doc.md)
