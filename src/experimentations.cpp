#include "experimentations.h"


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


    int timestamp=0;

    double timeToPerformAll;
    double timeToPerformStep;

    TableTuple buffer;

    list<TableTuple> mainDataSet;
    list<TableTuple> mainTopmost;
    ListVectorListUSetDualSpace ltVcLtUsDs;
    NegSkyStr structureNSC;
    int query_time=7;

    // before warm up

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
    
    NEG::InitStructure (mainDataSet, mainTopmost, ltVcLtUsDs, d);
    
    // after warm up

    while(timestamp < n){

        buffer.push_back(donnees[timestamp]);

        if (buffer.size()==bufferMaxSize){

            timeToPerformAll=debut();
            timeToPerformStep=debut();

            // Buffer at its maximum size, time to update and clear the buffer

            cerr << "at timestamp: "<< timestamp <<", time to update" << endl;

            // 1 compute topmost of the buffer
            timeToPerformStep=debut();
            TableTuple topmostBuffer;
            vector<Space> attList;
            for (int j=1;j<=d;j++) attList.push_back(j);
            ExecuteBSkyTree(attList, buffer, topmostBuffer);

            cerr << "Step 1 in " << duree(timeToPerformStep)<< endl;

            // 2 update main datasets and compute pairs of newly inserted tuples
            timeToPerformStep=debut();
            if (mainDataSet.size()==omega/bufferMaxSize){

                mainDataSet.pop_back();
                mainTopmost.pop_back();
                ltVcLtUsDs.pop_back();
                NEG::expiration(ltVcLtUsDs);
            }

            mainDataSet.push_front(buffer);
            mainTopmost.push_front(topmostBuffer);
            NEG::updateNSCt_step1(buffer, mainTopmost, ltVcLtUsDs, d);
            
            cerr << "Step 2 in " << duree(timeToPerformStep)<< endl;

            // 3 update the set of existing pairs
            timeToPerformStep=debut();
            
            NEG::updateNSCt_step2(topmostBuffer, mainDataSet, ltVcLtUsDs, d);

            cerr << "Step 3 in " << duree(timeToPerformStep)<< endl;

            // 5 clear buffer
            
            buffer.clear();

            // update done
            cerr << "Update done in " << duree(timeToPerformAll)<< mendl(1);
            cerr<<endl;
        }

        if (timestamp==omega+2*bufferMaxSize+query_time){
            // 4 Set header
            timeToPerformStep=debut();
            structureNSC.clear();
            int structSize=NEG::negativeSkycube(structureNSC, ltVcLtUsDs, d);
            
            cerr << "Step 4 "<<structSize<<" in " << duree(timeToPerformStep)<< endl;
            cerr << "Current timestamp" << timestamp<< endl; 
            
            // fill valid_data with valid blocs of mainDataSet
            TableTuple valid_data;

            for(auto it1=mainDataSet.rbegin(); it1!= mainDataSet.rend(); it1++ ){
                for(auto it2=it1->begin(); it2!=it1->end();it2++) valid_data.push_back(*it2);
            }
            //query answering by NSC
            NEG::skylinequery(dataName, structureNSC, valid_data.size(), d, k, subspaceN, donnees, vectSpaceN, timestamp-query_time-1);
            //query answering by BSKYTREE
            experimentation_TREE(dataName, valid_data, d, k, vectSpaceN, vectSpaceN);
            
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

    int timestamp=0;

    double timeToPerformAll;
    double timeToPerformStep;

    TableTuple buffer;

    list<TableTuple> mainDataSet;
    list<TableTuple> mainTopmost;
    TableTuple DBSky;
    Space All=(1<<d)-1;
    vector<TableTuple> DBRest(All);
    int query_time=7;

    while(timestamp <=omega+2*bufferMaxSize){

        buffer.push_back(donnees[timestamp]);

        if (buffer.size()==bufferMaxSize){

            timeToPerformAll=debut();
            timeToPerformStep=debut();

            // Buffer at its maximum size, time to update and clear the buffer

            cerr << "at timestamp: "<< timestamp <<", time to update" << endl;

            //step 1: push buffer to main dataset, remove outdated block if it exists

            mainDataSet.push_front(buffer);
            if (mainDataSet.size()==omega/bufferMaxSize+1){
                mainDataSet.pop_back();
            }

            //step 2: compute DBSky skyline of valid data AND DBRest skyline of (buffer union previous DBRest) 

            TableTuple valid_data;

            for(auto it1=mainDataSet.rbegin(); it1!= mainDataSet.rend(); it1++ ){
                for(auto it2=it1->begin(); it2!=it1->end();it2++) valid_data.push_back(*it2);
            }
            int DBSkySize=0;
            int DBRestSize=0;
            // loop on all subspaces
            for (int i=0;i<vectSpaceN.size();i++){

                // step 21: compute DBSky

                TableTuple Skyline;
                Skyline=subspaceSkylineSize_TREE(vectSpaceN[i], valid_data);
                DBSkySize+=Skyline.size(); 
                
                // step 22: compute DBRest, skyline of buffer + previous DBRest
                // dont compute for the first iteration
                if (timestamp>bufferMaxSize){
                    TableTuple dataset_for_DBRest=buffer;
                    // add DBRest[i] of the last iteration to dataset_for_DBRest
                    for(auto it1=DBRest[i].begin(); it1!= DBRest[i].end(); it1++ ){
                        dataset_for_DBRest.push_back(*it1);
                    }
                    DBRest[i]=subspaceSkylineSize_TREE(vectSpaceN[i], dataset_for_DBRest);
                    // remove Skyline from DBRest[i]
                    vector<int> DBRest_ids;
                    vector<int> Skyline_ids;
                    for (int k=0;k<DBRest[i].size();k++) DBRest_ids.push_back(DBRest[i][k][0]);
                    for (int k=0;k<Skyline.size();k++) Skyline_ids.push_back(Skyline[k][0]);
                    std::sort(DBRest_ids.begin(), DBRest_ids.begin()+DBRest_ids.size());
                    std::sort(Skyline_ids.begin(), Skyline_ids.begin()+Skyline_ids.size());
                    vector<int> v(DBRest_ids.size()+Skyline_ids.size());                 
                    std::vector<int>::iterator it_minus;
                    it_minus=std::set_difference (DBRest_ids.begin(), DBRest_ids.begin()+DBRest_ids.size(), 
                        Skyline_ids.begin(), Skyline_ids.begin()+Skyline_ids.size(), v.begin());
                    v.resize(it_minus-v.begin());
                    TableTuple DBRest_minus_Skyline;
                    for (auto it=DBRest[i].begin();it!=DBRest[i].end();it++){
                        if (std::find(v.begin(),v.end(),(*it)[0])!=v.end()){
                            DBRest_minus_Skyline.push_back((*it));
                        }
                    }
                    DBRest[i].swap(DBRest_minus_Skyline);
                    DBRestSize+=DBRest[i].size();
                }
            }
            cout << "DBSky size: "<<DBSkySize<<endl;
            cout << "DBRest size: "<<DBRestSize<<endl;
            cout << "DBSky + DBRest: "<<DBSkySize+DBRestSize<<endl;
            //step: compute DBRest, skyline of buffer + DBRest


            // last step: clear buffer
            buffer.clear();
        }
        timestamp++;
    }
}


void experimentation_menu(string dataName, TableTuple &donnees, Space d, int k, string path, int omega, int bufferSize){
        
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

    // Run the framework

    //Experiment_NSCt( dataName,  omega,  bufferSize,  donnees,  d,  k,  path, subspaceAll, listAllTabSpace);

    // Experiment DBSKY

    Experiment_DBSky(dataName,  omega,  bufferSize,  donnees,  d,  k,  path, subspaceAll, listAllTabSpace);
}

