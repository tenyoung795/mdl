/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser, 
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  frames: set num_frames (in misc_headers.h) for animation

  basename: set name (in misc_headers.h) for animation

  vary: manipluate knob values between two given frames
        over a specified interval

  set: set a knob to a given value
  
  setknobs: set all knobs to a given value

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix 
                     based on the provided values, then 
		     multiply the current top of the
		     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a 
		    temporary matrix, multiply it by the
		    current top of the origins stack, then
		    call draw_polygons.

  line: create a line based on the provided values. Store 
        that in a temporary matrix, multiply it by the
	current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live
  
  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "misc_headers.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "node.h"
#include "vector.h"
#include "shading.h"
#include "objparser.h"

const char DEFAULT_BASENAME[] = "mdl";
const double STEP = 0.05;
const int SIGNALS[] = {SIGABRT, SIGTERM, SIGINT};
const size_t NUM_SIGNALS = sizeof(SIGNALS) / sizeof(*SIGNALS);

char *basename = DEFAULT_BASENAME;
int num_frames = 1;
struct matrix tmp;
struct matrix mesh;
struct stack s;
vary_list *knobs = NULL;
color ambient = {0, 0, 0};
struct light *light = NULL;
struct vector view = {0, 0, -1};
enum shading_t shading = WIREFRAME;

int process_frame();
void print_knobs();
int process_knobs();
void first_pass();
vary_list *second_pass();
void cleanup();
void sighandler(int);

void cleanup() {
  int i;

  free_matrix(&tmp);
  free_matrix(&mesh);
  free_stack(&s);
  free_vary_list(knobs);
  free(knobs);

  for (i = 0; i < lastsym; i++)
    if (symtab[i].type == SYM_KNOBLIST) {
      free(symtab[i].s.k.knobs);
      free(symtab[i].s.k.values);
    }
}

void sighandler(int signum) {
  cleanup();
  signal(signum, SIG_DFL);
  raise(signum);
}

void err_exit(char *func) {
  perror(func);
  exit(EXIT_FAILURE);
}

/*======== void first_pass()) ==========
  Inputs:   
  Returns: 

  Checks the op array for any animation commands
  (frames, basename, vary)
  
  Should set num_frames and basename if the frames 
  or basename commands are present

  If vary is found, but frames is not, the entire
  program should exit.

  If frames is found, but basename is not, set name
  to some default value, and print out a message
  with the name being used.
  
  05/17/12 09:27:02
  jdyrlandweaver
  ====================*/
enum {
  HAS_FRAMES = 1,
  HAS_BASENAME = 2,
  HAS_VARY = 4,
  HAS_LIGHT = 8,
  HAS_TWEEN = 16,
  HAS_KNOBLIST = 32,
  HAS_TWO_KNOBLISTS = 64,
};

void first_pass() {
  int i;
  int results;
  struct command *c;

  results = 0;

  for (i = 0; i < lastop; i++) {
    c = op + i;
    switch (c->opcode)
      {
      case FRAMES:
	results |= HAS_FRAMES;
	num_frames = c->op.frames.num_frames;
	break;
      case BASENAME:
	results |= HAS_BASENAME;
	basename = c->op.basename.p->name;
	break;
      case VARY:
	results |= HAS_VARY;
	break;
      case TWEEN:
	results |= HAS_TWEEN;
	break;
      case SAVE_KNOBS:
	results |= (results & HAS_KNOBLIST?
		    HAS_TWO_KNOBLISTS : HAS_KNOBLIST);
	break;
      case SHADING:
	shading = c->op.shading.type;
	break;
      case AMBIENT:
	ambient = c->op.ambient.c;
	break;
      case LIGHT:
	light = &c->op.light.p->s.l;
	results |= HAS_LIGHT;
	break;
      case CAMERA:
	view = c->op.camera.aim;
	subtract_vectors(&view, &c->op.camera.eye);
	normalize(&view);
      default:
	break;
      }
  }

  if (results & HAS_VARY && !(results & HAS_FRAMES) ) {
    fputs("Vary command needs a frames command\n", stderr);
    exit(EXIT_FAILURE);
  }

  if (results & HAS_TWEEN &&
      !(results & HAS_FRAMES && results & HAS_TWO_KNOBLISTS) ) {
    fputs("Tween needs frames and two knoblists\n", stderr);
    exit(EXIT_FAILURE);
  }

  if (shading != WIREFRAME && !(results & HAS_LIGHT) ) {
    fputs("Shading needs a light command\n", stderr);
    exit(EXIT_FAILURE);
  }

  if (results & HAS_FRAMES && !(results & HAS_BASENAME) )
    printf("No basename given; will use \"%s\"", DEFAULT_BASENAME);
}


