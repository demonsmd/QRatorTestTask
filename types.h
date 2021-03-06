#ifndef TYPES_H
#define TYPES_H

#ifdef MAINFILE
    #define EXTERN
#else
    #define EXTERN extern
#endif

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

#define INF 2000000000

typedef std::vector<int> TopoPath;
typedef std::vector<TopoPath> NodeTopoPaths;
typedef std::pair<int, int> conPair;        //dst, prior
typedef std::multimap<int, conPair> ConPriorMMap;        //src, <dst, prior>

using namespace std;

EXTERN string logFile;     //глобальный логфайл
EXTERN ofstream logStream;

struct BGPRouter{
    int ASNumber;	//номер АС в исходном файле
    int nodeNumber;	//номер АС во внутреннем представлении
    vector<int> announceRoute;	//текущее правило на роутере для анонса
};

#endif // TYPES_H
