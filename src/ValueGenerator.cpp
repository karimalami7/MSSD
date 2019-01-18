#include "ValueGenerator.h"


VECTOR *get_basis(unsigned dim)
{
	unsigned i;
	VECTOR *ulist=NULL, *vlist, *u, *prev;
	CORD *cord = NULL;

	prev = NULL;
	for (i = 2; i <= dim; i++)
	{
		cord = (CORD *)safe_malloc(sizeof(CORD)*4);
		cord[1].id = 1;
		cord[1].val = -1;
		cord[2].id = i;
		cord[2].val = 1;
		cord[3].id = 0;

		u = (VECTOR *)safe_malloc(sizeof(VECTOR));
		u->cord = cord;
		u->next = NULL;

		if (prev == NULL)
		{
		  ulist = u;
		  prev = u;
		}
		else
		{
		  prev->next = u;
		  prev = u;
		}
	}

	vlist = orthonorm(ulist);
	free_vector(ulist);

	return (vlist);
}

void dg_uniform(unsigned dim, VECTOR *data)
{
  m_uniform(0, 1, dim, data);
}

void dg_corel(unsigned dim, VECTOR *basis, VECTOR *data)
{
	unsigned i;
	double r, sigma;
	VECTOR *diag_vec, *vec, *sum, *tmp;
	CORD *diag_cord;

	diag_vec = (VECTOR *)safe_malloc(sizeof(VECTOR));
	diag_vec->next = NULL;
	diag_cord = (CORD *)safe_malloc(sizeof(CORD)*(dim+2));

	r = gaussian(0.5, 0.16);
	ROUND(r);
	sigma = 0.16 * (1 - 2 * fabs(0.5 - r));

	for (i = 1; i <= dim; i++)
	{
		diag_cord[i].id = i;
		diag_cord[i].val = r;
	}
	diag_cord[i].id = 0;
	diag_vec->cord = diag_cord;

	copy_vector(&vec, basis);
	scalar_mult(vec, gaussian(0, sigma));
	sum = vec;

	while (basis->next != NULL)
	{
		basis = basis->next;
		copy_vector(&vec, basis);
		scalar_mult(vec, gaussian(0, sigma));
		tmp = sum;
		sum = vec_sum(sum, vec);
		free_vector(tmp);
		free_vector(vec);
	}

	tmp = sum;
	sum = vec_sum(sum, diag_vec);
	free_vector(tmp);
	free_vector(diag_vec);

	for (i = 1; i <= dim; i++)
		ROUND(sum->cord[i].val);

	i = 1;
	while (sum->cord[i].id != 0)
	{
		data->cord[i].id = sum->cord[i].id;
		data->cord[i].val = sum->cord[i].val;
		i++;
	}
	data->cord[i].id = 0;
	free_vector(sum);
}


void dg_anti(unsigned dim, VECTOR *basis, VECTOR *data)
{
	unsigned i;
	double r;
	double m = 0.5, v = 0.055, c = 0.65;
	VECTOR *diag_vec, *vec, *sum, *tmp;
	CORD *diag_cord;

	diag_vec = (VECTOR *)safe_malloc(sizeof(VECTOR));
	diag_vec->next = NULL;
	diag_cord = (CORD *)safe_malloc(sizeof(CORD)*(dim+2));

	r = gaussian(m, v);
	ROUND(r);

	for (i = 1; i <= dim; i++)
	{
		diag_cord[i].id = i;
		diag_cord[i].val = r;
	}
	diag_cord[i].id = 0;
	diag_vec->cord = diag_cord;

	copy_vector(&vec, basis);
	scalar_mult(vec, uniform(-c, c));
	sum = vec;

	while (basis->next != NULL)
	{
		basis = basis->next;
		copy_vector(&vec, basis);
		scalar_mult(vec, uniform(-c, c));
		tmp = sum;
		sum = vec_sum(sum, vec);
		free_vector(tmp);
		free_vector(vec);
	}

	tmp = sum;
	sum = vec_sum(sum, diag_vec);
	free_vector(tmp);
	free_vector(diag_vec);

	for (i = 1; i <= dim; i++)
		ROUND(sum->cord[i].val);

	i = 1;
	while (sum->cord[i].id != 0)
	{
		data->cord[i].id = sum->cord[i].id;
		data->cord[i].val = sum->cord[i].val;
		i++;
	}
	data->cord[i].id = 0;
	free_vector(sum);
}


