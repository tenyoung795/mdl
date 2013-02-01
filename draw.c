#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"
#include "misc_headers.h"
#include "shading.h"

enum {BOX_FACES = 6, BOX_VERTICES = 8};

enum {
  XYZ, XYZ1, XY1Z, XY1Z1, X1YZ, X1YZ1, X1Y1Z, X1Y1Z1
};

const int BOX_FACE_INDICES[BOX_FACES][4] =
  { {XYZ, XY1Z, X1Y1Z, X1YZ},
    {X1YZ1, X1Y1Z1, XY1Z1, XYZ1},
    {XYZ1, XYZ, X1YZ, X1YZ1},
    {XY1Z, XY1Z1, X1Y1Z1, X1Y1Z},
    {XYZ1, XY1Z1, XY1Z, XYZ},
    {X1YZ, X1Y1Z, X1Y1Z1, X1YZ1} };

void generate_box(struct vector v[BOX_VERTICES], struct box *);
void generate_sphere(struct vector *, struct sphere *, double step);
void generate_torus(struct vector *, struct torus *, double step);

/*======== void add_box() ==========
  Inputs:   struct matrix * points
            double x
	    double y
	    double z
	    double width
	    double height
	    double depth  
  Returns: 

  04/04/12 12:25:47
  jdyrlandweaver
  ====================*/
void add_box(struct matrix * points, struct box *b) {
  struct vector vertices[BOX_VERTICES];
  int i;
  polygon p;
  struct vector *tmp;

  generate_box(vertices, b);
  
  for (i = 0; i < BOX_FACES; i++) {
    set_polygon(p,
		vertices + BOX_FACE_INDICES[i][0],
		vertices + BOX_FACE_INDICES[i][1],
		vertices + BOX_FACE_INDICES[i][2]);
    add_polygon(points, p);

    set_polygon(p, p[2], vertices + BOX_FACE_INDICES[i][3], p[0]);
    add_polygon(points, p);
  }
}

void add_sphere( struct matrix * points, struct sphere *s, double step ) {
  struct vector *temp;
  int lat, longt;
  int i;
  double num_steps;
  int ns;
  polygon p;
  struct vector v;
  
  num_steps = 1.0 / step;
  ns = (int)num_steps;

  temp = malloc( (ns + 1) * (ns + 1) * sizeof(*temp) );
  if (temp == NULL)
    err_exit("add_sphere");
  generate_sphere(temp, s, step);

  for ( lat = 0; lat < ns; lat++ ) {
    for ( longt = 0; longt < ns; longt++ ) {      

      i = lat * ns + longt;  

      if ( lat != ns - 1 && longt != ns - 1 ) {
	set_polygon(p, temp + i, temp + i+1, temp + i+ns);
	add_polygon(points, p);

	set_polygon(p, p[1], temp + i+1+ns, p[2]);
	add_polygon(points, p);
      } //end of non edge cases
      
      else if ( lat == ns - 1 ) {
	if  ( longt != ns -1 ) {
	  set_polygon(p, temp + i, temp + i+1, temp + i%ns);
	  add_polygon(points, p);

	  set_polygon(p, p[1], temp + (i+1)%ns, p[2]);
	  add_polygon(points, p);
	}
	else {
	  v = temp[0];
	  v.x -= 2 * s->r;
	  set_polygon(p, temp+i, &v, temp+i%ns);
	  add_polygon(points, p);
	}
      } //end latitude edge
      else {
	v = temp[i+1];
	v.x -= 2 * s->r;
	set_polygon(p, temp+i, &v, temp+i+ns);
	add_polygon(points, p);
      } //end longitude edge (south pole)

    } //end for long
  }// end for lat

  free(temp);
}

