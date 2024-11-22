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