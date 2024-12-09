#include "lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define END_OF(c) ((c) == '(' ? ')' : (c) == '[' ? ']' \
                                  : (c) == '{'   ? '}' \
                                                 : '\0')
#define BEGIN_OF(c) ((c) == ')' ? '(' : (c) == ']' ? '[' \
                                    : (c) == '}'   ? '{' \
                                                   : '\0')

void copy_char_set(struct char_set *dst, struct char_set *src, size_t n)
{
    dst->n = src->n;
    strncpy(dst->c, src->c, n);
    dst->c[n] = '\0';
}

struct frontend_regexp *TFr_CharSet(struct char_set *c)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_CHAR_SET;
    fr->d.CHAR_SET = *c;
    return fr;
}

struct frontend_regexp *TFr_Option(struct frontend_regexp *r)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_OPTIONAL;
    fr->d.OPTION.r = r;
    return fr;
}

struct frontend_regexp *TFr_Star(struct frontend_regexp *r)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_STAR;
    fr->d.STAR.r = r;
    return fr;
}

struct frontend_regexp *TFr_Plus(struct frontend_regexp *r)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_PLUS;
    fr->d.PLUS.r = r;
    return fr;
}

struct frontend_regexp *TFr_String(char *s)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_STRING;
    fr->d.STRING.s = s;
    return fr;
}

struct frontend_regexp *TFr_SingleChar(char c)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_SINGLE_CHAR;
    fr->d.SINGLE_CHAR.c = c;
    return fr;
}

struct frontend_regexp *TFr_Union(struct frontend_regexp *r1, struct frontend_regexp *r2)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_UNION;
    fr->d.UNION.r1 = r1;
    fr->d.UNION.r2 = r2;
    return fr;
}

struct frontend_regexp *TFr_Concat(struct frontend_regexp *r1, struct frontend_regexp *r2)
{
    struct frontend_regexp *fr;
    fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
    fr->t = T_FR_CONCAT;
    fr->d.CONCAT.r1 = r1;
    fr->d.CONCAT.r2 = r2;
    return fr;
}

struct simpl_regexp *TS_CharSet(struct char_set *c)
{
    struct simpl_regexp *sr;
    sr = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
    sr->t = T_S_CHAR_SET;
    sr->d.CHAR_SET = *c;
    return sr;
}

struct simpl_regexp *TS_Star(struct simpl_regexp *r)
{
    struct simpl_regexp *sr;
    sr = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
    sr->t = T_S_STAR;
    sr->d.STAR.r = r;
    return sr;
}

struct simpl_regexp *TS_EmptyStr()
{
    struct simpl_regexp *sr;
    sr = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
    sr->t = T_S_EMPTY_STR;
    return sr;
}

struct simpl_regexp *TS_Union(struct simpl_regexp *r1, struct simpl_regexp *r2)
{
    struct simpl_regexp *sr;
    sr = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
    sr->t = T_S_UNION;
    sr->d.UNION.r1 = r1;
    sr->d.UNION.r2 = r2;
    return sr;
}

struct simpl_regexp *TS_Concat(struct simpl_regexp *r1, struct simpl_regexp *r2)
{
    struct simpl_regexp *sr;
    sr = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
    sr->t = T_S_CONCAT;
    sr->d.CONCAT.r1 = r1;
    sr->d.CONCAT.r2 = r2;
    return sr;
}

struct finite_automata *create_empty_graph()
{
    struct finite_automata *fa;
    fa = (struct finite_automata *)malloc(sizeof(struct finite_automata));
    fa->n = 0;
    fa->m = 0;
    fa->array_size = 100;
    fa->src = (int *)malloc(fa->array_size * sizeof(int));
    fa->dst = (int *)malloc(fa->array_size * sizeof(int));
    fa->adj = (int *)malloc(fa->array_size * sizeof(int));
    fa->next = (int *)malloc(fa->array_size * sizeof(int));
    fa->lb = (struct char_set *)malloc(fa->array_size * sizeof(struct char_set));
    fa->accepting = (int *)malloc(fa->array_size * sizeof(int));
    return fa;
}

