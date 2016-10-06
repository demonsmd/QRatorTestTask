#include "routingtablessolver.h"
#include <ctime>

routingTablesSolver::routingTablesSolver(const vector< vector<int> >* conPriorMatrix, const map <int,int>* RevASNum){
    topoSize = RevASNum->size();
    fillRoutersInfo(RevASNum);

    cout<<"Making passible rules...        ";
    clock_t begin = clock();
    makePossibleRoutes(conPriorMatrix);	//получаем всевозможные пути от 0 до всех узлов без повторений
    cout<<"done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" ms"<<endl;

    cout<<"Calculating routers rules...    ";
    begin = clock();
    calculateRules(conPriorMatrix);			//вычисляем итоговые правила
    cout<<"done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" ms"<<endl;
    pintRules();
}

void routingTablesSolver::fillRoutersInfo(const map <int,int>* RevASNum){
    for (int i=0;i<topoSize;i++){
        BGPRouter router;
        router.nodeNumber=i;
        router.ASNumber = RevASNum->find(i)->second;
        routers.push_back(router);
    }
}

void routingTablesSolver::makePossibleRoutes(const vector< vector<int> >* conPriorMatrix){
    possibleTopoRoutes.resize(topoSize);
    set<int> seenNodes;		//множество просмотренных вершин
    vector<int> curPath;		//текущий путь обходи в глубину

    DFS(0, &curPath, &seenNodes, conPriorMatrix);	//запускаем обход в глубину

}

void routingTablesSolver::DFS(int curNode, vector<int>* curPath, set<int> *seenNodes, const vector< vector<int> >* conPriorMatrix){
    seenNodes->insert(curNode);
    curPath->push_back(curNode);
    possibleTopoRoutes[curNode].push_back(*curPath);	//заносим найденный путь (путь от 0 до curNode)

    for (int i=0;i< topoSize;i++){
        if (seenNodes->count(i)>0)	//уже быи там
            continue;
        // если в i-ом узле ещё не были и есть туда путь, то идём туда
        if ((*conPriorMatrix)[i][curNode]!=-1)
            DFS(i, curPath, seenNodes, conPriorMatrix);
    }
    seenNodes->erase(curNode);
    curPath->pop_back();
}

void routingTablesSolver::calculateRules(const vector< vector<int> >* conPriorMatrix){
    routers[0].announceRoute.push_back(0);
    for (int node = 1; node<topoSize; node++){
        //выбираем путь от 0 до node
        //определяем приоритеты:
        set<int> candidatesNumbers;	//список текущих приоритетных путей
        int maxPrior = -1;
        for (int parent = 0; parent<topoSize; parent++){
            if ((*conPriorMatrix)[node][parent]>maxPrior){	//нашли новый максимальный приоритет
                candidatesNumbers.clear();
                maxPrior=(*conPriorMatrix)[node][parent];
                for (int i=0; i<possibleTopoRoutes[node].size(); i++){
                    if (possibleTopoRoutes[node][i][possibleTopoRoutes[node][i].size()-2]==parent)
                        //в i-ое правило от 0 до node получено от приоритетного узла
                        candidatesNumbers.insert(i);
                }
            }
        }
        //в candidatesNumbers лежат номера путей, полученных от приоритетных соседей
        //запускаем сравнение по длинне
        int shortestLength = topoSize;
        for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end(); iter++)
            if (possibleTopoRoutes[node][*iter].size()<shortestLength)
                shortestLength=possibleTopoRoutes[node][*iter].size();

        for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end();)
            if (possibleTopoRoutes[node][*iter].size()>shortestLength)
                candidatesNumbers.erase(iter++);
            else
                iter++;


        //остались самые приоритетные пути наименьшей длинны
        //выберем наименьший номер соседа
        int minNeighbour = -1;
        int minAS = INF;
        for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end(); iter++){
            int curNum = possibleTopoRoutes[node][*iter][possibleTopoRoutes[node][*iter].size()-2];
            if (routers[curNum].ASNumber < minAS){
                minAS = routers[curNum].ASNumber;
                minNeighbour = curNum;
            }
        }

        for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end();){
            int curNum = possibleTopoRoutes[node][*iter][possibleTopoRoutes[node][*iter].size()-2];
            if (curNum!=minNeighbour)
                candidatesNumbers.erase(iter++);
            else
                iter++;
        }
        //остались только те, которые пришли от приоритетного предка с меньшим номером, минимальной длинны.
        //т.к. длинна одна и та же, то из этих правил дойдёт только одно.
        //определим узел, где пути различаются:
        int step = 2;
        int commonNode = node;
        while (candidatesNumbers.size()>1){
            set<int> parents;
            for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end();  iter++)
                parents.insert(possibleTopoRoutes[node][*iter]
                        [possibleTopoRoutes[node][*iter].size()-step]);
            if (parents.size()>1){
                //в commonNode лежит общий нод путей, который рассыпается на несколько (parents)
                //определение приоритета
                int maxPriority = -1;
                int  minNodeNum = -1;
                int minAS = INF;
                for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end(); iter++)
                    if (maxPriority<(*conPriorMatrix)[commonNode][possibleTopoRoutes[node]
                            [*iter][possibleTopoRoutes[node][*iter].size()-step]])
                        maxPriority=(*conPriorMatrix)[commonNode][possibleTopoRoutes[node]
                                [*iter][possibleTopoRoutes[node][*iter].size()-step]];

                //определение наименьшего номера среди приоритетов
                for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end(); iter++){
                    int prev = possibleTopoRoutes[node][*iter][possibleTopoRoutes[node][*iter].size()-step];
                    if (maxPriority == (*conPriorMatrix)[commonNode][prev]
                            && routers[prev].ASNumber < minAS){
                        minAS = routers[prev].ASNumber;
                        minNodeNum= prev;
                    }
                }

                for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end();)
                    if (maxPriority!=(*conPriorMatrix)[commonNode][possibleTopoRoutes[node]
                            [*iter][possibleTopoRoutes[node][*iter].size()-step]]
                            || possibleTopoRoutes[node][*iter][possibleTopoRoutes[node][*iter].size()-step]!=minNodeNum)
                        candidatesNumbers.erase(iter++);
                    else
                        iter++;
            }
            commonNode = *parents.begin();
            step++;
        }
        if (possibleTopoRoutes[node].size()>0)
            routers[node].announceRoute=possibleTopoRoutes[node][*candidatesNumbers.begin()];
    }
}

void routingTablesSolver::pintRules(){
    cout<<"============= RESULTS ================"<<endl;
    cout<<"AS number   |    announce path        "<<endl;
    for (int i=0; i<routers.size(); i++){
        cout<<"    "<<routers[i].ASNumber<<"            ";
        if (routers[i].announceRoute.size()==0)
            cout<<" -------- ";
        else
            for (int j=0; j<routers[i].announceRoute.size(); j++)
                cout<<routers[routers[i].announceRoute[j]].ASNumber<<".";
        cout<<endl;
    }

}