/*======== struct vary_node ** second_pass()) ==========
  Inputs:   
  Returns: An array of vary_node linked lists

  In order to set the knobs for animation, we need to keep
  a seaprate value for each knob for each frame. We can do
  this by using an array of linked lists. Each array index
  will correspond to a frame (eg. knobs[0] would be the first
  frame, knobs[2] would be the 3rd frame and so on).

  Each index should contain a linked list of vary_nodes, each
  node contains a knob name, a value, and a pointer to the
  next node.

  Go through the opcode array, and when you find vary, go 
  from knobs[0] to knobs[frames-1] and add (or modify) the
  vary_node corresponding to the given knob with the
  appropirate value. 

  05/17/12 09:29:31
  jdyrlandweaver
  ====================*/
vary_list *second_pass() {
  vary_list *arr;
  int i;
  int f, sf, ef;
  double dv_df;
  double v, ev;
  struct command *c;
  SYMTAB *sym;
  int k, num_knobs;
  
  arr = malloc(num_frames * sizeof(*arr) );
  if (arr == NULL)
    err_exit("second_pass: malloc");

  for (f = 0; f < num_frames; f++)
    arr[f] = NULL;

  for (i = 0; i < lastop; i++) {
    c = op + i;
    switch(c->opcode) {
    case VARY:
      sym = c->op.vary.p;
      sf = c->op.vary.start_frame;
      ef = c->op.vary.end_frame;
      v = c->op.vary.start_val;
      ev = c->op.vary.end_val;

      if (sf >= ef) {
	fprintf(stderr,
		"Start frame greater than or equal to end frame\n%d:\tvary %s %d %d %f %f\n",
		i, sym->name, sf, ef, v, ev);
	exit(EXIT_FAILURE);
      }
      else if (ef >= num_frames) {
	fprintf(stderr,
		"End frame exceeds number of frames (%d)\n%d:\tvary %s %d %d %f %f\n",
		num_frames, i, sym->name, sf, ef, v, ev);
	exit(EXIT_FAILURE);
      }
      dv_df = (ev - v) / (ef - sf);

      for (f = sf; f <= ef; f++) {
	add_node(arr + f, sym, v);
	v += dv_df;
      }
      break;
    case TWEEN:
      sf = c->op.tween.start_frame;
      ef = c->op.tween.end_frame;

      if (sf >= ef) {
	fprintf(stderr,
		"Start frame greater than or equal to end frame\n%d:\ttween %d %d %s %s\n",
		i, sf, ef,
		c->op.tween.knob_list0->name,
		c->op.tween.knob_list1->name);
	exit(EXIT_FAILURE);
      }
      else if (ef >= num_frames) {
	fprintf(stderr,
		"End frame exceeds number of frames (%d)\n%d:\ttween %d %d %s %s\n",
		num_frames, i, sf, ef,
		c->op.tween.knob_list0->name,
		c->op.tween.knob_list1->name);
	exit(EXIT_FAILURE);
      }

      num_knobs = (c->op.tween.knob_list0->s.k.len < c->op.tween.knob_list1->s.k.len?
		   c->op.tween.knob_list0->s.k.len
		   : c->op.tween.knob_list1->s.k.len);

      for (k = 0; k < num_knobs; k++) {
	sym = c->op.tween.knob_list0->s.k.knobs[k];
	v = c->op.tween.knob_list0->s.k.values[k];
	ev = c->op.tween.knob_list1->s.k.values[k];

	dv_df = (ev - v) / (ef - sf);
	for (f = sf; f <= ef; f++) {
	  add_node(arr + f, sym, v);
	  v += dv_df;
	}

      }
      break;
    case TORUS:
      if (shading != WIREFRAME && c->op.torus.constants == NULL) {
	fprintf(stderr, "Shaded torus is missing constants (line %d)\n", i);
	exit(EXIT_FAILURE);
      }
    case SPHERE:
      if (shading != WIREFRAME && c->op.sphere.constants == NULL) {
	fprintf(stderr, "Shaded sphere is missing constants (line %d)\n", i);
	exit(EXIT_FAILURE);
      }
    case BOX:
      if (shading != WIREFRAME && c->op.box.constants == NULL) {
	fprintf(stderr, "Shaded box is missing constants (line %d)\n", i);
	exit(EXIT_FAILURE);
      }
    case MESH:
      if (shading != WIREFRAME && c->op.mesh.constants == NULL) {
	fprintf(stderr, "Shaded box is missing constants (line %d)\n", i);
	exit(EXIT_FAILURE);
      }
    default:
      break;
    }
  }

  return arr;
}

