#include "experimentations.h"

int NB_THREADS=24; 

void displayResult(string dataName, DataType n, Space d, DataType k, string step, long structSize, double timeToPerform, string method){
    cerr<<dataName<<" "<<n<<" "<<d<<" "<<k<<" "<<method<<" "<<step<<" "<<structSize<<" "<<timeToPerform<<endl;
}

void top_k(string dataName, ListVectorListUSetDualSpace &ltVcLtUsDs, int valid_data_size, DataType k, Space d){

    Space All=(1<<d)-1; 
    
    double timeToPerform=debut();
    vector<int> score(valid_data_size,0);
    
    int start_position_for_batch=0;
    for (auto batch: ltVcLtUsDs){
        #pragma omp parallel for schedule(dynamic) 
        for (int i=0; i<batch.size(); i++){
            bool *spaces = new bool[All];
            for (int j=0;j<All;j++) spaces[j]=true;
            vector<Space> listCouv;
            for (auto bucket: batch[i]){
                for (auto pair: bucket){
                    listCouverts(pair.dom, pair.equ, d, listCouv);
                }
            }
            for (Space subspace : listCouv){
                spaces[subspace]=false;
            }
            for (int j=0; j<All; j++) score[i+start_position_for_batch]+=spaces[j];
            delete spaces;
        }
        start_position_for_batch+=batch.size();
    }
    
    sort(score.begin(), score.end(), greater<int>());

    displayResult(dataName, valid_data_size, d, k, "topK" , score[0], duree(timeToPerform), "topK");
}

void top_k_v2(string dataName, NegSkyStr &structureNSC, int valid_data_size, DataType k, Space d){

    Space All=(1<<d)-1; 
    vector<unordered_set<int>> spaces(valid_data_size);
    double timeToPerform=debut();
    for (auto itXY=structureNSC.rbegin();itXY!=structureNSC.rend();++itXY){
        for (auto itY=(itXY->second).begin();itY!=(itXY->second).end();++itY) {
            vector<Space> listCouv;
            listCouverts(itXY->first-itY->first, itY->first, d, listCouv);
            for (int id : itY->second){
                spaces[id].insert(listCouv.begin(), listCouv.end());
            }
        }
    }

    vector<int> score(valid_data_size);
    for (int i=0; i<valid_data_size; i++){
        score[i]=All-spaces[i].size();
    }

    sort(score.begin(), score.end(), greater<int>());

    displayResult(dataName, valid_data_size, d, k, "topK" , score[0], duree(timeToPerform), "topK");
}

