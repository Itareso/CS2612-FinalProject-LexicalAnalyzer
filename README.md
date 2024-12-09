# CS2612-FinalProject-LexicalAnalyzer
The final project for SJTU C2612: Programming Languages and Compilers

## 编程作业

词法分析器，根据正则表达式生成 DFA

+ 将输入的正则表达式转化为“简化正则表达式”
+ 将每一个“简化正则表达式”转化为 NFA
+ 将一组 NFA 转化为一个 DFA，使得能依据“匹配字符串长度第一优先”、“匹配规则先后次序第二优先”进行此法分析
+ 依据 DFA 将输入的字符串转化为分段结果和分类结果。

其中，输入的正则表达式中可能包含的语法结构有：字符的集合（如 [a-z0-9] ）、可选的情形（即 ?r）、多种循环（即 r* 与 r+）、字符串（如 "ab\n" ）、单字符、并集与连接；“简化正则表达式”中可能包含的语法结构有字符的集合、空字符串、星号表示的循环（即 r*）、并集与连接。本任务中，用于存储正则表达式、 NFA 与 DFA 的数据结构已经在 lang.h 中提供了（详见regexp_NFA_DFA.zip）

