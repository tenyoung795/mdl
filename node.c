#include <stdlib.h>

#include "node.h"
#include "symtab.h"
#include "misc_headers.h"

void free_node(struct vary_node *);
void apply_node(struct vary_node *);

void add_node(vary_list *list, SYMTAB *knob, double value) {
  struct vary_node *node;

  if (*list == NULL) {
    *list = malloc(sizeof(**list) );
    if (*list == NULL)
      err_exit("add_node");
    node = *list;
    node->knob = knob;
    node->next = NULL;
  }
  
  else {
    for (node = *list;
	 node->knob != knob && node->next != NULL;
	 node = node->next);
    if (node->knob != knob) {
      node->next = malloc(sizeof(*node->next) );
      if (node->next == NULL)
	err_exit("add_node");
      node = node->next;
      node->knob = knob;
      node->next = NULL;
    }
  }
  
  node->value = value;
}

void free_vary_list(vary_list *list) {
  if (list != NULL)
    free_node(*list);
}

void free_node(struct vary_node *node) {
  if (node == NULL)
    return;
  free_node(node->next);
  free(node);
}

void apply_vary_list(vary_list *list) {
  apply_node(*list);
}

void apply_node(struct vary_node *node) {
  if (node == NULL)
    return;
  set_value(node->knob, node->value);
  apply_node(node->next);
}
