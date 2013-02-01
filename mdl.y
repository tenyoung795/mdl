%{
  /* C declarations */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shapes.h"
#include "vector.h"
#include "display.h"
#include "parser.h"
#include "matrix.h"
#include "misc_headers.h"
#include "shading.h"

  struct light l;
  struct constants c;
  struct knoblist k;
  struct command op[MAX_COMMANDS];
  tmatrix m;
  int lastop=0;
  int lineno=0;
  int i, j;
  
  void rot_exit(char);
  void shade_exit(char *);

  int yyerror(char *);
  int yywrap();

  int main(int, char **);

#define YYERROR_VERBOSE 1

  %}


/* Bison Declarations */

%union{
  double val;
  char string[256];

}

%token COMMENT
%token <val> DOUBLE
%token <string> LIGHT AMBIENT
%token <string> CONSTANTS SAVE_COORDS CAMERA 
%token <string> SPHERE TORUS BOX LINE CS MESH
%token <string> STRING
%token <string> SET MOVE SCALE ROTATE BASENAME SAVE_KNOBS TWEEN FRAMES VARY 
%token <string> PUSH POP SAVE
%token <string> SHADING SHADING_TYPE SETKNOBS FOCAL DISPLAY WEB
%token <string> CO
%%
/* Grammar rules */

input:
| input command
;

command: 
COMMENT {}|

LIGHT STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  l.c = new_color($3, $4, $5);
  set_vector(&l.l, $6, $7, $8);
  op[lastop].opcode=LIGHT;
  op[lastop].op.light.p = add_symbol($2,SYM_LIGHT,&l);
  lastop++;
}|

MOVE DOUBLE DOUBLE DOUBLE STRING
{ 
  lineno++;
  op[lastop].opcode = MOVE;
  set_vector(&op[lastop].op.move.d, $2, $3, $4);
  op[lastop].op.move.p = add_symbol($5,SYM_VALUE,NULL);
  lastop++;
}
|
MOVE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = MOVE;
  set_vector(&op[lastop].op.move.d, $2, $3, $4);
  op[lastop].op.move.p = NULL;
  lastop++;
}|

CONSTANTS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  c.r[Ka]=$3;
  c.r[Kd]=$4;
  c.r[Ks]=$5;

  c.g[Ka]=$6;
  c.g[Kd]=$7;
  c.g[Ks]=$8;

  c.b[Ka]=$9;
  c.b[Kd]=$10;
  c.b[Ks]=$11;

  c.e = DEFAULT_EMISSIVE_COLOR;

  op[lastop].op.constants.p =  add_symbol($2,SYM_CONSTANTS,&c);
  op[lastop].opcode=CONSTANTS;
  lastop++;
}|

CONSTANTS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  c.r[Ka]=$3;
  c.r[Kd]=$4;
  c.r[Ks]=$5;

  c.g[Ka]=$6;
  c.g[Kd]=$7;
  c.g[Ks]=$8;

  c.b[Ka]=$9;
  c.b[Kd]=$10;
  c.b[Ks]=$11;

  c.e = new_color($12, $13, $14);
  op[lastop].op.constants.p =  add_symbol($2,SYM_CONSTANTS,&c);
  op[lastop].opcode=CONSTANTS;
  lastop++;
}|

SAVE_COORDS STRING
{
  lineno++;
  op[lastop].opcode = SAVE_COORDS;
  op[lastop].op.save_coordinate_system.p = add_symbol($2,SYM_MATRIX,NULL);
  lastop++;
}|

CAMERA DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = CAMERA;
  set_vector(&op[lastop].op.camera.eye, $2, $3, $4);
  set_vector(&op[lastop].op.camera.aim, $5, $6, $7);
  lastop++;
}|
SPHERE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = SPHERE;
  set_vector(&op[lastop].op.sphere.s.c, $2, $3, $4);
  op[lastop].op.sphere.s.r = $5;
  op[lastop].op.sphere.constants = NULL;
  op[lastop].op.sphere.cs = NULL;
  lastop++;
}|
SPHERE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = SPHERE;
  set_vector(&op[lastop].op.sphere.s.c, $2, $3, $4);
  op[lastop].op.sphere.s.r = $5;
  op[lastop].op.sphere.constants = NULL;
  op[lastop].op.sphere.cs = add_symbol($6,SYM_MATRIX,NULL);
  lastop++;
}|
SPHERE STRING DOUBLE DOUBLE DOUBLE DOUBLE 
{
  lineno++;
  op[lastop].opcode = SPHERE;
  set_vector(&op[lastop].op.sphere.s.c, $3, $4, $5);
  op[lastop].op.sphere.s.r = $6;
  op[lastop].op.sphere.cs = NULL;
  op[lastop].op.sphere.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  lastop++;
}|
SPHERE STRING DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = SPHERE;
  set_vector(&op[lastop].op.sphere.s.c, $3, $4, $5);
  op[lastop].op.sphere.s.r = $6;
  op[lastop].op.sphere.cs = add_symbol($7,SYM_MATRIX,NULL);
  op[lastop].op.sphere.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  lastop++;
}| 

