#include "regex.h"
#include "lang.h"
#include <ctype.h>

struct simpl_regexp *simplify_regexp(struct frontend_regexp *r)
{
    struct simpl_regexp *sr;
    sr = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
    switch (r->t)
    {
    case T_FR_CHAR_SET:
        sr->t = T_S_CHAR_SET;
        sr->d.CHAR_SET = r->d.CHAR_SET;
        break;
    case T_FR_OPTIONAL:
        sr->t = T_S_UNION;
        struct simpl_regexp *none;
        none = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
        none->t = T_S_EMPTY_STR;
        sr->d.UNION.r1 = none;
        sr->d.UNION.r2 = simplify_regexp(r->d.OPTION.r);
        break;
    case T_FR_STAR:
        sr->t = T_S_STAR;
        sr->d.STAR.r = simplify_regexp(r->d.STAR.r);
        break;
    case T_FR_PLUS:
        sr->t = T_S_CONCAT;
        sr->d.CONCAT.r1 = simplify_regexp(r->d.PLUS.r);
        struct simpl_regexp *star;
        star = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
        star->t = T_S_STAR;
        star->d.STAR.r = simplify_regexp(r->d.PLUS.r);
        sr->d.CONCAT.r2 = star;
        break;
    case T_FR_STRING:
        if (strlen(r->d.STRING.s) == 0)
        {
            sr->t = T_S_EMPTY_STR;
            break;
        }
        struct simpl_regexp *schar;
        schar = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
        schar->t = T_S_CHAR_SET;
        struct char_set cs;
        cs.n = 1;
        cs.c = (char *)malloc(sizeof(char) * 2);
        strncpy(cs.c, r->d.STRING.s, 1);
        cs.c[1] = '\0';
        schar->d.CHAR_SET = cs;
        if (strlen(r->d.STRING.s) == 1)
        {
            sr = schar;
            break;
        }
        sr->t = T_S_CONCAT;
        sr->d.CONCAT.r1 = schar;
        if (strlen(r->d.STRING.s) == 2)
        {
            struct simpl_regexp *schar2;
            schar2 = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
            schar2->t = T_S_CHAR_SET;
            struct char_set cs2;
            cs2.n = 1;
            cs2.c = (char *)malloc(sizeof(char) * 2);
            strncpy(cs2.c, r->d.STRING.s + 1, 1);
            cs2.c[1] = '\0';
            schar2->d.CHAR_SET = cs2;
            sr->d.CONCAT.r2 = schar2;
        }
        else
        {
            struct frontend_regexp *rstring;
            rstring = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
            rstring->t = T_FR_STRING;
            rstring->d.STRING.s = (char *)malloc(sizeof(char) * strlen(r->d.STRING.s));
            strcpy(rstring->d.STRING.s, r->d.STRING.s + 1);
            sr->d.CONCAT.r2 = simplify_regexp(rstring);
            free(rstring);
        }
        break;
    case T_FR_SINGLE_CHAR:
    {
        sr->t = T_S_CHAR_SET;
        struct char_set cs;
        cs.n = 1;
        char *str = (char *)malloc(2 * sizeof(char));
        str[0] = r->d.SINGLE_CHAR.c;
        str[1] = '\0';
        cs.c = str;
        sr->d.CHAR_SET = cs;
    }
    break;
    case T_FR_UNION:
        sr->t = T_S_UNION;
        sr->d.UNION.r1 = simplify_regexp(r->d.UNION.r1);
        sr->d.UNION.r2 = simplify_regexp(r->d.UNION.r2);
        break;
    case T_FR_CONCAT:
        sr->t = T_S_CONCAT;
        sr->d.CONCAT.r1 = simplify_regexp(r->d.CONCAT.r1);
        sr->d.CONCAT.r2 = simplify_regexp(r->d.CONCAT.r2);
        break;
    }
    return sr;
}

struct char_set get_empty_charset()
{
    struct char_set empty;
    empty.n = 0;
    empty.c = (char *)malloc(sizeof(char));
    empty.c[0] = '\0';
    return empty;
}

int add_one_regexp(struct finite_automata *g, struct simpl_regexp *r, int start)
{
    int end_v;
    struct char_set emptycs = get_empty_charset();
    // printf("now matching %d\n", r->t);
    switch (r->t)
    {
    case T_S_CHAR_SET:
    {
        struct char_set target_charset = r->d.CHAR_SET;
        int target_v = add_one_vertex(g);
        int edge = add_one_edge(g, start, target_v, &target_charset);
        end_v = target_v;
        break;
    }

    case T_S_STAR:
    {
        int mid_v = add_one_vertex(g);
        int target_v = add_one_regexp(g, r->d.STAR.r, mid_v);
        int edge1 = add_one_edge(g, start, target_v, &emptycs);
        int edge2 = add_one_edge(g, target_v, mid_v, &emptycs);
        int target_v_extend = add_one_vertex(g);
        int edge3 = add_one_edge(g, target_v, target_v_extend, &emptycs);
        end_v = target_v_extend;
        break;
    }

    case T_S_EMPTY_STR:
    {
        int target_v = add_one_vertex(g);
        int edge = add_one_edge(g, start, target_v, &emptycs);
        end_v = target_v;
        break;
    }

    case T_S_UNION:
    {
        int mid_v1 = add_one_vertex(g);
        int mid_v2 = add_one_vertex(g);
        int mid_edge1 = add_one_edge(g, start, mid_v1, &emptycs);
        int mid_edge2 = add_one_edge(g, start, mid_v2, &emptycs);
        int target_v1 = add_one_regexp(g, r->d.UNION.r1, mid_v1);
        int target_v2 = add_one_regexp(g, r->d.UNION.r2, mid_v2);
        int target_v = add_one_vertex(g);
        int mid_edge3 = add_one_edge(g, target_v1, target_v, &emptycs);
        int mid_edge4 = add_one_edge(g, target_v2, target_v, &emptycs);
        end_v = target_v;
        break;
    }

    case T_S_CONCAT:
    {
        int mid_v1 = add_one_regexp(g, r->d.CONCAT.r1, start);
        int mid_v2 = add_one_vertex(g);
        int mid_edge = add_one_edge(g, mid_v1, mid_v2, &emptycs);
        int target_v = add_one_regexp(g, r->d.CONCAT.r2, mid_v2);
        end_v = target_v;
        break;
    }
    }
    return end_v;
}

void regexp_to_NFA(struct finite_automata *g, struct simpl_regexp *r)
{
    int start = add_one_vertex(g);
    int end_v = add_one_regexp(g, r, start);
    g->accepting[end_v] = 1;
}

void print_NFA(struct finite_automata *g)
{
    printf("Printed NFA:\n");
    int p = 0;
    while (g->adj[p] != -1)
    {
        int e = g->adj[p];
        printf("%d-%s->%d\n", p, g->lb[e].c, g->dst[e]);
        while (g->next[e] != -1)
        {
            e = g->next[e];
            printf("%d-%s->%d\n", p, g->lb[e].c, g->dst[e]);
        }
        p++;
    }
    for (int i = 0; i <= p; i++)
        if (g->accepting[i] == 1)
            printf("The ending state is %d\n", i);
}