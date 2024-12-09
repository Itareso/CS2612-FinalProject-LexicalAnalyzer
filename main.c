#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include "lang.h"
#include <ctype.h>

// 读入一个字符串
void readString(char *str, int size, int *length)
{
    fgets(str, size, stdin);

    // Remove the newline character if present
    str[strcspn(str, "\n")] = '\0';

    // Calculate the length of the string
    *length = strlen(str);
}

void free_finite_automata(struct finite_automata *fa)
{
    if (fa == NULL) return;
    free(fa->src);
    free(fa->dst);
    if (fa->lb) {
        free(fa->lb->c);
        free(fa->lb);
    }
    free(fa->adj);
    free(fa->next);
    free(fa->accepting);
    fa = NULL;
}

void free_D_finite_automata(struct D_finite_automata *dfa) {
    if (dfa == NULL) return;
    free(dfa->src);
    free(dfa->dst);
    free(dfa->adj);
    free(dfa->next);
    free(dfa->accepting);
    if (dfa->lb) {
        free(dfa->lb->c);
        free(dfa->lb);
    }
    if (dfa->nodes) {
        free(dfa->nodes->state);
        free(dfa->nodes);
    }
    dfa = NULL;
}


void free_frontend_regexp(struct frontend_regexp *regexp) {
    if (regexp == NULL) return;

    switch (regexp->t) {
        case T_FR_OPTIONAL:
            free_frontend_regexp(regexp->d.OPTION.r);
            break;

        case T_FR_STAR:
            free_frontend_regexp(regexp->d.STAR.r);
            break;

        case T_FR_PLUS:
            free_frontend_regexp(regexp->d.PLUS.r);
            break;

        case T_FR_STRING:
            free(regexp->d.STRING.s);
            break;

        case T_FR_SINGLE_CHAR:
            break;

        case T_FR_UNION:
            free_frontend_regexp(regexp->d.UNION.r1);
            free_frontend_regexp(regexp->d.UNION.r2);
            break;

        case T_FR_CONCAT:
            free_frontend_regexp(regexp->d.CONCAT.r1);
            free_frontend_regexp(regexp->d.CONCAT.r2);
            break;

        case T_FR_CHAR_SET:
            break;
    }

    regexp = NULL;
}

void free_simpl_regexp(struct simpl_regexp *regexp) {
    if (regexp == NULL) return;
    switch (regexp->t) {
        case T_S_CHAR_SET:
            if (regexp->d.CHAR_SET.c) free(regexp->d.CHAR_SET.c);
            break;
        case T_S_STAR:
            if (regexp->d.STAR.r) free_simpl_regexp(regexp->d.STAR.r);
            break;

        case T_S_UNION:
            if (regexp->d.UNION.r1) free_simpl_regexp(regexp->d.UNION.r1);
            if (regexp->d.UNION.r2) free_simpl_regexp(regexp->d.UNION.r2);
            break;

        case T_S_CONCAT:
            if (regexp->d.CONCAT.r1) free_simpl_regexp(regexp->d.CONCAT.r1);
            if (regexp->d.CONCAT.r2) free_simpl_regexp(regexp->d.CONCAT.r2);
            break;
        case T_S_EMPTY_STR:
            break;
    }
    regexp = NULL;
}

// 打印辅助函数：打印字符串中的特殊字符（比如控制符）
void print_string(const char *str)
{
    while (*str)
    {
        if (*str == '\n')
        {
            printf("\\n");
        }
        else if (*str == '\t')
        {
            printf("\\t");
        }
        else
        {
            putchar(*str);
        }
        str++;
    }
}

// 打印字符集
void print_char_set(struct char_set *cs)
{
    if (cs->n == 0)
    {
        printf("Epsilon");
    }
    else
    {
        print_string(cs->c);
    }
}

// 打印正则表达式树
void printTree(struct frontend_regexp *node, int level)
{
    // 打印缩进
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }

    if (!node)
    {
        printf("NULL\n");
        return;
    }

    switch (node->t)
    {
    case T_FR_CHAR_SET:
        printf("CHAR_SET: ");
        print_char_set(&node->d.CHAR_SET);
        printf("\n");
        break;

    case T_FR_OPTIONAL:
        printf("OPTIONAL:\n");
        printTree(node->d.OPTION.r, level + 1);
        break;

    case T_FR_STAR:
        printf("STAR:\n");
        printTree(node->d.STAR.r, level + 1);
        break;

    case T_FR_PLUS:
        printf("PLUS:\n");
        printTree(node->d.PLUS.r, level + 1);
        break;

    case T_FR_STRING:
        printf("STRING: %s\n", node->d.STRING.s);
        break;

    case T_FR_SINGLE_CHAR:
        printf("SINGLE_CHAR: '%c'\n", node->d.SINGLE_CHAR.c);
        break;

    case T_FR_UNION:
        printf("UNION:\n");
        printTree(node->d.UNION.r1, level + 1);
        printTree(node->d.UNION.r2, level + 1);
        break;

    case T_FR_CONCAT:
        printf("CONCAT:\n");
        printTree(node->d.CONCAT.r1, level + 1);
        printTree(node->d.CONCAT.r2, level + 1);
        break;

    default:
        printf("Unknown type\n");
        break;
    }
}

