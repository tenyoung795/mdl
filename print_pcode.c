#include <stdio.h>

#include "shading.h"
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"
#include "vector.h"

void print_pcode()
{
  int i;
  for (i=0;i<lastop;i++)
    {
      printf("%d: ",i);
      switch (op[i].opcode)
	{
	case LIGHT:
	  printf("Light: %s at: %u %u %u",
		 op[i].op.light.p->name,
		 op[i].op.light.p->s.l.c.r, op[i].op.light.p->s.l.c.g,
		 op[i].op.light.p->s.l.c.b);
	  break;
	case AMBIENT:
	  printf("Ambient: %u %u %u",
		 op[i].op.ambient.c.r,
		 op[i].op.ambient.c.g,
		 op[i].op.ambient.c.b);
	  break;

	case CONSTANTS:
	  printf("Constants: %s",op[i].op.constants.p->name);
	  break;
	case SAVE_COORDS:
	  printf("Save Coords: %s",op[i].op.save_coordinate_system.p->name);
	  break;
	case CAMERA:
	  printf("Camera: eye: %6.2f %6.2f %6.2f\taim: %6.2f %6.2f %6.2f",
		 op[i].op.camera.eye.x, op[i].op.camera.eye.y,
		 op[i].op.camera.eye.z,
		 op[i].op.camera.aim.x, op[i].op.camera.aim.y,
		 op[i].op.camera.aim.z);

	  break;
	case SPHERE:
	  printf("Sphere: %6.2f %6.2f %6.2f r=%6.2f",
		 op[i].op.sphere.s.c.x,op[i].op.sphere.s.c.y,
		 op[i].op.sphere.s.c.z,
		 op[i].op.sphere.s.r);
	  if (op[i].op.sphere.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.sphere.constants->name);
	    }
	  if (op[i].op.sphere.cs != NULL)
	    {
	      printf("\tcs: %s",op[i].op.sphere.cs->name);
	    }

	  break;
	case TORUS:
	  printf("Torus: %6.2f %6.2f %6.2f r0=%6.2f r1=%6.2f",
		 op[i].op.torus.t.c.x,op[i].op.torus.t.c.y,
		 op[i].op.torus.t.c.z,
		 op[i].op.torus.t.ri,op[i].op.torus.t.ro);
	  if (op[i].op.torus.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.torus.constants->name);
	    }
	  if (op[i].op.torus.cs != NULL)
	    {
	      printf("\tcs: %s",op[i].op.torus.cs->name);
	    }

	  break;
	case BOX:
	  printf("Box: d0: %6.2f %6.2f %6.2f d1: %6.2f %6.2f %6.2f",
		 op[i].op.box.b.c.x,op[i].op.box.b.c.y,
		 op[i].op.box.b.c.z,
		 op[i].op.box.b.w,op[i].op.box.b.h,
		 op[i].op.box.b.d);
	  if (op[i].op.box.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.box.constants->name);
	    }
	  if (op[i].op.box.cs != NULL)
	    {
	      printf("\tcs: %s",op[i].op.box.cs->name);
	    }

	  break;
	case LINE:
	  printf("Line: from: %6.2f %6.2f %6.2f to: %6.2f %6.2f %6.2f",
		 op[i].op.line.p0.x,op[i].op.line.p0.y,
		 op[i].op.line.p0.z,
		 op[i].op.line.p1.x,op[i].op.line.p1.y,
		 op[i].op.line.p1.z);
	  if (op[i].op.line.constants != NULL)
	    {
	      printf("\n\tConstants: %s",op[i].op.line.constants->name);
	    }
	  if (op[i].op.line.cs0 != NULL)
	    {
	      printf("\n\tCS0: %s",op[i].op.line.cs0->name);
	    }
	  if (op[i].op.line.cs1 != NULL)
	    {
	      printf("\n\tCS1: %s",op[i].op.line.cs1->name);
	    }
	  break;
	case MESH:
	  printf("Mesh: filename: %s",op[i].op.mesh.name);
	  if (op[i].op.mesh.constants != NULL)
	    {
	      printf("\tconstants: %s",op[i].op.mesh.constants->name);
	    }
	  break;
	case SET:
	  printf("Set: %s %6.2f",
		 op[i].op.set.p->name,
		 op[i].op.set.p->s.value);
	  break;
	case MOVE:
	  printf("Move: %6.2f %6.2f %6.2f",
		 op[i].op.move.d.x,op[i].op.move.d.y,
		 op[i].op.move.d.z);
	  if (op[i].op.move.p != NULL)
	    {
	      printf("\tknob: %s",op[i].op.move.p->name);
	    }
	  break;
	case SCALE:
	  printf("Scale: %6.2f %6.2f %6.2f",
		 op[i].op.scale.d.x,op[i].op.scale.d.y,
		 op[i].op.scale.d.z);
	  if (op[i].op.scale.p != NULL)
	    {
	      printf("\tknob: %s",op[i].op.scale.p->name);
	    }
	  break;
	case ROTATE:
	  printf("Rotate: axis: %d degrees: %6.2f",
		 op[i].op.rotate.axis,
		 op[i].op.rotate.degrees);
	  if (op[i].op.rotate.p != NULL)
	    {
	      printf("\tknob: %s",op[i].op.rotate.p->name);
	    }
	  break;
	case BASENAME:
	  printf("Basename: %s",op[i].op.basename.p->name);
	  break;
	case SAVE_KNOBS:
	  printf("Save knobs: %s",op[i].op.save_knobs.p->name);
	  break;
	case TWEEN:
	  printf("Tween: %d %d, %s %s",
		 op[i].op.tween.start_frame,
		 op[i].op.tween.end_frame,
		 op[i].op.tween.knob_list0->name,
		 op[i].op.tween.knob_list1->name);
	  break;
	case FRAMES:
	  printf("Num frames: %d",op[i].op.frames.num_frames);
	  break;
	case VARY:
	  printf("Vary: %d %d, %4.0f %4.0f",
		 op[i].op.vary.start_frame,
		 op[i].op.vary.end_frame,
		 op[i].op.vary.start_val,
		 op[i].op.vary.end_val);
	  break;
	case PUSH:
	  printf("Push");
	  break;
	case POP:
	  printf("Pop");
	  break;
	case SAVE:
	  printf("Save: %s",op[i].op.save.p->name);
	  break;
	case SHADING:
	  switch (op[i].op.shading.type) {
	  case WIREFRAME:
	    printf("Shading: wireframe");
	    break;
	  case FLAT:
	    printf("Shading: flat");
	    break;
	  case GOROUD:
	    printf("Shading: goroud");
	    break;
	  case PHONG:
	    printf("Shading: phong");
	    break;
	  }
	  break;
	case SETKNOBS:
	  printf("Setknobs: %f",op[i].op.setknobs.value);
	  break;
	case FOCAL:
	  printf("Focal: %f",op[i].op.focal.value);
	  break;
	case DISPLAY:
	  printf("Display");
	  break;
    }
      printf("\n");
    }
}


