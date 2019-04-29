#include <iostream>
#include <fstream>
#include "Circuit.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 6) {
        cout << "Usage ./[Exe] "; 
        cout << "[Input.blif] ";
        cout << "[Result.txt] "; 
        cout << "[AND_CONSTRAINT] "; 
        cout << "[OR_CONSTRAINT] ";
        cout << "[NOT_CONSTRAINT]" << endl;
        exit(1);
    }

    Circuit circuit(stoi(argv[3]), stoi(argv[4]), stoi(argv[5]));
    circuit.parse(argv[1]);

    // circuit.print();

    fstream fout_csv;
    fout_csv.open("result.csv", ios::out | ios::app);
    fout_csv << circuit.model() << ",";

    cout << "check model " << circuit.model() << " ..." << endl;
    if(!circuit.checkCorrectness(argv[2])) {
        cout << "model " << circuit.model() << " ";
        cout << "check failure" << endl;
        fout_csv << "-" << endl;
    }
    else {
        cout << "scheduling result: " << circuit.completeCycle() << endl; 
        cout << "model " << circuit.model() << " ";
        cout << "check successfully" << endl;
        fout_csv << circuit.completeCycle() << endl;
    }

    fout_csv.close();
}
