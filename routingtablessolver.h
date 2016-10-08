#ifndef ROUTINGTABLESSOLVER_H
#define ROUTINGTABLESSOLVER_H

#include "types.h"

#include <map>
#include <vector>
#include <set>

class routingTablesSolver
{
public:
    routingTablesSolver(const ConPriorMMap* conPriorMap, const ConPriorMMap* revConPriorMap, const map <int,int>* RevASNum);
    const vector<BGPRouter>* getBGPRouters(){return &routers;}
    virtual void Solve() = 0;

protected:
    unsigned topoSize;                           //размер топологии
    const ConPriorMMap* conPriorMap;      //внутреннее представление сети
    const ConPriorMMap* revConPriorMap;      //обратное внутреннее представление сети
    const map <int,int>* RevASNum;          //отображение внутренних номеров АС на реальные
    vector<BGPRouter> routers;              //набор BGP роутеров сети

    void fillRoutersInfo();                 //заполняет routers экземплярами BGPRouter с указаниями внутренних и реальных номеров АС
    void pintRules();                       //выводит таблицы маршрутизации с путями всех маршрутизаторов в routers
};

class DijkstraBasedAlgotithm: public routingTablesSolver
{
    vector<TopoPath> ShortestTopoPaths;         //список кратчайших путей от истока в каждую из вершин
    map<int, int> SPLengthKeyNode;              //<node, length>
    multimap<int, int> SPLengthKeyLength;              //<node, length>

    void ModifiedDijkstraAlgorithm();
    void calculateRules();

public:
    DijkstraBasedAlgotithm(const ConPriorMMap* conPriorMap, const ConPriorMMap* revConPriorMap, const map <int,int>* RevASNum);
    virtual void Solve();
};

#endif // ROUTINGTABLESSOLVER_H
