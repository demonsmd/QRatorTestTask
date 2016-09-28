#include "types.h"
#include "exceptions.h"
#include "parcer.h"
#include "routingtablessolver.h"

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

int main(int argc, char **argv){
    try{
        printNotes();
        ensureExp(argc==2, 0, "Usage ./QratorTestTask <filename>");
        Parcer parcer = Parcer(argv[1]);
        cout<<"File successfully parced!"<<endl;
        cout<<"Starting solving..."<<endl;
        routingTablesSolver solver = routingTablesSolver(parcer.getConRpiorMatrix(), parcer.getRevASNum());
        return 0;
    }
    catch (Exceptions ex){
        cout<<ex.message<<endl;
        exit(1);
    }
    catch (...){
        cout<<"WTF?"<<endl;
        exit(1);
    }
}
