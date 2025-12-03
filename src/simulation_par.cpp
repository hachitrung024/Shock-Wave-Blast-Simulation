#include "simulation.hpp"
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <cmath>

static std::vector<std::vector<double>>* g_grid = nullptr;
static bool g_stop = false;

struct Task {
    int row;
    int t;
    Task(int r, int tt) : row(r), t(tt) {}
};

class TaskQueue {
public:
    void push(Task* task) {
        std::lock_guard<std::mutex> lock(mtx);
        q.push(task);
        cv.notify_one();
    }

    Task* pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&](){ return !q.empty() || g_stop; });
        if (q.empty()) return nullptr;
        Task* t = q.front();
        q.pop();
        return t;
    }

private:
    std::queue<Task*> q;
    std::mutex mtx;
    std::condition_variable cv;
};

static TaskQueue task_queue;

void worker_thread(int id) {
    const int N = GRID_SIZE;
    const int cx = N / 2;
    const int cy = N / 2;

    while (true) {
        Task* task = task_queue.pop();
        if (task == nullptr) {
            if (g_stop) break;
            else continue;
        }

        int row = task->row;
        int t   = task->t;
        delete task;

        double Rmax = SOUND_SPEED * t;
        double Rmax_cell = Rmax / CELL_SIZE;

        double dx_cell = row - cx;
        double dx = dx_cell * CELL_SIZE;

        if (std::abs(dx_cell) > Rmax_cell)
            continue;

        double dy_max_cell = std::sqrt(Rmax_cell * Rmax_cell - dx_cell * dx_cell);
        int jmin = std::max(0, (int)(cy - dy_max_cell));
        int jmax = std::min(N - 1, (int)(cy + dy_max_cell));

        for (int j = jmin; j <= jmax; j++) {
            if ((*g_grid)[row][j] != 0.0)
                continue;

            double dy = (j - cy) * CELL_SIZE;
            double R = std::sqrt(dx * dx + dy * dy);
            double arrival = R / SOUND_SPEED;

            if (arrival <= t)
                (*g_grid)[row][j] = compute_overpressure(R);
        }
    }
}

void run_parallel(std::vector<std::vector<double>>& grid, int num_threads)
{
    g_grid = &grid;
    g_stop = false;
    const int NUM_THREADS = (num_threads > 0) ? num_threads : std::thread::hardware_concurrency();
    const int N = GRID_SIZE;
    const int SIM_TIME = 100;

    std::cout << "[PAR] Starting parallel simulation with "
              << NUM_THREADS << " threads...\n";

    std::vector<std::thread> workers;
    workers.reserve(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++)
        workers.emplace_back(worker_thread, i);

    for (int t = 0; t <= SIM_TIME; t++) {
        double Rmax = SOUND_SPEED * t;
        double Rmax_cell = Rmax / CELL_SIZE;

        int imin = std::max(0, (int)(GRID_SIZE/2 - Rmax_cell));
        int imax = std::min(N - 1, (int)(GRID_SIZE/2 + Rmax_cell));

        for (int row = imin; row <= imax; row++)
            task_queue.push(new Task(row, t));
    }

    g_stop = true;

    {
        Task* dummy = nullptr;
        for (int i = 0; i < NUM_THREADS; i++)
            task_queue.push(dummy);
    }

    for (auto& t : workers)
        t.join();

    std::cout << "[PAR] Simulation complete.\n";
}
