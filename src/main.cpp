#include <iostream>
#include <fstream>
#include <iomanip>
#include "simulation.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cerr << "Usage: " << argv[0] << " <mode>\n";
        cerr << "Mode: 0 for sequential, 1 for parallel\n";
        return 1;
    }

    vector<vector<float>> grid(N, vector<float>(N, 0.0f));

    int mode = stoi(argv[1]);

    if (mode == 0) {
        cout << "Running sequential mode...\n";
        run_sequential(grid);
    }
    else if (mode == 1) {
        cout << "Running parallel mode...\n";
        run_work_pool(grid);
    }
    else {
        cerr << "Invalid mode.\n";
        return 1;
    }

    // ===============================
    // GHI RA CSV — 2 CHỮ SỐ THẬP PHÂN
    // ===============================
    ofstream outfile("output/overpressure_matrix.csv");
    if (!outfile) {
        cerr << "Cannot open output file!\n";
        return 1;
    }

    cout << "Writing CSV... (may take some seconds)\n";

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            outfile << fixed << setprecision(2) << grid[i][j];
            if (j < N - 1) outfile << ",";
        }
        outfile << "\n";
    }

    outfile.close();
    cout << "Saved to output/overpressure_matrix.csv\n";

    return 0;
}
