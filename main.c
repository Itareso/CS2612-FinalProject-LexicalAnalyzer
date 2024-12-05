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

int main()
{
    char *regex;
    int len;

    while (true)
    {
        printf(">>> ");
        char regex[100];
        int len = 0;
        readString(regex, sizeof(regex), &len);

        if (len == 0)
            break;
        // printf("Input regex: \"%s\"\n", regex);

        struct frontend_regexp *tree = parse_regex(regex, len);
        printf("[Regex Tree]\n");
        printTree(tree, 0);
        struct simpl_regexp *simplified = simplify_regexp(tree);
        printf("[Simplified Regex Tree]\n");
        printSimplifiedTree(simplified, 0);

        // 对指定字符串进行匹配
        printf("(string to match) >>> ");
        char input[100];
        int input_len = 0;
        readString(input, sizeof(input), &input_len);

        struct finite_automata *nfa = create_empty_graph();

        regexp_to_NFA(nfa, simplified);
        print_NFA(nfa);

        struct D_finite_automata *dfa = nfa_to_dfa(nfa);
        print_DFA(dfa);
        bool accept = dfa_accepts_string(dfa, input);
        printf("[Accept]: %s\n", accept ? "true" : "false");
    }
    return 0;
}