#include "simulation.hpp"
#include <fstream>
#include <iomanip>

double compute_overpressure(double R)
{
    double W = YIELD_KG;

    if (R == 0) return 1e5; // avoid log(0)

    double Z = R * std::pow(W, -1.0/3.0);
    double U = -0.21436 + 1.35034 * std::log10(Z);

    double C[9] = {
        2.611369, -1.690128, 0.00805, 0.336743,
        -0.005162, -0.080923, -0.004785,
        0.007930, 0.000768
    };

    double logP = 0.0;
    double Ui = 1.0;

    for (int i = 0; i < 9; i++) {
        logP += C[i] * Ui;
        Ui *= U;
    }

    return std::pow(10.0, logP);
}

void save_to_csv(const std::string& path,
                 const std::vector<std::vector<double>>& grid)
{
    std::ofstream file(path);

    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << "\n";
        return;
    }

    for (auto &row : grid) {
        for (size_t j = 0; j < row.size(); j++) {
            file << std::fixed << std::setprecision(2) << row[j];
            if (j + 1 < row.size()) file << ",";
        }
        file << "\n";
    }

    file.close();
    std::cout << "[INFO] Saved CSV to " << path << "\n";
}
