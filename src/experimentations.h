#ifndef EXPERIMENTATIONS_H_INCLUDED
#define EXPERIMENTATIONS_H_INCLUDED

#include "negative.h"
#include "declarations.h"
#include "generateur.h"
#include "bskytree.h"

void Experiment_NSCt(string dataName, int omega, int bufferMaxSize, TableTuple &donnees, Space d, int k, string path, vector<Space> &subspaceN, vector<vector<Space>> &vectSpaceN);

void Experiment_DBSky(string dataName, int omega, int bufferMaxSize, TableTuple &donnees, Space d, int k, string path, vector<Space> &subspaceN, vector<vector<Space>> &vectSpaceN);

void experimentation_TREE(string dataName, TableTuple donnees, Space d, DataType k, vector<vector<Space>> &vectSpaceN);

void experimentation_menu(string dataName, TableTuple &donnees, Space d, int k, string path, int omega, int bufferSize, int method);

void top_k(string dataName, ListVectorListUSetDualSpace &ltVcLtUsDs, int valid_data_size, DataType k, Space d);

void top_k_v2(string dataName, NegSkyStr &structureNSC, int valid_data_size, DataType k, Space d);

#endif