#ifndef DRAW_H
#define DRAW_H

#include "shapes.h"
#include "ml6.h"
#include "matrix.h"
#include "vector.h"
#include "symtab.h"
#include "shading.h"

static const color WIREFRAME_COLOR = {0, 255, 255};

void draw_line(edge, struct screen *, color);
void add_point(struct matrix *, struct vector *);
void add_edge(struct matrix *, edge);
void draw_lines(struct matrix *, struct screen *, color);

//polygon draw routines
void add_polygon(struct matrix *, polygon);
void draw_polygons(struct matrix *,
		   struct screen *,
		   enum shading_t,
		   struct constants *,
		   color,
		   struct light *,
		   struct vector *);

//polygons
void add_box(struct matrix *, struct box *);
void add_sphere(struct matrix *, struct sphere *, double step);
void add_torus(struct matrix *, struct torus *, double step);

#endif
