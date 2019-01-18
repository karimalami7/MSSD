#ifndef _VECTORCOMMON_H_
#define _VECTORCOMMON_H_

#include <stdio.h>
#include <sys/types.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FALSE 0
#define TRUE 1

typedef struct _CORD {
  unsigned id;
  double val;
} CORD;

typedef struct _VECTOR {
  CORD *cord;
  struct _VECTOR *next;
} VECTOR;

void scalar_mult(VECTOR *, double);
double length(VECTOR *);
double inner_prod(VECTOR *, VECTOR *);
VECTOR *vec_sum(VECTOR *, VECTOR *);
VECTOR *orthonorm(VECTOR *);
void *safe_malloc(size_t s);
void copy_vector(VECTOR **, VECTOR *);
void free_vector(VECTOR *);
int read_vector(FILE *, VECTOR *);
void print_vector(VECTOR *);
void print_vector_list(VECTOR *);

#endif