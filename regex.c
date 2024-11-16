#include "regex.h"

struct simpl_regexp *simplify_regexp(struct frontend_regexp *r)
{
    struct simpl_regexp *sr;
    sr = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
    switch (r->t)
    {
    case T_FR_CHAR_SET:
        sr->t = T_S_CHAR_SET;
        sr->d.CHAR_SET = r->d.CHAR_SET;
    case T_FR_OPTIONAL:
        sr->t = T_S_UNION;
        struct simpl_regexp *none;
        none = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
        none->t = T_S_EMPTY_STR;
        sr->d.UNION.r1 = none;
        sr->d.UNION.r2 = simplify_regexp(r->d.OPTION.r);
    case T_FR_STAR:
        sr->t = T_S_STAR;
        sr->d.STAR.r = simplify_regexp(r->d.STAR.r);
    case T_FR_PLUS:
        sr->t = T_S_CONCAT;
        sr->d.CONCAT.r1 = simplify_regexp(r->d.PLUS.r);
        struct simpl_regexp *star;
        star = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
        star->t = T_S_STAR;
        star->d.STAR.r = simpl_regexp(r->d.PLUS.r);
        sr->d.CONCAT.r2 = star;
    case T_FR_STRING:
        sr->t = T_S_CONCAT;
        struct simpl_regexp *schar;
        schar = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
        schar->t = T_S_CHAR_SET;
        struct char_set cs;
        cs.n = 1;
        strncpy(cs.c, r->d.STRING.s, 1);
        schar->d.CHAR_SET = cs;
        sr->d.CONCAT.r1 = schar;
        if (strlen(r->d.STRING.s) == 2)
        {
            struct simpl_regexp *schar2;
            schar2 = (struct simpl_regexp *)malloc(sizeof(struct simpl_regexp));
            schar2->t = T_S_CHAR_SET;
            struct char_set cs2;
            cs2.n = 1;
            strncpy(cs2.c, r->d.STRING.s + 1, 1);
            schar2->d.CHAR_SET = cs2;
            sr->d.CONCAT.r2 = schar2;
        }
        else
        {
            struct frontend_regexp *rstring;
            rstring = (struct frontend_regexp *)malloc(sizeof(struct frontend_regexp));
            rstring->t = T_FR_STRING;
            strcpy(rstring->d.STRING.s, r->d.STRING.s + 1);
            sr->d.CONCAT.r2 = simplify_regexp(rstring);
            free(rstring);
        }
    case T_FR_SINGLE_CHAR:
        sr->t = T_S_CHAR_SET;
        struct char_set cs;
        cs.n = 1;
        char *str = (char *)malloc(2 * sizeof(char));
        str[0] = r->d.SINGLE_CHAR.c;
        str[1] = '\0';
        cs.c = str;
        sr->d.CHAR_SET = cs;
    case T_FR_UNION:
        sr->t = T_S_UNION;
        sr->d.UNION.r1 = simplify_regexp(r->d.UNION.r1);
        sr->d.UNION.r2 = simplify_regexp(r->d.UNION.r2);
    case T_FR_CONCAT:
        sr->t = T_S_CONCAT;
        sr->d.CONCAT.r1 = simplify_regexp(r->d.CONCAT.r1);
        sr->d.CONCAT.r2 = simplify_regexp(r->d.CONCAT.r2);
    }
    return sr;
}

struct char_set get_empty_charset()
{
    struct char_set empty;
    empty.n = 0;
    char c[] = {'\0'};
    char *p = c;
    empty.c = p;
    return empty;
}

int add_one_regexp(struct finite_automata *g, struct simpl_regexp *r, int start)
{
    int end_v;
    struct char_set emptycs = get_empty_charset();
    switch (r->t)
    {
    case T_S_CHAR_SET:
        struct char_set target_charset = r->d.CHAR_SET;
        int target_v = add_one_vertex(g);
        int edge = add_one_edge(g, start, target_v, &target_charset);
        end_v = target_v;
    case T_S_STAR:
        int mid_v = add_one_vertex(g);
        int target_v = add_one_regexp(g, r->d.STAR.r, mid_v);
        int edge1 = add_one_edge(g, start, target_v, &emptycs);
        int edge2 = add_one_edge(g, target_v, mid_v, &emptycs);
        end_v = target_v;
    case T_S_EMPTY_STR:
        int target_v = add_one_vertex(g);
        int edge = add_one_edge(g, start, target_v, &emptycs);
        end_v = target_v;
    case T_S_UNION:
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
    case T_S_CONCAT:
        int mid_v1 = add_one_regexp(g, r->d.CONCAT.r1, start);
        int mid_v2 = add_one_vertex(g);
        int mid_edge = add_one_edge(g, mid_v1, mid_v2, &emptycs);
        int target_v = add_one_regexp(g, r->d.CONCAT.r2, mid_v2);
        end_v = target_v;
    }
    return end_v;
}