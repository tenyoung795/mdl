#ifndef SHADING_H
#define SHADING_H

#include "shapes.h"
#include "symtab.h"
#include "ml6.h"

enum shading_t {WIREFRAME, FLAT, GOROUD, PHONG};

void scanline_convert(polygon,
		      struct screen *,
		      enum shading_t,
		      struct constants *,
		      color,
		      struct light *,
		      struct vector n[3],
		      struct vector *);

void draw_shaded_edges(polygon,
		       struct screen *,
		       enum shading_t,
		       struct constants *,
		       color,
		       struct light *,
		       struct vector n[3],
		       struct vector *);

#endif
