#include "DataGenerator.h"
#include <stdlib.h>
using namespace std;



void GenerateIndependent(int nNumObj, int nNumAtt, bool bDistinct, vector<double*> &result)
{
	//cout<<"je rentre\n";
	vector<double*> TotalData;

	map<double, double>* AttValueMap = new map<double, double>[nNumAtt+1];
	for (int nObjID = 0; nObjID < nNumObj; nObjID++)
	{
		double * dValue = new double[nNumAtt+1];

		dValue[0] = nObjID;
		for (int nAttID = 1; nAttID <= nNumAtt; nAttID++)
		{
			do
			{
				dValue[nAttID] = (double)rand() / RAND_MAX;
				//dValue[nAttID]= (int) rand()% nNumObj;
			} while (dValue[nAttID] == 0.0);
		}
/*
		if (bDistinct)
			CheckIndDistinctValue(nNumAtt, dValue, AttValueMap);
*/
		TotalData.push_back(dValue);
	}

	delete[] AttValueMap;

	result.swap(TotalData);
}




void GenerateCorrelated(int nNumObj, int nNumAtt, bool bDistinct, vector<double*> &result)
{
	vector<double*> TotalData;

	VECTOR *data, *basis;
	CORD *cord;

	map<double, double>* AttValueMap = new map<double, double>[nNumAtt+1];

	data = (VECTOR *)safe_malloc(sizeof(VECTOR));
	cord = (CORD *)safe_malloc(sizeof(CORD)*(nNumAtt+2));
	data->cord = cord;
	data->next = NULL;

	basis = get_basis(nNumAtt);

	for (int i = 0; i < nNumObj; i++)
	{
		double * dValue = new double[nNumAtt+1];
		dValue[0] = i;
		dg_corel(nNumAtt, basis, data);

		int j = 1;
		while (data->cord[j].id != 0)
		{
			dValue[j] = data->cord[j].val;
			j++;
		}

		for (int k=1; k<=nNumAtt; k++)
		{
			if (dValue[k] == 0.0)
				dValue[k] += 0.00001;
			else if (dValue[k] == 1.0)
				dValue[k] -= 0.00001;
		}

		if (bDistinct)
			CheckCorDistinctValue(nNumAtt, dValue, AttValueMap, data, basis);

		TotalData.push_back(dValue);
	}

	free(cord);
	free(data);

	delete[] AttValueMap;

	result.swap(TotalData);
}



void GenerateAntiCorrelated(int nNumObj, int nNumAtt, bool bDistinct, vector<double*> &result)
{
	vector<double*> TotalData;

	VECTOR *data, *basis;
	CORD *cord;

	map<double, double>* AttValueMap = new map<double, double>[nNumAtt+1];

	data = (VECTOR *)safe_malloc(sizeof(VECTOR));
	cord = (CORD *)safe_malloc(sizeof(CORD)*(nNumAtt+2));
	data->cord = cord;
	data->next = NULL;

	basis = get_basis(nNumAtt);

	for (int i = 0; i < nNumObj; i++)
	{
		double * dValue = new double[nNumAtt+1];
		dValue[0] = i;
		dg_anti(nNumAtt, basis, data);

		int j = 1;
		while (data->cord[j].id != 0)
		{
			dValue[j] = data->cord[j].val;
			j++;
		}

		for (int k=1; k<=nNumAtt; k++)
		{
			if (dValue[k] == 0.0)
				dValue[k] += 0.00001;
			else if (dValue[k] == 1.0)
				dValue[k] -= 0.00001;
		}

		if (bDistinct)
			CheckAntiDistinctValue(nNumAtt, dValue, AttValueMap, data, basis);

		TotalData.push_back(dValue);
	}

	free(cord);
	free(data);

	delete[] AttValueMap;

	result.swap(TotalData);
}





void CheckIndDistinctValue(int nNumAtt, double* dValue, map<double, double>* AttValueMap)
{
	for (int nAttID = 1; nAttID <= nNumAtt; nAttID++)
	{
		if (dValue[nAttID] == 1.0)
		{
			do
			{
				dValue[nAttID] = (double)rand() / ((double)(RAND_MAX) + (double)(1));
			} while (dValue[nAttID] == 0.0);
		}

		while (AttValueMap[nAttID].find(dValue[nAttID]) != AttValueMap[nAttID].end())
			dValue[nAttID] += 0.00000001;

		AttValueMap[nAttID].insert(pair<double, double>(dValue[nAttID], dValue[nAttID]));
	}
}


void CheckCorDistinctValue(int nNumAtt, double* dValue, map<double, double>* AttValueMap, VECTOR* Data, VECTOR* Basis)
{
	while (1)
	{
		bool bCheck = true;
		for (int nAttID = 1; nAttID <= nNumAtt; nAttID++)
		{
			if (AttValueMap[nAttID].find(dValue[nAttID]) != AttValueMap[nAttID].end())
			{
				bCheck = false;
				break;
			}
		}

		if (bCheck)
		{
			for (int nAttID = 1; nAttID <= nNumAtt; nAttID++)
				AttValueMap[nAttID].insert(pair<double, double>(dValue[nAttID], dValue[nAttID]));

			break;
		}

		dg_corel(nNumAtt, Basis, Data);

		int j = 1;
		while (Data->cord[j].id != 0)
		{
			dValue[j] = Data->cord[j].val;
			j++;
		}

		for (int k=1; k<=nNumAtt; k++)
		{
			if (dValue[k] == 0.0)
				dValue[k] += 0.00001;
			else if (dValue[k] == 1.0)
				dValue[k] -= 0.00001;
		}
	}
}
void CheckAntiDistinctValue(int nNumAtt, double* dValue, map<double, double>* AttValueMap, VECTOR* Data, VECTOR* Basis)
{
	while (1)
	{
		bool bCheck = true;
		for (int nAttID = 1; nAttID <= nNumAtt; nAttID++)
		{
			if (AttValueMap[nAttID].find(dValue[nAttID]) != AttValueMap[nAttID].end())
			{
				bCheck = false;
				break;
			}
		}

		if (bCheck)
		{
			for (int nAttID = 1; nAttID <= nNumAtt; nAttID++)
				AttValueMap[nAttID].insert(pair<double, double>(dValue[nAttID], dValue[nAttID]));

			break;
		}

		dg_anti(nNumAtt, Basis, Data);

		int j = 1;
		while (Data->cord[j].id != 0)
		{
			dValue[j] = Data->cord[j].val;
			j++;
		}

		for (int k=1; k<=nNumAtt; k++)
		{
			if (dValue[k] == 0.0)
				dValue[k] += 0.00001;
			else if (dValue[k] == 1.0)
				dValue[k] -= 0.00001;
		}
	}
}
