#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <iomanip>
#include "simulation.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: ./main <mode>\n";
        std::cerr << "0 = sequential, 1 = parallel\n";
        return 1;
    }

    int mode = std::stoi(argv[1]);

    std::vector<std::vector<double>> grid(
        GRID_SIZE, std::vector<double>(GRID_SIZE, 0.0));

    auto start = std::chrono::high_resolution_clock::now();

    if (mode == 0) {
        run_sequential(grid);
    } else if (mode == 1) {
        int num_threads = 0;
        if (argc >= 3) {
            num_threads = std::stoi(argv[2]);
        }
        run_parallel(grid, num_threads);
    } else {
        std::cerr << "Invalid mode.\n";
        return 1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double sec = std::chrono::duration<double>(end - start).count();

    std::cout << "[INFO] Elapsed Time: " << std::fixed << std::setprecision(6) << sec << " seconds\n";

    save_to_csv("output/overpressure_matrix.csv", grid);

    std::ofstream log("output/time_log.csv", std::ios::app);
    if (log.is_open())
        log << (mode == 0 ? "Sequential" : "Parallel") << "," << sec << "\n";

    return 0;
}
