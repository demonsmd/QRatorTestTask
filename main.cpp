#define MAINFILE

#include "types.h"
#include "exceptions.h"
#include "parcer.h"
#include "routingtablessolver.h"
#include <fstream>
#include <ctime>

#include <string.h>

void printNotes(){
    cout<<"=========================================================================="<<endl;
    cout<<"This program is written by Guskov Dmitry as a test task for QRator Labs"<<endl;
    cout<<"Some solution notes:"<<endl;
    cout<<" - The algorithm computes route tables for ONE initial announcement made from FIRST met BGP router in input file."<<endl;
    cout<<" - When BGP router gets announcement with its number in path he drop this announcement, even if that announcement came from the most foreground router."<<endl;
    cout<<" - I assume that announcements are made simultaneously in special time quants and all announcements spread exactly one hop per time quant."<<endl;
    cout<<" - I assume, that between every two BGP routers cant be more than one physical link, so there can be only two directed edges between them: forward and backward."<<endl;
    cout<<" - There is fixed topo size = 4 in task description. I wrote algorithm, that solve this task for any topo size >=2"<<endl;
    cout<<"=========================================================================="<<endl;
    cout<<endl;
    cout<<endl;
}

void generateTopo(int topoSize, string outfile, int neighProb, int maxPrior){
    ofstream fout(outfile);
    ensureExp(fout.is_open(), true, "cant generate topo");
    for (int node=0;node<topoSize;node++){  //для каждой вершины
        for (int neighbour=0;neighbour<topoSize;neighbour++){  //ищем соседа
            if (rand()%neighProb!=0)
                continue;
            fout<<node<<" "<<neighbour<<" "<<rand()%maxPrior+1<<endl;
        }
    }
    fout.close();
}

int main(int argc, char **argv){
    try{
        printNotes();
//        generateTopo(100000, "100000-30000-10.txt", 30000, 10);
        ensureExp(argc==3, 0, "Usage ./QratorTestTask <filename> <logFile>");

        logFile = argv[2];
        logStream.open(logFile);
        ensureExp(logStream.is_open(), true, "cant open log file");
        clock_t begin = clock();
        Parcer parcer = Parcer(argv[1]);
        cout<<"File successfully parced in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;
        logStream<<"File successfully parced in "<<double(clock() - begin) / CLOCKS_PER_SEC<<" s"<<endl;

//        PossibleTopoRoutesBasedAlgorithm solver(parcer.getConRpiorMatrix(), parcer.getRevASNum());
        DijkstraBasedAlgotithm solver(parcer.getConRpiorMap(), parcer.getRevConRpiorMap(), parcer.getRevASNum());
        solver.Solve();
        logStream.close();
        return 0;
    }
    catch (Exceptions ex){
        cout<<ex.message<<endl;
        logStream<<ex.message<<endl;
        logStream.close();
        exit(1);
    }
    catch (...){
        cout<<"uncought exception"<<endl;
        logStream<<"uncought exception"<<endl;
        logStream.close();
        exit(1);
    }
}
