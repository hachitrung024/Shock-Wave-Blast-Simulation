#include "simulation.hpp"

using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t num_workers) : stop_flag(false) {
        for (size_t i = 0; i < num_workers; ++i) {
            workers.emplace_back([this] { thread_worker(); });
        }
    }

    ~ThreadPool() {
        shutdown();
    }

    // non-copyable
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void push(function<void()> fn) {
        {
            unique_lock<mutex> lk(mtx);
            tasks.push(std::move(fn));
        }
        cv.notify_one();
    }

    // Wait until all tasks submitted so far are finished.
    void wait_for_completion() {
        unique_lock<mutex> lk(done_mtx);
        done_cv.wait(lk, [this] { 
            unique_lock<mutex> lk2(mtx);
            return tasks.empty() && (active_workers.load() == 0);
        });
    }

    void shutdown() {
        {
            unique_lock<mutex> lk(mtx);
            stop_flag = true;
        }
        cv.notify_all();
        for (thread &t : workers) {
            if (t.joinable()) t.join();
        }
    }

private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex mtx;
    condition_variable cv;
    bool stop_flag;

    atomic<int> active_workers {0};

    // for wait_for_completion()
    mutex done_mtx;
    condition_variable done_cv;

    void thread_worker() {
        while (true) {
            function<void()> task;
            {
                unique_lock<mutex> lk(mtx);
                cv.wait(lk, [this] { return stop_flag || !tasks.empty(); });
                if (stop_flag && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
                active_workers.fetch_add(1, std::memory_order_relaxed);
            }

            // execute task outside lock
            try {
                task();
            } catch (...) {
                // swallow exceptions to avoid terminating worker loop
            }

            active_workers.fetch_sub(1, std::memory_order_relaxed);

            // notify potential waiters when there are no pending tasks and no active workers
            {
                unique_lock<mutex> lk(mtx);
                if (tasks.empty() && active_workers.load() == 0) {
                    // notify wait_for_completion
                    unique_lock<mutex> lk2(done_mtx);
                    done_cv.notify_all();
                }
            }
        }
    }
};

// run_work_pool: divides grid into row-block tasks and processes in parallel
void run_work_pool(vector<vector<float>>& grid) {
    cout << "[Parallel] Starting work-pool simulation...\n";

    const int num_threads = thread::hardware_concurrency() ? thread::hardware_concurrency() : 4;
    // choose block size (rows per task). trade-off: smaller block -> more tasks -> better load balancing; larger -> less overhead
    const int ROWS_PER_TASK = 32; // reasonable default for 4000 rows -> 125 tasks

    ThreadPool pool(num_threads);

    int cx = N / 2;
    int cy = N / 2;

    atomic<int> tasks_submitted{0};
    int total_tasks = (N + ROWS_PER_TASK - 1) / ROWS_PER_TASK;

    auto t0 = chrono::high_resolution_clock::now();

    for (int row_start = 0; row_start < N; row_start += ROWS_PER_TASK) {
        int r0 = row_start;
        int r1 = std::min(N, row_start + ROWS_PER_TASK); // [r0, r1)

        // increment submitted count
        tasks_submitted.fetch_add(1, std::memory_order_relaxed);

        // push task: compute overpressure for rows [r0, r1)
        pool.push([r0, r1, cx, cy, &grid]() {
            for (int i = r0; i < r1; ++i) {
                // precompute dx for row i
                float dx = (i - cx) * CELL_SIZE;
                float dx2 = dx * dx;
                // process all columns
                for (int j = 0; j < N; ++j) {
                    float dy = (j - cy) * CELL_SIZE;
                    float R = sqrtf(dx2 + dy * dy);
                    float t_arrive = R / SOUND_SPEED;
                    if (t_arrive <= SIM_TIME) {
                        grid[i][j] = compute_overpressure(R);
                    }
                }
            }
        });
    }

    pool.wait_for_completion();

    auto t1 = chrono::high_resolution_clock::now();
    double seconds = chrono::duration_cast<chrono::milliseconds>(t1 - t0).count() / 1000.0;
    cout << "[Parallel] All tasks completed.\n";
    cout << "[Parallel] Threads used: " << num_threads << "\n";
    cout << "[Parallel] Task blocks: " << total_tasks << " (rows per task = " << ROWS_PER_TASK << ")\n";
    cout << "[Parallel] Time: " << seconds << " s\n";

    pool.shutdown();
}
