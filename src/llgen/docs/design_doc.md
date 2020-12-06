# llgen设计文档

llgen用于生成LL(1) parser使用的parse table。该表最终以二维常数字符串数组表示，表每行表示：*non-terminal在terminal作为lookahead符号的情况下，对应的操作*。操作分为三类：

1. 推导出的production的索引
2. error。恢复方式：省略该lookahead character
3. error。恢复方式：省略该non-terminal

## 输入输出定义

### 命令行参数定义

```
llgen -i [input file path] [-o [output file path]]
```

+ 输出文件默认路径为`$PWD/parser.c

### 索引定义

+ terminal索引：和token的enum类型中的值相同，因此要求terminal段的输入和enum类型**顺序相同**。同时要求按enum中定义的顺序作为输入。
+ non-terminal索引：在terminal基础上依据输入顺序。
+ production索引：按输入顺序依次递增

### production格式

production表示为`[head] -> [product]`。

## 数据结构

### 0. 映射的维护

+ *动态字符串数组*维护索引到terminal和non-terminal的映射
+ *哈希表*维护terminal和non-terminal到索引的映射

### 1. 表示production的数据结构

+ 使用动态数组表示，索引和non-terminal的索引一一对应
+ 每一项对应一个动态数组，用于存储同一head对应的所有production
+ 单个production也是用动态数组表示
+ 因此属于三维数组结构

### 2. 表示first+集合

+ 使用动态数组表示所有集合，索引和nonterminal索引一一对应
+ 每一项用一个set表示

## 算法

### 1. 计算first集合

定义：*For a grammar symbol a, FIRST(a) is the set of terminals that can appear at the start of a sentence derived from a*

+ fixed-point算法
+ 因此可以包含epsilon

```
for each terminal i {
    first[i] = i
}
for each non-terminal i {
    first[i] = empty
}
while (first sets变化) {
     for each productions {
         for each production {
             first[head] += first[product[0]] - epsilon
             for i = 1; i < len(product) - 1 && epsilon in first[i-1]; i++ {
                 first[head] += first[product[i]] - epsilon
             }
             if i == len(product) - 1 and epsilon in first[product[i]] {
                 first[head] += epsilon
             }
             first[head] += first
         }
     }
}
```

### 2. 计算follow集合

定义：*For a nonterminal a, FOLLOW(a) contains the set of words that can occur immediately after a in a sentence.*

+ fixed-point算法
+ 定义域只包含*nonterminal*且是first集合包含epsilon的nonterminal
+ follow集合的计算满足两条规则
    1. 若有`a->b1b2b3`则follow(b2)一定包含first(b3)，对于bn则一定包含follow(a)
    2. 若first(bi)包含epsilon，则follow(bi-1)一定包含first(bi+1)
+ follow集合没有必要包含epsilon

```
for each non-terminal i {
    follow[i] = empty
}
follow(S) = {eof}
while (follow sets变化) {
    for each production {
        RECORD = follow[head]
        for i = len(product) - 1 to 0 {
            follow[product[i]] += RECORD
            if (epsilon in first[product[i]]) {
                RECORD += first[product[i]] - epsilon
            } else {
                RECORD = first[product[i]]
            }
        }
    }
}
```

### 3. LL(1) parse table 生成

+ first集合和follow集合合并作为检测first+集合
+ 遍历所有non-terminal，对每一种terminal判断是否在first+集合中

```
#define ERROR -1
for each non-terminal i {
    for each terminal j except for epsilon {
        if j in fisrt+[i] {
            table[i][j] = i
        }
    }
}

for other item in talbe {
    item = ERROR
}
```

### TODO: 4. 错误处理

+ panic mode

### 合理性

+ 集合的实现这里复用了哈希表，即哈希表同时用作hashset和hashmap。对于hashset，其键和值相等。

## 输出结构

需要生成的东西。

+ production的持久化：三维数组
+ parse table：二维数组
