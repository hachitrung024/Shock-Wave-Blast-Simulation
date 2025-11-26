#include <iostream>
#include "simulation.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cerr << "Usage: " << argv[0] << " <mode>\n";
        cerr << "Mode: 0 for sequential, 1 for MPI\n";
        return 1;
    }

    int mode = stoi(argv[1]);
    if(mode == 0) {
        cout << "Running in sequential mode.\n";
        run_sequential();
    } else if(mode == 1) {
        cout << "Running in MPI mode.\n";
        run_work_pool();
    } else {
        cerr << "Invalid mode. Use 0 for sequential or 1 for MPI.\n";
        return 1;   
    }
}