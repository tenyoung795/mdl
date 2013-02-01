#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#include "vector.h"
#include "ml6.h"

color new_color(uint8_t r, uint8_t g, uint8_t b);

void plot(struct screen *s, color c, struct vector *);
void clear_screen(struct screen *s, color ambient);
void save_ppm(struct screen *s, char *file);
void save_extension(struct screen *s, char *file);
void display(struct screen *s);

color change_color( int i );
#endif
