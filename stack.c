#include <stdio.h>
#include <stdlib.h>

#include "misc_headers.h"
#include "matrix.h"
#include "stack.h"

/*======== struct stack * new_stack()) ==========
  Inputs:   
  Returns: 
  
  Creates a new stack and puts an identity
  matrix at the top.

  04/19/12 11:25:19
  jdyrlandweaver
  ====================*/
void new_stack(struct stack *s) {
  tmatrix *m;

  m = malloc( STACK_SIZE * sizeof(*m) );
  if (m == NULL)
    err_exit("new_stack");

  s->size = STACK_SIZE;
  s->top = 0;
  s->data = m;
  ident(*m);
}

/*======== void push() ==========
  Inputs:   struct stack *s  
  Returns: 

  Puts a new matrix on top of s
  The new matrix should be a copy of the curent
  top matrix
  
  04/19/12 11:29:04
  jdyrlandweaver
  ====================*/
void push( struct stack *s ) {
  tmatrix *tmp;

  if ( s->top == s->size - 1 ) {
    tmp = realloc( s->data, (s->size + STACK_SIZE) * sizeof(*tmp) );
    if (tmp == NULL)
      err_exit("push");
    s->data = tmp;
    s->size += STACK_SIZE;
  }

  copy_tmatrix(s->data[s->top], s->data[s->top+1]);
  s->top++;
}

/*======== void pop() ==========
  Inputs:   struct stack * s 
  Returns: 
  
  Remove and free the matrix at the top
  Note you do not need to return anything.

  04/19/12 11:33:12
  jdyrlandweaver
  ====================*/
void pop( struct stack * s) {
  s->top--;
}

/*======== void free_stack() ==========
  Inputs:   struct stack *s 
  Returns: 

  Deallocate all the memory used in the stack

  04/19/12 11:34:17
  jdyrlandweaver
  ====================*/
void free_stack( struct stack *s) {
  int i;
  free(s->data);
  s->data = NULL;
}

void print_stack(struct stack *s) {

  int i;
  for (i=s->top; i >= 0; i--) {

    print_tmatrix(s->data[i]);
    printf("\n");
  }

}
