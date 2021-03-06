#ifndef NEGATIVE_H_INCLUDED
#define NEGATIVE_H_INCLUDED

#include "declarations.h"
#include "bskytree.h"
#include "algoglouton.h"
#include "experimentations.h"

namespace NEG{

DataType compteLesMarques(bool* table, DataType length);

void triScore(TableTuple &donnees, Space d);

void visualisation_pairs(vector<USetDualSpace> listUSetDualSpace);

bool pet_pair(const DualSpace &sp1, const DualSpace &sp2 );

inline DualSpace domDualSubspace_1(const Point &t1, const Point &t2, const Space &d);

void insertDualSpaceToUSet(DualSpace sp, const Space &d, USetDualSpace &uSetDualSpace, Space &all, Space &maxDom, Space &sizeMaxDom, bool &sortie);

long creationStructureNSC(NegSkyStrAux &structure, list<vector<ListUSetDualSpace>> &list_Vc_lt_UsDs, Space d);

bool pet(const pair<Space, TableTuple> &p1, const pair<Space, TableTuple> &p2 );

void choixPivot(TableTuple &topmost, Space d);

// down     ,  transform NegSkyStrAux to NegSkyStr,   map -> vector
long negativeSkycube(NegSkyStr &structure, list<vector<ListUSetDualSpace>> &list_Vc_lt_UsDs, Space d);

bool* subspaceSkyline_NSC(NegSkyStr &structure, int size_data, const Space subspace);

bool* subspaceSkylineSize_NSC(NegSkyStr &structure, int size_data, Space subspace);

void displayResult(string dataName, DataType n, Space d, DataType k, string step, long structSize, double timeToPerform);

void skylinequery(string dataName, NegSkyStr &structure0,  int indexedDataSize, Space d, DataType k, vector<Space> subspaceN);

void expiration(ListVectorListUSetDualSpace &listVcUSetDualSpace);

void CompresserParInclusion_cascade(vector<list<DualSpace>> &toCompress, Space d, ListUSetDualSpace & l);

void CompresserParInclusion(list<DualSpace> &l);

void updateNSCt_step1(list<TableTuple> &mainDataset, TableTuple &valid_topmost, int decalage, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d);

void updateNSCt_step1_real_data(list<TableTuple> &mainDataset, TableTuple &valid_topmost, int decalage, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d, vector<int> &blocks_startId);

void updateNSCt_step2(list<TableTuple> &mainDataset, TableTuple &valid_topmost, TableTuple &old_topmost, int decalage, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d, int buffer_size);

void updateNSCt_step2_real_data(list<TableTuple> &mainDataset, TableTuple &valid_topmost, TableTuple &old_topmost, int decalage, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d, vector<int> &blocks_startId);

void InitStructure (list<TableTuple> &mainDataset, TableTuple &topmost, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d, vector<int> &blocks_size);

void InitStructure_real_data (list<TableTuple> &mainDataset, TableTuple &topmost, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d, vector<int> &blocks_size, vector<int> &blocks_startId);

}



#endif
