/*====================== display.c ========================
Contains functions for basic manipulation of a screen 
represented as a 2 dimensional array of colors.

A color is an ordered triple of ints, with each value standing
for red, g and b respectively
==================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "ml6.h"
#include "display.h"

const color BG = {0, 0, 0};

color new_color(uint8_t r, uint8_t g, uint8_t b) {
  color c;

  c.r = r;
  c.g = g;
  c.b = b;

  return c;
}

color change_color( int i ) {
  i %= 7;

  switch( i ) {
    
  case 0:
    return new_color(255, 255, 255);
  case 1:
    return new_color(255, 0, 0);
  case 2:
    return new_color(0, 255, 0);
  case 3:
    return new_color(0, 0, 255);
  case 4:
    return new_color(255, 255, 0);
  case 5:
    return new_color(255, 0, 255);
  case 6:
    return new_color(0, 255, 255);
  }
}

/*======== void plot() ==========
Inputs:   struct screen *s
         color c
         int x
         int y 
Returns: 
Sets the color at pixel x, y to the color represented by c
Note that s->xy[0][0] will be the upper left hand corner 
of the screen. 
If you wish to change this behavior, you can change the indicies
of s that get set. For example, using s->xy[x][YRES-1-y] will have
pixel 0, 0 located at the lower left corner of the screen

02/12/10 09:09:00
jdyrlandweaver
====================*/
void plot(struct screen *s, color c, struct vector *v) {
  int x, newy;
  double z;

  x = v->x;
  newy = YRES - 1 - v->y;
  z = v->z;

  if ( x >= 0 && x < XRES
       && newy >=0 && newy < YRES
       && z > s->z[x][newy]) {
    s->xy[x][newy] = c;
    s->z[x][newy] = z;
  }
}

/*======== void clear_screen() ==========
Inputs:   struct screen *s  
Returns: 
Sets every color in struct screen *s to black

02/12/10 09:13:40
jdyrlandweaver
====================*/
void clear_screen(struct screen *s, color ambient) {
  int x, y;
  color c;

  for ( y=0; y < YRES; y++ )
    for ( x=0; x < XRES; x++) { 
      s->xy[x][y] = ambient;
      s->z[x][y] = -DBL_MAX;
    }
}

/*======== void save_ppm() ==========
Inputs:   struct screen *s
         char *file 
Returns: 
Saves struct screen *s as a valid ppm file using the
settings in ml6.h

02/12/10 09:14:07
jdyrlandweaver
====================*/
void save_ppm( struct screen *s, char *file) {

  int x, y;
  FILE *f;
  
  f = fopen(file, "w");
  fprintf(f, "P3\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
  for ( y=0; y < YRES; y++ ) {
    for ( x=0; x < XRES; x++) 
      
      fprintf(f, "%d %d %d ", s->xy[x][y].r, s->xy[x][y].g, s->xy[x][y].b);
    fprintf(f, "\n");
  }
  fclose(f);
}
 
/*======== void save_extension() ==========
Inputs:   struct screen *s
         char *file 
Returns: 
Saves the struct screen *stor in s to the filename represented
by file. 
If the extension for file is an image format supported
by the "convert" command, the image will be saved in
that format.

02/12/10 09:14:46
jdyrlandweaver
====================*/
void save_extension( struct screen *s, char *file) {
  
  int x, y;
  FILE *f;
  char line[256];

  sprintf(line, "convert - %s", file);

  f = popen(line, "w");
  fprintf(f, "P3\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
  for ( y=0; y < YRES; y++ ) {
    for ( x=0; x < XRES; x++) 
      
      fprintf(f, "%d %d %d ", s->xy[x][y].r, s->xy[x][y].g, s->xy[x][y].b);
    fprintf(f, "\n");
  }
  pclose(f);
}


/*======== void display() ==========
Inputs:   struct screen *s 
Returns: 
Will display the struct screen *s on your monitor

02/12/10 09:16:30
jdyrlandweaver
====================*/
void display( struct screen *s) {
 
  int x, y;
  FILE *f;

  f = popen("display", "w");

  fprintf(f, "P3\n%d %d\n%d\n", XRES, YRES, MAX_COLOR);
  for ( y=0; y < YRES; y++ ) {
    for ( x=0; x < XRES; x++) 
      
      fprintf(f, "%d %d %d ", s->xy[x][y].r, s->xy[x][y].g, s->xy[x][y].b);
    fprintf(f, "\n");
  }
  pclose(f);
}

