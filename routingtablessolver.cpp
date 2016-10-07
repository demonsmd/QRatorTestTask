#include "routingtablessolver.h"
#include <ctime>

routingTablesSolver::routingTablesSolver(const SqrMatrix<int>* conPriorMatrix, const map <int,int>* RevASNum)
    :topoSize(RevASNum->size()), conPriorMatrix(conPriorMatrix), RevASNum(RevASNum)
{
    fillRoutersInfo();
}

void routingTablesSolver::fillRoutersInfo(){
    for (unsigned i=0;i<topoSize;i++){
        BGPRouter router;
        router.nodeNumber=i;
        router.ASNumber = RevASNum->find(i)->second;
        routers.push_back(router);
    }
}

void routingTablesSolver::pintRules(){
    logStream<<"============= RESULTS ================"<<endl;
    logStream<<"AS number   |    announce path        "<<endl;
    for (unsigned i=0; i<routers.size(); i++){
        logStream<<"    "<<routers[i].ASNumber<<"            ";
        if (routers[i].announceRoute.size()==0)
            logStream<<" -------- ";
        else
            for (unsigned j=0; j<routers[i].announceRoute.size(); j++)
                logStream<<routers[routers[i].announceRoute[j]].ASNumber<<".";
        logStream<<endl;
    }
    cout<<"see topo rules in "<<logFile<<endl;
}

PossibleTopoRoutesBasedAlgorithm::PossibleTopoRoutesBasedAlgorithm(const SqrMatrix<int> *conPriorMatrix, const map<int, int> *RevASNum):routingTablesSolver(conPriorMatrix, RevASNum)
{}

void PossibleTopoRoutesBasedAlgorithm::makePossibleRoutes(){
    possibleTopoRoutes.resize(topoSize);
    set<int> seenNodes;		//множество просмотренных вершин
    vector<int> curPath;		//текущий путь обхода в глубину

    DFS(0, &curPath, &seenNodes);	//запускаем обход в глубину

}

void PossibleTopoRoutesBasedAlgorithm::Solve(){
    cout<<"Making passible rules...        ";
    logStream<<"Making passible rules...        ";
    clock_t begin = clock();
    makePossibleRoutes();	//получаем всевозможные пути от 0 до всех узлов без повторений
    cout<<"done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
    logStream<<"done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;

    cout<<"Calculating routers rules...    ";
    logStream<<"Calculating routers rules...    ";
    begin = clock();
    calculateRules();			//вычисляем итоговые правила
    cout<<"done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
    logStream<<"done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
    pintRules();
}

void PossibleTopoRoutesBasedAlgorithm::DFS(int curNode, vector<int>* curPath, set<int> *seenNodes){
    seenNodes->insert(curNode);
    curPath->push_back(curNode);
    possibleTopoRoutes[curNode].push_back(*curPath);	//заносим найденный путь (путь от 0 до curNode)

    for (unsigned i=0;i<topoSize;i++){
        if (seenNodes->count(i)>0)	//уже быи там
            continue;
        // если в i-ом узле ещё не были и есть туда путь, то идём туда
        if (conPriorMatrix->getElem(i,curNode)!=-1)
            DFS(i, curPath, seenNodes);
    }
    seenNodes->erase(curNode);
    curPath->pop_back();
}

