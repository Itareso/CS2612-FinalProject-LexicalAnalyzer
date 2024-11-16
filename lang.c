#include "lang.h"

void copy_char_set(struct char_set *dst, struct char_set *src)
{
    dst->n = src->n;
    strcpy(dst->c, src->n);
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