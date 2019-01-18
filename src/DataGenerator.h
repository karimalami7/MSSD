#ifndef _DATAGENERATOR_H
#define _DATAGENERATOR_H

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "VectorCommon.h"
#include "ValueGenerator.h"

using namespace std;

void GenerateIndependent(int nNumObj, int nNumAtt, bool bDistinct, vector<double*> &result);
void GenerateCorrelated(int nNumObj, int nNumAtt, bool bDistinct, vector<double*> &result);
void GenerateAntiCorrelated(int nNumObj, int nNumAtt, bool bDistinct, vector<double*> &result);

void CheckIndDistinctValue(int nNumAtt, double* dValue, map<double, double>* AttValueMap);
void CheckCorDistinctValue(int nNumAtt, double* dValue, map<double, double>* AttValueMap, VECTOR* Data, VECTOR* Basis);
void CheckAntiDistinctValue(int nNumAtt, double* dValue, map<double, double>* AttValueMap, VECTOR* Data, VECTOR* Basis);

#endif
