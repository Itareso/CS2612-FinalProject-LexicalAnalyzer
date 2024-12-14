#ifndef REGEX_H_INCLUDED
#define REGEX_H_INCLUDED

#include "lang.h"

struct simpl_regexp *simplify_regexp(struct frontend_regexp *r);
int add_one_regexp(struct finite_automata *g, struct simpl_regexp *r, int start);
void regexp_to_NFA(struct finite_automata *g, struct simpl_regexp *r);
void print_NFA(struct finite_automata *g);

#endif