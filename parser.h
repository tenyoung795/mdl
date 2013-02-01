#ifndef PARSER_H
#define PARSER_H

#include "vector.h"
#include "shapes.h"
#include "symtab.h"
#include "matrix.h"
#include "shading.h"

enum {MAX_COMMANDS = 512};
static const color DEFAULT_EMISSIVE_COLOR = {0, 0, 0};

extern int lastop;

struct command
{
  int opcode;
  union
  {
    struct {
      SYMTAB *p;
    } light;
    struct {
      color c;
    } ambient;
    struct {
      SYMTAB *p;
      /* each triple holds ka kd and ks for red green and blue
	 respectively */
    } constants;
    struct {
      SYMTAB *p;
    } save_coordinate_system;
    struct {
      struct vector eye, aim;
    } camera;
    struct {
      SYMTAB *constants;
      struct sphere s;
      SYMTAB *cs;
    } sphere;
    struct {
      SYMTAB *constants;
      struct torus t;
      SYMTAB *cs;
    } torus;
    struct {
      SYMTAB *constants;
      struct box b;
      SYMTAB *cs;
    } box;
    struct {
      SYMTAB *constants;
      struct vector p0, p1;
      SYMTAB *cs0,*cs1;
    } line;
    struct {
      SYMTAB *constants;
      char name[MAX_NAME];
      SYMTAB *cs; 
    } mesh;
    struct {
      SYMTAB *p;
      double val;
    } set;
    struct {
      struct vector d;
      SYMTAB *p;
    } move;
    struct {
      struct vector d;
      SYMTAB *p;
    } scale;
    struct {
      int axis;
      double degrees;
      SYMTAB *p;
    } rotate;
    struct {
      SYMTAB *p;
    } basename;
    struct {
      SYMTAB *p;
    } save_knobs;
    struct {
      int start_frame, end_frame;
      SYMTAB *knob_list0;
      SYMTAB *knob_list1;
    } tween;
    struct {
      int num_frames;
    } frames;
    struct {
      SYMTAB *p;
      int start_frame, end_frame;
      double start_val, end_val;
    } vary;
    struct {
      SYMTAB *p;
    } save;
    struct {
      enum shading_t type;
    } shading;
    struct {
      double value;
    } setknobs;
    struct { 
      double value;
    } focal;
  } op;
};

extern struct command op[MAX_COMMANDS];

void print_pcode();
#endif
