#include "simulation.hpp"
#include <iostream>
#include <cmath>

void run_sequential(std::vector<std::vector<double>>& grid)
{
    const int N = GRID_SIZE;
    const int cx = N / 2;
    const int cy = N / 2;

    const int SIM_TIME = 100;

    std::cout << "[SEQ] Starting sequential simulation...\n";

    for (int t = 0; t <= SIM_TIME; t++)
    {
        double Rmax = SOUND_SPEED * t;
        double Rmax_cell = Rmax / CELL_SIZE;

        int imin = std::max(0, cx - (int)Rmax_cell);
        int imax = std::min(N - 1, cx + (int)Rmax_cell);

        for (int i = imin; i <= imax; i++)
        {
            
            double dx_cell = (i - cx);
            double dy_max_cell = std::sqrt(Rmax_cell * Rmax_cell - dx_cell * dx_cell);
            
            int jmin = std::max(0, (int)(cy - dy_max_cell));
            int jmax = std::min(N - 1, (int)(cy + dy_max_cell));
            
            for (int j = jmin; j <= jmax; j++)
            {
                if (grid[i][j] > 0.0)
                continue;
                
                double dx = (i - cx) * CELL_SIZE;
                double dy = (j - cy) * CELL_SIZE;
                
                double R = std::sqrt(dx * dx + dy * dy);

                double arrival_time = R / SOUND_SPEED;

                if (arrival_time <= t)
                    grid[i][j] = compute_overpressure(R);
            }
        }

        // if (t % 10 == 0)
        //     std::cout << "[SEQ] t = " << t << "/100\n";
    }

    std::cout << "[SEQ] Simulation complete.\n";
}
