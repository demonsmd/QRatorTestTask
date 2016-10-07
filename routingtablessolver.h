#ifndef ROUTINGTABLESSOLVER_H
#define ROUTINGTABLESSOLVER_H

#include "types.h"
#include "matrix.h"

#include <map>
#include <vector>
#include <set>

class routingTablesSolver
{
public:
    routingTablesSolver(const SqrMatrix<int>* conPriorMatrix, const map <int,int>* RevASNum);
    const vector<BGPRouter>* getBGPRouters(){return &routers;}
    virtual void Solve() = 0;

protected:
    unsigned topoSize;                           //размер топологии
    const SqrMatrix<int>* conPriorMatrix;   //матрица смежности
    const map <int,int>* RevASNum;          //отображение внутренних номеров АС на реальные
    vector<BGPRouter> routers;              //набор BGP роутеров сети

    void fillRoutersInfo();                 //заполняет routers экземплярами BGPRouter с указаниями внутренних и реальных номеров АС
    void pintRules();                       //выводит таблицы маршрутизации с путями всех маршрутизаторов в routers
};


class PossibleTopoRoutesBasedAlgorithm: public routingTablesSolver
//This algorithm works, but it has too much complexity.
{
public:
    PossibleTopoRoutesBasedAlgorithm(const SqrMatrix<int>* conPriorMatrix, const map <int,int>* RevASNum);
    virtual void Solve();

private:
    vector<NodeTopoPaths> possibleTopoRoutes;

    void makePossibleRoutes();
    void DFS (int curNode, vector<int>* curPath, set<int>* seenNodes);
    void calculateRules();
};

class DijkstraBasedAlgotithm: public routingTablesSolver
{
    vector<TopoPath> ShortestTopoPaths;         //список кратчайших путей от истока в каждую из вершин
    map<int, int> SPLengthKeyNode;              //<node, length>
    multimap<int, int> SPLengthKeyLength;              //<node, length>

    void ModifiedDijkstraAlgorithm();
    void calculateRules();

public:
    DijkstraBasedAlgotithm(const SqrMatrix<int>* conPriorMatrix, const map <int,int>* RevASNum);
    virtual void Solve();
};

#endif // ROUTINGTABLESSOLVER_H
