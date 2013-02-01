#ifndef STACK_H
#define STACK_H

enum {STACK_SIZE = 2};

struct stack {
  int size;
  int top;
  tmatrix *data;
};

static const struct stack EMPTY_STACK = {0, -1, NULL};

void new_stack(struct stack *);
void push( struct stack *s );
void pop(struct stack *s);

void free_stack( struct stack *);
void print_stack( struct stack *);

#endif