// 打印简化正则表达式树
void printSimplifiedTree(struct simpl_regexp *node, int level)
{
    // 打印缩进
    for (int i = 0; i < level; i++)
    {
        printf("  ");
    }

    if (!node)
    {
        printf("NULL\n");
        return;
    }

    switch (node->t)
    {
    case T_S_CHAR_SET:
        printf("CHAR_SET: ");
        print_char_set(&node->d.CHAR_SET);
        printf("\n");
        break;

    case T_S_STAR:
        printf("STAR:\n");
        printSimplifiedTree(node->d.STAR.r, level + 1);
        break;

    case T_S_EMPTY_STR:
        printf("EMPTY_STR\n");
        break;

    case T_S_UNION:
        printf("UNION:\n");
        printSimplifiedTree(node->d.UNION.r1, level + 1);
        printSimplifiedTree(node->d.UNION.r2, level + 1);
        break;

    case T_S_CONCAT:
        printf("CONCAT:\n");
        printSimplifiedTree(node->d.CONCAT.r1, level + 1);
        printSimplifiedTree(node->d.CONCAT.r2, level + 1);
        break;

    default:
        printf("Unknown type\n");
        break;
    }
}

char regex[100];
char input[100];

void demo() {
    int len;
    struct finite_automata *nfa = NULL;
    struct D_finite_automata *dfa = NULL;
    struct frontend_regexp *tree = NULL;
    struct simpl_regexp *simplified = NULL;
    while (true) {
        printf(">>> ");
        memset(regex, 0, sizeof(regex));
        int len = 0;
        readString(regex, sizeof(regex), &len);

        if (len == 0)
            break;
        // printf("Input regex: \"%s\"\n", regex);
        // printf("length = %d\n", len);
        tree = parse_regex(regex, len);
        printf("[Regex Tree]\n");
        printTree(tree, 0);
        simplified = simplify_regexp(tree);
        printf("[Simplified Regex Tree]\n");
        printSimplifiedTree(simplified, 0);

        // 对指定字符串进行匹配
        printf("(string to match) >>> ");
        memset(input, 0, sizeof(input));
        int input_len = 0;
        readString(input, sizeof(input), &input_len);

        nfa = create_empty_graph();

        regexp_to_NFA(nfa, simplified);
        print_NFA(nfa);

        dfa = nfa_to_dfa(nfa);
        //printf("dfa->n: %d\n",dfa->n);
        print_DFA(dfa);
        bool accept = dfa_accepts_string(dfa, input);
        printf("[Accept]: %s\n", accept ? "true" : "false");
        // free_frontend_regexp(tree);
        // free_simpl_regexp(simplified);
        // free_finite_automata(nfa);
        // free_D_finite_automata(dfa);
    }
    memset(regex, 0, sizeof(regex));
    memset(input, 0, sizeof(input));
    free_frontend_regexp(tree);
    free_simpl_regexp(simplified);
    free_finite_automata(nfa);
    free_D_finite_automata(dfa);

}



void test() {
    struct test_case {
        char r[100];
        char in[100];
        bool expected_accept;
    } test_cases[] = {
        {"ab|cd", "cd", true},
        {"((def )[_abcdefghijklmnopqrstuvwxyz0123]+)?", "def func1", true},
        {"((def )[_abcdefghijklmnopqrstuvwxyz0123]+)?", "def _func2", true},
        {"((def )[_abcdefghijklmnopqrstuvwxyz0123]+)?", "", true},
        {"((def )[_abcdefghijklmnopqrstuvwxyz0123]+)?", "deffunc1", false},
        {"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "cdf", false}
    };

    int num_cases = sizeof(test_cases) / sizeof(test_cases[0]);
    
    // 逐个执行测试
    for (int i = 0; i < num_cases; i++) {
        printf("Test Case %d: Regex = %s, Input = %s\n", i+1, test_cases[i].r, test_cases[i].in);

        // 模拟demo函数中的操作
        memset(regex, 0, sizeof(regex));
        strncpy(regex, test_cases[i].r, sizeof(regex)-1);

        struct frontend_regexp *tree = parse_regex(regex, strlen(regex));
        struct simpl_regexp *simplified = simplify_regexp(tree);

        memset(input, 0, sizeof(input));
        strncpy(input, test_cases[i].in, sizeof(input)-1);

        struct finite_automata *nfa = create_empty_graph();
        regexp_to_NFA(nfa, simplified);
        struct D_finite_automata *dfa = nfa_to_dfa(nfa);

        bool accept = dfa_accepts_string(dfa, input);
        printf("[Expected Accept]: %s\n", test_cases[i].expected_accept ? "true" : "false");
        printf("[Actual Accept]: %s\n", accept ? "true" : "false");
        
        // 比对结果
        if (accept == test_cases[i].expected_accept) {
            printf("Test Case %d Passed.\n\n", i+1);
        } else {
            printf("Test Case %d Failed.\n\n", i+1);
        }
    }
}

int main()
{
    // demo();
    test();
    return 0;
}