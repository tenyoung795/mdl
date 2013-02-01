#ifndef SHAPES_H
#define SHAPES_H

#include "vector.h"

typedef struct vector *edge[2];

void set_edge(edge, struct vector * p0, struct vector * p1);

typedef struct vector *polygon[3];

void set_polygon(polygon,
		 struct vector * p0, struct vector * p1, struct vector * p2);

struct sphere {
  struct vector c;
  double r;
};

struct torus {
  struct vector c;
  double ri, ro;
};

struct box {
  struct vector c; /* upper-left-outer corner */
  double w, h, d;
};

#endif