void experimentation_TREE(string dataName, TableTuple donnees, Space d, DataType k, vector<vector<Space>> &vectSpaceN){
    double timeToPerform;
    long structSize=0;
    DataType i;
    DataType N=vectSpaceN.size();
    stringstream ss;ss<<"N="<<N;

    structSize=0;
    timeToPerform=debut();
    for (i=0;i<N;i++){
        std::vector<Point> Skyline;
        Skyline=subspaceSkylineSize_TREE(vectSpaceN[i], donnees);
        structSize+=Skyline.size();     
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
    cerr << "Distrib = " << dataName <<mendl(2);

    int timestamp=0;

    double timeToPerformAll;
    double timeToPerformStep;

    TableTuple buffer;

    list<TableTuple> mainDataSet;//une liste de transaction
    list<TableTuple> mainTopmost;//une liste de topmosts
    ListVectorListUSetDualSpace ltVcLtUsDs; //??
    NegSkyStr structureNSC;

    int nb_max_batch_after_warmup=5;
    int nb_batch_processed_after_warmup=0;
    TableTuple valid_data;

    //************************************************************************
    // before warm up
    timeToPerformStep=debut();
    for (int i=0; i<omega/bufferMaxSize; i++){
        buffer.insert(buffer.begin(),donnees.begin()+(i*bufferMaxSize),donnees.begin()+((i+1)*bufferMaxSize));
        mainDataSet.push_front(buffer);
        buffer.clear();
    } 
    timestamp=omega-1;
    TableTuple valid_topmost, old_topmost;
    vector<Space> attList;
    for (int j=1;j<=d;j++) attList.push_back(j);
    valid_data.insert(valid_data.begin(), donnees.begin(), donnees.begin()+omega);
    ExecuteBSkyTree(attList, valid_data, valid_topmost);
    NEG::InitStructure (mainDataSet, valid_topmost, ltVcLtUsDs, d, bufferMaxSize);
    cerr << "Structure initialized in " << duree(timeToPerformStep)<< " ms"<< endl;
    //*************************************************************************

    //*************************************************************************
    //query answering
    timeToPerformStep=debut();
    structureNSC.clear();
    int structSize=NEG::negativeSkycube(structureNSC, ltVcLtUsDs, d);
      
    cerr << structSize<<" pairs indexed in " << duree(timeToPerformStep)<< " ms"<< mendl(2);

    //query answering by NSC
    int valid_data_size=0;
    for(auto it1=mainDataSet.begin(); it1!= mainDataSet.end(); it1++ ){
        valid_data_size+=(*it1).size();
    }
    NEG::skylinequery(dataName, structureNSC, valid_data_size, d, k, subspaceN);
    //query answering by BSKYTREE
    valid_data.clear();
    valid_data.insert(valid_data.begin(), donnees.begin()+nb_batch_processed_after_warmup*bufferMaxSize, donnees.begin()+omega+nb_batch_processed_after_warmup*bufferMaxSize);
    experimentation_TREE(dataName, valid_data, d, k, vectSpaceN);
    // query top k
    top_k_v2(dataName, structureNSC, valid_data_size, k, d);
    cerr<<endl;
    //*************************************************************************

    //*************************************************************************
    // after warm up

    timestamp++;

    while(timestamp < n && nb_batch_processed_after_warmup<nb_max_batch_after_warmup){

        buffer.push_back(donnees[timestamp]);//on met dans le buffer les tuples les uns après ls autres

        if (buffer.size()==bufferMaxSize){//si le buffer est plein, on déclenche une mise à jour ==> ondéclence le chronomètre

            nb_batch_processed_after_warmup++;

            timeToPerformAll=debut();

            // Buffer at its maximum size, time to update and clear the buffer

            cerr << "--> at timestamp: "<< timestamp <<", time to update" << endl;

            //************************************************************************
            // 1 update datasets
            timeToPerformStep=debut();
            mainDataSet.pop_back();
            ltVcLtUsDs.pop_back();
            NEG::expiration(ltVcLtUsDs);
            mainDataSet.push_front(buffer);
            //************************************************************************

            //************************************************************************
            //2 compute pairs of newly inserted records
            valid_data.clear();
            for (auto it_list=mainDataSet.begin(); it_list!=mainDataSet.end(); it_list++){
                valid_data.insert(valid_data.begin(), it_list->begin(), it_list->end());
            }
            valid_topmost.clear();
            vector<Space> attList;
            for (int j=1;j<=d;j++) attList.push_back(j);
            ExecuteBSkyTree(attList, valid_data, valid_topmost); 
            NEG::updateNSCt_step1(mainDataSet, valid_topmost, nb_batch_processed_after_warmup, ltVcLtUsDs, d);
            cerr << "Update Step 1 in " << duree(timeToPerformStep)<< endl;
            //*************************************************************************
            
            //*************************************************************************
            // 3 update set of pairs of existing records
            timeToPerformStep=debut();
            NEG::updateNSCt_step2(mainDataSet, valid_topmost, old_topmost, nb_batch_processed_after_warmup, ltVcLtUsDs, d, bufferMaxSize);
            cerr << "Update Step 2 in " << duree(timeToPerformStep)<< endl;
            //*************************************************************************
            
            //*************************************************************************
            // 4 clear buffer and memorize topmost  
            buffer.clear();
            cerr << "Update done in " << duree(timeToPerformAll)<< mendl(1);
            old_topmost.swap(valid_topmost);
            //*************************************************************************

            //*************************************************************************
            //query answering
            timeToPerformStep=debut();
            structureNSC.clear();
            int structSize=NEG::negativeSkycube(structureNSC, ltVcLtUsDs, d);
            
            cerr << structSize<<" pairs indexed in " << duree(timeToPerformStep)<< " ms"<< mendl(2);

            //query answering by NSC
            int valid_data_size=0;
            for(auto it1=mainDataSet.rbegin(); it1!= mainDataSet.rend(); it1++ ){
               valid_data_size+=(*it1).size();
            }
            NEG::skylinequery(dataName, structureNSC, valid_data_size, d, k, subspaceN);
            //query answering by BSKYTREE
            valid_data.clear();
            valid_data.insert(valid_data.begin(), donnees.begin()+(nb_batch_processed_after_warmup*bufferMaxSize), donnees.begin()+omega+(nb_batch_processed_after_warmup*bufferMaxSize));
            experimentation_TREE(dataName, valid_data, d, k, vectSpaceN);
            // query top k
            top_k_v2(dataName, structureNSC, valid_data_size, k, d);
            cerr<<endl;
            //*************************************************************************
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
    Space N=100; //number of random queries
    Space All=(1<<d)-1; //number of queries in the skycube
    vector<Space> subspaceN;
    vector<vector<Space>> listNTabSpace(N);
    
    // for skylines
    for (int i=1;i<=N;i++){
        spAux=rand() % All + 1;
        subspaceN.push_back(spAux);
        listeAttributsPresents(spAux, d, listNTabSpace[i-1]);
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
            // Run MSSD
            Experiment_NSCt( dataName,  omega,  bufferSize,  donnees,  d,  k,  path, subspaceAll, listAllTabSpace);
            break;
        case 2: 
            // Run DBSKY
            Experiment_DBSky(dataName,  omega,  bufferSize,  donnees,  d,  k,  path, subspaceAll, listAllTabSpace);
            break;
    }
}


