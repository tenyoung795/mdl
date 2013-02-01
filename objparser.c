#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "vector.h"
#include "shapes.h"
#include "matrix.h"
#include "misc_headers.h"

struct vector *get_vertex(struct matrix *, int);

void parse_obj(char *name, struct matrix *polygons, struct matrix *vertices) {
  FILE *f;
  char *result;
  char line[256];
  char cmd;
  int i0, i1, i2;
  struct vector v;
  polygon p;
  double junk;

  f = fopen(name, "r");
  if (f == NULL)
    err_exit("parse_obj: fopen");

  while (1) {
    result = fgets(line, 256, f);
    if (result == NULL)
      break;

    if (sscanf(line, " %c ", &cmd) < 1)
      continue;
    
    switch (cmd) {
    case 'v':
      if (sscanf(line, " %c %lf %lf %lf ", &cmd, &v.x, &v.y, &v.z) < 4
	  && sscanf(line, " %c %lf %lf %lf %lf", &cmd, &v.x, &v.y, &v.z, &junk) < 5)
	break;
      add_point(vertices, &v);
      break;

    case 'f':
      if (sscanf(line, " %c %d %d %d ", &cmd, &i0, &i1, &i2) < 4
	  && sscanf(line, " %c %d / %d %d / %d %d / %d", &cmd,
		    &i0, &junk, &i1, &junk, &i2, &junk) < 7
	  && sscanf(line, " %c %d / / %d %d / / %d %d / / %d", &cmd,
		    &i0, &junk, &i1, &junk, &i2, &junk) < 7
	  && sscanf(line, " %c %d / %d / %d %d / %d / %d %d / %d / %d", &cmd,
		    &i0, &junk, &junk,
		    &i1, &junk, &junk,
		    &i2, &junk, &junk) < 10)
	break;
      set_polygon(p,
		  get_vertex(vertices, i0),
		  get_vertex(vertices, i1),
		  get_vertex(vertices, i2) );
      add_polygon(polygons, p);

    default:
      break;
    }
    
  }
  if (ferror(f) )
    err_exit("parse_obj");
}

struct vector *get_vertex(struct matrix *m, int i) {
  if (i > 0)
    return m->m + i - 1;
  else if (i < 0)
    return m->m + m->lastcol + i;
  else {
    fputs("OBJ files do not use zero indices", stderr);
    exit(EXIT_FAILURE);
  }
}
