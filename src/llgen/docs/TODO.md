# TODO

这里记录一些暂时不会改动，但是设计不优雅的地方。

1. llgen的输入文件格式中的internal段，当前必须和token类型一一对应且出现顺序一致，若实现的语言的grammar不存在该token，则需要用placeholder占位。
    + epsilon的处理也不够优雅
    + 这是因为希望terminal的索引和token的enum的值相同，方便处理。
2. 使用hashset，后期考虑红黑树的实现