void print_knobs() {
  
  int i;

  printf( "ID\tNAME\t\tTYPE\t\tVALUE\n" );
  for ( i=0; i < lastsym; i++ ) {

    if ( symtab[i].type == SYM_VALUE ) {
      printf( "%d\t%s\t\t", i, symtab[i].name );

      printf( "SYM_VALUE\t");
      printf( "%6.2f\n", symtab[i].s.value);
    }
  }
}

int process_knobs() {
  
  int i;
  double v;
  int has_set = 0;
  char str[256];

  if ( lastsym == 0 )
    return -1;

  do {
    printf( "Knob List:\n" );
    print_knobs();
    printf( "Enter knob ID to set (-1 to stop): ");
    fgets(str, 256, stdin);
    if (sscanf(str, " %d ", &i) < 1)
      i = -2;

    if ( i >= lastsym || i < -1 || (i != -1 && symtab[i].type != SYM_VALUE) )
      printf( "Invalid entry, please try again.\n" );

    else if ( i != -1 ) {
      printf( "Enter new value for %s: ", symtab[i].name );
      scanf( "%lf", &v );
      symtab[i].s.value = v;
      has_set = 1;
    }
    printf("\n");
    
  } while ( i != -1 );

  return has_set? 0 : -1;
}
     
/*======== void my_main() ==========
  Inputs:   int polygons  
  Returns: 

  This is the main engine of the interpreter, it should
  handle most of the commadns in mdl.

  If frames is not present in the source (and therefore 
  num_frames is 1, then process_knobs should be called.

  If frames is present, the enitre op array must be
  applied frames time. At the end of each frame iteration
  save the current screen to a file named the
  provided basename plus a numeric string such that the
  files will be listed in order, then clear the screen and
  reset any other data structures that need it.

  Important note: you cannot just name your files in 
  regular sequence, like pic0, pic1, pic2, pic3... if that
  is done, then pic1, pic10, pic11... will come before pic2
  and so on. In order to keep things clear, add leading 0s
  to the numeric portion of the name. If you use sprintf, 
  you can use "%0xd" for this purpose. It will add at most
  x 0s in front of a number, if needed, so if used correctly,
  and x = 4, you would get numbers like 0001, 0002, 0011,
  0487

  05/17/12 09:41:35
  jdyrlandweaver
  ====================*/
