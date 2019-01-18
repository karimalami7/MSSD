#ifndef _GAUSSIAN_H
#define _GAUSSIAN_H

#include "VectorCommon.h"
#include <math.h>
#include <stdlib.h>

void m_uniform(double min, double max, unsigned dim, VECTOR *vec);
void m_gaussian(double mu, double sigma, unsigned dim, VECTOR *vec);
double gaussian(double mu, double sigma);
double uniform(double min, double max);


#endif
