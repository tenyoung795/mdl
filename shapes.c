#include "vector.h"
#include "shapes.h"

void set_edge(edge e, struct vector * p0, struct vector * p1) {
  e[0] = p0;
  e[1] = p1;
}

void set_polygon(polygon p, struct vector * p0, struct vector * p1, struct vector * p2) {
  p[0] = p0;
  p[1] = p1;
  p[2] = p2;
}
