# CS2612-FinalProject-LexicalAnalyzer
The final project for SJTU C2612: Programming Languages and Compilers

## 编程作业

词法分析器，根据正则表达式生成 DFA

+ 将输入的正则表达式转化为“简化正则表达式”
+ 将每一个“简化正则表达式”转化为 NFA
+ 将一组 NFA 转化为一个 DFA，使得能依据“匹配字符串长度第一优先”、“匹配规则先后次序第二优先”进行此法分析
+ 依据 DFA 将输入的字符串转化为分段结果和分类结果。

其中，输入的正则表达式中可能包含的语法结构有：字符的集合（如 [a-z0-9] ）、可选的情形（即 ?r）、多种循环（即 r* 与 r+）、字符串（如 "ab\n" ）、单字符、并集与连接；“简化正则表达式”中可能包含的语法结构有字符的集合、空字符串、星号表示的循环（即 r*）、并集与连接。本任务中，用于存储正则表达式、 NFA 与 DFA 的数据结构已经在 lang.h 中提供了（详见regexp_NFA_DFA.zip）


## 编译和运行项目

```shell
# 在项目根目录下
make clean
make MEMORYCHECK=False TEST=True
```

其中两个编译编译选项： `MEMORYCHECK=True` 时启用 `ddressSanitizer` (`-lasan`) 和 `UndefinedBehaviorSanitizer` (`-lubsan`) 用于检查运行时的内存错误和未定义行为。注意，在 MinGW-w64 GCC 上不包含这些工具，为使用这两个 Sanitizer 要在 Linux 上编译（或使用 Clang 编译器（未测试））。正常非开发环境下，该编译选项默认关闭，可以在 MinGW-w64 GCC 上正常编译。

`TEST=True` 启用时，在 `main.c` 的程序入口处将会编译执行 `test()` 函数，从指定的 `/test/final_test/` 目录下读取所有 `*.in` 文件（测试样例），并对照 `*.out` 检查正确性。`/test/final_test/` 目录下为我们制造的所有测试样例，具体内容参考后面的 ***测试*** 一节。当 `TEST=False` 禁用时，将会编译执行 `demo()` 过程，逐条接收用户输入的正则表达式，打印其转化的表达式树和简化表达式树，然后再接收用户输入的字符串，打印该字符串在 DFA 上的行走过程，输出匹配结果。


## 如何使用接口

1. 输入正则表达式树 `struct frontend_regexp *tree`，通过 `simplify_regexp(tree)` 返回简化的正则表达式树 `struct simpl_regexp *simplified`
2. 输入简化的正则表达式树 `struct simpl_regexp *simplified` 和一个 NFA 空图 `struct finite_automata *nfa`，通过 `regexp_to_NFA(nfa, simplified)` 将简化正则表达式树转化的 NFA 存储在 `nfa` 中
3. 输入某个正则表达式对应的 NFA `struct finite_automata *nfa`，通过 `nfa_to_dfa(nfa)` 返回转化的 DFA 图（DFA 我们存储于一个新结构 `struct D_finite_automata` 中，它比 NFA 的结构多了一个“终止状态”的信息）
4. 输入某个正则表达式 DFA 和待匹配的字符串 `char str[]`，通过 `dfa_accepts_string(dfa, str)` 返回布尔值的匹配结果

```c
/******* 示例 *******/
char *regex = "a|b|ab";
char *str = "ab";
// 解析正则表达式为树
struct frontend_regexp *tree = parse_regexp(str);
// 简化正则表达式树
struct simpl_regexp *simpl_tree = simplify_regexp(tree);
// 创建空 NFA 图然后由简化正则表达式树转化 NFA
struct finite_automata *nfa = create_empty_NFA();
regexp_to_NFA(nfa, simpl_tree);
// NFA 转 DFA
struct D_finite_automata *dfa = nfa_to_dfa(nfa);
// 匹配结果
bool result = dfa_accepts_string(dfa, str);
```

## 测试

### 测试工具函数

1. 正则表达式转表达式树
```c
// In [lang.c]
struct frontend_regexp *parse_regex(char *ori_str, int len);
```

`parse_regex(regex, len)` 函数接收一个字符串类型的原始正则表达式和长度，返回对应的表达式树 `struct frontend_regexp *`。

例如 `ab|c, 4`，返回的表达式树结构为：
```
UNION:
  STRING: ab
  SINGLE_CHAR: c
```

例如 `([0123]+[abcd]+)*z, 18`，返回的表达式树结构为：
```
CONCAT:
  STAR:
    CONCAT:
      PLUS:
        CHAR_SET: 0123
      PLUS:
        CHAR_SET: abcd
  SINGLE_CHAR: z
```

由于该函数仅仅便于我们用正则表达式生成表达式树的测试样例，因此只要保证最终的树是我们想要的结构，不用保证最初的正则表达式符合“标准语法”。具体来说，我们在 `parse_regex` 的实现过程中为了简化正则表达式的扫描，去除了很多正则表达式应该有的特性（如 `\d` `[a-z]` 等简写符号不支持，需要写全 `[0123456789]`），在某些情况下也可能与标准语法的优先级有所不同（在我们的实现中，`|` 的优先级最高，在保证 `()` 都匹配的情况下，优先拆分所有的 `|`），同时也不支持转义字符（例如你需要匹配若干个 `*`，需要使用 `[*]*`）。等等这一切特性都源于我们对扫描正则表达式的简化实现，


2. 可视化表达式树
```c
// In [main.c]
void printTree(struct frontend_regexp *node, int level);
void printSimplifiedTree(struct simpl_regexp *node, int level);
```

这两个函数用来打印表达式树和简化后的表达式树，其效果正如上面所示，`level` 参数用来控制缩进级数

3. 文件测试
```c
// In [main.c]
void test_single_file(const char *test_in_name, const char *test_out_name);
```

接收两个文件名（在我们的测试中就是 `xxx.in` 和 `xxx.out`），将每次读取 `test_in_name` 中的两行，第一行作为正则表达式，第二行作为待匹配字符串，然后匹配结果和 `test_out_name` 中的结果进行比对

### 自动测试样例

自动测试样例共分为三种类型，均位于 `./test/final_test/` 目录下，分别为 `basic` `complex` `practical`，分别测试正则表达式匹配的基础语法、复杂组合语法、实际使用的有意义的正则表达式。

当编译选项采用调用 `test()` 时，将会遍历 `./test/final_test/` 目录下的所有测试样例，依次输出测试结果，没有通过的结果将会详细打印。

* `basic.in`：主要单独测试各个语法组件
* `complex.in`：正则表达式的复杂可以分为两种：一种是长字符串、分支较多的纯文本精确匹配；另一种是多层逻辑嵌套、字符逻辑关系复杂
* `practical.in`：主要测试了一些实际可能使用到的有现实意义的正则表达式匹配情景，比如：
  * [x] 序数词
  * [x] IP地址
  * [x] 邮箱
  * [x] HTML前后</>标签（特定语法结构）
  * [x] 大/小驼峰变量名
  * [x] 日期
  * [x] 文件名
  * [x] 程序声明变量的语句