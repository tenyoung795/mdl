#include <stdio.h>
#include <math.h>

#include "vector.h"

double get_component(struct vector *v, int i) {
  switch (i)
    {
    case X:
      return v->x;
    case Y:
      return v->y;
    case Z:
      return v->z;
    case 3:
      return 1;
    default:
      return NAN;
    };
}

void set_component(struct vector *v, int i, double d) {
  switch (i)
    {
    case X:
      v->x = d;
      break;
    case Y:
      v->y = d;
      break;
    case Z:
      v->z = d;
    default:
      break;
    }
}

void set_vector(struct vector *v, double x, double y, double z) {
  v->x = x;
  v->y = y;
  v->z = z;
}

/* v += u */
void add_vectors(struct vector *a, struct vector *b) {
  a->x += b->x;
  a->y += b->y;
  a->z += b->z;
}

/* v -= u */
void subtract_vectors(struct vector *a, struct vector *b) {
  a->x -= b->x;
  a->y -= b->y;
  a->z -= b->z;
}

/* v *= s (s is a scalar) */
void scalar_mult(double d, struct vector *v) {
  v->x *= d;
  v->y *= d;
  v->z *= d;
}

void scalar_div(double d, struct vector *v) {
  v->x /= d;
  v->y /= d;
  v->z /= d;
}

double mag(struct vector *v) {
  return sqrt(dot(v, v) );
}

/* v /= m (m is the magnitude) */
void normalize(struct vector *v) {
  double m = mag(v);
  if (!m) return;
  scalar_mult(1.0 / m, v);
}

double dot(struct vector *a, struct vector *b) {
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

/* n = a x b */
void cross(struct vector *n, struct vector *a, struct vector *b) {
  set_vector(n,
	     a->y * b->z - a->z * b->y,
	     a->z * b->x - a->x * b->z,
	     a->x * b->y - a->y * b->x);
}

void print_vector(struct vector *v) {
  printf("<%f, %f, %f>\n", v->x, v->y, v->z);
}

int are_equal_vectors(struct vector *a, struct vector *b) {
  return (a->x == b->x) && (a->y == b->y) && (a->z == b->z);
}
