# CS2612-FinalProject-LexicalAnalyzer
The final project for SJTU C2612: Programming Languages and Compilers

## 编程作业

词法分析器，根据正则表达式生成 DFA

+ 将输入的正则表达式转化为“简化正则表达式”
+ 将每一个“简化正则表达式”转化为 NFA
+ 将一组 NFA 转化为一个 DFA，使得能依据“匹配字符串长度第一优先”、“匹配规则先后次序第二优先”进行此法分析
+ 依据 DFA 将输入的字符串转化为分段结果和分类结果。

其中，输入的正则表达式中可能包含的语法结构有：字符的集合（如 [a-z0-9] ）、可选的情形（即 ?r）、多种循环（即 r* 与 r+）、字符串（如 "ab\n" ）、单字符、并集与连接；“简化正则表达式”中可能包含的语法结构有字符的集合、空字符串、星号表示的循环（即 r*）、并集与连接。本任务中，用于存储正则表达式、 NFA 与 DFA 的数据结构已经在 lang.h 中提供了（详见regexp_NFA_DFA.zip）


## 编译项目

```shell
# 在项目根目录下
make clean
make MEMORYCHECK=False TEST=True
```

其中两个编译编译选项： `MEMORYCHECK=True` 时启用 `ddressSanitizer` (`-lasan`) 和 `UndefinedBehaviorSanitizer` (`-lubsan`) 用于检查运行时的内存错误和未定义行为。注意，在 MinGW-w64 GCC 上不包含这些工具，为使用这两个 Sanitizer 要在 Linux 上编译（或使用 Clang 编译器（未测试））。正常非开发环境下，该编译选项默认关闭，可以在 MinGW-w64 GCC 上正常编译。

`TEST=True` 启用时，在 `main.c` 的程序入口处将会编译执行 `test()` 函数，从指定的 `/test/final_test/` 目录下读取所有 `*.in` 文件（测试样例），并对照 `*.out` 检查正确性。`/test/final_test/` 目录下为我们制造的所有测试样例，具体内容参考后面的 <u>测试</u> 一节。当 `TEST=False` 禁用时，将会编译执行 `demo()` 过程，逐条接收用户输入的正则表达式，打印其转化的表达式树和简化表达式树，然后再接收用户输入的字符串，打印该字符串在 DFA 上的行走过程，输出匹配结果。


## 测试

自动测试样例共分为三部分，位于 `./test/final_test/` 目录下，分别为 `basic` `complex` `practical`，测试正则表达式匹配的基础语法、复杂组合语法、实际使用的有意义的正则表达式。

`main()` 函数中调用 `test()` 将会遍历 `./test/final_test/` 目录下的所有测试样例，依次输出测试结果，没有通过的结果将会详细打印。

* `basic.in`：主要单独测试各个语法组件
* `complex.in`：分为两种复杂，一种是长字符串、分支较多的纯文本精确匹配，另一种是多层逻辑嵌套、字符关系复杂的正则表达式
* `practical.in`：测试了一些实际可能使用的正则表达式匹配情景，有：
  * [x] 序数词
  * [x] IP地址
  * [x] 邮箱
  * [x] HTML前后</>标签（特定语法结构）
  * [x] 大/小驼峰变量名
  * [x] 日期
  * [x] 文件名
  * [x] 定义变量的完整语句