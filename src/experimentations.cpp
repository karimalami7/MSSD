#include "experimentations.h"

int NB_THREADS=24;

void displayResult(string dataName, DataType n, Space d, DataType k, string step, long structSize, double timeToPerform, string method){
    cerr<<dataName<<" "<<n<<" "<<d<<" "<<k<<" "<<method<<" "<<step<<" "<<structSize<<" "<<timeToPerform<<endl;
}



void experimentation_TREE(string dataName, TableTuple donnees, Space d, DataType k, vector<vector<Space>> &vectSpaceN, vector<vector<Space>> &vectSpaceAll){
    double timeToPerform;
    long structSize=0;
    DataType i;
    DataType N=vectSpaceN.size();
    DataType All=vectSpaceAll.size();
    stringstream ss;ss<<"N="<<N;

    structSize=0;
    timeToPerform=debut();
    for (i=0;i<N;i++){
        //cout <<"\r"<<i<<endl;
        std::vector<Point> Skyline;
        Skyline=subspaceSkylineSize_TREE(vectSpaceN[i], donnees);
        structSize+=Skyline.size();
        //for (int l=0;l<Skyline.size();l++) cerr << " "<<Skyline[l][0] << " ;"<<endl;     
    }
    timeToPerform=duree(timeToPerform);
    displayResult(dataName, donnees.size(), d, k, ss.str(), structSize, timeToPerform, "TREE");

}

