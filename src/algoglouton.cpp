#include "algoglouton.h"

bool operator<(const DualSpace &a, const DualSpace &b){
    if (a.poids < b.poids) return true;
    if (a.dom < b.dom) return true;
    else return a.equ<b.equ;
}

void listeAttributsPresents(Space subspace, Space d, vector<Space> &result){
/*  retourne la liste des attributs d'un sous espace
    le résultat se présente sous la forme d'un vector
    où chaque cellule correspond au numéro de l'attribut 1, 2, 3, ...,d
    la première valeur [0] est le nombre d'attributs */
    vector<Space> sortie;
    bitset<SPACESIZE> subspace_aux=subspace;
    Space j;
    for (j=0;j<d;j++) if (subspace_aux[j]) sortie.push_back(j+1);
    result.swap(sortie);
}

void listSousEspacesR(vector<Space> &listSubspace, Space actualSpace, vector<Space> &missingAttrib, Space nbDiscarded){
//  aide pour la recherche des sous-espaces d'un espace donné.
//  doit être appelée par la procédure ci-dessous
    Space i, newActual;
    for (i=nbDiscarded;i<(Space)missingAttrib.size();i++){
        newActual=actualSpace+(1<<(missingAttrib[i]-1));
        listSubspace.push_back(newActual);
        listSousEspacesR(listSubspace, newActual, missingAttrib, i+1);
    }
}


void inline listSousEspaces(vector<Space> &listSubspace, Space &originSpace, const Space &d){
//  Cette procédure liste de façon récursive les sous-espaces d'un espace donné
//  appelle la procédure ci-dessus
    vector<Space> listSubspaceR;
    vector<Space> missingAttrib;
    listeAttributsPresents(originSpace, d, missingAttrib);
    listSousEspacesR(listSubspaceR, 0, missingAttrib, 0);
    listSubspace.swap(listSubspaceR);
}


void inline listCouvertsR(Space actualSpace, vector<Space> &missingAttrib, Space nbDiscarded, vector<Space> &listEquSubspace, vector<Space> &listSpace){
//  aide à retrouver les espacescouverts par un DualSPace donné
//  doit être appelé par la procédure ci-dessous
    Space newActual;
    Space i, j;
    for (i=nbDiscarded;i<(Space)missingAttrib.size();i++){
        newActual=actualSpace+(1<<(missingAttrib[i]-1));
        listSpace.push_back(newActual);
        for (j=0;j<(Space)listEquSubspace.size();j++){
            listSpace.push_back(newActual+listEquSubspace[j]);
        }
        listCouvertsR(newActual, missingAttrib, i+1, listEquSubspace, listSpace);
    }
}

void listCouverts(Space domSpace, Space equSpace, Space &d, vector<Space> &listSpace){
//  Liste l'ensemble des espaces couverts par un dualspace donné
//  appelle la procédure ci-dessus
    vector<Space> listSubspace;
    listSousEspaces(listSubspace, equSpace, d);
    vector<Space> missingAttrib;
    listeAttributsPresents(domSpace, d, missingAttrib);
    Space s1=0, s2=0;
    listCouvertsR(s1, missingAttrib, s2, listSubspace, listSpace);
}


void set_of_bits(int i, 	vector<int> &V){
    //on récupère les position des bits à 1 dans i qu'on met dans V
    //Par ex: si i = 101 (en binaire) alors V=[1, 3], i.e., les bits 1 et 3 sont à 1
    int j=__builtin_ffs(i);//on récupère la position du bit le plus faible positionné à 1	
    while(i!=0){
        if (j==1) {
            i=i-1;
            V.push_back(j);
        }
        else{
            i = (i & (~(1<<(j-1))));//on met à 0 le bit j
            V.push_back(j);
        }
        j=__builtin_ffs(i);//on récupère la position du prochain bit à 1
    }
}

void mis(vector<int> &f, int i, vector<vector<int> > &V1)
{
	//on génère les sous-ensemble de f qu'on met dans V1
	// Ex: si f=[1, 3], alors V1=[ [1], [3], [1,3] ]
	// Noter qu'on ne met pas l'ensemble vide dans V1
	vector<int> v;
	if (i >f.size()){
		return ;
	}
	else if (i == f.size()){
		for(int j = 0; j < f.size(); ++j){
			if (f[j]!=0) {
				v.push_back(f[j]);
			}
		}
		if(v.size()>0) V1.push_back(v);
		return ;
	}
	int x=f[i];
	f[i] = 0;
	mis(f, i + 1, V1);
	f[i] = x;
	mis(f, i + 1, V1);
	return ;
}

