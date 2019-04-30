#ifndef _DECLARATIONS_H_INCLUDED_
#define _DECLARATIONS_H_INCLUDED_

using namespace std;
#include <iostream>
#include <list>
#include <vector>
#include <bitset>
#include <ctime>
#include <fstream>
#include <sstream>
#include <stack>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <omp.h>
#include <queue>

#define SPACESIZE	30

#define NB_METHOD 12
#define NAIF 0
#define TREE 1
#define CSC 2
#define BUA 3
#define UBA 4
#define DPI 5
#define NSC 6
#define NSC2 7
#define NSCk 8
#define NSC3 9
#define NSCv2 10
#define NSCwM 11



struct DualSpace;
struct TupleSpace;


typedef int DataType;
typedef DataType* Point;
typedef int  Space;
typedef vector<map<int,vector<int>>> values_map;
typedef unordered_set<DualSpace> USetDualSpace;
typedef unordered_map<DualSpace, int> mapDualSpace;
typedef vector<DualSpace> VectorDualSpace;
typedef vector<TupleSpace> VectorTupleSpace;
typedef vector<VectorTupleSpace> VectorVectorTupleSpace;
typedef vector<Point> TableTuple;
typedef map<int,Point> TableTuplev2;
typedef unordered_set<DataType> USetId;
typedef vector<USetId> VectorUSetId;
typedef unordered_map<Space, unordered_map<Space,vector<DataType>>> NegSkyStrAux;
typedef vector<pair<Space,vector<pair<Space,vector<DataType>>>>> NegSkyStr;
typedef list<USetDualSpace> ListUSetDualSpace;
typedef vector<ListUSetDualSpace> VectorListUSetDualSpace;
typedef list<VectorListUSetDualSpace> ListVectorListUSetDualSpace;
typedef list<TableTuple> listTT;
// NSCv2
typedef map<int,map<string,pair<bool,pair<DualSpace,vector<int>>>>> index_structure;


inline Space spaceSize(Space subspace);

struct DualSpace{
// couple de deux sous espaces (dom, equ), résultat de comparaison entre deux tuples t_i et t_j, où:
//  - dom est le sous-espace dans lequel le premier tuple domine strictement le second (t_i[dom]<t_j[dom])
//  - equ est le sous-espace dans lequel les deux tuples sont égaux t_i[equ]=t_j[equ]
    Space dom;
    Space equ;
    Space poids;
    bool operator==(const DualSpace &ds)const{
        return (this->dom==ds.dom && this->equ==ds.equ);
    }
    bool operator!=(const DualSpace &ds)const{
        return (this->dom!=ds.dom || this->equ!=ds.equ);
    }
    Space dsSize(Space d){
        return spaceSize(dom+equ);
    }
};



namespace std {
    template <>
    struct hash<DualSpace>  {
        public:
            size_t operator()(const DualSpace &s) const{
                //polynome de Cantor f(x,y)= ((x+y)^2+3x+y)/2
                return (pow(s.dom+s.equ, 2)+3*s.dom+s.equ)/2;
            }
    };
}

struct TupleSpace{
// couple formé de l'identifiant d'un tuple idTuple et d'un sous-espace eqSubspace où il n'est pas strictement dominé
//  relativement au couple (dom, eqSubspace) qui l'a généré.
    DataType idTuple;
    Space eqSubspace;
    const bool operator<(const TupleSpace &ts2) const{
        return (this->idTuple<ts2.idTuple);
    }
};

inline double debut(){
    return omp_get_wtime();
}

inline double duree(double start){
// calcule la durée en millisecondes entre l'instant 'start' et maintenant
;
    //return (clock()-start)*(1000.0/CLOCKS_PER_SEC);
    return (omp_get_wtime()-start)*(1000.0);
}

inline bool disjoints(const Space &spc1, const Space &spc2){
/*  teste si spc1 et spc2 sont disjoints tous codés en décimal
    exemples (codage) quand d=4, ABCD->15, AD->9, A->1, B->2, C->4, D->8, BD->10
    retourne vrai s'ils sont disjoints et faux sinon                */
    return ((spc1 & spc2)==0);
}

