#include "routingtablessolver.h"
#include <ctime>

routingTablesSolver::routingTablesSolver(const ConPriorMMap* conPriorMap, const ConPriorMMap* revConPriorMap, const map <int,int>* RevASNum)
    :topoSize(RevASNum->size()), conPriorMap(conPriorMap), revConPriorMap(revConPriorMap), RevASNum(RevASNum)
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


DijkstraBasedAlgotithm::DijkstraBasedAlgotithm(const ConPriorMMap *conPriorMap, const ConPriorMMap* revConPriorMap, const map<int, int> *RevASNum)
    :routingTablesSolver(conPriorMap, revConPriorMap, RevASNum), ShortestTopoPaths(vector<TopoPath>(topoSize)){}

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
        int maxPrior = -1;
        set<int> maxNodes;      //список наиболее приоритетных узлов

        auto mit = revConPriorMap->equal_range(node);
        for (auto it = mit.first; it!=mit.second; it++)
            //в it лежит указатель на <dst, <src, prior>> где dst = node
            if ((it->second).second > maxPrior){
                maxPrior = (it->second).second;
                maxNodes.clear();
                maxNodes.insert((it->second).first);
            } else if ((it->second).second == maxPrior)
                maxNodes.insert((it->second).first);

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
//        if (seenNodes.size()%10000==0)
//            cout<<"node "<<seenNodes.size()<<" of "<<topoSize<<endl;

        auto mit = conPriorMap->equal_range(node);
        for (auto iConPair=mit.first; iConPair!=mit.second; iConPair++){
            //в iConPair лежит указатель на < src, map<dst, prior>> где src = node
            if (seenNodes.count((iConPair->second).first) > 0)
                continue;   //уже были тут

            int nextNode = (iConPair->second).first;
            //nextNode - соседняя вершина, которую ещё не смотрели
            if (SPLengthKeyNode[nextNode] < SPLengthKeyNode[node]+1)
                continue;   //прежний путь короче
            if (SPLengthKeyNode[nextNode]==SPLengthKeyNode[node]+1){
                //если расстояния равны, то это особый случай
                auto prevNodeIt = ShortestTopoPaths[nextNode].rbegin();
                int prevNode = *(prevNodeIt + 1); //номер предыдущей вершины в текущем кратчайшем пути

                auto mit = revConPriorMap->equal_range(nextNode);
                int nodePr, prevNodePr;
                for (auto it = mit.first; it!=mit.second; it++)
                    //в it <dst, <src, prior>> где dst = nextNode
                    if ((it->second).first == node)
                        nodePr = (it->second).second;
                    else if ((it->second).first == prevNode)
                        prevNodePr = (it->second).second;

                if (prevNodePr > nodePr)
                        continue;   //прежний путь более приоритетный
                if (prevNodePr == nodePr){
                    //приоритеты равны, длины путей - тоже надо смотреть на номера узлов.
                    if (prevNode<node)
                        continue;
                }
            }
            //текущий путь лучше
            //удаление прошлой пары из мультимножества
            auto ipair = SPLengthKeyLength.equal_range(SPLengthKeyNode[nextNode]);
            for (auto it = ipair.first; it!=ipair.second; it++){
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
