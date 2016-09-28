#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <vector>

#define INF 65536

using namespace std;

struct BGPRouter{
    int ASNumber;	//номер АС в исходном файле
    int nodeNumber;	//номер АС во внутреннем представлении
    vector<int> announceRoute;	//текущее правило на роутере для анонса
};

#endif // TYPES_H