inline bool estInclusDans(const Space &spc1, const Space &spc2){
/*  test si spc1 est inclus dans spc2 tous codés en décimal
    exemples (codage) quand d=4, ABCD->15, AD->9, A->1, B->2, C->4, D->8, BD->10
    retourne vrai si cette inclusion est vérifiée et faux sinon                  */
    return ((spc1 & spc2)==spc1);
}

inline bool estEgale(const Space &spc1, const Space &spc2){
/*  test si spc1 est inclus dans spc2 tous codés en décimal
    exemples (codage) quand d=4, ABCD->15, AD->9, A->1, B->2, C->4, D->8, BD->10
    retourne vrai si cette inclusion est vérifiée et faux sinon                  */
    return (spc1==spc2);
}

inline Space soustraction(const Space &spc1, const Space &spc2){
/*  retourne spc1\spc2                  */
    return (spc1 & ~spc2);
}

inline int quiReduitQui_DS(const DualSpace &ds1, const DualSpace &ds2){
    //teste entre deux DualSpace lequel est susceptible de simplifier l'autre 
    if ((ds1.dom | ds2.dom)==0) return 0;
    if (estInclusDans(ds2.dom+ds2.equ,ds1.dom)) return 1;
    if (estInclusDans(ds1.dom+ds1.equ,ds2.dom)) return 2;
    return 0;
}

inline bool estCouvertPar(const Space &sp, const DualSpace &ds){
//teste si un sous-espace est couvert par un couple (DualSPace)
    return ((sp & (ds.dom+ds.equ))==sp && !disjoints(sp, ds.dom));
}

inline bool estCouvertPar(const DualSpace &ds1, const DualSpace &ds2){
//teste si un couple d'espaces est couvert par un autre couple (DualSPace)
    return estInclusDans(ds1.dom+ds1.equ, ds2.dom+ds2.equ) && estInclusDans(ds1.dom, ds2.dom);
}

inline void sortIndexes(const vector<DataType> &tab, vector<DataType> &index) {
//Cette procédure trie le vecteur index suivant l'ordre de leur valeur associées contenues dans le vecteur tab
  sort(index.begin(), index.end(), [&tab](DataType i1, DataType i2) {return tab[i1] < tab[i2];});
}

inline void sortIndexes(const vector<double> &tab, vector<DataType> &index) {
//Cette procédure trie le vecteur index suivant l'ordre de leur valeur associées contenues dans le vecteur tab
  sort(index.begin(), index.end(), [&tab](DataType i1, DataType i2) {return tab[i1] < tab[i2];});
}

inline vector<DataType>::iterator maxIndexes(const vector<DataType> &tab, vector<DataType> &index) {
//Cette procédure trie le vecteur index suivant l'ordre de leur valeur associées contenues dans le vecteur tab
  return max_element(index.begin(), index.end(), [&tab](DataType i1, DataType i2) {return tab[i1] < tab[i2];});
}



inline Space spaceSize(const Space subspace){
/*  retourne la taille d'un sous-espace (nombre d'attributs */
    return __builtin_popcount(subspace);
}

inline Space spacePair(const DualSpace p){
 //retourne la taille d'un sous-espace (nombre d'attributs 
    return (__builtin_popcount(p.dom)+__builtin_popcount(p.equ));
}

class mendl
{
public:
    explicit mendl(unsigned int i) : n(i) {}
private:
    unsigned int n;

    template <class charT, class Traits>
    friend basic_ostream<charT,Traits>& operator<< (
                                         basic_ostream<charT,Traits>& os,
                                         const mendl& w)
    {
        // the manipulation: insert end-of-line characters and flush
        for (unsigned int i=0; i<w.n; i++)
            os << '\n';
        os.flush();
        return os;
    }
};

#endif // DECLARATIONS_H_INCLUDED