TORUS DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = TORUS;
  set_vector(&op[lastop].op.torus.t.c, $2, $3, $4);
  op[lastop].op.torus.t.ri = $5;
  op[lastop].op.torus.t.ro = $6;
  op[lastop].op.torus.constants = NULL;
  op[lastop].op.torus.cs = NULL;

  lastop++;
}|
TORUS DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = TORUS;
  set_vector(&op[lastop].op.torus.t.c, $2, $3, $4);
  op[lastop].op.torus.t.ri = $5;
  op[lastop].op.torus.t.ro = $6;
  op[lastop].op.torus.constants = NULL;
  op[lastop].op.torus.cs = add_symbol($7,SYM_MATRIX,NULL);
  lastop++;
}|
TORUS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = TORUS;
  set_vector(&op[lastop].op.torus.t.c, $3, $4, $5);
  op[lastop].op.torus.t.ri = $6;
  op[lastop].op.torus.t.ro = $7;
  op[lastop].op.torus.cs = NULL;
  op[lastop].op.torus.constants = add_symbol($2,SYM_CONSTANTS,NULL);

  lastop++;
}|
TORUS STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = TORUS;
  set_vector(&op[lastop].op.torus.t.c, $3, $4, $5);
  op[lastop].op.torus.t.ri = $6;
  op[lastop].op.torus.t.ro = $7;
  op[lastop].op.torus.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.torus.cs = add_symbol($8,SYM_MATRIX,NULL);

  lastop++;
}| 
BOX DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = BOX;
  set_vector(&op[lastop].op.box.b.c, $2, $3, $4);
  op[lastop].op.box.b.w = $5;
  op[lastop].op.box.b.h = $6;
  op[lastop].op.box.b.d = $7;

  op[lastop].op.box.constants = NULL;
  op[lastop].op.box.cs = NULL;
  lastop++;
}|
BOX DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = BOX;
  set_vector(&op[lastop].op.box.b.c, $2, $3, $4);
  op[lastop].op.box.b.w = $5;
  op[lastop].op.box.b.h = $6;
  op[lastop].op.box.b.d = $7;

  op[lastop].op.box.constants = NULL;
  op[lastop].op.box.cs = add_symbol($8,SYM_MATRIX,NULL);
  lastop++;
}|
BOX STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = BOX;
  set_vector(&op[lastop].op.box.b.c, $3, $4, $5);
  op[lastop].op.box.b.w = $6;
  op[lastop].op.box.b.h = $7;
  op[lastop].op.box.b.d = $8;
  op[lastop].op.box.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.box.cs = NULL;
  lastop++;
}|
BOX STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = BOX;
  set_vector(&op[lastop].op.box.b.c, $3, $4, $5);
  op[lastop].op.box.b.w = $6;
  op[lastop].op.box.b.h = $7;
  op[lastop].op.box.b.d = $8;
  op[lastop].op.box.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.box.cs = add_symbol($9,SYM_MATRIX,NULL);

  lastop++;
}|


