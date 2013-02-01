#include <math.h>

#include "shading.h"
#include "shapes.h"
#include "ml6.h"
#include "display.h"
#include "vector.h"
#include "draw.h"

void ctov(struct vector *, color);
color vtoc(struct vector *);

void horizontal_convert(polygon,
			struct screen *,
			enum shading_t,
			struct constants *,
			color,
			struct light *,
			struct vector n[3],
			struct vector *);

void vertical_convert(polygon,
		      struct screen *,
		      enum shading_t,
		      struct constants *,
		      color,
		      struct light *,
		      struct vector n[3],
		      struct vector *);

void calculate_intensity(struct vector *c,
			 struct constants *,
			 color,
			 struct light *,
			 struct vector *p, /* endpoint of normal */
			 struct vector *n, /* normal */
			 struct vector *v); /* view vector */

void draw_shaded_edge(int,
		      polygon,
		      struct screen *,
		      enum shading_t,
		      struct constants *,
		      color,
		      struct light *,
		      struct vector n[3],
		      struct vector *);

void ctov(struct vector *v, color c) {
  set_vector(v, c.r, c.g, c.b);
}

color vtoc(struct vector *v) {
  return new_color( (v->x > MAX_COLOR? MAX_COLOR : v->x),
		    (v->y > MAX_COLOR? MAX_COLOR : v->y),
		    (v->z > MAX_COLOR? MAX_COLOR : v->z) );
}

void scanline_convert(polygon p,
		      struct screen *s,
		      enum shading_t type,
		      struct constants *k,
		      color ambient,
		      struct light *l,
		      struct vector n[3],
		      struct vector *v) {
  horizontal_convert(p, s, type, k, ambient, l, n, v);
  vertical_convert(p, s, type, k, ambient, l, n, v);
}

