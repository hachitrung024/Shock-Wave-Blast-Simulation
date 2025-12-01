#ifndef SIMULATION_HPP
#define SIMULATION_HPP
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>
#include <cmath>
#include "utils.hpp"
void run_sequential(std::vector<std::vector<float>>& grid);
void run_work_pool(std::vector<std::vector<float>>& grid);
#endif // SIMULATION_HPP