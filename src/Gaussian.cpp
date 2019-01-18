#include "Gaussian.h"

void m_uniform(double min, double max, unsigned dim, VECTOR *vec)
{
  unsigned i;

  for (i = 1; i <= dim; i++)
    {
      vec->cord[i].id = i;
      vec->cord[i].val = uniform(min, max);
    }
  vec->cord[i].id = 0;
}

void m_gaussian(double mu, double sigma, unsigned dim, VECTOR *vec)
{
  unsigned i;

  for (i = 1; i <= dim; i++)
    {
      vec->cord[i].id = i;
      vec->cord[i].val = gaussian(mu, sigma);
    }
}

double gaussian(double mu, double sigma)
{
  double v1, v2, x, s;

  do
  {
    v1 = 2*uniform(0.0, 1.0) - 1;
    v2 = 2*uniform(0.0, 1.0) - 1;
    s = v1*v1 + v2*v2;
  } while (s >= 1.0);

  x = v1*sqrt(-2.0 * log(s)/s);
  x = mu + x*sigma;

  return (x);
}


double uniform(double min, double max)
{
	double u, r;

	r = rand() / (RAND_MAX+1.0);
	u = min + (max-min)*r;

	return u;
}
