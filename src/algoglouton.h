#ifndef ALGOGLOUTON_H_INCLUDED
#define ALGOGLOUTON_H_INCLUDED

#include "declarations.h"

bool operator<(const DualSpace &a, const DualSpace &b);

void listeAttributsPresents(Space subspace, Space d, vector<Space> &result);

void listSousEspacesR(vector<Space> &listSubspace, Space actualSpace, vector<Space> &missingAttrib, Space nbDiscarded);


void inline listSousEspaces(vector<Space> &listSubspace, Space &originSpace, const Space &d);


void inline listCouvertsR(Space actualSpace, vector<Space> &missingAttrib, Space nbDiscarded, vector<Space> &listEquSubspace, vector<Space> &listSpace);

void listCouverts(Space domSpace, Space equSpace, Space &d, vector<Space> &listSpace);


void set_of_bits(int i, 	vector<int> &V);

void mis(vector<int> &f, int i, vector<vector<int> > &V1);

void vect_to_int(vector<int> &V, int &i);


void vect_of_vect_to_vect_of_int (vector<vector<int> > &v1, vector<int> &v2);

void deleteEmptyEntries(vector<Space> &poidsDualSpace, vector<DataType> &indiceValide);

void fusionGloutonne(list<DualSpace> &uSetDualSpace, Space d);



#endif // ALGOGLOUTON_H_INCLUDED
