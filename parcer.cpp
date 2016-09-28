#include "parcer.h"
#include "exceptions.h"
#include <sstream>
#include <fstream>

Parcer::Parcer(string inFile){
    ifstream fin;

    //определяем количество АС
    fin.open(inFile);
    ensureExp(fin.is_open(),0,"cant open file: ", inFile);

    string str;
    int n=0;
    int src, dst, priority;
    while(getline(fin, str)){
        istringstream is(str);
        is>>src;
        is>>dst;
        is>>priority;
        if (ASNum.count(src) == 0){
            ASNum.insert(make_pair(src, n));
            RevASNum.insert(make_pair(n++, src));
        }
        if (ASNum.count(dst) == 0){
            ASNum.insert(make_pair(dst, n));
            RevASNum.insert(make_pair(n++, dst));
        }
    }
    fin.close();
    fin.clear();

    //создаём матрицу связности нужного размера
    topoSize=ASNum.size();
    conPriorMatrix.resize(topoSize);
    for (int i=0;i<topoSize;i++){
        conPriorMatrix[i].resize(topoSize);
        for (int j=0;j<topoSize;j++)
            conPriorMatrix[i][j]=-1;
    }

    //заполняем матрицу
    fin.open(inFile);
    ensureExp(fin.is_open(),0,"cant open file: ", inFile);
    int i,j;
    while(getline(fin, str)){
        istringstream is(str);
        is>>src;
        is>>dst;
        is>>priority;
        i=ASNum.find(src)->second;
        j=ASNum.find(dst)->second;
        conPriorMatrix[j][i]=priority;
    }
    fin.close();
}
