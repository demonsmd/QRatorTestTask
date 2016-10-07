#ifndef PARCER_H
#define PARCER_H

#include <types.h>
#include <vector>
#include <map>
#include <matrix.h>


class Parcer
{
public:
    Parcer(string inFile);
    ~Parcer(){
        delete conPriorMatrix;
    }

    const SqrMatrix<int>* getConRpiorMatrix() {return conPriorMatrix;}
    const map <int,int>* getRevASNum() {return &RevASNum;}

private:
    SqrMatrix<int>* conPriorMatrix;	//матрица связности с указаниями приоритетов на входе. conPriorMatrix[i][j] = приоритету АС j для АС i
    map <int,int> ASNum;	//asnumber arrayNumber
    map <int,int> RevASNum;	//reverce asnumber arrayNumber
    unsigned topoSize;

};

#endif // PARCER_H
