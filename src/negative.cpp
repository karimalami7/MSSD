#include "negative.h"

using namespace std;

namespace NEG{

void triScore(TableTuple &donnees, Space d){
//  Cette procédure trie les données suivant le score (somme des valeurs des attributs)
    DataType i, n=donnees.size();
    Space j;
    vector<DataType> aux(n);
    TableTuple auxT=donnees;
    for (i=0;i<n;i++){
        aux[i]=donnees[i][1];
        for (j=2;j<=d;j++) aux[i]+=donnees[i][j];
    }
    vector<DataType> index(donnees.size());
    for (i = 0; i != (DataType)index.size(); ++i) index[i] = i;
    sortIndexes(aux, index);
    for (i=0;i<n;i++){
        donnees[i]=auxT[index[i]];
    }
}

void visualisation_pairs(vector<USetDualSpace> listUSetDualSpace){

    cout <<"*****visualisation_pairs*****"<<endl;
    for(int i=0; i<listUSetDualSpace.size(); i++){
        cout <<"t"<<i<<": ";
        for (auto it_uset = listUSetDualSpace[i].begin(); it_uset!=listUSetDualSpace[i].end(); it_uset++){
            cout <<it_uset->dom <<" "<<it_uset->equ <<" ; ";
        }
        cout <<endl;
    }

}

bool pet_pair(const DualSpace &sp1, const DualSpace &sp2 ){
    //retourne TRUE si la paire sp1 contient plus de dimensions que sp2
    //ça sert à trier les listes/vecteurs de paires par ordre décroissant de ce nombre
    //pour ensuite faciliter les tests d'inclusion 
    auto n1=sp1.dom + sp1.equ;
    auto n2=sp2.dom + sp2.equ;
    return __builtin_popcount(n1) >__builtin_popcount(n2);
}

inline DualSpace domDualSubspace_1(const Point &t1, const Point &t2, const Space &d){
//  retourne le sous espace dans lequel t1 domine t2//je pense que c'est t2 qui domine t1
//  les sous espaces sont codés en un nombre décimal
//  exemples (codage) quand d=4, ABCD->15, AD->9, A->1, B->2, C->4, BC -> 5
    Space j;
    Space poids1=0, poids2=0;
    DualSpace sortie;
    sortie.dom=0;
    sortie.equ=0;
    long pow=1;
    unsigned int dec=1;
    if (t1[0]==t2[0]) return sortie;
    for(j = 1; j <= d ; ++j){
        if(t1[j] < t2[j]) {sortie.dom+=pow;++poids1;}
        else if(t1[j] == t2[j]) {sortie.equ+=pow;++poids2;}
        pow*=2;
       //pow = (pow << 1);
    }
    sortie.poids=(1<<(poids1+poids2))-(1<<poids2);
    return sortie;
}


void insertDualSpaceToUSet(DualSpace sp, const Space &d, USetDualSpace &uSetDualSpace, Space &all, Space &maxDom, Space &sizeMaxDom, bool &sortie){
//retourne true si le tuple est completement dominé (ne plus le comparer à d'autres tuples)
    if (sp.dom==all){
        USetDualSpace sp1;
        sp1.insert(sp);
        uSetDualSpace.swap(sp1);
        sortie=true;
        return;
    }else if (sp.dom!=0){
        if (!estInclusDans(sp.dom+sp.equ, maxDom)){
            if (spaceSize(sp.dom)>sizeMaxDom){
                maxDom=sp.dom;
                sizeMaxDom=spaceSize(sp.dom);
            }
            uSetDualSpace.insert(sp);
        }
    }
}





long creationStructureNSC(NegSkyStrAux &structure, ListVectorListUSetDualSpace &list_Vc_lt_UsDs, Space d){
    Space all=(1<<d)-1;
    long structSize=0;
    DataType idTuple=0;
  
    auto iterator_list=list_Vc_lt_UsDs.rbegin();
    while (iterator_list!=list_Vc_lt_UsDs.rend()){
        for (int i=0;i<iterator_list->size();++i){ // on boucle sur tous les tuples taille n
                auto it_us=(*iterator_list)[i].rbegin();//it_us++;
                for (;it_us!=(*iterator_list)[i].rend();++it_us){
                    for (auto it=(*it_us).begin();it!=(*it_us).end();++it){ // on boucle sur tous les paris (X|Y) de ce tuple
                        Space spaceXY=it->dom+it->equ;
                        Space spaceY=it->equ;
                        auto it2=structure.find(spaceXY);
                        if (it2==structure.end()){
                            unordered_map<Space,vector<DataType>> mapAux;
                            vector<DataType> vectAux;
                            vectAux.push_back(idTuple);
                            mapAux.insert(pair<Space,vector<DataType>>(spaceY,vectAux));
                            structure.insert(pair<Space,unordered_map<Space,vector<DataType>>>(spaceXY,mapAux));
                        }else{
                            auto it3=(it2->second).find(spaceY);
                            if (it3==(it2->second).end()){
                                vector<DataType> vectAux;
                                vectAux.push_back(idTuple);
                                (it2->second).insert(pair<Space,vector<DataType>>(spaceY,vectAux));
                            }else{
                                (it3->second).push_back(idTuple);
                            }
                        }
                        structSize++;
                    }
                }
                idTuple++;
        }
        iterator_list++;
    }
    return structSize;
}

// down     ,  transform NegSkyStrAux to NegSkyStr,   map -> vector
long negativeSkycube(NegSkyStr &structure, ListVectorListUSetDualSpace &list_Vc_lt_UsDs, Space d){  
    long structSize;   
    Space spXY, spY;    
    NegSkyStrAux structure0;  

    structSize = creationStructureNSC(structure0, list_Vc_lt_UsDs, d);  
   
    for (auto itXY=structure0.begin();itXY!=structure0.end();itXY++){   
        spXY=itXY->first;   
        vector<pair<Space,vector<DataType>>> vY;    
        for (auto itY=(itXY->second).begin();itY!=(itXY->second).end();itY++){  
            spY=itY->first; 
            vector<Space> vId;  
            for (auto itId=(itY->second).begin();itId!=(itY->second).end();itId++){ 
                vId.push_back(*itId);   
            }   
            vY.push_back(pair<Space, vector<DataType>>(spY, vId));  
        }   
        structure.push_back(pair<Space, vector<pair<Space,vector<DataType>>>>(spXY, vY));   
    }   
    return structSize;  
}

bool pet(const pair<Space, TableTuple> &p1, const pair<Space, TableTuple> &p2 ){
    return __builtin_popcount(p1.first) >__builtin_popcount(p2.first);
}

void choixPivot(TableTuple &topmost, Space d){
    DataType n= topmost.size();
    DataType iMinMax=-1, minMax=n, maxVal;
    for (auto i=0;i<n;i++){
        maxVal=topmost[i][0];
        for (auto j=1;j<d;j++) if (maxVal<topmost[i][j]) maxVal=topmost[i][j];
        if (maxVal<minMax){
            iMinMax=i;
            minMax=maxVal;
        }
    }
    Point pointAux=topmost[0];
    topmost[0]=topmost[iMinMax];
    topmost[iMinMax]=pointAux;
}







bool* subspaceSkyline_NSC(NegSkyStr &structure, int size_data, const Space subspace){
    DataType i;
    bool* tableSkyline=new bool[size_data];
    for (i=0;i<size_data;i++) tableSkyline[i]=true;
    //for (auto itXY=structure.rbegin();itXY!=structure.rend() && subspace<=(itXY->first);++itXY){
    for (auto itXY=structure.rbegin();itXY!=structure.rend();++itXY){
        if (estInclusDans(subspace, itXY->first)){
            for (auto itY=(itXY->second).begin();itY!=(itXY->second).end();++itY) {
                if (!estInclusDans(subspace, itY->first)) {
                    for (auto itId=(itY->second).begin();itId!=(itY->second).end();++itId){
                        tableSkyline[*itId]=false;
                    }
                }
            }
        }
    }

    return tableSkyline;
}

bool* subspaceSkylineSize_NSC(NegSkyStr &structure, int size_data, Space subspace){
    DataType skySize;

    bool* skyline=subspaceSkyline_NSC(structure, size_data, subspace);
    skySize=compteLesMarques(skyline, size_data);
    //delete[] skyline;
    
    return skyline;

}


DataType compteLesMarques(bool* table, DataType length){
/*  function qui compte le nombre de cellules valant 'vrai' dans une table de booleens
    utile pour calculer la taille du skyline*/
    DataType i, total=0;
    for (i=0; i<length; i++) total+=table[i];
    return total;
}



void displayResult(string dataName, DataType n, Space d, DataType k, string step, long structSize, double timeToPerform){
     cerr<<dataName<<" "<<n<<" "<<d<<" "<<k<<" "<<"NSCt"<<" "<<step<<" "<<structSize<<" "<<timeToPerform<<endl;
}


void skylinequery(string dataName, NegSkyStr &structure0, int indexedDataSize, Space d, DataType k, vector<Space> subspaceN){

    int N=subspaceN.size();
    string nombreRequete ="N="+to_string(N);

    int structSize=0;
    double timeToPerform;
    timeToPerform=debut();
    for (int i=0; i<N; i++){
        bool *skyline_NSC;
        skyline_NSC=subspaceSkylineSize_NSC(structure0, indexedDataSize, subspaceN[i]);
        for (int j=0;j<indexedDataSize;j++){
            if (skyline_NSC[j]==true) {
                structSize++;
            }
        }
    }
    timeToPerform=duree(timeToPerform);
    displayResult(dataName, indexedDataSize, d, k, nombreRequete, structSize, timeToPerform); 

}




void updateNSCt_step1(list<TableTuple> &mainDataset, TableTuple &valid_topmost, int decalage, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d){
    Space All=(1<<d)-1;   
    int number_batch=mainDataset.size();
    TableTuple buffer=(*mainDataset.begin());
    int size_buffer=buffer.size(); 
    
    VectorListUSetDualSpace buffer_pairs(buffer.size());
    
    bool completely_dominated[buffer.size()];
    for (int k=0;k<size_buffer;k++) completely_dominated[k]=false;

    //***********************************************************
    //   for each tuple t in the buffer
    #pragma omp parallel for schedule(dynamic) 
    for (int k=0; k<buffer.size(); k++){

        int smaller_position_all=number_batch;
        vector<list<DualSpace>> pairsToCompress(number_batch); 
        vector<unordered_set<DualSpace>> pairsToCompress_in_set(number_batch);
        
        //for each tuple t' in topmost 
        for (int j=0; j<valid_topmost.size(); j++) {
            DualSpace ds;
            ds=NEG::domDualSubspace_1(valid_topmost[j], buffer[k], d);
            if(ds.dom==All){ //t completely dominated by t'
                if((number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1)==0){ // if t older than t'
                    completely_dominated[k]=true; 
                    pairsToCompress_in_set[number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1].clear();
                    pairsToCompress_in_set[number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1].insert(ds);
                    smaller_position_all=0;
                    break;
                }
                else{// if t' older than t
                    if((number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1)<smaller_position_all){
                        smaller_position_all=number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1;
                        pairsToCompress_in_set[number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1].clear();
                        pairsToCompress_in_set[number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1].insert(ds);
                    }
                }
            }
            else{
                pairsToCompress_in_set[number_batch - (valid_topmost[j][0] / size_buffer - decalage) -1].insert(ds);  
            }          
        }
       
        // if t not completely dominated, we compress its pairs
        if(!completely_dominated[k]){
            //empty buckets followed by buckets having ds.All
            for (int i=smaller_position_all+1;i<number_batch;i++){
                pairsToCompress_in_set[i].clear();
            }
            
            // move pairs in pairToCompress structure
            for (int i=0;i<number_batch;i++){
                pairsToCompress[i].insert(pairsToCompress[i].begin(), pairsToCompress_in_set[i].begin(), pairsToCompress_in_set[i].end());
            }

            CompresserParInclusion_cascade(pairsToCompress,d,buffer_pairs[k]);
        }  
    }
    //**************************************************************

    //***************************************************************
    // delete tuples completely dominated 
    int nb_non_domines=0;
    for (int i = 0; i<size_buffer;i++){if(!completely_dominated[i]) nb_non_domines++;}
    VectorListUSetDualSpace buffer_pairs_to_swap(nb_non_domines);
    TableTuple batch_data_to_swap(nb_non_domines);
    auto it_bloc_data=mainDataset.begin();
    int j=0;
    for (int i = 0; i<size_buffer;i++){
        if(!completely_dominated[i]){
            buffer_pairs_to_swap[j]=buffer_pairs[i];
            batch_data_to_swap[j]=(*it_bloc_data)[i];
            j++;
        }
    }
    buffer_pairs.swap(buffer_pairs_to_swap);
    it_bloc_data->swap(batch_data_to_swap);
    //***************************************************************

    ltVcLtUsDs.push_front(buffer_pairs);

}

void updateNSCt_step2(list<TableTuple> &mainDataset, TableTuple &valid_topmost, TableTuple &old_topmost, int decalage, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d, int buffer_size){
    Space All=(1<<d)-1;

    // compute useful topmost: tuples that were not in the topmost in the previous maintenance time
    TableTuple useful_topmost;
    unordered_set<Point> old;
    old.insert(old_topmost.begin(),old_topmost.end());
    for(auto it=valid_topmost.begin();it!=valid_topmost.end();it++) if(old.find((*it))==old.end()) useful_topmost.push_back(*it);

    auto it_bloc_data=mainDataset.begin();//iterator on the most recent tuples
    auto it_bloc_pair=ltVcLtUsDs.begin();//iterator on the most recent tuples
    it_bloc_data++;it_bloc_pair++; // the first one does not need to be updated

    //******************************************************************************
    // loop on all existing transactions
    int block_position=1;
    while (it_bloc_data!=mainDataset.end()){

        bool all_yes[it_bloc_data->size()];
        for (int k=0;k<it_bloc_data->size();k++) all_yes[k]=false;

        //**************************************************************************
        // for each tuple t of a transaction
        #pragma omp parallel for schedule(dynamic) 
        for (int i=0; i<(*it_bloc_data).size(); i++){
            USetDualSpace usDS;
            // for each tuple t' in useful_topmost
            for (int j=0; j <useful_topmost.size();j++){
                DualSpace ds;
                ds=NEG::domDualSubspace_1(useful_topmost[j], (*it_bloc_data)[i], d);
                
                if (ds.dom==All && useful_topmost[j][0] >= (*it_bloc_data)[i][0]) {all_yes[i]=true;break;}

                // compute where to put the pair
                int pair_position=mainDataset.size() - block_position - (useful_topmost[j][0]/buffer_size) +decalage;
                if (pair_position-1>=0)
                {   auto it=(*it_bloc_pair)[i].begin();;
                    for (int m=0;m<pair_position-1;m++) it++;
                    it->insert(ds);    
                }
                else{
                    auto it=(*it_bloc_pair)[i].begin();
                    it->insert(ds);    
                }
            }
            // compress only if the tuple is not totally dominated
            if(!all_yes[i]){ 
                vector<list<DualSpace>> pairsToCompress((*it_bloc_pair)[i].size());                
                int indice=0;
                for (auto it_list = (*it_bloc_pair)[i].begin() ; it_list!=(*it_bloc_pair)[i].end();it_list++){
                    pairsToCompress[indice].insert(pairsToCompress[indice].begin(),it_list->begin(),it_list->end()); 
                    indice++;
                }
                (*it_bloc_pair)[i].clear();
                CompresserParInclusion_cascade(pairsToCompress, d, (*it_bloc_pair)[i]);
            }
        }
        //*********************************************************************


        //********************************************************************
        // delete completely dominated tuples
        int nb_non_domines=0;
        for (int i = 0; i<(*it_bloc_data).size();i++){if(!all_yes[i]) nb_non_domines++;}
        VectorListUSetDualSpace bloc_pairs_to_swap(nb_non_domines);
        TableTuple dataset_to_swap(nb_non_domines);
        int j=0;
        for (int i = 0; i<(*it_bloc_data).size();i++){
            if(!all_yes[i]){
                bloc_pairs_to_swap[j]=(*it_bloc_pair)[i];
                dataset_to_swap[j]=(*it_bloc_data)[i];
                j++;
            }
        }
        (*it_bloc_pair).swap(bloc_pairs_to_swap);
        (*it_bloc_data).swap(dataset_to_swap);
        //********************************************************************


        it_bloc_data++;
        it_bloc_pair++;
        block_position++;
    }

    //*******************************************************************************
  
}



void expiration(ListVectorListUSetDualSpace &listVcUSetDualSpace){

    for (auto it_list = listVcUSetDualSpace.begin(); it_list != listVcUSetDualSpace.end(); it_list++){
        for (auto it_vector = it_list->begin(); it_vector!=it_list->end(); it_vector++){
            it_vector->pop_back();
        }
    }

}


void CompresserParInclusion(list<DualSpace> &l){
    l.sort(NEG::pet_pair);
    for(auto it=l.begin(); it!=l.end();it++){
        auto it1=it;
        it1++;
        while(it1!=l.end() ){
            if (estInclusDans((*it1).dom,(*it).dom) && (estInclusDans((*it1).equ, (*it).dom + (*it).equ))){
                it1=l.erase(it1);
            }
            else{
                it1++;
            }
        }
    }
}

void compresserParInclusion2liste(list<DualSpace> &l,list<DualSpace> &s){

s.sort(NEG::pet_pair);

    auto it=l.begin();
    while(it!=l.end()){
        auto it1=s.begin();
        bool comp=(spacePair(*it)<=spacePair(*it1));
        while(it1!=s.end() && comp){
            if (estInclusDans((*it).dom,(*it1).dom) && (estInclusDans((*it).equ, (*it1).dom + (*it1).equ))){
                it=l.erase(it);
                break;
            }
            it1++;
        }
        if(it1==s.end() || !comp){
            it++;
        }
    }
}

void CompresserParInclusion_cascade(vector<list<DualSpace>> &toCompress, Space d, ListUSetDualSpace &l){

    list<DualSpace> lds;
    for (int i=0;i<toCompress.size();i++){ 
        USetDualSpace usDs;
        CompresserParInclusion(toCompress[i]);
        if(i>0){
            lds.insert(lds.begin(),toCompress[i-1].begin(),toCompress[i-1].end());
            lds.sort(pet_pair);
            auto it=toCompress[i].begin();
            while(it!=toCompress[i].end()){
                auto it1=lds.begin();
                bool comp=(spacePair(*it)<=spacePair(*it1));
                while(it1!=lds.end() && comp){
                    if (estInclusDans((*it).dom,(*it1).dom) && (estInclusDans((*it).equ, (*it1).dom + (*it1).equ))){
                        it=toCompress[i].erase(it);
                        break;
                    }
                    it1++;
                }
                if(it1==lds.end() || !comp){
                    it++;
                }
            }
        }
        fusionGloutonne(toCompress[i],d);
        usDs.insert(toCompress[i].begin(),toCompress[i].end());
        l.push_back(usDs);
    }

}

void InitStructure (list<TableTuple> &mainDataset, TableTuple &topmost, ListVectorListUSetDualSpace &ltVcLtUsDs, Space d, int buffer_size ){

    int block_position=0;
    Space All=(1<<d)-1;
    int number_batch=mainDataset.size(); 

    for (auto it_listDataset=mainDataset.rbegin();it_listDataset!=mainDataset.rend();it_listDataset++){   
        VectorListUSetDualSpace bloc_pairs(buffer_size);
        
        bool all_yes[buffer_size];
        for (int k=0;k<buffer_size;k++) all_yes[k]=false;
        
        //***************************************************************************
        //for each record of the current transaction  
        #pragma omp parallel for schedule(dynamic)
        for (int i = 0; i<buffer_size;i++){

            vector<list<DualSpace>> pairsToCompress(block_position+1);
            vector<unordered_set<DualSpace>> pairsToCompress_in_set(block_position+1);
            USetDualSpace usDs;

            //for each record of the topmost
            for (int j=0; j<topmost.size();j++){
                int position_topmost=topmost[j][0]/buffer_size;
                DualSpace ds;
                ds=NEG::domDualSubspace_1(topmost[j], (*it_listDataset)[i], d);
                
                //if the tuple is older than the topmost tuple and ds= All then the tuple is dominated during its lifetime
                if(block_position <= position_topmost && ds.dom==All){all_yes[i]=true;break;}
                
                if(position_topmost <= block_position){
                    pairsToCompress_in_set[block_position- position_topmost].insert(ds);
                }
                else{
                    pairsToCompress_in_set[0].insert(ds);
                }
            }
            //compress pairs of non completely dominated tuples
            if(!all_yes[i]){
                for(int j=0;j<block_position+1;j++) {
                    pairsToCompress[j].insert(pairsToCompress[j].begin(), pairsToCompress_in_set[j].begin(), pairsToCompress_in_set[j].end());
                }
                CompresserParInclusion_cascade(pairsToCompress,d,bloc_pairs[i]);
            }
        }
        //********************************************************************************

        //*******************************************************
        //delete completely dominated tuples
        VectorListUSetDualSpace bloc_pairs_to_swap;
        TableTuple dataset_to_swap;
        for (int i = 0; i<buffer_size;i++){
            if(!all_yes[i]){
                bloc_pairs_to_swap.push_back(bloc_pairs[i]);
                dataset_to_swap.push_back((*it_listDataset)[i]);
            }
        }
        (*it_listDataset).swap(dataset_to_swap);
        bloc_pairs.swap(bloc_pairs_to_swap);
        //*******************************************************

        ltVcLtUsDs.push_front(bloc_pairs); 
        block_position++;
    }
}

}