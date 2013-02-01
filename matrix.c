#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "misc_headers.h"
#include "vector.h"
#include "matrix.h"

/*-------------- struct matrix *new_matrix() --------------
Inputs:  int rows
         int cols 
Returns: 

Once allocated, access the matrix as follows:
m->m[r][c]=something;
if (m->lastcol)... 
*/
void new_matrix(struct matrix *m, int cols) {
  struct vector *tmp;

  tmp = malloc(cols * sizeof(*tmp) );
  if (tmp == NULL)
    err_exit("new_matrix");
    
  m->m=tmp;
  m->cols = cols;
  m->lastcol = 0;
}


/*-------------- void free_matrix() --------------
Inputs:  struct matrix *m 
Returns: 

1. free individual rows
2. free array holding row pointers
3. free actual matrix
*/
void free_matrix(struct matrix *m) {
  free(m->m);
  m->m = NULL;
}


/*======== void grow_matrix() ==========
Inputs:  struct matrix *m
         int newcols 
Returns: 

Reallocates the memory for m->m such that it now has
newcols number of collumns
====================*/
void grow_matrix(struct matrix *m, int newcols) {
  struct vector *tmp;

  if (newcols <= m->cols)
    return;

  tmp = realloc(m->m, newcols * sizeof(*tmp) );
  if (tmp == NULL)
    err_exit("grow_matrix");

  m->m = tmp;
  m->cols = newcols;
}


/*-------------- void print_matrix() --------------
Inputs:  struct matrix *m 
Returns: 

print the matrix
*/
void print_matrix(struct matrix *m) {
  int r, c;
  for (r=0; r < 4; r++) {
    for (c=0; c < m->lastcol; c++) 
      printf("%f ", get_component(m->m + c, r) );
    printf("\n");
  }
}

void print_tmatrix(tmatrix m) {
  int r, c;
  for (r=0; r < 4; r++) {
    for (c=0; c < 4; c++) 
      printf("%f ", m[c][r]);
    printf("\n");
  }
}

/*-------------- void ident() --------------
Inputs:  struct matrix *m <-- assumes m is a square matrix
Returns: 

turns m in to an identity matrix
*/
void ident(tmatrix m) {

  int r, c;
  for (r=0; r < 4; r++)
    for (c=0; c < 4; c++) 
      m[c][r] = (r == c);
}


/*-------------- void matrix_mult() --------------
Inputs:  struct matrix *a
         struct matrix *b 
Returns: 

a*b->b
*/
void tmatrix_mult(tmatrix a, tmatrix b) {
  int r, c, n;
  tmatrix tmp;

  for (r = 0; r < 4; r++)
    for (c = 0; c < 4; c++) {
      tmp[c][r] = 0;
      for (n = 0; n < 4; n++)
	tmp[c][r] += a[n][r] * b[c][n];
    }
  
  memcpy(b, tmp, sizeof(tmp) );
}

void transform_pt(tmatrix a, struct vector *b) {
  struct vector tmp;
  int r, n;
  double d;

  for (r = 0; r < 3; r++) {
    d = 0;
    for (n = 0; n < 4; n++)
      d += a[n][r] * get_component(b, n);
    set_component(&tmp, r, d);
  }

  *b = tmp;
}

void transform_pts(tmatrix a, struct matrix *b) {
  struct vector *tmp;
  int r, c, n;
  double d;

  tmp = malloc(b->cols * sizeof(*tmp) );
  if (tmp == NULL)
    err_exit("transform_pts");

  for (r = 0; r < 3; r++)
    for (c = 0; c < b->lastcol; c++) {
      d = 0;
      for (n = 0; n < 4; n++)
	d += a[n][r] * get_component(b->m + c, n);
      set_component(tmp + c, r, d);
    }

  free(b->m);
  b->m = tmp;
}



/*-------------- void copy_matrix() --------------
Inputs:  struct matrix *a
         struct matrix *b 
Returns: 

copy matrix a to matrix b
*/
void copy_tmatrix(tmatrix a, tmatrix b) {
  int r, c;

  for (r=0; r < 4; r++) 
    for (c=0; c < 4; c++)  
      b[c][r] = a[c][r];  
}

/*======== void make_translate() ==========
Inputs:  tmatrix m
         int x
         int y
         int z 
Returns: 
Turns m into a translation matrix using x, y and z 
as the translation offsets.
====================*/
void make_translate(tmatrix m, struct vector * d) {
  size_t r, c;

  for (r = 0; r < 4; r++)
    for (c = 0; c < 3; c++)
      m[c][r] = (r == c);

  m[c][X] = d->x;
  m[c][Y] = d->y;
  m[c][Z] = d->z;
  m[c][3] = 1;
}

/*======== void make_scale() ==========
Inputs:  tmatrix m
         int x
         int y
         int z 
Returns:
Turns m into a scale matrix using x, y and z
as the scale factors
====================*/
void make_scale(tmatrix m, struct vector * s) {
  size_t r, c;

  for (r = 0; r < 4; r++)
    for (c = 0; c < 4; c++)
      m[c][r] = (r != c? 0 : get_component(s, r) );
}

/*======== void make_rotX() ==========
Inputs:  tmatrix m
         double theta

Returns:

Turns m into a rotation matrix using theta as the 
angle of rotation and X as the axis of rotation.
====================*/
void make_rotX(tmatrix m, double theta) {
  ident(m);
  m[1][Y] = m[2][Z] = cos(theta);
  m[2][Y] = -sin(theta);
  m[1][Z] = sin(theta);
}

/*======== void make_rotY() ==========
Inputs:  tmatrix m
         double theta

Returns:

Turns m into a rotation matrix using theta as the 
angle of rotation and Y as the axis of rotation.
====================*/
void make_rotY(tmatrix m, double theta) {
  ident(m);
  m[0][X] = m[2][Z] = cos(theta);
  m[2][X] = -sin(theta);
  m[0][Z] = sin(theta);
}

/*======== void make_rotZ() ==========
Inputs:  tmatrix m
         double theta

Returns:

Turns m into a rotation matrix using theta as the 
angle of rotation and Z as the axis of rotation.
====================*/
void make_rotZ(tmatrix m, double theta) {
  ident(m);
  m[0][X] = m[1][Y] = cos(theta);
  m[1][X] = -sin(theta);
  m[0][Y] = sin(theta);
}
