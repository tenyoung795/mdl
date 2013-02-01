#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ml6.h"
#include "vector.h"
#include "parser.h"
#include "symtab.h"
#include "matrix.h"
#include "misc_headers.h"

SYMTAB symtab[MAX_SYMBOLS];
int lastsym = 0;


void print_constants(struct constants *p)
{
  printf("\tRed -\t  Ka: %6.2f Kd: %6.2f Ks: %6.2f\n",
	 p->r[Ka],p->r[Kd],p->r[Ks]);

  printf("\tGreen -\t  Ka: %6.2f Kd: %6.2f Ks: %6.2f\n",
	 p->g[Ka],p->g[Kd],p->g[Ks]);

  printf("\tBlue -\t  Ka: %6.2f Kd: %6.2f Ks: %6.2f\n",
	 p->b[Ka],p->b[Kd],p->b[Ks]);

  printf("Red - %u\tGreen - %u\tBlue - %u\n",
	 p->e.r,p->e.g,p->e.b);
}


void print_light(struct light *p)
{
  printf("Location -\t %6.2f %6.2f %6.2f\n",
	p->l.x,p->l.y,p->l.z);

  printf("Brightness -\t r:%u g:%u b:%u\n",
	p->c.r,p->c.g,p->c.b);
}

void print_knoblist(struct knoblist *k) {
  int i;
  for (i = 0; i < k->len; i++)
    printf("%6.2f ", k->values[i]);
  putchar('\n');
}


void print_symtab()
{
  int i;
  for (i=0; i < lastsym;i++)
    {
      printf("Name: %s\n",symtab[i].name);
      switch (symtab[i].type)
	{
	case SYM_MATRIX:
	  printf("Type: SYM_MATRIX\n");
	  print_tmatrix(symtab[i].s.m);
	  break;
	case SYM_CONSTANTS:
	  printf("Type: SYM_CONSTANTS\n");
	  print_constants(&symtab[i].s.c);
	  break;
	case SYM_LIGHT:
	  printf("Type: SYM_LIGHT\n");
	  print_light(&symtab[i].s.l);
	  break;
	case SYM_VALUE:
	  printf("Type: SYM_VALUE\n");
	  printf("Value: %6.2f\n", symtab[i].s.value);
	  break;
	case SYM_FILE:
	  printf("Type: SYM_FILE\n");
	  printf("Name: %s\n",symtab[i].name);
	  break;
	case SYM_STRING:
	  printf("Type: SYM_FILE\n");
	  printf("String: %s\n",symtab[i].name);
	  break;
	case SYM_KNOBLIST:
	  printf("Type: SYM_KNOBLIST\n");
	  print_knoblist(&symtab[i].s.k);
	  break;
	}
      
      printf("\n");
    }
}

SYMTAB *add_symbol(char *name, enum sym_t type, void *data)
{
  SYMTAB *t;
  size_t len;

  t = (SYMTAB *)lookup_symbol(name);
  if (t==NULL)
    {
      if (lastsym >= MAX_SYMBOLS)
	{
	  return NULL;
	}
      t = (SYMTAB *)&(symtab[lastsym]);
      lastsym++;
    }
  else
    {
      return t;
    }

  strncpy(t->name,name, MAX_NAME);
  t->name[MAX_NAME] = '\0';
  t->type = type;
  switch (type)
    {
    case SYM_CONSTANTS:
      len = sizeof(t->s.c);
      break;
    case SYM_MATRIX:
      len = sizeof(t->s.m);
      break;
    case SYM_LIGHT:
      len = sizeof(t->s.l);
      break;
    case SYM_KNOBLIST:
      len = sizeof(t->s.k);
      break;
    case SYM_VALUE:
      t->s.value = (double)(int)data;
    case SYM_FILE:
    case SYM_STRING:
      len = 0;
      break;
    }
  if (data != NULL && len)
    memcpy(&t->s, data, len);
  return (SYMTAB *)&(symtab[lastsym-1]);
}


SYMTAB *lookup_symbol(char *name)
{
  int i;
  for (i=0;i<lastsym;i++)
    {
      if (!strncmp(name,symtab[i].name, MAX_NAME + 1))
	{
	  return (SYMTAB *) &(symtab[i]);
	}
    }
  return (SYMTAB *)NULL;
}

void set_value(SYMTAB *p, double value)
{
  p->s.value = value;
}


