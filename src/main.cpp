using namespace std;

#include <unistd.h>

#include "declarations.h"
#include "generateur.h"
#include "experimentations.h"


int main(int argc,char **argv){

	cerr << "***************Start****************" << endl;

	time_t val=1448639227;
    srand (val);

    TableTuple donnees;

    string dataName;
    string path;
    DataType k;
    DataType n;
    Space d;
    int cycles;
    int omega;
    int bufferSize;
    int method;

    int c = 0;
    opterr = 0;

    while ((c = getopt(argc, argv, "f:k:o:c:d:b:m:")) != -1) {
        switch(c){

        case 'f':  
            dataName=string(optarg);
            break;

        case 'k': 
            if (dataName=="ANTI"||dataName=="INDE"||dataName=="CORR"||dataName=="PERS"){
            k=atoi(optarg);
            path="";
            }
            break;
        case 'o':
            omega=atoi(optarg);
            break;    
        case 'c':
            cycles=atoi(optarg);
            break;
        case 'd':
            d=atoi(optarg);
            break;
        case 'b':
            bufferSize=atoi(optarg);
            break;
        case 'm':
            method=atoi(optarg);
            break;
        default:
            if (isprint(optopt))
                fprintf( stderr, "Unknown option `-%c'.\n", optopt);
            return 1;
        }
    }


	loadData(dataName, path, omega*cycles, d, k, donnees);
    
    experimentation_menu(dataName, donnees, d, k, path, omega, bufferSize, method);

    cerr <<"***************End****************"<< endl;

}