void my_main() {
  int i, j, f;
  struct vector v, u;
  double val, knob_value;
  tmatrix transform;
  struct screen t;
  struct vary_node *vn;
  char frame_name[MAX_NAME];
  edge e;
  size_t blen, flen;
  char filename[MAX_NAME + sizeof("0000.png")];

  s = EMPTY_STACK;
  tmp = mesh = EMPTY_MATRIX;

  for (i = 0; i < NUM_SIGNALS; i++)
    signal(SIGNALS[i], sighandler);
  atexit(cleanup);

  first_pass();
  
  if ( num_frames > 1 )
    knobs = second_pass();

  new_stack(&s);
  new_matrix(&tmp, 1000);
  new_matrix(&mesh, 667);

  for (f = 0; f < num_frames; f++) {
    if (num_frames > 1)
      apply_vary_list(knobs + f);

    clear_screen(&t, ambient);

    for (i=0;i<lastop;i++) {
  
      switch (op[i].opcode) {
      
      case SET:
	set_value( lookup_symbol( op[i].op.set.p->name ), 
		   op[i].op.set.p->s.value );
	break;

      case SETKNOBS:
	for ( j=0; j < lastsym; j++ ) 
	  if ( symtab[j].type == SYM_VALUE )
	    symtab[j].s.value = op[i].op.setknobs.value;
	break;

      case SPHERE:
	add_sphere(&tmp, &op[i].op.sphere.s, STEP);
	//apply the current top origin
	transform_pts( (op[i].op.sphere.cs == NULL?
			s.data[ s.top ] : op[i].op.sphere.cs->s.m),
		       &tmp );
	draw_polygons(&tmp,
		      &t,
		      shading,
		      (op[i].op.sphere.constants == NULL?
		       NULL : &op[i].op.sphere.constants->s.c),
		      ambient,
		      light,
		      &view);
	tmp.lastcol = 0;
	break;

      case TORUS:
	add_torus(&tmp, &op[i].op.torus.t, STEP);
	transform_pts( (op[i].op.torus.cs == NULL?
			s.data[ s.top ] : op[i].op.torus.cs->s.m),
		       &tmp );
	draw_polygons(&tmp,
		      &t,
		      shading,
		      (op[i].op.torus.constants == NULL?
		       NULL : &op[i].op.torus.constants->s.c),
		      ambient,
		      light,
		      &view);
	tmp.lastcol = 0;
	break;

      case BOX:
	add_box(&tmp, &op[i].op.box.b);
	transform_pts( (op[i].op.box.cs == NULL?
			s.data[ s.top ] : op[i].op.box.cs->s.m),
		       &tmp );
	draw_polygons(&tmp,
		      &t,
		      shading,
		      (op[i].op.box.constants == NULL?
		       NULL : &op[i].op.box.constants->s.c),
		      ambient,
		      light,
		      &view);
	tmp.lastcol = 0;
	break;

      case MESH:
	parse_obj(op[i].op.mesh.name, &tmp, &mesh);
	transform_pts( (op[i].op.mesh.cs == NULL?
			s.data[ s.top ] : op[i].op.mesh.cs->s.m),
		       &tmp );
	draw_polygons(&tmp,
		      &t,
		      shading,
		      (op[i].op.mesh.constants == NULL?
		       NULL : &op[i].op.mesh.constants->s.c),
		      ambient,
		      light,
		      &view);
	tmp.lastcol = mesh.lastcol = 0;
	break;

      case LINE:
	v = op[i].op.line.p0;
	if (op[i].op.line.cs0 != NULL)
	  transform_pt(op[i].op.line.cs0->s.m, &v);

	u = op[i].op.line.p1;
	if (op[i].op.line.cs1 != NULL)
	  transform_pt(op[i].op.line.cs1->s.m, &u);

	set_edge(e, &v, &u);
	add_edge( &tmp, e);
	draw_lines( &tmp, &t, WIREFRAME_COLOR);
	tmp.lastcol = 0;
	break;

      case MOVE:
	//get the factors
	v = op[i].op.move.d;

	if ( op[i].op.move.p != NULL ) {

	  knob_value = lookup_symbol( op[i].op.move.p->name )->s.value;
	  scalar_mult(knob_value, &v);
	}

	make_translate(transform, &v);
	//multiply by the existing origin
	tmatrix_mult( s.data[ s.top ], transform );
	//put the new matrix on the top
	copy_tmatrix( transform, s.data[ s.top ] );
	break;

      case SCALE:
	v = op[i].op.scale.d;
      
	if ( op[i].op.scale.p != NULL ) {

	  knob_value = lookup_symbol( op[i].op.scale.p->name )->s.value;
	  scalar_mult(knob_value, &v);
	}

	make_scale(transform, &v);
	tmatrix_mult( s.data[ s.top ], transform );
	//put the new matrix on the top
	copy_tmatrix( transform, s.data[ s.top ] );
	break;

      case ROTATE:
	val = op[i].op.rotate.degrees * ( M_PI / 180 );

	//if knob exists, scale value accordingly
	if ( op[i].op.rotate.p != NULL ) {

	  knob_value = lookup_symbol( op[i].op.rotate.p->name )->s.value;
	  val *= knob_value;
	}
	
	//get the axis
	switch (op[i].op.rotate.axis) {
	case X:
	  make_rotX(transform, val);
	  break;
	case Y:
	  make_rotY(transform, val);
	  break;
	case Z:
	  make_rotZ(transform, val);
	  break;
	}

	tmatrix_mult( s.data[ s.top ], transform );
	//put the new matrix on the top
	copy_tmatrix( transform, s.data[ s.top ] );
	break;

      case PUSH:
	push( &s );
	break;
      case POP:
	pop( &s );
	break;
      case SAVE:
	save_extension( &t, op[i].op.save.p->name );
	break;
      case DISPLAY:
	display(&t );
	break;
      case SAVE_COORDS:
	copy_tmatrix(s.data[s.top],
		     op[i].op.save_coordinate_system.p->s.m);
	break;
      }
    }
 
    s.top = 0;
    if (num_frames > 1) {
      printf("Creating frame %d\n", f);
      sprintf(filename, "%s%04d.png", basename, f);
      save_extension(&t, filename);
    }
    else if (process_knobs() != -1)
      f--;
  }
}
