#include "generateur.h"
#include <stdlib.h>
using namespace std;



void normaliseDonnees(TableTuple &donnees, Space d, TableTuple &sortie){
//  normalise un tableau de données
//  une valeur est remplacée par le nombre de valeurs qui lui sont meilleures
//  il s'agit des fréquences cumulées (conserve l'ordre)
    //map<DataType,Freq>* distrib=getFrequences(donnees, d);
    DataType i, n=donnees.size();
    Space j;
    Point p;
    TableTuple result;

    for (i=0;i<n;i++){
        p=new DataType[d+1];
        p[0]=donnees[i][0];
        for (j=1;j<=d;j++) p[j]=donnees[i][j]+1;//(distrib[j-1][donnees[i][j]]).F;
        result.push_back(p);
    }
    //delete[] distrib;
    sortie.swap(result);
}

void readTextPointList(int n, int d, string strFileName, vector<Point> &result){
	vector<Point> PointList;
	string strLine;
	ifstream inFile (strFileName.c_str());
	int i=0, j, aa;
	if (inFile.is_open()){
		while (!inFile.eof() && i<n){
			getline (inFile, strLine,'\n');
			if(strLine.size()!=0){
				stringstream sin (stringstream::in | stringstream::out);
				sin << strLine;
				Point Value = new DataType[d+1];
				Value[0]= i++;
				for (j = 1; j <= d; j++){
					sin >> aa;
					Value[j]=abs(aa);
				}
				sin.clear();
				PointList.push_back(Value);
			}
		}

	} else cout << "There is no File." << endl;
	inFile.close();
	result.swap(PointList);
}

void readTextPointList(int d, string strFileName, vector<Point> &result, int start_id){
    //cout << "reading file: "<<strFileName<<endl;
    vector<Point> PointList;
    string strLine;
    ifstream inFile (strFileName.c_str());
    int i=start_id, j, aa;
    if (inFile.is_open()){
        while (!inFile.eof()){
            getline (inFile, strLine,'\n');
            if(strLine.size()!=0){
                stringstream sin (stringstream::in | stringstream::out);
                sin << strLine;
                Point Value = new DataType[d+1];
                Value[0]= i++;
                for (j = 1; j <= d; j++){
                    sin >> aa;
                    Value[j]=abs(aa);
                }
                sin.clear();
                PointList.push_back(Value);
            }
        }

    } else cout << "There is no File." << endl;
    inFile.close();
    result.swap(PointList);
}

void dataDoubleToInt(vector<double*> &dataDouble, Space d, DataType k, vector<Point> &result){
    DataType i, n=dataDouble.size();
    Point unTuple;
    Space j;
    vector<Point> sortie;
    for (i=0;i<n;i++){
        unTuple=new DataType[d+1];
        unTuple[0]=dataDouble[i][0];
        for (j=1;j<=d;j++) unTuple[j]=(DataType)(dataDouble[i][j]*k+1);
        sortie.push_back(unTuple);
    }
    result.swap(sortie);
}

void maVisionAnticorrelation(vector<double*> &resultD, int d){
    int n=resultD.size();
    int i, j;
    for (i=0;i<n;i++) for (j=1;j<=d;j+=2) resultD[i][j]=1-resultD[i][j];
}


void loadData(string dataName, string path, int n, int d, int &k, vector<Point> &result){
    bool gen=false;
    vector<double*> resultD;
    vector<Point> resultAux;
         if (dataName=="ANTI") {GenerateAntiCorrelated(n, d, true, resultD);gen=true;}
    else if (dataName=="INDE") {GenerateIndependent(n, d, true, resultD);gen=true;}
    else if (dataName=="CORR") {GenerateCorrelated(n, d, true, resultD);gen=true;}
    else if (dataName=="PERS") {GenerateCorrelated(n, d, true, resultD);maVisionAnticorrelation(resultD, d);gen=true;}
    if (gen) dataDoubleToInt(resultD, d, k, result);
    else{
        k=0;
        readTextPointList(n, d, path, resultAux);
        normaliseDonnees(resultAux, d, result);
    }
}

void afficheDonnees(vector<Point> donnees, Space d){
    DataType i, n=donnees.size();
    Space j;
    for (i=0;i<n;i++){
        //cout<<"t"<<donnees[i][0]+1<<": ";
        for (j=1;j<=d;j++){
            cout<<donnees[i][j]<<" ";
        }
        cout<<endl;
    }
}