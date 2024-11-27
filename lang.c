#include "lang.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void copy_char_set(struct char_set *dst, struct char_set *src)
{
    dst->n = src->n;
    strcpy(dst->c, src->c);
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
    return fa;
}

int add_one_vertex(struct finite_automata *g)
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
        g->lb = realloc(g->lb, g->array_size * sizeof(struct char_set));
    }
    g->adj[new_vertex_id] = -1;
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
int *epsilon_closure(struct finite_automata *nfa, int state_number, int *states) {
    
    int *closure = (int *)malloc(nfa->n * sizeof(int));
    memset(closure, 0, nfa->n * sizeof(int));

    // 将初始状态加入ε-闭包
    for (int i = 0; i < state_number; i++) {
        if (states[i] == 0) continue;  // 跳过无效状态
        closure[i] = 1;
    }

    // 扩展ε-闭包，处理所有通过epsilon转换的状态
    int changes;
    do {
        changes = 0;
        for (int i = 0; i < nfa->m; i++) {
            if (closure[nfa->src[i]]) { // source is in closure
                if (nfa->lb[i].n == 0) { // epsilon transition
                    int dst = nfa->dst[i];
                    if (!closure[dst]) {
                        closure[dst] = 1;
                        changes = 1;
                    }
                }
            }
        }
    } while (changes); // 如果有变化，继续扩展

    return closure;
}

// return the states after running a single input char, also considering the epsilon transitions
int *move(struct finite_automata *nfa, int *states, int state_number, char input)
{
    int *result = (int *)malloc(nfa->n * sizeof(int));
    memset(result, 0, nfa->n * sizeof(int));
    for (int i = 0; i < state_number; i++) {
        if (states[i] == 0) continue;  // 跳过无效状态
        for (int e = nfa->adj[i]; e != -1; e = nfa->next[e]) {
            if (nfa->lb[e].n == 0 || strchr(nfa->lb[e].c, input) != NULL) { // Epsilon or matching char
                result[nfa->dst[e]] = 1;
            }
        }
    }
    result = epsilon_closure(nfa, state_number, result);
    return result;
}

// compare two sets of states
int compare_sets(int *setA, int *setB, int nfa_size) {
    for (int i = 0; i < nfa_size; i++) {
        if (setA[i] != setB[i]) {
            return 0;  // 如果存在不同，返回0
        }
    }
    return 1;  // 相同，返回1
}

// 从状态集合生成DFA状态ID
int generate_state_id(struct D_finite_automata *dfa, int *state_set, int state_count) {
    for (int i = 0; i < dfa->n; i++) {
        if (compare_sets(dfa->nodes[i].state, state_set, dfa->nodes[i].length)) {
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
struct D_finite_automata *nfa_to_dfa(struct finite_automata *nfa) {
    struct D_finite_automata *dfa = create_dfa_empty_graph();

    // 使用一个数组来存储NFA状态集合，初始化为NFA的起始状态的epsilon闭包
    int *start_closure = (int *)malloc(nfa->n * sizeof(int));
    memset(start_closure, 0, nfa->n * sizeof(int));
    start_closure[0] = 1;  // 假设NFA的起始状态是状态0，并加入到闭包
    
    // 计算起始状态的epsilon闭包
    start_closure = epsilon_closure(nfa, start_closure, 1); //get the first state of dfa

    // 将起始状态的epsilon闭包加入DFA
    int start_state_id = generate_state_id(dfa, start_closure, nfa->n);
    // add_one_vertex_to_dfa(dfa); 
    // TODO:

    // DFA状态队列，用于遍历DFA的每个状态
    int *state_queue = (int *)malloc(dfa->n * sizeof(int));
    int queue_head = 0, queue_tail = 0;

    // 初始状态加入队列
    state_queue[queue_tail++] = start_state_id;

    // 对DFA中的每个状态，遍历所有字符的转换
    while (queue_head < queue_tail) {
        int current_state_id = state_queue[queue_head++];
        
        // 对当前DFA状态，检查每一个字符
        for (char c = 'a'; c <= 'z'; c++) { 
            // TODO: 可能需要修改字符范围, 也可以考虑改为只查找这个NFA状态的出边
            int *new_state = move(nfa, dfa->nodes[current_state_id].state, dfa->nodes[current_state_id].length, c);
            
            if (new_state != NULL) {
                int new_state_id = generate_state_id(dfa, new_state, nfa->n);

                // 如果新状态不在队列中，则加入队列，并添加相应的边
                int is_new_state = 1; 
                //TODO: 判断是否是新状态存疑
                for (int i = queue_head; i < queue_tail; i++) {
                    if (dfa->nodes[state_queue[i]].id == new_state_id) {
                        is_new_state = 0;
                        break;
                    }
                }
                
                if (is_new_state) {
                    state_queue[queue_tail++] = new_state_id;

                    // 为DFA添加边
                    struct char_set *cs = (struct char_set *)malloc(sizeof(struct char_set));
                    cs->n = 1;
                    cs->c = (char *)malloc(sizeof(char));
                    cs->c[0] = c;  // 将字符c作为边标签
                    add_one_edge_to_dfa(dfa, current_state_id, new_state_id, cs);
                }
                else {
                    //TODO: add the char to the edge

                }

                free(new_state);
            }
        }
    }

    // 处理DFA的接受状态
    for (int i = 0; i < dfa->n; i++) {
        for (int j = 0; j < nfa->n; j++) {
            if (nfa->accepting[j] && compare_sets(dfa->nodes[i].state, &j, 1)) {
                dfa->accepting[i] = 1;  // 设置DFA接受状态
            }
        }
    }

    return dfa;
}







// run the dfa on the input string
// 获取DFA某个状态对某个字符的转移目标状态
int get_dfa_next_state(struct finite_automata *dfa, int current_state, char input_char) {
    for (int e = dfa->adj[current_state]; e != -1; e = dfa->next[e]) {
        if (dfa->lb[e].n == 1 && dfa->lb[e].c[0] == input_char) {
            return dfa->dst[e];  // 找到匹配的转移边并返回目标状态
        }
    }
    return -1;  // 没有匹配的转移，返回-1表示没有转移
}

// 执行DFA的字符串匹配
int match_string_with_dfa(struct finite_automata *dfa, const char *input_string) {
    int current_state = 0; // 从DFA的起始状态开始

    // 遍历输入字符串
    for (int i = 0; i < strlen(input_string); i++) {
        char input_char = input_string[i];
        
        // 获取DFA当前状态对于输入字符的下一个状态
        current_state = get_dfa_next_state(dfa, current_state, input_char);

        // 如果没有匹配的转移，直接返回失败
        if (current_state == -1) {
            return 0; // 匹配失败
        }
    }

    // 如果当前状态是一个接受状态，则匹配成功
    if (dfa->accepting[current_state] == 1) {
        return 1; // 匹配成功
    }

    return 0; // 匹配失败
}

