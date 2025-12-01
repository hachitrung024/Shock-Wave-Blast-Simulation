#include "simulation.hpp"

using namespace std;

void run_sequential(vector<vector<float>>& grid) {
    cout << "[Sequential] Running...\n";

    int cx = N / 2;
    int cy = N / 2;

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {

            float dx = (i - cx) * CELL_SIZE;
            float dy = (j - cy) * CELL_SIZE;
            float R = sqrtf(dx * dx + dy * dy);

            float t_arrive = R / SOUND_SPEED;

            if (t_arrive <= SIM_TIME) {
                grid[i][j] = compute_overpressure(R);
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    float sec =
        chrono::duration_cast<chrono::milliseconds>(end - start).count() / 1000.0f;

    cout << "[Sequential] Done in " << sec << " seconds.\n";
}
