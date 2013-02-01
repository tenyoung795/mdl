#ifndef SYMTAB_H
#define SYMTAB_H

#include "matrix.h"
#include "vector.h"
#include "ml6.h"

enum {MAX_SYMBOLS = 512, MAX_NAME = 255};

enum {Ka, Kd, Ks};

enum sym_t {
  SYM_MATRIX,
  SYM_VALUE,
  SYM_CONSTANTS,
  SYM_LIGHT,
  SYM_FILE,
  SYM_STRING,
  SYM_KNOBLIST
};

struct constants 
{
  double r[3];
  double g[3];
  double b[3];
  color e; /* emissive color */
};

struct light
{
  struct vector l;
  color c;
};

struct knoblist {
  void **knobs;
  double *values;
  int len;
};

typedef struct 
{
  char name[MAX_NAME + 1];
  int type;
  union{
    tmatrix m;
    struct constants c;
    struct light l;
    struct knoblist k;
    double value;
  } s;
} SYMTAB;

extern SYMTAB symtab[MAX_SYMBOLS];
extern int lastsym;

SYMTAB *lookup_symbol(char *name);
SYMTAB *add_symbol(char *name, enum sym_t type, void *data);
void print_constants(struct constants *p);
void print_light(struct light *p);
void print_knoblist(struct knoblist *k);
void print_symtab();
void set_value(SYMTAB *p, double value);

#endif