void vect_to_int(vector<int> &V, int &i){
	//transforme un vecteur V de positions en un entier i
	//Ex: si V=[1, 3, 4] alors i = 1101 (en binair)
	i=0;
	for(int j=0; j<V.size();j++){
		if(V[j]==1){
			i++;
		}
		else{
			i+=(1<<(V[j]-1));
		}
	}
}

void vect_of_vect_to_vect_of_int (vector<vector<int> > &v1, vector<int> &v2){
	for(int i=0; i<v1.size();i++){
		int k;
		vect_to_int(v1[i],k);
		v2.push_back(k);
	}
}

void deleteEmptyEntries(vector<Space> &poidsDualSpace, vector<DataType> &indiceValide){
    DataType i;
    vector<DataType> result;
    for (i=0;i<(DataType)indiceValide.size();i++) if (poidsDualSpace[indiceValide[i]]>0) result.push_back(indiceValide[i]);
    indiceValide.swap(result);
}

void fusionGloutonne(USetDualSpace &uSetDualSpace, Space d){
	//fonction qui retourne un sous-ensemble minimal de USetDualSpace et qui lui est équivalent
	//le principe de la fonction est un algorithme glouton
    DataType i;
    USetDualSpace result;
    Space all=1<<d;

    map<DualSpace, vector<Space>> M;

    auto itu=uSetDualSpace.begin();
    //pour chaque paire, on génère les espaces qu'elle couvre 
    while (itu!=uSetDualSpace.end()){
        vector<Space> listCouv;
        listCouverts(itu->dom, itu->equ, d, listCouv);//les espaces couverts par la paire itu
        M.insert(pair<DualSpace, vector<Space>>(*itu, listCouv));
        ++itu;
    }



    DataType maxValue;
    map<DualSpace, vector<Space>>::iterator itX, itm, iti;
    DualSpace newDs, inter, ds;
    DataType nbDelete;
    vector<Space>::iterator itj, itk;

    while(M.size()>0){
        //on cherche la paire qui couvre le max d'espaces non encore couverts
        itX=M.begin();
        maxValue=(itX->second).size();
        itm=M.begin();
        while (itm!=M.end()){
            if ((Space)(itm->second).size()>maxValue){
                itX=itm;
                maxValue=(itm->second).size();
            }
            itm++;
        }
	   //on met les espaces couverts par la paire choisie dans un vecteur intermédiaire
        newDs = itX->first;
	   //on ajoute cette paire au résultat
        result.insert(newDs);
	   //on efface cette paire
        M.erase(itX);
        
        nbDelete=0;
        for (iti=M.begin(); iti!=M.end(); iti++){
            ds=iti->first;
		  //on construit une paire inter qui résume les espaces communs à newDS et iti->first
            //inter.dom = newDs.dom & ds.dom;
            //inter.equ = soustraction((newDs.dom+newDs.equ) & (ds.dom+ds.equ), inter.dom);
            vector<Space> usp;
            //chaque espace de iti->second qui n'est pas couvert par inter, on le met dans usp
            for (auto itk=(iti->second).begin();itk!=(iti->second).end();itk++){
                //if (!estCouvertPar(*itk,inter)) usp.push_back(*itk);
				if (!estCouvertPar(*itk,newDs)) usp.push_back(*itk);
            }
            //les espaces qui restent associés à iti sont ceux non couverts par inter, cad ceux dans usp
		  //on fait donc l'échange entre les deux
            (iti->second).swap(usp);
            if ((iti->second).size()==0) nbDelete++;
        }
        //on enlève de M toutes les paires dont le nombre d'espaces qui restent à couvrir est nul.
        if (nbDelete>0){
            iti=M.begin();
            while (iti!=M.end()){
                itX=iti;iti++;
                if ((itX->second).size()==0) M.erase(itX);
            }
        }
    }
    uSetDualSpace.swap(result);
}