#ifndef MATRIX_H
#define MATRIX_H

#include <stddef.h>
#include "vector.h"

struct matrix {
  struct vector *m;
  int cols;
  int lastcol;
} matrix;

static const struct matrix EMPTY_MATRIX = {NULL, 0, -1};

typedef double tmatrix[4][4];

//Basic matrix manipulation routines
void new_matrix(struct matrix *m, int cols);
void free_matrix(struct matrix *m);
void grow_matrix(struct matrix *m, int newcols);

void copy_tmatrix(tmatrix a, tmatrix b);

void print_matrix(struct matrix *m);
void print_tmatrix(tmatrix m);

void ident(tmatrix m);
void tmatrix_mult(tmatrix a, tmatrix b); /* a *= b */
void transform_pt(tmatrix a, struct vector *b);
void transform_pts(tmatrix a, struct matrix *b); /* b = a * b */

/* transformation routines */
void make_translate(tmatrix m, struct vector *d);
void make_scale(tmatrix m, struct vector *d);
void make_rotX(tmatrix m, double theta);
void make_rotY(tmatrix m, double theta);
void make_rotZ(tmatrix m, double theta);

#endif