int add_one_vertex(struct finite_automata *g)
{
    int new_vertex_id = g->n;
    // printf("now adding vertex %d\n", g->n);
    g->n++;
    if (g->n >= g->array_size)
    {
        g->array_size *= 2;
        g->src = realloc(g->src, g->array_size * sizeof(int));
        g->dst = realloc(g->dst, g->array_size * sizeof(int));
        g->adj = realloc(g->adj, g->array_size * sizeof(int));
        g->next = realloc(g->next, g->array_size * sizeof(int));
        g->lb = realloc(g->lb, g->array_size * sizeof(struct char_set));
        g->accepting = realloc(g->next, g->array_size * sizeof(int));
    }
    g->adj[new_vertex_id] = -1;
    g->accepting[new_vertex_id] = 0;
    return new_vertex_id;
}

int add_one_edge(struct finite_automata *g, int src, int dst, struct char_set *c)
{
    int new_edge_id = g->m;
    g->m++;
    if (g->m >= g->array_size)
    {
        g->array_size *= 2;
        g->src = realloc(g->src, g->array_size * sizeof(int));
        g->dst = realloc(g->dst, g->array_size * sizeof(int));
        g->adj = realloc(g->adj, g->array_size * sizeof(int));
        g->next = realloc(g->next, g->array_size * sizeof(int));
        g->lb = realloc(g->lb, g->array_size * sizeof(struct char_set));
        g->accepting = realloc(g->next, g->array_size * sizeof(int));
    }
    g->src[new_edge_id] = src;
    g->dst[new_edge_id] = dst;
    g->lb[new_edge_id] = *c;
    g->next[new_edge_id] = -1;
    if (g->adj[src] == -1)
    {
        g->adj[src] = new_edge_id;
    }
    else
    {
        int e = g->adj[src];
        while (g->next[e] != -1)
            e = g->next[e];
        g->next[e] = new_edge_id;
    }
    return new_edge_id;
}

struct D_finite_automata *create_dfa_empty_graph()
{
    struct D_finite_automata *fa;
    fa = (struct D_finite_automata *)malloc(sizeof(struct D_finite_automata));
    fa->n = 0;
    fa->m = 0;
    fa->array_size = 100;
    fa->src = (int *)malloc(fa->array_size * sizeof(int));
    fa->dst = (int *)malloc(fa->array_size * sizeof(int));
    fa->adj = (int *)malloc(fa->array_size * sizeof(int));
    fa->next = (int *)malloc(fa->array_size * sizeof(int));
    fa->accepting = (int *)malloc(fa->array_size * sizeof(int));
    fa->lb = (struct char_set *)malloc(fa->array_size * sizeof(struct char_set));
    fa->nodes = (struct dfa_node *)malloc(fa->array_size * sizeof(struct dfa_node));

    return fa;
}
int add_one_vertex_to_dfa(struct D_finite_automata *g)
{
    int new_vertex_id = g->n;
    g->n++;
    if (g->n >= g->array_size)
    {
        g->array_size *= 2;
        g->src = realloc(g->src, g->array_size * sizeof(int));
        g->dst = realloc(g->dst, g->array_size * sizeof(int));
        g->adj = realloc(g->adj, g->array_size * sizeof(int));
        g->next = realloc(g->next, g->array_size * sizeof(int));
        g->accepting = realloc(g->next, g->array_size * sizeof(int));
        g->lb = realloc(g->lb, g->array_size * sizeof(struct char_set));
        g->nodes = realloc(g->nodes, g->array_size * sizeof(struct dfa_node));
    }
    g->adj[new_vertex_id] = -1;
    return new_vertex_id;
}