void Experiment_NSCt(string dataName, int omega, int bufferMaxSize, TableTuple &donnees, Space d, int k, string path, vector<Space> &subspaceN, vector<vector<Space>> &vectSpaceN){
    
    int n =donnees.size();

    cerr << "data size = " << n <<endl;
    cerr << "omega = " << omega <<endl; 
    cerr << "bufferMaxSize = " << bufferMaxSize <<endl;
    cerr << "Space = " << d <<endl;
    cerr << "Distrib = " << dataName <<endl;
    int timestamp=0;

    double timeToPerformAll;
    double timeToPerformStep;

    TableTuple buffer;

    list<TableTuple> mainDataSet;//une liste de transaction
    list<TableTuple> mainTopmost;//une liste de topmosts
    ListVectorListUSetDualSpace ltVcLtUsDs; //??
    NegSkyStr structureNSC;
    int query_time=7;//??

    // before warm up
    double deb_warm_up=omp_get_wtime();
    while (timestamp<omega){
        buffer.push_back(donnees[timestamp]);
        if (buffer.size()==bufferMaxSize){
            TableTuple topmostBuffer;
            vector<Space> attList;
            for (int j=1;j<=d;j++) attList.push_back(j);
            ExecuteBSkyTree(attList, buffer, topmostBuffer);
            mainDataSet.push_front(buffer);
            mainTopmost.push_front(topmostBuffer); 
            buffer.clear();
        }
        timestamp++;
    }
    cerr<<"le warm_up a pris "<<omp_get_wtime()-deb_warm_up<<endl;
    deb_warm_up=omp_get_wtime();
    NEG::InitStructure (mainDataSet, mainTopmost, ltVcLtUsDs, d);
    cerr<<"L'initialisation de la structure a pris "<<omp_get_wtime()-deb_warm_up<<endl;
    
    // after warm up

    while(timestamp < n){

        buffer.push_back(donnees[timestamp]);//on met dans le buffer les tuples les uns après ls autres

        if (buffer.size()==bufferMaxSize){//si le buffer est plein, on déclenche une mise à jour ==> ondéclence le chronomètre

            timeToPerformAll=debut();//oour mesurer le temps global
            timeToPerformStep=debut();//pour mesurer chaque étape

            // Buffer at its maximum size, time to update and clear the buffer

            cerr << "at timestamp: "<< timestamp <<", time to update" << endl;

            // 1 compute topmost of the buffer
            timeToPerformStep=debut();
            TableTuple topmostBuffer;
            vector<Space> attList;
            for (int j=1;j<=d;j++) attList.push_back(j);
            ExecuteBSkyTree(attList, buffer, topmostBuffer);//on calcule le topmost du buffer

            cerr << "Step 0 in " << duree(timeToPerformStep)<< " to compute the topmost of the new tuples"<<endl;

            // 2 update main datasets and compute pairs of newly inserted tuples
            timeToPerformStep=debut();
            //remove outdated tuples and their respective information
            //if (mainDataSet.size()==omega/bufferMaxSize){

            mainDataSet.pop_back();//remove the oldest transaction
            mainTopmost.pop_back();//remove the topmost of the oldest transaction
            ltVcLtUsDs.pop_back(); //remove the sequence of pairs buckets associated to outdated tuples
               // double deb=debut();
            NEG::expiration(ltVcLtUsDs);//it removes the buckets that are associated to the removed transaction
               // cerr << "L'expiration a pris " << duree(deb)<< endl;
            //}
            cerr<<"l'expiration a pris "<<duree(timeToPerformStep)<< endl;
            timeToPerformStep=debut();
            //Add th new tuples
            mainDataSet.push_front(buffer);
            mainTopmost.push_front(topmostBuffer);
            NEG::updateNSCt_step1(buffer, mainTopmost, ltVcLtUsDs, d);
            
            cerr << " updateNSCt_step1 in " << duree(timeToPerformStep)<< endl;

            // 3 update the set of existing pairs
            timeToPerformStep=debut();
            
            NEG::updateNSCt_step2(topmostBuffer, mainDataSet, ltVcLtUsDs, d);

            cerr << "updateNSCt_step2 in " << duree(timeToPerformStep)<< endl;

            // 5 clear buffer
            
            buffer.clear();

            // update done
            cerr << "Update done in " << duree(timeToPerformAll)<< mendl(1);
            cerr<<endl;
        }

        if (timestamp==omega+4*bufferMaxSize+query_time){
            // 4 Set header
            timeToPerformStep=debut();
            structureNSC.clear();
            int structSize=NEG::negativeSkycube(structureNSC, ltVcLtUsDs, d);
            
            cerr << "Step 4 "<<structSize<<" in " << duree(timeToPerformStep)<< endl;
            cerr << "Current timestamp" << timestamp<< endl; 
            
            // fill valid_data with valid blocs of mainDataSet
            double deb_remp=omp_get_wtime();
            TableTuple valid_data;

            for(auto it1=mainDataSet.rbegin(); it1!= mainDataSet.rend(); it1++ ){
                for(auto it2=it1->begin(); it2!=it1->end();it2++) valid_data.push_back(*it2);
            }
            cerr<<"le remplissage de la table des tuples valides a pris"<<omp_get_wtime()-deb_remp<<endl;
            cerr<<"valid data contient"<<valid_data.size()<<" tuples"<<endl;

            // vector<Space> subspaceN_temp;
            // vector<vector<Space>> listNTabSpace_temp;

            // subspaceN_temp.insert(subspaceN_temp.begin(),subspaceN.begin(),subspaceN.begin()+10);
            // listNTabSpace_temp.insert(listNTabSpace_temp.begin(), vectSpaceN.begin(), vectSpaceN.begin()+10);

            // //query answering by NSC
            // NEG::skylinequery(dataName, structureNSC, valid_data.size(), d, k, subspaceN_temp, donnees, listNTabSpace_temp, timestamp-query_time-1);
            // //query answering by BSKYTREE
            // experimentation_TREE(dataName, valid_data, d, k, listNTabSpace_temp, listNTabSpace_temp);
            
            // subspaceN_temp.clear();
            // listNTabSpace_temp.clear();

            // subspaceN_temp.insert(subspaceN_temp.begin(),subspaceN.begin(),subspaceN.begin()+100);
            // listNTabSpace_temp.insert(listNTabSpace_temp.begin(), vectSpaceN.begin(), vectSpaceN.begin()+100);

            // //query answering by NSC
            // NEG::skylinequery(dataName, structureNSC, valid_data.size(), d, k, subspaceN_temp, donnees, listNTabSpace_temp, timestamp-query_time-1);
            // //query answering by BSKYTREE
            // experimentation_TREE(dataName, valid_data, d, k, listNTabSpace_temp, listNTabSpace_temp);
            
            // subspaceN_temp.clear();
            // listNTabSpace_temp.clear();

            // subspaceN_temp.insert(subspaceN_temp.begin(),subspaceN.begin(),subspaceN.begin()+1000);
            // listNTabSpace_temp.insert(listNTabSpace_temp.begin(), vectSpaceN.begin(), vectSpaceN.begin()+1000);

            // //query answering by NSC
            // NEG::skylinequery(dataName, structureNSC, valid_data.size(), d, k, subspaceN_temp, donnees, listNTabSpace_temp, timestamp-query_time-1);
            // //query answering by BSKYTREE
            // experimentation_TREE(dataName, valid_data, d, k, listNTabSpace_temp, listNTabSpace_temp);
            
            // subspaceN_temp.clear();
            // listNTabSpace_temp.clear();

            // subspaceN_temp.insert(subspaceN_temp.begin(),subspaceN.begin(),subspaceN.begin()+10000);
            // listNTabSpace_temp.insert(listNTabSpace_temp.begin(), vectSpaceN.begin(), vectSpaceN.begin()+10000);

            // //query answering by NSC
            // NEG::skylinequery(dataName, structureNSC, valid_data.size(), d, k, subspaceN_temp, donnees, listNTabSpace_temp, timestamp-query_time-1);
            // //query answering by BSKYTREE
            // experimentation_TREE(dataName, valid_data, d, k, listNTabSpace_temp, listNTabSpace_temp);
            
            // subspaceN_temp.clear();
            // listNTabSpace_temp.clear();


            //query answering by NSC
            NEG::skylinequery(dataName, structureNSC, valid_data.size(), d, k, subspaceN, donnees, vectSpaceN, timestamp-query_time-1);
            //query answering by BSKYTREE
            //valid_data.clear();
            //valid_data.insert(valid_data.begin(), donnees.begin()+40000,donnees.begin()+140000);
            experimentation_TREE(dataName, valid_data, d, k, vectSpaceN, vectSpaceN);
            size_t nbnb=0, nbmin=100000000, nbmax=0;
            cerr<<"On a "<<ltVcLtUsDs.size()<<" transactions"<<endl;
            nbnb=0;
            int ccccc=0;
            for (auto it_list = ltVcLtUsDs.begin(); it_list != ltVcLtUsDs.end(); it_list++){//pour chaque transaction/batch
                for (auto it_vector = (*it_list).begin(); it_vector!=(*it_list).end(); it_vector++){//pour chaque tuple t dans le batch/transactin
                    if((*it_vector).size()<nbmin)nbmin=(*it_vector).size();
                    if((*it_vector).size()>nbmax)nbmax=(*it_vector).size();
                    nbnb+=(*it_vector).size();
                    if((*it_vector).size()>1000) ccccc++;
                    //(*it_vector).pop_back();//supprimer le plus vieux bucket de paires associé à t
                }
            }
            cerr<<"On a un total de "<<nbnb<<" buckets"<<endl;
            cerr<<"le nombre max de buckets par tuple est" <<nbmax<<" le min est "<<nbmin<<endl;
            cerr<<"le nombre de tuples avec plus de 1000 buckets est "<<ccccc<<endl;
            exit(0);
        }
        timestamp++;
    }

}