void PossibleTopoRoutesBasedAlgorithm::calculateRules(){
    routers[0].announceRoute.push_back(0);
    for (unsigned node = 1; node<topoSize; node++){
        //выбираем путь от 0 до node
        //определяем приоритеты:
        set<int> candidatesNumbers;	//список текущих приоритетных путей
        int maxPrior = -1;
        for (unsigned parent = 0; parent<topoSize; parent++){
            if (conPriorMatrix->getElem(node, parent) > maxPrior){	//нашли новый максимальный приоритет
                candidatesNumbers.clear();
                maxPrior=conPriorMatrix->getElem(node, parent);
                for (unsigned i=0; i<possibleTopoRoutes[node].size(); i++){
                    if (possibleTopoRoutes[node][i][possibleTopoRoutes[node][i].size()-2]==parent)
                        //в i-ое правило от 0 до node получено от приоритетного узла
                        candidatesNumbers.insert(i);
                }
            }
        }
        //в candidatesNumbers лежат номера путей, полученных от приоритетных соседей
        //запускаем сравнение по длинне
        unsigned shortestLength = topoSize;
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
                    if (maxPriority<conPriorMatrix->getElem(commonNode, possibleTopoRoutes[node]
                            [*iter][possibleTopoRoutes[node][*iter].size()-step]))
                        maxPriority=conPriorMatrix->getElem(commonNode, possibleTopoRoutes[node]
                                [*iter][possibleTopoRoutes[node][*iter].size()-step]);

                //определение наименьшего номера среди приоритетов
                for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end(); iter++){
                    int prev = possibleTopoRoutes[node][*iter][possibleTopoRoutes[node][*iter].size()-step];
                    if (maxPriority == conPriorMatrix->getElem(commonNode, prev)
                            && routers[prev].ASNumber < minAS){
                        minAS = routers[prev].ASNumber;
                        minNodeNum= prev;
                    }
                }

                for (set<int>::iterator iter = candidatesNumbers.begin(); iter!=candidatesNumbers.end();)
                    if (maxPriority!=conPriorMatrix->getElem(commonNode, possibleTopoRoutes[node]
                            [*iter][possibleTopoRoutes[node][*iter].size()-step])
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


DijkstraBasedAlgotithm::DijkstraBasedAlgotithm(const SqrMatrix<int> *conPriorMatrix, const map<int, int> *RevASNum)
    :routingTablesSolver(conPriorMatrix, RevASNum), ShortestTopoPaths(vector<TopoPath>(topoSize)){}

void DijkstraBasedAlgotithm::Solve(){
    clock_t begin = clock();
    cout<<"Applying ModifiedDijkstraAlgorithm...";
    logStream<<"Applying ModifiedDijkstraAlgorithm...";
    //===============================================================================
    ModifiedDijkstraAlgorithm();    //в ShortestTopoPaths кратчайшие пути с учётом приоритетов.
    //===============================================================================
    cout<<"      done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
    logStream<<"      done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
    cout<<"Calculating rules...";
    logStream<<"Calculating rules...";
    begin = clock();
    //===============================================================================
    calculateRules();       //вычисление таблиц маршрутизации на основе кратчайших путей
    //===============================================================================
    cout<<"      done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
    logStream<<"      done in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
    pintRules();
}

void DijkstraBasedAlgotithm::calculateRules(){
    for (unsigned node = 1; node<topoSize; node++){
        //для каждого узла определяем маршрут
        int max = -1;
        set<int> maxNodes;      //список наиболее приоритетных узлов
        for (unsigned prevNode=0; prevNode<topoSize; prevNode++)
            if (conPriorMatrix->getElem(node, prevNode)>max){
                max = conPriorMatrix->getElem(node, prevNode);
                maxNodes.clear();
                maxNodes.insert(prevNode);
            } else if (conPriorMatrix->getElem(node, prevNode) == max)
                maxNodes.insert(prevNode);
        if (maxNodes.size()==1){
            routers[node].announceRoute = ShortestTopoPaths[*maxNodes.begin()];
            continue;
        } else {
            //есть несколько приоритетных вершин. Побеждает та, у которой путь короче.
            set<int> minLenNodes;   //список наиболее приоритетных вершин с минимальными путями
            unsigned minPathLen = INF;
            for (auto it = maxNodes.begin(); it!= maxNodes.end(); it++)
                if (ShortestTopoPaths[*it].size() < minPathLen){
                    minPathLen = ShortestTopoPaths[*it].size();
                    minLenNodes.clear();
                    minLenNodes.insert(*it);
                } else if (ShortestTopoPaths[*it].size() == minPathLen)
                    minLenNodes.insert(*it);
            if (minLenNodes.size()==1){
                routers[node].announceRoute = ShortestTopoPaths[*minLenNodes.begin()];
                continue;
            } else {
                //есть несколько приоритетных вершин с одинаковыми по длинне путями. Побеждает минимальный номер АС
                int minASnum = INF;
                int ASnode;
                for (auto it = minLenNodes.begin(); it!= minLenNodes.end(); it++){
                    if (routers[*it].ASNumber<minASnum){
                        minASnum = routers[*it].ASNumber;
                        ASnode = *it;
                    }
                }
                routers[node].announceRoute = ShortestTopoPaths[ASnode];
                continue;
            }
        }
    }
}

void DijkstraBasedAlgotithm::ModifiedDijkstraAlgorithm(){
    set<int> seenNodes;     //множество просмотренных узлов
    unsigned node;

    vector<int> path;
    path.push_back(0);
    ShortestTopoPaths[0] = path;        //путь до истока уже найден
    SPLengthKeyLength.insert(pair<int, int>(0, 0)); //обнуляем метки истока
    SPLengthKeyNode.insert(pair<int, int>(0, 0)); //обнуляем метки истока

    for(unsigned i=1; i<topoSize; i++){//заносим INF во остальные
        SPLengthKeyLength.insert(pair<int, int>(INF, i));
        SPLengthKeyNode.insert(pair<int, int>(i, INF));
    }

    while (seenNodes.size()!=topoSize){
        //определяем нод с меньшей меткой
        //std::map сортирует по возрастанию ключа
        for (auto it = SPLengthKeyLength.begin(); it!=SPLengthKeyLength.end(); it++)
            if (seenNodes.count(it->second)==0){
                node = it->second;          //это вершина, которую ещё не рассматривали и у неё наименьшая метка
                break;
            }

        seenNodes.insert(node);
        for (unsigned nextNode = 0; nextNode<topoSize; nextNode++){
            if (seenNodes.count(nextNode)>0)
                continue;
            if (conPriorMatrix->getElem(nextNode, node)>=0){
                //nextNode - соседняя вершина, которую ещё не смотрели
                if (SPLengthKeyNode[nextNode] < SPLengthKeyNode[node]+1)
                    continue;   //прежний путь короче
                if (SPLengthKeyNode[nextNode]==SPLengthKeyNode[node]+1){
                    //если расстояния равны, то это особый случай
                    auto prevNodeiT = ShortestTopoPaths[nextNode].rbegin();
                    int prevNode = *(prevNodeiT + 1);
                    if (conPriorMatrix->getElem(nextNode, prevNode) > conPriorMatrix->getElem(nextNode, node))
                        continue;   //прежний путь более приоритетный
                    if (conPriorMatrix->getElem(nextNode, prevNode) == conPriorMatrix->getElem(nextNode, node)){
                        //приоритеты равны, длины путей - тоже надо смотреть на номера узлов.
                        if (prevNode<node)
                            continue;
                    }
                }
                //текущий путь лучше
                //удаление прошлой пары из мультимножества
                auto iterpair = SPLengthKeyLength.equal_range(SPLengthKeyNode[nextNode]);
                for (auto it = iterpair.first; it!=iterpair.second; it++){
                    if (it->second == nextNode) {
                            SPLengthKeyLength.erase(it);
                            break;
                        }
                }

                SPLengthKeyLength.insert(pair<int, int>(SPLengthKeyNode[node]+1, nextNode));
                SPLengthKeyNode[nextNode] = SPLengthKeyNode[node]+1;
                ShortestTopoPaths[nextNode] = ShortestTopoPaths[node];
                ShortestTopoPaths[nextNode].push_back(nextNode);
            }
        }
    }
}
