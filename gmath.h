#ifndef GMATH_H
#define GMATH_H

#include "shapes.h"
#include "vector.h"
#include "matrix.h"

void calculate_normal(struct vector *, struct matrix *, int);
void calculate_vertex_normals(struct vector n[3],
			      struct matrix *,
			      int);
void calculate_centroid(struct vector *, polygon);

#endif