void Experiment_DBSky(string dataName, int omega, int bufferMaxSize, TableTuple &donnees, Space d, int k, string path, vector<Space> &subspaceN, vector<vector<Space>> &vectSpaceN){

    int n =donnees.size();
    cerr << "DBSky" << endl;
    cerr << "data size = " << n <<endl;
    cerr << "omega = " << omega <<endl; 
    cerr << "bufferMaxSize = " << bufferMaxSize <<endl;
    cerr << "Space = " << d <<endl;

    

    double timeToPerformAll;
    double timeToPerformStep;

    TableTuple buffer;

    list<TableTuple> mainDataSet;
    list<TableTuple> mainTopmost;
    Space All=(1<<d);
    vector<TableTuple> DBSky(All);
    vector<TableTuple> DBRest(All);

    // before warm up
    timeToPerformAll=debut();
    TableTuple valid_data(donnees.begin(),donnees.begin()+omega);
    cout <<valid_data[omega-1][0]<<endl;
    cout <<"valid data size " << valid_data.size()<<endl;
    #pragma omp parallel for num_threads(NB_THREADS) schedule(dynamic)
    for (Space subspace=1;subspace<All;subspace++){

        // DBSKY = skyline of valid data, we compute it with BSKYTREE

        DBSky[subspace-1]=subspaceSkylineSize_TREE(vectSpaceN[subspace-1], valid_data);

        //cout << "DBSky of subspace: "<< subspace << ", size: "<<DBSky[subspace-1].size()<<endl;

        // DBREST = records that are dominated by more recent ones and not dominated by ancient one

        // 1 compute valid data minus DBSky 

        TableTuple valid_data_minus_dbsky;

        for (int i=0;i<omega;i++){
            bool found=false;
            for (int j=0;j<DBSky[subspace-1].size() && DBSky[subspace-1][j][0]<=valid_data[i][0] ;j++){
                if(valid_data[i][0]==DBSky[subspace-1][j][0]){
                    found=true;
                    break;
                }
            }
            if(!found){
                valid_data_minus_dbsky.push_back(valid_data[i]);
            }
        }

        // 2 compare records in valid data minus DBSky to records in DBSky and DBRest
        vector<Space> list_space;
        listeAttributsPresents(subspace, d, list_space);
        for (int i=0; i<valid_data_minus_dbsky.size();i++){
          
            bool inDBRest=true;
            // first compare to DBSky
            for (int j=0; j<DBSky[subspace-1].size();j++){
                if(DBSky[subspace-1][j][0]>valid_data_minus_dbsky[i][0]){
                    bool dominated=true;
                    int equality=0;
                    for (auto space=list_space.begin();space!=list_space.end();space++){
                       
                        if(valid_data_minus_dbsky[i][(*space)+1]<DBSky[subspace-1][j][(*space)+1]){
                            dominated=false;
                            break;
                        }else if(valid_data_minus_dbsky[i][(*space)+1]==DBSky[subspace-1][j][(*space)+1]){
                            equality++;
                        }
                    }
            
                if(dominated==true && equality<list_space.size()){
                    // the record is dominated
                    inDBRest=false;
                    break;
                    }
                }
            }
            // second compare to DBRest if still inDBRest is true
            if(inDBRest)
            {
                for (int j=0; j<DBRest[subspace-1].size();j++){
                    if(DBRest[subspace-1][j][0]>valid_data_minus_dbsky[i][0]){
                        bool dominated=true;
                        int equality=0;
                        for (auto space=list_space.begin();space!=list_space.end();space++){
                           
                            if(valid_data_minus_dbsky[i][(*space)+1]<DBRest[subspace-1][j][(*space)+1]){
                                dominated=false;
                                break;
                            }else if(valid_data_minus_dbsky[i][(*space)+1]==DBRest[subspace-1][j][(*space)+1]){
                                equality++;
                            }
                        }
                  
                    if(dominated==true && equality<list_space.size()){
                        // the record is dominated
                        inDBRest=false;
                        break;
                        }
                    }
                }
            }
            // if inDBrest true, we add the record to DBRest
            if (inDBRest){

                DBRest[subspace-1].push_back(valid_data_minus_dbsky[i]);
            }
        }

        //cout << "DBRest of subspace: "<< subspace << ", size: "<<DBRest[subspace-1].size()<<endl;
    }

    int DBSky_size=0;
    int DBRest_size=0;
    for (Space subspace=1;subspace<All;subspace++){
        DBSky_size+=DBSky[subspace-1].size();
        DBRest_size+=DBRest[subspace-1].size();
    }
    cout << "DBSky size: "<<DBSky_size<<endl;
    cout << "DBrest size: "<<DBRest_size<<endl;
    cout << "Duration: " << duree(timeToPerformAll)<<endl;
    // after warm up
    cout << "--------After warm up--------"<<endl;

    int timestamp=omega;
    timeToPerformAll=debut();
    
    while (timestamp<omega+bufferMaxSize){

        vector<TableTuple> data_for_dominance_test(All);
        #pragma omp parallel for num_threads(NB_THREADS) schedule(dynamic)
        for (Space subspace=1;subspace<All;subspace++){

            // LPM
            // compare donnees[omega] to all records in DBSKY

            //cout <<"++++++LPM++++++"<<endl;

            //cout << "subspace: "<< subspace<< endl;
            data_for_dominance_test[subspace]=DBSky[subspace-1];

            data_for_dominance_test[subspace].push_back(donnees[timestamp]);

            DBSky[subspace-1]=subspaceSkylineSize_TREE(vectSpaceN[subspace-1], data_for_dominance_test[subspace]);

            bool found=false;
            for (auto it_DBSky=DBSky[subspace-1].begin();it_DBSky!=DBSky[subspace-1].end();it_DBSky++){
                if(donnees[timestamp][0]==(*it_DBSky)[0]){
                    found=true; 
                    break;
                }
            }
            if (!found){
                DBRest[subspace-1].push_back(donnees[timestamp]);
                //cout << "record "<< timestamp<<" dominated, added to DBRest "<<endl; 
            }
            else{
                //cout << "record "<< timestamp<<" added to DBSky "<<endl;
            }
        
            // LMM
            
            //cout <<"++++++LMM++++++"<<endl;

            if(valid_data[0][0]==DBRest[subspace-1][0][0]){
                //cout << "record in DBRest: "<< DBRest[subspace-1][0][0] << " outdated"<<endl;
                DBRest[subspace-1].erase(DBRest[subspace-1].begin());
            }
            else{
                for (int i=0; i<DBSky[subspace-1].size();i++){
                    if(valid_data[0][0]==DBSky[subspace-1][i][0]){
                        //remove DBSky[subspace-1][i]
                        //cout << "record in DBSky: "<< DBSky[subspace-1][i][0] << " outdated"<<endl;
                        DBSky[subspace-1].erase(DBSky[subspace-1].begin()+i);
                        data_for_dominance_test[subspace]=DBSky[subspace-1];
                        auto it=data_for_dominance_test[subspace].begin();
                        data_for_dominance_test[subspace].insert(it,DBRest[subspace-1].begin(),DBRest[subspace-1].end());
                        DBSky[subspace-1]=subspaceSkylineSize_TREE(vectSpaceN[subspace-1], data_for_dominance_test[subspace]);
                        break;
                    }
                }
            }    
        }
        timestamp++;
    }
    DBSky_size=0;
    DBRest_size=0;
    for (Space subspace=1;subspace<All;subspace++){
        DBSky_size+=DBSky[subspace-1].size();
        DBRest_size+=DBRest[subspace-1].size();
    }
    cout << "DBSky size: "<<DBSky_size<<endl;
    cout << "DBrest size: "<<DBRest_size<<endl;
    cout << "Duration: " << duree(timeToPerformAll)<<endl;
}