void horizontal_convert(polygon poly,
			struct screen *s,
			enum shading_t type,
			struct constants *k,
			color ambient,
			struct light *l,
			struct vector norm[3],
			struct vector *v) {
  const double
    y0 = poly[0]->y,
    y1 = poly[1]->y,
    y2 = poly[2]->y;

  int ib, im, it;
  int dy_bt, dy_bm, dy_mt;
  double dx;

  struct vector pb, pm, pt;
  struct vector dp_dy_bt, dp_dy_bm, dp_dy_mt;
  struct vector p0, p1;
  edge e = {&p0, &p1};
  struct vector p;
  double dz_dx;

  struct vector cb, cm, ct;
  struct vector dc_dy_bt, dc_dy_bm, dc_dy_mt;
  struct vector c0, c1;
  struct vector c;
  struct vector dc_dx;

  struct vector dn_dy_bt, dn_dy_bm, dn_dy_mt;
  struct vector n0, n1;
  struct vector n;
  struct vector dn_dx;

  if (y0 <= y1 && y1 <= y2) {
    ib = 0;
    im = 1;
    it = 2;
  }
  else if (y0 <= y2 && y2 <= y1) {
    ib = 0;
    im = 2;
    it = 1;
  }
  else if (y1 <= y0 && y0 <= y2) {
    ib = 1;
    im = 0;
    it = 2;
  }
  else if (y1 <= y2 && y2 <= y0) {
    ib = 1;
    im = 2;
    it = 0;
  }
  else if (y2 <= y0 && y0 <= y1) {
    ib = 2;
    im = 0;
    it = 1;
  }
  else {
    ib = 2;
    im = 1;
    it = 0;
  }

  pb = *poly[ib];
  pm = *poly[im];
  pt = *poly[it];

  pb.y = (int)pb.y; 
  pm.y = (int)pm.y; 
  pt.y = (int)pt.y; 

  dy_bt = pt.y - pb.y;
  dy_bm = pm.y - pb.y;
  dy_mt = pt.y - pm.y;

  p0 = pb;
  p1 = pb.y != pm.y? pb : pm;

  dp_dy_bt = pt;
  subtract_vectors(&dp_dy_bt, &pb);
  scalar_div(dy_bt, &dp_dy_bt);

  dp_dy_bm = pm;
  subtract_vectors(&dp_dy_bm, &pb);
  scalar_div(dy_bm, &dp_dy_bm);

  dp_dy_mt = pt;
  subtract_vectors(&dp_dy_mt, &pm);
  scalar_div(dy_mt, &dp_dy_mt);

  switch (type) {
  case FLAT:
    calculate_centroid(&p, poly);
    calculate_intensity(&c, k, ambient, l, &p, norm, v);

    for (p.y = pb.y; p.y <= pt.y; p.y++) {
      p0.y = p1.y = p.y;
      draw_line(e, s, vtoc(&c) );

      add_vectors(&p0, &dp_dy_bt);
      add_vectors(&p1, (p.y < pm.y? &dp_dy_bm : &dp_dy_mt) );
    }
    break;

  case GOROUD:
    calculate_intensity(&cb, k, ambient, l, poly[ib], norm + ib, v);
    calculate_intensity(&cm, k, ambient, l, poly[im], norm + im, v);
    calculate_intensity(&ct, k, ambient, l, poly[it], norm + it, v);

    c0 = cb;
    c1 = (pb.y != pm.y? cb : cm);

    dc_dy_bt = ct;
    subtract_vectors(&dc_dy_bt, &cb);
    scalar_div(dy_bt, &dc_dy_bt);

    dc_dy_bm = cm;
    subtract_vectors(&dc_dy_bm, &cb);
    scalar_div(dy_bm, &dc_dy_bm);

    dc_dy_mt = ct;
    subtract_vectors(&dc_dy_mt, &cm);
    scalar_div(dy_mt, &dc_dy_mt);

    for (p.y = pb.y; p.y <= pt.y; p.y++) {
      dx = p1.x - p0.x;

      dc_dx = c1;
      subtract_vectors(&dc_dx, &c0);
      scalar_div(dx, &dc_dx);

      dz_dx = (p1.z - p0.z) / dx;

      p.x = p0.x;
      p.z = p0.z;

      c = c0;
      while (1) {
	plot(s, vtoc(&c), &p);

	if (p0.x < p1.x) {
	  p.z += dz_dx;
	  add_vectors(&c, &dc_dx);
	  p.x++;
	  if (p.x > p1.x)
	    break;
	}
	else {
	  p.z -= dz_dx;
	  subtract_vectors(&c, &dc_dx);
	  p.x--;
	  if (p.x < p1.x)
	    break;
	}
      }

      add_vectors(&p0, &dp_dy_bt);
      add_vectors(&c0, &dc_dy_bt);

      if (p.y < pm.y) {
	add_vectors(&p1, &dp_dy_bm);
	add_vectors(&c1, &dc_dy_bm);
      }
      else {
	add_vectors(&p1, &dp_dy_mt);
	add_vectors(&c1, &dc_dy_mt);
      }
    }

    break;
      
  case PHONG:
    n0 = norm[ib];
    n1 = norm[pb.y != pm.y? ib : im];
    
    dn_dy_bt = norm[it];
    subtract_vectors(&dn_dy_bt, norm + ib);
    scalar_div(dy_bt, &dn_dy_bt);

    dn_dy_bm = norm[im];
    subtract_vectors(&dn_dy_bm, norm + ib);
    scalar_div(dy_bm, &dn_dy_bm);

    dn_dy_mt = norm[it];
    subtract_vectors(&dn_dy_mt, norm + im);
    scalar_div(dy_mt, &dn_dy_mt);

    for (p.y = pb.y; p.y <= pt.y; p.y++) {
      dx = p1.x - p0.x;

      dn_dx = n1;
      subtract_vectors(&dn_dx, &n0);
      scalar_div(dx, &dn_dx);

      dz_dx = (p1.z - p0.z) / dx;

      p = p0;
      n = n0;
      while (1) {
	calculate_intensity(&c, k, ambient, l, &p, &n, v);
	plot(s, vtoc(&c), &p);

	if (p0.x < p1.x) {
	  p.z += dz_dx;
	  add_vectors(&n, &dn_dx);
	  p.x++;
	  if (p.x > p1.x)
	    break;
	}
	else {
	  p.z -= dz_dx;
	  subtract_vectors(&n, &dn_dx);
	  p.x--;
	  if (p.x < p1.x)
	    break;
	}
      }

      add_vectors(&p0, &dp_dy_bt);
      add_vectors(&n0, &dn_dy_bt);
      
      if (p.y < pm.y) {
	add_vectors(&p1, &dp_dy_bm);
	add_vectors(&n1, &dn_dy_bm);
      }
      else {
	add_vectors(&p1, &dp_dy_mt);
	add_vectors(&n1, &dn_dy_mt);
      }
    }

    break;
  }
}

