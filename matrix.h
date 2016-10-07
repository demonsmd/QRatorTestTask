#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include "exceptions.h"

template <class T>
class SqrMatrix
{
    std::vector<T> elements;
    unsigned size;

protected:
    bool rangeCheck(unsigned i, unsigned j) const;

public:
    SqrMatrix();
    SqrMatrix(unsigned size);
    SqrMatrix(unsigned size, T init);

    T& getElem(unsigned i, unsigned j);
    const T& getElem(unsigned i, unsigned j) const ;

    T& operator()(unsigned i, unsigned j);
    const T& operator()(unsigned i, unsigned j) const;

    unsigned getSize(){return size;}
};



template<class T>
SqrMatrix<T>::SqrMatrix():size(0){}

template<class T>
SqrMatrix<T>::SqrMatrix(unsigned size):size(size){
    elements.resize(size*size);
}

template<class T>
SqrMatrix<T>::SqrMatrix(unsigned size, T init):size(size){
    elements.resize(size*size, init);
}

template<class T>
T& SqrMatrix<T>::getElem(unsigned i, unsigned j){
    ensureExp(rangeCheck(i,j), true, "out of Matrix scope");
    return elements[i*size+j];
}

template<class T>
const T& SqrMatrix<T>::getElem(unsigned i, unsigned j) const {
    ensureExp(rangeCheck(i,j), true, "out of Matrix scope");
    return elements[i*size+j];
}

template<class T>
bool SqrMatrix<T>::rangeCheck(unsigned i, unsigned j) const {
    if (i<size && j<size)
        return true;
    return false;
}

template<class T>
T& SqrMatrix<T>::operator()(unsigned i, unsigned j){
    ensureExp(rangeCheck(i,j), true, "out of Matrix scope");
    return elements[i*size+j];
}

template<class T>
const T& SqrMatrix<T>::operator()(unsigned i, unsigned j) const {
    ensureExp(rangeCheck(i,j), true, "out of Matrix scope");
    return elements[i*size+j];
}

#endif // MATRIX_H
