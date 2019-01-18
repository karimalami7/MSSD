#include "VectorCommon.h"

void scalar_mult(VECTOR *vec, double s)
{
	unsigned int i = 1;
	while (vec->cord[i].id != 0)
	{
		vec->cord[i].val *= s;
		i++;
	}
}

double length(VECTOR *vec)
{
	unsigned int i = 1;
	double sqr_sum = 0.0;

	while (vec->cord[i].id != 0)
	{
		sqr_sum += vec->cord[i].val * vec->cord[i].val;
		i++;
	}

	return (sqrt(sqr_sum));
}


double inner_prod(VECTOR *a, VECTOR *b)
{
	unsigned i = 1, j = 1;
	double sum = 0.0;

	while (a->cord[i].id != 0 && b->cord[j].id != 0)
	{
		while (a->cord[i].id != 0 && a->cord[i].id < b->cord[j].id) i++;
		while (b->cord[j].id != 0 && b->cord[j].id < a->cord[i].id) j++;
		while (a->cord[i].id != 0 && a->cord[i].id == b->cord[j].id)
		{
			sum += a->cord[i].val * b->cord[j].val;
			i++;
			j++;
		}
	}

	return (sum);
}

VECTOR *vec_sum(VECTOR *a, VECTOR *b)
{
	unsigned i = 1, j = 1, k = 0;
	VECTOR *tmp = NULL;
	CORD *cord = NULL;

	while (a->cord[i].id != 0 && b->cord[j].id != 0)
	{
		while (a->cord[i].id != 0 && a->cord[i].id < b->cord[j].id) { i++; k++; }
		while (b->cord[j].id != 0 && b->cord[j].id < a->cord[i].id) { j++; k++; }
		while (a->cord[i].id != 0 && a->cord[i].id == b->cord[j].id) 
		{ 
			i++;
			j++;
			k++; 
		}
	}
	while (a->cord[i].id != 0) { i++; k++; }
	while (b->cord[j].id != 0) { j++; k++; }

	cord = (CORD *)safe_malloc(sizeof(CORD)*(k+2));

	i = j = k = 1;
	while (a->cord[i].id != 0 && b->cord[j].id != 0)
	{
		while (a->cord[i].id != 0 && a->cord[i].id < b->cord[j].id) 
		{ 
			cord[k].id = a->cord[i].id;
			cord[k].val = a->cord[i].val;
			i++;
			k++;
		}
		while (b->cord[j].id != 0 && b->cord[j].id < a->cord[i].id)
		{
			cord[k].id = b->cord[j].id;
			cord[k].val = b->cord[j].val;
			j++;
			k++;
		}
		while (a->cord[i].id != 0 && a->cord[i].id == b->cord[j].id)
		{
			cord[k].id = a->cord[i].id;
			cord[k].val = a->cord[i].val + b->cord[j].val;
			i++;
			j++;
			k++;
		}
	}
	while (a->cord[i].id != 0) 
	{
		cord[k].id = a->cord[i].id;
		cord[k].val = a->cord[i].val;
		i++;
		k++;
	}
	while (b->cord[j].id != 0)
	{
		cord[k].id = b->cord[j].id;
		cord[k].val = b->cord[j].val;
		j++;
		k++;
	}

	cord[k].id = 0;
	tmp = (VECTOR *)safe_malloc(sizeof(VECTOR));
	tmp->cord = cord;
	tmp->next = NULL;

	return (tmp);
}

VECTOR *orthonorm(VECTOR *ulist)
{
	VECTOR *vlist, *v, *u, *iter;
	VECTOR *sum, *tmp;
	double iprod, len_square;

	u = ulist;
	copy_vector(&vlist, u);

	u = ulist->next;
	while (u != NULL)
	{
		copy_vector(&v, vlist);
		iprod = inner_prod(u, v);
		len_square = pow(length(v), 2.0);
		scalar_mult(v, (iprod/len_square));
		sum = v;

		iter = vlist;
		while (iter->next != NULL)
		{
			iter = iter->next;
			copy_vector(&v, iter);
			iprod = inner_prod(u, v);
			len_square = pow(length(v), 2.0);
			scalar_mult(v, (iprod/len_square));
			tmp = sum;
			sum = vec_sum(sum, v);
			free_vector(tmp);
			free_vector(v);
		}
		scalar_mult(sum, -1.0);
		v = vec_sum(u, sum);
		v->next = NULL;
		iter->next = v;
		u = u->next;

		free_vector(sum);
	}

		iter = vlist;
		while (iter != NULL)
		{
			scalar_mult(iter, 1/length(iter));
			iter = iter->next;
		}

	return (vlist);
}

void *safe_malloc(size_t s)
{
	void *tmp;
	tmp = malloc(s);
	if (tmp == NULL)
	{
		printf("malloc failed.\n");
		exit(1);
	}

  return (tmp);
}

void copy_vector(VECTOR **a, VECTOR *b)
{
	unsigned i;
	CORD *cord;

	i = 1;
	while (b->cord[i].id != 0) i++;
	cord = (CORD *)safe_malloc(sizeof(CORD)*(i+1));

	i = 1;
	while (b->cord[i].id != 0)
	{
		cord[i].id = b->cord[i].id;
		cord[i].val = b->cord[i].val;
		i++;
	}
	cord[i].id = 0;
	(*a) = (VECTOR *)safe_malloc(sizeof(VECTOR));
	(*a)->cord = cord;
	(*a)->next = NULL;
}

void free_vector(VECTOR *a)
{
	VECTOR *vec;
	VECTOR *tmp;

	vec = a;
	while (vec != NULL)
	{
		tmp = vec;
		vec = vec->next;
		free(tmp->cord);
		free(tmp);
	}
}

int read_vector(FILE *fp, VECTOR *vec)
{
	unsigned i = 1;
	char *str;
	char buf[128];

	if (fgets(buf, 128, fp) == NULL)
	return EOF;

	str = strtok(buf, " \t");
	while (str != NULL)
	{
		vec->cord[i].id = i;
		vec->cord[i].val = atof(str);
		str = strtok(NULL, " \t");
		i++;
	}
	vec->cord[i].id = 0;
	vec->next = NULL;

	return 1;
}

void print_vector(VECTOR *a)
{
	unsigned i = 1;

	while (a->cord[i].id != 0)
	{
		printf("%f\t", a->cord[i].val);
		i++;
	}
	printf("\n");
}

void print_vector_list(VECTOR *a)
{
	unsigned i;

	while (a != NULL)
	{
		i = 1;
		while (a->cord[i].id != 0)
		{
			printf("%f\t", a->cord[i].val);
			i++;
		}
		printf("\n");
		a = a->next;
	}
}