void vertical_convert(polygon poly,
		      struct screen *s,
		      enum shading_t type,
		      struct constants *k,
		      color ambient,
		      struct light *l,
		      struct vector norm[3],
		      struct vector *v) {  
  const double
    x0 = poly[0]->x,
    x1 = poly[1]->x,
    x2 = poly[2]->x;

  int il, im, ir;
  int dx_lr, dx_lm, dx_mr;
  double dy;

  struct vector pl, pm, pr;
  struct vector dp_dx_lr, dp_dx_lm, dp_dx_mr;
  struct vector p0, p1;
  edge e = {&p0, &p1};
  struct vector p;
  double dz_dy;

  struct vector cl, cm, cr;
  struct vector dc_dx_lr, dc_dx_lm, dc_dx_mr;
  struct vector c0, c1;
  struct vector c;
  struct vector dc_dy;

  struct vector dn_dx_lr, dn_dx_lm, dn_dx_mr;
  struct vector n0, n1;
  struct vector n;
  struct vector dn_dy;

  if (x0 <= x1 && x1 <= x2) {
    il = 0;
    im = 1;
    ir = 2;
  }
  else if (x0 <= x2 && x2 <= x1) {
    il = 0;
    im = 2;
    ir = 1;
  }
  else if (x1 <= x0 && x0 <= x2) {
    il = 1;
    im = 0;
    ir = 2;
  }
  else if (x1 <= x2 && x2 <= x0) {
    il = 1;
    im = 2;
    ir = 0;
  }
  else if (x2 <= x0 && x0 <= x1) {
    il = 2;
    im = 0;
    ir = 1;
  }
  else {
    il = 2;
    im = 1;
    ir = 0;
  }

  pl = *poly[il];
  pm = *poly[im];
  pr = *poly[ir];

  pl.x = (int)pl.x; 
  pm.x = (int)pm.x; 
  pr.x = (int)pr.x; 

  dx_lr = pr.x - pl.x;
  dx_lm = pm.x - pl.x;
  dx_mr = pr.x - pm.x;

  p0 = pl;
  p1 = pl.x != pm.x? pl : pm;

  dp_dx_lr = pr;
  subtract_vectors(&dp_dx_lr, &pl);
  scalar_div(dx_lr, &dp_dx_lr);

  dp_dx_lm = pm;
  subtract_vectors(&dp_dx_lm, &pl);
  scalar_div(dx_lm, &dp_dx_lm);

  dp_dx_mr = pr;
  subtract_vectors(&dp_dx_mr, &pm);
  scalar_div(dx_mr, &dp_dx_mr);

  switch (type) {
  case FLAT:
    calculate_centroid(&p, poly);
    calculate_intensity(&c, k, ambient, l, &p, norm, v);

    for (p.x = pl.x; p.x <= pr.x; p.x++) {
      p0.x = p1.x = p.x;
      draw_line(e, s, vtoc(&c) );

      add_vectors(&p0, &dp_dx_lr);
      add_vectors(&p1, (p.x < pm.x? &dp_dx_lm : &dp_dx_mr) );
    }
    break;

  case GOROUD:
    calculate_intensity(&cl, k, ambient, l, poly[il], norm + il, v);
    calculate_intensity(&cm, k, ambient, l, poly[im], norm + im, v);
    calculate_intensity(&cr, k, ambient, l, poly[ir], norm + ir, v);

    c0 = cl;
    c1 = (pl.x != pm.x? cl : cm);

    dc_dx_lr = cr;
    subtract_vectors(&dc_dx_lr, &cl);
    scalar_div(dx_lr, &dc_dx_lr);

    dc_dx_lm = cm;
    subtract_vectors(&dc_dx_lm, &cl);
    scalar_div(dx_lm, &dc_dx_lm);

    dc_dx_mr = cr;
    subtract_vectors(&dc_dx_mr, &cm);
    scalar_div(dx_mr, &dc_dx_mr);

    for (p.x = pl.x; p.x <= pr.x; p.x++) {
      dy = p1.y - p0.y;

      dc_dy = c1;
      subtract_vectors(&dc_dy, &c0);
      scalar_div(dy, &dc_dy);

      dz_dy = (p1.z - p0.z) / dy;

      p.y = p0.y;
      p.z = p0.z;

      c = c0;
      while (1) {
	plot(s, vtoc(&c), &p);

	if (p0.y < p1.y) {
	  p.z += dz_dy;
	  add_vectors(&c, &dc_dy);
	  p.y++;
	  if (p.y > p1.y)
	    break;
	}
	else {
	  p.z -= dz_dy;
	  subtract_vectors(&c, &dc_dy);
	  p.y--;
	  if (p.y < p1.y)
	    break;
	}
      }

      add_vectors(&p0, &dp_dx_lr);
      add_vectors(&c0, &dc_dx_lr);

      if (p.x < pm.x) {
	add_vectors(&p1, &dp_dx_lm);
	add_vectors(&c1, &dc_dx_lm);
      }
      else {
	add_vectors(&p1, &dp_dx_mr);
	add_vectors(&c1, &dc_dx_mr);
      }
    }

    break;
      
  case PHONG:
    n0 = norm[il];
    n1 = norm[pl.x != pm.x? il : im];
    
    dn_dx_lr = norm[ir];
    subtract_vectors(&dn_dx_lr, norm + il);
    scalar_div(dx_lr, &dn_dx_lr);

    dn_dx_lm = norm[im];
    subtract_vectors(&dn_dx_lm, norm + il);
    scalar_div(dx_lm, &dn_dx_lm);

    dn_dx_mr = norm[ir];
    subtract_vectors(&dn_dx_mr, norm + im);
    scalar_div(dx_mr, &dn_dx_mr);

    for (p.x = pl.x; p.x <= pr.x; p.x++) {
      dy = p1.y - p0.y;

      dn_dy = n1;
      subtract_vectors(&dn_dy, &n0);
      scalar_div(dy, &dn_dy);

      dz_dy = (p1.z - p0.z) / dy;

      p = p0;
      n = n0;
      while (1) {
	calculate_intensity(&c, k, ambient, l, &p, &n, v);
	plot(s, vtoc(&c), &p);

	if (p0.y < p1.y) {
	  p.z += dz_dy;
	  add_vectors(&n, &dn_dy);
	  p.y++;
	  if (p.y > p1.y)
	    break;
	}
	else {
	  p.z -= dz_dy;
	  subtract_vectors(&n, &dn_dy);
	  p.y--;
	  if (p.y < p1.y)
	    break;
	}
      }

      add_vectors(&p0, &dp_dx_lr);
      add_vectors(&n0, &dn_dx_lr);
      
      if (p.x < pm.x) {
	add_vectors(&p1, &dp_dx_lm);
	add_vectors(&n1, &dn_dx_lm);
      }
      else {
	add_vectors(&p1, &dp_dx_mr);
	add_vectors(&n1, &dn_dx_mr);
      }
    }

    break;
  }
}

