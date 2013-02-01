#ifndef NODE_H
#define NODE_H

#include "symtab.h"

struct vary_node {
  SYMTAB *knob;
  double value;
  struct vary_node *next;
};

typedef struct vary_node *vary_list;

void add_node(vary_list *, SYMTAB *, double);
void free_vary_list(vary_list *);
void apply_vary_list(vary_list *);

#endif