int add_one_edge_to_dfa(struct D_finite_automata *g, int src, int dst, struct char_set *c)
{
    int new_edge_id = g->m;
    g->m++;
    if (g->m >= g->array_size)
    {
        g->array_size *= 2;
        g->src = realloc(g->src, g->array_size * sizeof(int));
        g->dst = realloc(g->dst, g->array_size * sizeof(int));
        g->adj = realloc(g->adj, g->array_size * sizeof(int));
        g->next = realloc(g->next, g->array_size * sizeof(int));
        g->accepting = realloc(g->next, g->array_size * sizeof(int));
        g->lb = realloc(g->lb, g->array_size * sizeof(struct char_set));
        g->nodes = realloc(g->nodes, g->array_size * sizeof(struct dfa_node));
    }
    g->src[new_edge_id] = src;
    g->dst[new_edge_id] = dst;
    g->lb[new_edge_id] = *c;
    g->next[new_edge_id] = -1;
    if (g->adj[src] == -1)
    {
        g->adj[src] = new_edge_id;
    }
    else
    {
        int e = g->adj[src];
        while (g->next[e] != -1)
            e = g->next[e];
        g->next[e] = new_edge_id;
    }
    return new_edge_id;
}

// From nfa to dfa

/*
    input a state
    return the state after running all the possible episilon transitions
*/
int *epsilon_closure(struct finite_automata *nfa, int *states, int state_count)
{
    int *closure = (int *)malloc(nfa->n * sizeof(int));
    memset(closure, 0, nfa->n * sizeof(int));

    // 将初始状态加入闭包
    for (int i = 0; i < state_count; i++)
    {
        if (states[i] == 0)
            continue; // 跳过无效状态
        closure[i] = 1;
    }

    // 扩展闭包，处理所有通过epsilon转换的状态
    int changes;
    do
    {
        changes = 0;
        for (int i = 0; i < nfa->m; i++)
        {
            if (closure[nfa->src[i]] && nfa->lb[i].n == 0)
            { // epsilon transition
                int dst = nfa->dst[i];
                if (!closure[dst])
                {
                    closure[dst] = 1;
                    changes = 1;
                }
            }
        }
    } while (changes); // 如果有变化，继续扩展

    return closure;
}

// move：返回在给定状态集合下，对某字符的移动后的新状态集合
int *move(struct finite_automata *nfa, int *states, int state_count, char input)
{
    int *result = (int *)malloc(nfa->n * sizeof(int));
    memset(result, 0, nfa->n * sizeof(int));
    for (int i = 0; i < state_count; i++)
    {
        if (states[i] == 0)
            continue; // 跳过无效状态
        for (int e = nfa->adj[i]; e != -1; e = nfa->next[e])
        {
            if (strchr(nfa->lb[e].c, input) != NULL)
            { // Epsilon 或匹配字符
                // printf("%d\n",input);
                result[nfa->dst[e]] = 1;
            }
        }
    }
    // 计算新状态集合的epsilon闭包
    result = epsilon_closure(nfa, result, nfa->n);

    return result;
}

// 比较两个状态集合，判断它们是否相同
int compare_sets(int *setA, int *setB, int nfa_size)
{
    for (int i = 0; i < nfa_size; i++)
    {
        if (setA[i] != setB[i])
        {
            return 0; // 如果存在不同，返回0
        }
    }
    return 1; // 相同，返回1
}

// 从状态集合生成DFA状态ID
int generate_state_id(struct D_finite_automata *dfa, int *state_set, int state_count)
{
    for (int i = 0; i < dfa->n; i++)
    {
        if (compare_sets(dfa->nodes[i].state, state_set, dfa->nodes[i].length))
        {
            return dfa->nodes[i].id;
        }
    }

    // 如果没有找到，创建新状态并返回ID
    int new_id = add_one_vertex_to_dfa(dfa);
    dfa->nodes[new_id].state = state_set;
    dfa->nodes[new_id].length = state_count;
    dfa->nodes[new_id].id = new_id;

    return new_id;
}