void calculate_intensity(struct vector *c,
			 struct constants *k,
			 color ambient,
			 struct light *light,
			 struct vector *p,
			 struct vector *n,
			 struct vector *v) {
  /* I = Ke + Ia Ka + Ip Kd (->L * ->N) + Ip Ks [(2->N(->L * ->N)- ->L) * ->V] */
  double dotd, dots;
  struct vector l, tmp;

  l = *p;
  subtract_vectors(&l, &light->l);
  normalize(&l);

  dotd = dot(&l, n);
  if (dotd < 0) {
    dotd = dots = 0;
    goto add_terms;
  }
    
  tmp = *n;
  scalar_mult(2 * dotd, &tmp);
  subtract_vectors(&tmp, &l);
  dots = dot(&tmp, v);
  if (dots < 0)
    dots = 0;

 add_terms:
  ctov(c, k->e);

  ctov(&tmp, ambient);
  tmp.x *= k->r[Ka];
  tmp.y *= k->g[Ka];
  tmp.z *= k->b[Ka];
  add_vectors(c, &tmp);

  ctov(&tmp, light->c);
  tmp.x *= k->r[Kd] * dotd + k->r[Ks] * dots;
  tmp.y *= k->g[Kd] * dotd + k->g[Kd] * dots;
  tmp.z *= k->b[Kd] * dotd + k->b[Ks] * dots;
  add_vectors(c, &tmp);
}