void experimentation_menu(string dataName, TableTuple &donnees, Space d, int k, string path, int omega, int bufferSize, int method){
        
    // generate subspaces for queries
    Space spAux;
    Space N=1000; //number of random queries
    Space All=(1<<d)-1; //number of queries in the skycube
    vector<Space> subspaceN;
    vector<vector<Space>> listNTabSpace(N);
    
    // for skyline
    for (int i=1;i<=N;i++){
        spAux=rand() % All + 1;
        subspaceN.push_back(spAux);
        //cerr<< "space: "<<spAux <<endl;
        listeAttributsPresents(spAux, d, listNTabSpace[i-1]);
        //displaySubspace(spAux, d);cout<<endl;
    }

    // for skycube
    vector<Space> subspaceAll;
    vector<vector<Space>> listAllTabSpace(All);
    for (int i=1;i<=All;i++){
        subspaceAll.push_back(i);
        listeAttributsPresents(i, d, listAllTabSpace[i-1]);
    }

    switch (method) {

        case 1: 
            // Run the framework
            Experiment_NSCt( dataName,  omega,  bufferSize,  donnees,  d,  k,  path, subspaceN, listNTabSpace);
            break;
        case 2: 
            // Experiment DBSKY
            Experiment_DBSky(dataName,  omega,  bufferSize,  donnees,  d,  k,  path, subspaceAll, listAllTabSpace);
            break;
    }
}