// nfa_to_dfa：将NFA转换为DFA
struct D_finite_automata *nfa_to_dfa(struct finite_automata *nfa)
{
    struct D_finite_automata *dfa = create_dfa_empty_graph();

    int size = 100;
    // 使用一个数组来存储NFA状态集合，初始化为NFA的起始状态的epsilon闭包
    int *start_closure = (int *)malloc(nfa->n * sizeof(int));
    memset(start_closure, 0, nfa->n * sizeof(int));
    start_closure[0] = 1; // 假设NFA的起始状态是状态0，并加入到闭包

    // 计算起始状态的epsilon闭包
    start_closure = epsilon_closure(nfa, start_closure, 1);
    // print_state(start_closure, nfa->n);
    //  将起始状态的epsilon闭包加入DFA
    int start_state_id = generate_state_id(dfa, start_closure, nfa->n);

    // DFA状态队列，用于遍历DFA的每个状态
    int *state_queue = (int *)malloc(size * sizeof(int));
    int queue_head = 0, queue_tail = 0;

    // 初始状态加入队列
    state_queue[queue_tail++] = start_state_id;

    // 状态集合检查（用于判断状态是否已经被添加到DFA中）
    int **state_sets = (int **)malloc(size * sizeof(int *));
    int state_sets_size = 0;

    // 对DFA中的每个状态，遍历所有字符的转换
    while (queue_head < queue_tail)
    {
        int current_state_id = state_queue[queue_head++];
        // printf("%d",current_state_id);
        //  对当前DFA状态，检查每一个字符
        for (char c = 'a'; c <= 'z'; c++) // TODO: 从'a'到'z'遍历所有字符
        {
            int *new_state = move(nfa, dfa->nodes[current_state_id].state, dfa->nodes[current_state_id].length, c);
            // print_state(new_state, nfa->n);

            bool flag = false;
            for (int i = 0; i < nfa->n; i++)
            {
                if (new_state[i] == 1)
                {
                    flag = true;
                    break;
                }
            }
            if (!flag)
            {
                continue;
            }

            if (new_state != NULL)
            {
                // 判断新状态是否已经存在
                int new_state_id = generate_state_id(dfa, new_state, nfa->n);

                // 处理新的边
                int is_new_edge = 1;
                for (int i = 0; i < dfa->n; i++)
                {
                    for (int edge = dfa->adj[i]; edge != -1; edge = dfa->next[edge])
                    {
                        if (dfa->src[edge] == current_state_id && dfa->dst[edge] == new_state_id)
                        {
                            is_new_edge = 0;
                            new_state_id = edge; // 取已有边的ID
                            break;
                        }
                    }
                }

                if (is_new_edge)
                {
                    //printf("test2");
                    // TODO: if the size of state_sets is not enough, realloc it
                    if (state_sets_size >= size)
                    {
                        size *= 2;
                        state_sets = (int **)realloc(state_sets, size * sizeof(int *));
                        state_queue = (int *)realloc(state_queue, size * sizeof(int));
                    }
                    state_sets[state_sets_size++] = new_state;
                    state_queue[queue_tail++] = new_state_id;

                    // 为DFA添加边
                    struct char_set *cs = (struct char_set *)malloc(sizeof(struct char_set));
                    cs->n = 1;
                    cs->c = (char *)malloc(sizeof(char) + 1);
                    cs->c[0] = c; // 将字符c作为边标签
                    cs->c[1] = '\0';
                    add_one_edge_to_dfa(dfa, current_state_id, new_state_id, cs);
                }
                else
                {
                    if (strchr(dfa->lb[new_state_id].c,c) == NULL)
                    {
                        // 将字符添加到已有边的char_set中
                        size_t len = dfa->lb[new_state_id].n;
                        dfa->lb[new_state_id].c = realloc(dfa->lb[new_state_id].c, (len + 2) * sizeof(char));
                        dfa->lb[new_state_id].c[len] = c;
                        dfa->lb[new_state_id].c[len + 1] = '\0';
                        dfa->lb[new_state_id].n += 1;
                    }
                }
            }
        }
    }

