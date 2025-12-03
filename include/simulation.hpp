#pragma once
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

inline const int GRID_SIZE = 4000;
inline const double CELL_SIZE = 10.0;     // meters per cell
inline const double SOUND_SPEED = 343.0;  // m/s
inline const double YIELD_KT = 5000.0;    // kilotons
inline const double YIELD_KG = YIELD_KT * 1000000.0; // TNT kg
inline const int CENTER = GRID_SIZE / 2;
inline const int SIM_TIME = 100;          // seconds

double compute_overpressure(double R);   // Pso at distance R (meters)


void run_sequential(std::vector<std::vector<double>>& grid);
void run_parallel(std::vector<std::vector<double>>& grid,int num_threads);


void save_to_csv(const std::string& path,
                 const std::vector<std::vector<double>>& grid);
