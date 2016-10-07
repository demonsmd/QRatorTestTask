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

    //создаём матрицу связности нужного размера и заполняем -1
    topoSize=ASNum.size();
    conPriorMatrix = new SqrMatrix<int>(topoSize, -1);

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
        conPriorMatrix->getElem(j,i)=priority;
    }
    fin.close();
}