LINE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $2, $3, $4);
  set_vector(&op[lastop].op.line.p1, $5, $6, $7);
  op[lastop].op.line.constants = NULL;
  op[lastop].op.line.cs0 = NULL;
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
/* first do cs0, then cs1, then both - BUT NO CONSTANTS */
LINE DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $2, $3, $4);
  set_vector(&op[lastop].op.line.p1, $6, $7, $8);
  op[lastop].op.line.constants = NULL;
  op[lastop].op.line.cs0 = add_symbol($5,SYM_MATRIX,NULL);
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
LINE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $2, $3, $4);
  set_vector(&op[lastop].op.line.p1, $5, $6, $7);
  op[lastop].op.line.constants = NULL;
  op[lastop].op.line.cs0 = NULL;
  op[lastop].op.line.cs1 = add_symbol($8,SYM_MATRIX,NULL);
  lastop++;
}|
LINE DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $2, $3, $4);
  set_vector(&op[lastop].op.line.p1, $6, $7, $8);
  op[lastop].op.line.constants = NULL;
  op[lastop].op.line.cs0 = add_symbol($5,SYM_MATRIX,NULL);
  op[lastop].op.line.cs1 = add_symbol($9,SYM_MATRIX,NULL);
  lastop++;
}|
/* now do constants, and constants with the cs stuff */
LINE STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $3, $4, $5);
  set_vector(&op[lastop].op.line.p1, $6, $7, $8);
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.line.cs0 = NULL;
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
LINE STRING DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $3, $4, $5);
  set_vector(&op[lastop].op.line.p1, $7, $8, $9);
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.line.cs0 = add_symbol($6,SYM_MATRIX,NULL);
  op[lastop].op.line.cs1 = NULL;
  lastop++;
}|
LINE STRING DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $3, $4, $5);
  set_vector(&op[lastop].op.line.p1, $6, $7, $8);
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.line.cs0 = NULL;
  op[lastop].op.line.cs1 = add_symbol($9,SYM_MATRIX,NULL);
  op[lastop].op.line.cs0 = NULL;
  lastop++;
}|
LINE STRING DOUBLE DOUBLE DOUBLE STRING DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = LINE;
  set_vector(&op[lastop].op.line.p0, $3, $4, $5);
  set_vector(&op[lastop].op.line.p1, $7, $8, $9);
  op[lastop].op.line.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.line.cs0 = add_symbol($6,SYM_MATRIX,NULL);
  op[lastop].op.line.cs1 = add_symbol($10,SYM_MATRIX,NULL);
  lastop++;
}|
MESH CO STRING
{
  lineno++;
  op[lastop].opcode = MESH;
  strncpy(op[lastop].op.mesh.name,$3,MAX_NAME);
  op[lastop].op.mesh.name[MAX_NAME] = '\0';
  op[lastop].op.mesh.constants = NULL;
  op[lastop].op.mesh.cs = NULL;
  lastop++;
}|
MESH STRING CO STRING
{ /* name and constants */
  lineno++;
  op[lastop].opcode = MESH;
  strncpy(op[lastop].op.mesh.name,$4,MAX_NAME);
  op[lastop].op.mesh.name[MAX_NAME] = '\0';
  op[lastop].op.mesh.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.mesh.cs = NULL;
  lastop++;
} |
MESH STRING CO STRING STRING
{
  lineno++;
  op[lastop].opcode = MESH;
  strncpy(op[lastop].op.mesh.name,$4,MAX_NAME);
  op[lastop].op.mesh.name[MAX_NAME] = '\0';
  op[lastop].op.mesh.constants = add_symbol($2,SYM_CONSTANTS,NULL);
  op[lastop].op.mesh.cs = add_symbol($5,SYM_MATRIX,NULL);
  lastop++;
} |
SET STRING DOUBLE
{
  lineno++;
  op[lastop].opcode = SET;
  op[lastop].op.set.p = add_symbol($2,SYM_VALUE,NULL);
  set_value(op[lastop].op.set.p,$3);
  lastop++;
}|
SCALE DOUBLE DOUBLE DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = SCALE;
  set_vector(&op[lastop].op.scale.d, $2, $3, $4);
  op[lastop].op.scale.p = add_symbol($5,SYM_VALUE,NULL);
  lastop++;
}|
SCALE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = SCALE;
  set_vector(&op[lastop].op.scale.d, $2, $3, $4);
  op[lastop].op.scale.p = NULL;
  lastop++;
}|
ROTATE STRING DOUBLE STRING
{
  lineno++;
  op[lastop].opcode = ROTATE;
  switch (*$2)
    {
    case 'x':
    case 'X': 
      op[lastop].op.rotate.axis = X;
      break;
    case 'y':
    case 'Y': 
      op[lastop].op.rotate.axis = Y;
      break;
    case 'z':
    case 'Z': 
      op[lastop].op.rotate.axis = Z;
      break;
    default:
      rot_exit(*$2);
    }

  op[lastop].op.rotate.degrees = $3;
  op[lastop].op.rotate.p = add_symbol($4,SYM_VALUE,0);
  
  lastop++;
}|
ROTATE STRING DOUBLE
{
  lineno++;
  op[lastop].opcode = ROTATE;
  switch (*$2)
    {
    case 'x':
    case 'X': 
      op[lastop].op.rotate.axis = X;
      break;
    case 'y':
    case 'Y': 
      op[lastop].op.rotate.axis = Y;
      break;
    case 'z':
    case 'Z': 
      op[lastop].op.rotate.axis = Z;
      break;
    }
  op[lastop].op.rotate.degrees = $3;
  op[lastop].op.rotate.p = NULL;
  lastop++;
}|
BASENAME STRING
{
  lineno++;
  op[lastop].opcode = BASENAME;
  op[lastop].op.basename.p = add_symbol($2,SYM_STRING,NULL);
  lastop++;
}|
SAVE_KNOBS STRING
{
  lineno++;

  k.len = 0;
  for (i = 0; i < lastsym; i++)
    if (symtab[i].type == SYM_VALUE)
      k.len++;

  k.knobs = malloc(k.len * sizeof(*k.knobs) );
  if (k.knobs == NULL)
    err_exit("SAVE_KNOBS");
  
  k.values = malloc(k.len * sizeof(*k.values) );
  if (k.values == NULL)
    err_exit("SAVE_KNOBS");

  j = 0;
  for (i = 0; i < lastsym; i++)
    if (symtab[i].type == SYM_VALUE) {
      k.knobs[j] = symtab + i;
      k.values[j] = ( (SYMTAB*)k.knobs[j])->s.value;
      j++;
    }

  op[lastop].opcode = SAVE_KNOBS;
  op[lastop].op.save_knobs.p = add_symbol($2,SYM_KNOBLIST,&k);
  lastop++;
}|
TWEEN DOUBLE DOUBLE STRING STRING
{
  lineno++;
  op[lastop].opcode = TWEEN;
  op[lastop].op.tween.start_frame = $2;
  op[lastop].op.tween.end_frame = $3;
  op[lastop].op.tween.knob_list0 = add_symbol($4,SYM_KNOBLIST,NULL);
  op[lastop].op.tween.knob_list1 = add_symbol($5,SYM_KNOBLIST,NULL);
  lastop++;
}|
FRAMES DOUBLE
{
  lineno++;
  op[lastop].opcode = FRAMES;
  op[lastop].op.frames.num_frames = $2;
  lastop++;
}|
VARY STRING DOUBLE DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = VARY;
  op[lastop].op.vary.p = add_symbol($2,SYM_STRING,NULL);
  op[lastop].op.vary.start_frame = $3;
  op[lastop].op.vary.end_frame = $4;
  op[lastop].op.vary.start_val = $5;
  op[lastop].op.vary.end_val = $6;
  lastop++;
}|
PUSH
{
  lineno++;
  op[lastop].opcode = PUSH;
  lastop++;
}|
POP
{
  lineno++;
  op[lastop].opcode = POP;
  lastop++;
}|
SAVE STRING
{
  lineno++;
  op[lastop].opcode = SAVE;
  op[lastop].op.save.p = add_symbol($2,SYM_FILE,NULL);
  lastop++;
}|
SHADING SHADING_TYPE
{
  lineno++;
  op[lastop].opcode = SHADING;
  if (!strcmp($2, "wireframe") )
    op[lastop].op.shading.type = WIREFRAME;
  else if (!strcmp($2, "flat") )
    op[lastop].op.shading.type = FLAT;
  else if (!strcmp($2, "goroud") )
    op[lastop].op.shading.type = GOROUD;
  else if (!strcmp($2, "phong") )
    op[lastop].op.shading.type = PHONG;
  else
    shade_exit($2);
  lastop++;
}|
SETKNOBS DOUBLE
{
  lineno++;
  op[lastop].opcode = SETKNOBS;
  op[lastop].op.setknobs.value = $2;
  lastop++;
}|
FOCAL DOUBLE
{
  lineno++;
  op[lastop].opcode = FOCAL;
  op[lastop].op.focal.value = $2;
  lastop++;
}|
DISPLAY
{
  lineno++;
  op[lastop].opcode = DISPLAY;
  lastop++;
}|
AMBIENT DOUBLE DOUBLE DOUBLE
{
  lineno++;
  op[lastop].opcode = AMBIENT;
  op[lastop].op.ambient.c = new_color($2, $3, $4);
  lastop++;
};

%%

/* Other C stuff */

void rot_exit(char axis) {
  fprintf(stderr, "Invalid axis %c; must be x, y, or z\n", axis);
  exit(EXIT_FAILURE);
}

void shade_exit(char *type) {
  fprintf(stderr, "Invalid shading type %s; must be wireframe, flat, goroud, or phong\n", type);
  exit(EXIT_FAILURE);
}

int yyerror(char *s)
{
  fprintf(stderr, "Error in line %d:%s\n",lineno,s);
  exit(EXIT_FAILURE);
  return -1;
}

int yywrap()
{
  return 1;
}

extern FILE *yyin;

int main(int argc, char **argv)
{
  if (argc > 1) {
    yyin = fopen(argv[1],"r");
    if (yyin == NULL)
      err_exit("fopen");
  }
  else {
    fputs("No input file\n", stderr);
    exit(EXIT_FAILURE);
  }
  
  yyparse();
  print_pcode();
  my_main();

  return 0;    
}
