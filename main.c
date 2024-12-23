#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include "lang.h"
#include <ctype.h>
#include <dirent.h>

// 读入一个字符串
void readString(char *str, int size, int *length)
{
    fgets(str, size, stdin);
    str[strcspn(str, "\n")] = '\0';
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



void demo() {
    char regex[500];
    char input[500];
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
    }
    memset(regex, 0, sizeof(regex));
    memset(input, 0, sizeof(input));
}


void test_single_file(const char *test_in_name, const char *test_out_name) {
    printf("[Test file %s]\n", test_in_name);
    FILE *test_in = fopen(test_in_name, "r");
    FILE *test_out = fopen(test_out_name, "r");
    if (!test_in || !test_out) {
        printf("Error opening test files.\n");
        return;
    }

    char regex[500];
    char input[500];
    char expected_output[10];
    
    int case_number = 1;

    while (fgets(regex, sizeof(regex), test_in)) {
        regex[strcspn(regex, "\n")] = '\0';
        if (!fgets(input, sizeof(input), test_in)) {
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if (!fgets(expected_output, sizeof(expected_output), test_out)) {
            break;
        }
        expected_output[strcspn(expected_output, "\n")] = '\0';  // 去除换行符

        //====================== 测试主体 ======================//
        struct frontend_regexp *tree = parse_regex(regex, strlen(regex));
        struct simpl_regexp *simplified = simplify_regexp(tree);

        struct finite_automata *nfa = create_empty_graph();
        regexp_to_NFA(nfa, simplified);
        struct D_finite_automata *dfa = nfa_to_dfa(nfa);

        bool accept = dfa_accepts_string(dfa, input);
        bool ground_truth = strcmp(expected_output, "true") == 0 ? true : false;
        printf("%d\t| %s\n", case_number++, (accept==ground_truth) ? "Pass" : "### Fail ###");
        if (accept != ground_truth) {
            printf("[Regex] %s\n[Input]\t\t%s\n", regex, input);
            printf("[Expected]\t%s\n", expected_output);
            printf("[Output]\t%s\n", accept ? "true" : "false");
        }
        //====================================================//
    }
    memset(regex, 0, sizeof(regex));
    memset(input, 0, sizeof(input));
    fclose(test_in);
    fclose(test_out);
}

void test() {
    const char *folder = "./test/final_test/";
    struct dirent *entry;
    DIR *dp = opendir(folder);

    if (dp == NULL) {
        printf("Error opening directory %s\n", folder);
        return;
    }
    printf("[Start testing in %s]\n", folder);
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] != '.') // 忽略隐藏文件和当前目录、父目录
        {
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".in") == 0)
            {
                char input_file[256];
                char output_file[256];

                snprintf(input_file, sizeof(input_file), "%s%s", folder, entry->d_name);
                snprintf(output_file, sizeof(output_file), "%s%s", folder, entry->d_name);
                output_file[strlen(output_file) - 3] = '\0'; // Remove ".in"
                strcat(output_file, ".out");

                test_single_file(input_file, output_file);
            }
        }
    }
}

int main()
{
#ifdef TEST_MODE
    printf("[[[[ This is test mode ]]]]\n");
    printf("[[[[ Read samples from file ]]]]\n");
    test();
#else
    printf("[[[[ This is demo mode ]]]]\n");
    printf("[[[[ Input a regex first, then input a string to match. ]]]]\n");
    demo();
#endif
    return 0;
}