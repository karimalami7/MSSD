#ifndef _VALUEGENERATOR_H_
#define _VALUEGENERATOR_H_

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "Gaussian.h"

#define ROUND(x) { if ((x) < 0.0) (x) = 0; else if ((x) > 1.0) (x) = 1; }

VECTOR *get_basis(unsigned);
void dg_uniform(unsigned, VECTOR *);
void dg_corel(unsigned, VECTOR *, VECTOR *);
void dg_anti(unsigned, VECTOR *, VECTOR *);

#endif
