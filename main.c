#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include "lang.h"

int main() {
    // 假设DFA已经被构建并初始化
    struct finite_automata *dfa = create_empty_graph();
    
    // 假设DFA的接受状态已设置
    int accepting_states[] = {1, 3}; // 假设状态1和状态3是接受状态
    dfa->accepting = accepting_states;

    // 假设已为DFA添加转移
    // 例如：从状态0，通过字符'a'转移到状态1
    // dfa->src[0] = 0, dfa->dst[0] = 1, dfa->lb[0] = 'a'

    // 示例：使用DFA来匹配一个字符串
    const char *test_string = "ab";
    
    // 调用匹配函数
    if (match_string_with_dfa(dfa, test_string)) {
        printf("匹配成功！\n");
    } else {
        printf("匹配失败！\n");
    }

    return 0;
}