void draw_shaded_edges(polygon p,
		       struct screen *s,
		       enum shading_t type,
		       struct constants *k,
		       color ambient,
		       struct light *l,
		       struct vector n[3],
		       struct vector *v) {
  struct vector centroid;
  struct vector cv;
  color c;
  edge e;

  if (type == FLAT) {
    calculate_centroid(&centroid, p);
    calculate_intensity(&cv, k, ambient, l, &centroid, n, v);
    c = vtoc(&cv);
    
    set_edge(e, p[0], p[1]);
    draw_line(e, s, c);

    set_edge(e, p[1], p[2]);
    draw_line(e, s, c);

    set_edge(e, p[2], p[0]);
    draw_line(e, s, c);
  }
  else {
    draw_shaded_edge(0, p, s, type, k, ambient, l, n, v);
    draw_shaded_edge(1, p, s, type, k, ambient, l, n, v);
    draw_shaded_edge(2, p, s, type, k, ambient, l, n, v);
  }
}

void draw_shaded_edge(int x,
		      polygon poly,
		      struct screen *s,
		      enum shading_t type,
		      struct constants *k,
		      color ambient,
		      struct light *l,
		      struct vector norm[3],
		      struct vector *v) {
  int i, j, tmp;

  struct vector p, *p1, dp;
  int d;
  double derz; /* derz: derivative of z */
  
  struct vector n, dern;
  struct vector c;

  switch (x) {
  case 0:
    i = 0;
    j = 1;
    break;
  case 1:
    i = 1;
    j = 2;
    break;
  case 2:
    i = 2;
    j = 0;
    break;
  }
  
  //swap points so we're always draing left to right
  if (poly[i]->x > poly[j]->x) {
    tmp = i;
    i = j;
    j = tmp;
  }

  p = *poly[i];
  p1 = poly[j];

  dp = *p1;
  subtract_vectors(&dp, &p);

  //positive slope: Octants 1, 2 (5 and 6)
  if (dp.y > 0 ) {

    //slope < 1: Octant 1 (5)
    if (dp.x > dp.y) {
      d = 2 * dp.y - dp.x;
      derz = dp.z / dp.x;

      n = norm[i];
	
      dern = norm[j];
      subtract_vectors(&dern, &n);
      scalar_div(dp.x, &dern);

      for (; p.x <= p1->x; p.x++) {
	calculate_intensity(&c, k, ambient, l, &p, &n, v);
	plot(s, vtoc(&c), &p);

	p.z += derz;
	add_vectors(&n, &dern);

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

      n = norm[i];
	
      dern = norm[j];
      subtract_vectors(&dern, &n);
      scalar_div(dp.y, &dern);

      for (; p.y <= p1->y; p.y++) {
	calculate_intensity(&c, k, ambient, l, &p, &n, v);
	plot(s, vtoc(&c), &p);

	p.z += derz;
	add_vectors(&n, &dern);

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

    //slope > -1: Octant 8 (4)
  else {
    if ( dp.x > fabs(dp.y) ) {
      d = 2 * dp.y + dp.x;
      derz = dp.z / dp.x;

      n = norm[i];
	
      dern = norm[j];
      subtract_vectors(&dern, &n);
      scalar_div(dp.x, &dern);

      for (; p.x <= p1->x; p.x++) {
	calculate_intensity(&c, k, ambient, l, &p, &n, v);
	plot(s, vtoc(&c), &p);

	p.z += derz;
	add_vectors(&n, &dern);

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

      n = norm[i];
	
      dern = norm[j];
      subtract_vectors(&dern, &n);
      scalar_div(dp.y, &dern);

      for (; p.y >= p1->y; p.y--) {
	calculate_intensity(&c, k, ambient, l, &p, &n, v);
	plot(s, vtoc(&c), &p);

	p.z -= derz;
	subtract_vectors(&n, &dern);

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


