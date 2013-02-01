#ifndef VECTOR_H
#define VECTOR_H

struct vector {
  double x, y, z;
};

enum {X, Y, Z};

double get_component(struct vector *, int);
void set_component(struct vector *, int, double);
void set_vector(struct vector *, double, double, double);
void add_vectors(struct vector *a, struct vector *b);
void subtract_vectors(struct vector *a, struct vector *b);
void scalar_mult(double, struct vector *);
void scalar_div(double, struct vector *);
double mag(struct vector *);
void normalize(struct vector *);
double dot(struct vector *a, struct vector *b);
void cross(struct vector *n, struct vector *a, struct vector *b);
void print_vector(struct vector *);
int are_equal_vectors(struct vector *, struct vector *);

#endif
