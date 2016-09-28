#ifndef PARCER_H
#define PARCER_H

#include <types.h>
#include <vector>
#include <map>


class Parcer
{
public:
	Parcer(string inFile);

	const vector< vector<int> >* getConRpiorMatrix() {return &conPriorMatrix;}
	const map <int,int>* getRevASNum() {return &RevASNum;}

private:
	FILE* inFile;
	vector< vector<int> > conPriorMatrix;	//матрица связности с указаниями приоритетов на входе. conPriorMatrix[i][j] = приоритету АС j для АС i
	map <int,int> ASNum;	//asnumber arrayNumber
	map <int,int> RevASNum;	//reverce asnumber arrayNumber
	int topoSize;

};

#endif // PARCER_H