void add_torus( struct matrix * points, struct torus *t, double step ) {
  struct vector *temp;
  int lat, longt;
  int i;
  double num_steps;
  int ns;
  polygon p;
  
  num_steps = 1.0 / step;
  ns = (int)num_steps;

  temp = malloc( (ns + 1) * (ns + 1) * sizeof(*temp) );
  if (temp == NULL)
    err_exit("add_torus");
  //generate the points on the torus
  generate_torus(temp, t, step);

  //latitudinal lines  
  for ( lat = 0; lat < ns; lat++ ) {
    for ( longt = 0; longt < ns; longt++ ) {

      i = lat * ns + longt;


      if ( lat != ns - 1 && longt != ns - 1 ) {
	set_polygon(p, temp + i, temp + i+1, temp + i+ns);
	add_polygon(points, p);

	set_polygon(p, p[1], temp + i+1+ns, p[2]);
	add_polygon(points, p);
      } //end of non edge cases

      else if ( lat == ns - 1 ) {
	if  ( longt != ns -1 ) {
	  set_polygon(p, temp + i, temp + i+1, temp + i%ns);
	  add_polygon(points, p);

	  set_polygon(p, p[1], temp + (i+1)%ns, p[2]);
	  add_polygon(points, p);
	}
	else { 
	  set_polygon(p, temp + i, temp + i+1-ns, temp + i%ns);
	  add_polygon(points, p);

	  set_polygon(p, p[1], temp, p[2]);
	  add_polygon(points, p);
	}
      } //end latitude edge
      else {
	set_polygon(p, temp + i, temp + i+1-ns, temp + i+ns);
	add_polygon(points, p);

	set_polygon(p, p[1], temp + i+1, p[2]);
	add_polygon(points, p);
      } //end longitude edge (south pole)
	  
    
   } //end for longt
  } //end for lat
  free(temp);
}

/*======== void draw_polygons() ==========
  Inputs:   struct matrix *points
            screen s
	    color c  
  Returns: 
  
  Go through the point matrix as if it were a polygon matrix
  Call drawline in batches of 3s to create triangles.
  
  04/04/12 13:39:09
  jdyrlandweaver
  ====================*/
void draw_polygons(struct matrix *points,
		   struct screen *s,
		   enum shading_t type,
		   struct constants *k,
		   color ambient,
		   struct light *l,
		   struct vector *v) {

  int i, j;
  polygon p;
  edge e;
  struct vector n[3];
  color c;

  if ( points->lastcol < 3 ) {
    fputs("Need at least 3 points to draw a polygon\n", stderr);
    return;
  }
  
  for( i=0; i < points->lastcol - 2; i+=3 ) {
    calculate_normal(n, points, i);
    if (dot(n, v) > 0) {
      set_polygon(p,
		  points->m + i,
		  points->m + i+1,
		  points->m + i+2);

      if (type == WIREFRAME || k == NULL) {
	set_edge(e, p[0], p[1]);
	draw_line(e, s, WIREFRAME_COLOR);

	set_edge(e, p[1], p[2]);
	draw_line(e, s, WIREFRAME_COLOR);

	set_edge(e, p[2], p[0]);
	draw_line(e, s, WIREFRAME_COLOR);
      }

      else {
	if (type != FLAT) 
	  calculate_vertex_normals(n, points, i);
	scanline_convert(p, s, type, k, ambient, l, n, v);
	draw_shaded_edges(p, s, type, k, ambient, l, n, v);
      }
    }
  }
}

/*======== void add_polygon() ==========
  Inputs:   struct matrix *points
            double x0
	    double y0
	    double z0
	    double x1
	    double y1
	    double z1
	    double x2
	    double y2
	    double z2  
  Returns: 
  
  Adds points (x0, y0, z0), (x1, y1, z1) and (x2, y2, z2) to the
  polygon matrix
  
  04/04/12 12:43:57
  jdyrlandweaver
  ====================*/
void add_polygon( struct matrix *points, polygon p) {
  add_point(points, p[0]);
  add_point(points, p[1]);
  add_point(points, p[2]);
}

void generate_box(struct vector points[BOX_VERTICES], struct box *b) {
  struct vector v;
  int i;

  for (i = 0; i < BOX_VERTICES; i++) {
    points[i] = b->c;
    set_vector(&v,
	       ( (i & 4) != 0) * b->w,
	       -( (i & 2) != 0) * b->h,
	       -( (i & 1) != 0) * b->d);
    add_vectors(points + i, &v);
  }
}

