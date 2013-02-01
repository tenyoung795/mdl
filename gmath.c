#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "vector.h"
#include "matrix.h"
#include "gmath.h"

void calculate_normal(struct vector *n, struct matrix *points, int i) {
  struct vector a, b;

  a = points->m[i+1];
  b = points->m[i];

  subtract_vectors(&a, &b);
  subtract_vectors(&b, points->m + i+2);

  cross(n, &a, &b);
  normalize(n);
}

void calculate_vertex_normals(struct vector n[3],
			      struct matrix *points,
			      int i) {
  int num_polygons[3];
  int j, k, l;
  struct vector tmp;
  
  for (k = 0; k < 3; k++) {
    set_vector(n + k, 0, 0, 0);
    num_polygons[k] = 0;
  }

  for (j = 0; j < points->lastcol - 2; j += 3) {
    calculate_normal(&tmp, points, j);
    for (k = 0; k < 3; k++)
      for (l = 0; l < 3; l++)
	if (are_equal_vectors(points->m + j+l, points->m + i+k) ) {
	  add_vectors(n + k, &tmp);
	  num_polygons[k]++;
	  break;
	}
  }

  for (k = 0; k < 3; k++)
    scalar_mult(1.0 / num_polygons[k], n + k);
}

void calculate_centroid(struct vector *c, polygon p) {
  *c = *p[0];
  add_vectors(c, p[1]);
  add_vectors(c, p[2]);
  scalar_mult(1.0/3, c);
}
