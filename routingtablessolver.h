#ifndef ROUTINGTABLESSOLVER_H
#define ROUTINGTABLESSOLVER_H

#include "types.h"
#include <map>
#include <vector>
#include <set>

class routingTablesSolver
{
public:
    routingTablesSolver(const vector< vector<int> >* conPriorMatrix, const map <int,int>* RevASNum);
    const vector<BGPRouter>* getBGPRouters(){return &routers;}

private:
    int topoSize;
    vector<BGPRouter> routers;
    vector<vector<vector<int> > > possibleTopoRoutes;

    void fillRoutersInfo(const map <int,int>* RevASNum);
    void makePossibleRoutes(const vector< vector<int> >* conPriorMatrix);
    void DFS (int curNode, vector<int>* curPath, set<int>* seenNodes, const vector< vector<int> >* conPriorMatrix);
    void calculateRules(const vector< vector<int> >* conPriorMatrix);
    void pintRules();
};

#endif // ROUTINGTABLESSOLVER_H
