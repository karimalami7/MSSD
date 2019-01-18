#ifndef GENERATEUR_H_INCLUDED
#define GENERATEUR_H_INCLUDED

#include "DataGenerator.h"
#include "declarations.h"

void normaliseDonnees(TableTuple &donnees, Space d, TableTuple &sortie);

void readTextPointList(int n, int d, string strFileName, vector<Point> &result);

void dataDoubleToInt(vector<double*> &dataDouble, Space d, DataType k, vector<Point> &result);

void maVisionAnticorrelation(vector<double*> &resultD, int d);


void loadData(string dataName, string path, int n, int d, int &k, vector<Point> &result);

void afficheDonnees(vector<Point> donnees, Space d);

#endif // GENERATEUR_H_INCLUDED