    // 处理DFA的接受状态
    for (int i = 0; i < dfa->n; i++)
    {
        for (int j = 0; j < nfa->n; j++)
        {
            if (dfa->nodes[i].state[j] == 1 && nfa->accepting[j] == 1)
            {
                dfa->accepting[i] = 1;
                break;
            }
        }
    }
    return dfa;
}

void print_state(int *state, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (state[i] == 1)
        {
            printf("%d ", i);
        }
    }
    printf("\n");
}

void print_DFA(struct D_finite_automata *g)
{
    printf("Printed DFA:\n");
    int p = 0;

    while (p < g->n)
    {
        if (g->adj[p] == -1)
        {
            p++;
            continue;
        }
        int e = g->adj[p];
        printf("%d: ", e);
        print_state(g->nodes[e].state, g->nodes[e].length);
        printf("%d-%s->%d\n", p, g->lb[e].c, g->dst[e]);
        while (g->next[e] != -1)
        {
            e = g->next[e];
            printf("%d: ", e);
            print_state(g->nodes[e].state, g->nodes[e].length);
            printf("%d-%s->%d\n", p, g->lb[e].c, g->dst[e]);
        }
        e = g->dst[e];
        printf("%d: ", e);
        print_state(g->nodes[e].state, g->nodes[e].length);
        p++;
    }
    for (int i = 0; i <= p; i++)
        if (g->accepting[i] == 1)
            printf("The ending state is %d\n", i);
    printf("\n");
}

// run the dfa on the input string
bool dfa_accepts_string(struct D_finite_automata *dfa, const char *str)
{
    int current_state = 0; // 假设初始状态是0
    int i = 0;

    while (str[i] != '\0')
    {
        char current_char = str[i];
        bool transition_found = false;

        // 遍历当前状态的所有邻接边
        for (int edge = dfa->adj[current_state]; edge != -1; edge = dfa->next[edge])
        {
            // 获取边上的标签
            char *label = dfa->lb[edge].c;

            bool flag = false;

            for (int j = 0; j < dfa->lb[edge].n; j++)
            {
                if (label[j] == current_char)
                {
                    flag = true;
                    break;
                }
            }

            // 如果当前字符在标签集中，则进行转移
            if (flag)
            {
                current_state = dfa->dst[edge]; // 转移到目标状态
                transition_found = true;

                break;
            }
        }

        if (!transition_found)
        {
            return false; // 如果没有找到合法的转移，说明无法接受
        }

        i++; // 处理下一个字符
    }
    // 最后检查当前状态是否是接受状态
    if (dfa->accepting[current_state] == 1)
    {
        return true;
    }
    return false;
}

