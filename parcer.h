#ifndef PARCER_H
#define PARCER_H

#include <types.h>
#include <vector>
#include <map>


class Parcer
{
public:
    Parcer(string inFile);
    ~Parcer(){}

    const ConPriorMMap* getConRpiorMap() {return &conPriorMap;}
    const ConPriorMMap* getRevConRpiorMap() {return &revConPriorMap;}
    const map <int,int>* getRevASNum() {return &RevASNum;}

private:
    ConPriorMMap conPriorMap;    //multimap < src, map<dst, prior> >
    ConPriorMMap revConPriorMap; //multimap < dst, map<src, prior> >
    map <int,int> ASNum;        //asnumber arrayNumber
    map <int,int> RevASNum;     //reverce asnumber arrayNumber
    unsigned topoSize;

};

#endif // PARCER_H