/*======== void generate_sphere_mesh() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  sphere with center (cx, cy) and radius r

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_sphere( struct vector * points, struct sphere *s, double step ) {
  int i;
  double circle, rotation;
  
  i = 0;
  for ( rotation = 0; rotation <= 1; rotation += step ) {
    for ( circle = 0; circle <= 1; circle+= step ) {
      set_vector(points + i,
		 cos(M_PI * circle),
		 sin(M_PI * circle) * cos(2 * M_PI * rotation),
		 sin(M_PI * circle) * sin(2 * M_PI * rotation) );
      scalar_mult(s->r, points + i);
      add_vectors(points + i, &s->c);
      i++;
    }
  }
}    

/*======== void generate_torus() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  tarus with center (cx, cy) and radii r1 and r2

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_torus( struct vector * points, struct torus *t, double step ) {
  int i;
  double circle, rotation;

  for ( rotation = 0; rotation <= 1; rotation += step ) {
    for ( circle = 0; circle <= 1; circle+= step ) {
      set_vector(points + i,
		 cos(2 * M_PI * rotation) * (t->ri * cos(2 * M_PI * circle) + t->ro),
		 t->ri * sin(2 * M_PI * circle),
		 sin(2 * M_PI * rotation) * (t->ri * cos(2 * M_PI * circle) + t->ro) );
      add_vectors(points + i, &t->c);
      i++;
    }
  }

}

/*======== void add_point() ==========
Inputs:   struct matrix * points
         int x
         int y
         int z 
Returns: 
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, struct vector *p) {
  
  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );

  points->m[points->lastcol] = *p;
  points->lastcol++;
}

/*======== void add_edge() ==========
Inputs:   struct matrix * points
          int x0, int y0, int z0, int x1, int y1, int z1
Returns: 
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points, edge e) {
  add_point( points, e[0]);
  add_point( points, e[1]);
}

/*======== void draw_lines() ==========
Inputs:   struct matrix * points
         screen s
         color c 
Returns: 
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, struct screen *s, color c) {
  int i;
  edge e;
 
  if ( points->lastcol < 2 ) {
    
    printf("Need at least 2 points to draw a line!\n");
    return;
  }

  for ( i = 0; i < points->lastcol - 1; i+=2 ) {
    set_edge(e, points->m + i, points->m + i+1);
    draw_line(e, s, c);
  } 	       
}


void draw_line(edge e, struct screen *s, color c) {
  struct vector p, *p1, dp;
  int d;
  double derz; /* derz: derivative of z */

  //swap points so we're always draing left to right
  if (e[0]->x > e[1]->x)
    set_edge(e, e[1], e[0]);

  p = *e[0];
  p1 = e[1];

  //need to know dx and dy for this version
  dp = *p1;
  subtract_vectors(&dp, &p);

  //positive slope: Octants 1, 2 (5 and 6)
  if (dp.y > 0 ) {

    //slope < 1: Octant 1 (5)
    if (dp.x > dp.y) {
      d = 2 * dp.y - dp.x;
      derz = dp.z / dp.x;

      for (; p.x <= p1->x; p.x++) {
	plot(s, c, &p);
	p.z += derz;

	if ( d < 0 )
	  d += 2 * dp.y;
	else {
	  p.y++;
	  d += 2 * (dp.y - dp.x);
	}
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = dp.y - 2 * dp.x;
      derz = dp.z / dp.y;

      for(; p.y <= p1->y; p.y++) {
	plot(s, c, &p);
	p.z += derz;

	if ( d > 0 )
	  d -= 2 * dp.x;
	else {
	  p.x++;
	  d += 2 * (dp.y - dp.x);
	}
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dp.x > fabs(dp.y) ) {
      d = 2 * dp.y + dp.x;
      derz = dp.z / dp.x;
  
      for (; p.x <= p1->x; p.x++) {
	plot(s, c, &p);
	p.z += derz;

	if ( d > 0 )
	  d += 2 * dp.y;
	else {
	  p.y--;
	  d += 2 * (dp.y + dp.x);
	}
      }
    }

    //slope < -1: Octant 7 (3)
    else {
      d = dp.y + 2 * dp.x;
      derz = dp.z / dp.y;

      for (; p.y >= p1->y; p.y--) {
	plot(s, c, &p);
	p.z -= derz;

	if ( d < 0 )
	  d += 2 * dp.x;
	else {
	  p.x++;
	  d += 2 * (dp.y + dp.x);
	}
      }
    }
  }
}