struct frontend_regexp *parse_regex(char *ori_str, int len)
{
    char *str = (char *)malloc(len + 1);
    strncpy(str, ori_str, len);
    str[len] = '\0';
    // printf("parse_regex: %d: %s\n", len, str);
    int r = 0;
    struct frontend_regexp *fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));

    if (len == 1)
    {
        return TFr_SingleChar(str[0]);
    }

    int small_bracket = 0;
    for (; r < len; ++r)
    {
        if (str[r] == '(')
        {
            small_bracket++;
        }
        else if (str[r] == ')')
        {
            small_bracket--;
        }
        else if (str[r] == '|' && small_bracket == 0)
        {
            fr->t = T_FR_UNION;
            fr->d.UNION.r1 = parse_regex(str, r);
            fr->d.UNION.r2 = parse_regex(str + r + 1, len - r - 1);
            return fr;
        }
    }
    // 最外围没有 | 的情况
    // 1. 第一个括号是()包围, 以及紧随着的 * + ? 三种情况
    if (str[0] == '(')
    {
        int p = 0;
        small_bracket = 0;
        while (p < len)
        { // 找到匹配的另一半括号
            if (str[p] == '(')
                small_bracket++;
            else if (str[p] == ')' && small_bracket == 1)
                break;
            else if (str[p] == ')')
                small_bracket--;
            p++;
        }
        if (p == len - 1)
        { // 整体就是一个括号 直接去除首尾
            return parse_regex(str + 1, len - 2);
        }
        else if (p == len - 2)
        { // 整体是一个括号 加 情况符
            if (str[len - 1] == '*')
            {
                fr->t = T_FR_STAR;
                fr->d.STAR.r = parse_regex(str + 1, len - 3);
            }
            else if (str[len - 1] == '+')
            {
                fr->t = T_FR_PLUS;
                fr->d.PLUS.r = parse_regex(str + 1, len - 3);
            }
            else if (str[len - 1] == '?')
            {
                fr->t = T_FR_OPTIONAL;
                fr->d.OPTION.r = parse_regex(str + 1, len - 3);
            }
            else
            { // 最后一位是单独字符
                struct frontend_regexp *other_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
                other_fr->t = T_FR_SINGLE_CHAR;
                other_fr->d.SINGLE_CHAR.c = str[len - 1];
                fr->t = T_FR_CONCAT;
                fr->d.CONCAT.r1 = parse_regex(str + 1, len - 3);
                fr->d.CONCAT.r2 = other_fr;
            }
            return fr;
        }
        else
        { // 不是最后的括号包围
            fr->t = T_FR_CONCAT;
            if (str[p + 1] == '*' || str[p + 1] == '+' || str[p + 1] == '?')
            {
                fr->d.CONCAT.r1 = parse_regex(str, p + 2);
                fr->d.CONCAT.r2 = parse_regex(str + p + 2, len - p - 2);
            }
            else
            { // 只有一对括号没有情况符 则去除括号
                fr->d.CONCAT.r1 = parse_regex(str + 1, p - 1);
                fr->d.CONCAT.r2 = parse_regex(str + p + 1, len - p - 1);
            }
            return fr;
        }
    }
    // 2. 第一个括号是[]包围, 以及紧随着的 * + ? 三种情况
    if (str[0] == '[')
    {
        int p = 0;
        int mid_bracket = 0;
        while (p < len)
        {
            if (str[p] == '[')
                mid_bracket++;
            else if (str[p] == ']' && mid_bracket == 1)
                break;
            else if (str[p] == ']')
                mid_bracket--;
            p++;
        }
        char *c = (char *)malloc(p);
        strncpy(c, str + 1, p);
        c[p - 1] = '\0';
        struct char_set *cs = (struct char_set *)malloc(sizeof(struct char_set));
        cs->n = p - 1;
        cs->c = c;
        // printf("Input regex: \"%s\"\n", c);
        if (p == len - 1)
        { // 整体就是一个括号 直接去除首尾
            fr->t = T_FR_CHAR_SET;
            fr->d.CHAR_SET = *cs;
            return fr;
        }
        else if (p == len - 2)
        { // 整体是一个括号 加 特殊符/单字符
            if (str[len - 1] == '*')
            {
                fr->t = T_FR_STAR;
                fr->d.STAR.r = TFr_CharSet(cs);
            }
            else if (str[len - 1] == '+')
            {
                fr->t = T_FR_PLUS;
                fr->d.PLUS.r = TFr_CharSet(cs);
            }
            else if (str[len - 1] == '?')
            {
                fr->t = T_FR_OPTIONAL;
                fr->d.OPTION.r = TFr_CharSet(cs);
            }
            else
            { // 最后一位是单独字符
                struct frontend_regexp *other_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
                other_fr->t = T_FR_SINGLE_CHAR;
                other_fr->d.SINGLE_CHAR.c = str[len - 1];
                fr->t = T_FR_CONCAT;
                fr->d.CONCAT.r1 = TFr_CharSet(cs);
                fr->d.CONCAT.r2 = other_fr;
            }
            return fr;
        }
        else
        { // 不是最后的括号包围
            fr->t = T_FR_CONCAT;
            // printf("charset in []: %s\n", c);
            struct frontend_regexp *other_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
            if (str[p + 1] == '*')
            {
                other_fr->t = T_FR_STAR;
                other_fr->d.STAR.r = TFr_CharSet(cs);
                fr->d.CONCAT.r1 = other_fr;
                fr->d.CONCAT.r2 = parse_regex(str + p + 2, len - p - 2);
            }
            else if (str[p + 1] == '+')
            {
                other_fr->t = T_FR_PLUS;
                other_fr->d.PLUS.r = TFr_CharSet(cs);
                fr->d.CONCAT.r1 = other_fr;
                fr->d.CONCAT.r2 = parse_regex(str + p + 2, len - p - 2);
            }
            else if (str[p + 1] == '?')
            {
                other_fr->t = T_FR_OPTIONAL;
                other_fr->d.OPTION.r = TFr_CharSet(cs);
                fr->d.CONCAT.r1 = other_fr;
                fr->d.CONCAT.r2 = parse_regex(str + p + 2, len - p - 2);
            }
            else
            {
                fr->d.CONCAT.r1 = TFr_CharSet(cs);
                fr->d.CONCAT.r2 = parse_regex(str + p + 1, len - p - 1);
            }
            return fr;
        }
    }
    // 3. 第一个是字符, 则直到括号出现之前都视为string
    if (str[0] != '(' && str[0] != '[')
    {
        int p = 0;
        while (p < len && str[p] != '(' && str[p] != '[' && str[p] != '+' && str[p] != '*' && str[p] != '?')
            ++p;
        if (p == len)
        { // 整体是一个字符串
            fr->t = T_FR_STRING;
            fr->d.STRING.s = str;
        }
        else if (str[p] == '[' || str[p] == '(')
        {
            char *s = (char *)malloc(p + 1);
            strncpy(s, str, p);
            s[p] = '\0';
            struct frontend_regexp *other_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
            other_fr->t = T_FR_STRING;
            other_fr->d.STRING.s = s;
            fr->t = T_FR_CONCAT;
            fr->d.CONCAT.r1 = other_fr;
            fr->d.CONCAT.r2 = parse_regex(str + p, len - p);
        }
        else
        {                                                                                                         // + * ? 针对单字符的情况
            struct frontend_regexp *string_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp)); // 记录之前的string
            char *s = (char *)malloc(p);
            strncpy(s, str, p - 1);
            s[p - 1] = '\0';
            string_fr->t = T_FR_STRING;
            string_fr->d.STRING.s = s;
            struct frontend_regexp *single_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp)); // 记录最后单个有特殊情况的字符
            single_fr->t = T_FR_SINGLE_CHAR;
            single_fr->d.SINGLE_CHAR.c = str[p - 1];
            struct frontend_regexp *concat_fr_1 = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp)); // 用于连接前面两部分
            concat_fr_1->t = T_FR_CONCAT;
            concat_fr_1->d.CONCAT.r1 = string_fr;
            if (str[p] == '*')
            {
                struct frontend_regexp *star_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
                star_fr->t = T_FR_STAR;
                star_fr->d.STAR.r = single_fr;
                concat_fr_1->d.CONCAT.r2 = star_fr;
            }
            else if (str[p] == '+')
            {
                struct frontend_regexp *plus_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
                plus_fr->t = T_FR_PLUS;
                plus_fr->d.PLUS.r = single_fr;
                concat_fr_1->d.CONCAT.r2 = plus_fr;
            }
            else if (str[p] == '?')
            {
                struct frontend_regexp *option_fr = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
                option_fr->t = T_FR_OPTIONAL;
                option_fr->d.OPTION.r = single_fr;
                concat_fr_1->d.CONCAT.r2 = option_fr;
            }
            fr->t = T_FR_CONCAT;
            fr->d.CONCAT.r1 = concat_fr_1;
            fr->d.CONCAT.r2 = parse_regex(str + p + 1, len - p - 1);
        }
        return fr;
    }
    return fr;
}