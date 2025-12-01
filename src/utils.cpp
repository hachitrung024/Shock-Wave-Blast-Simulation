#include "utils.hpp"
#include <cmath>

const int N = 4000;
const float CELL_SIZE = 10.0f;
const float SOUND_SPEED = 343.0f;
const float SIM_TIME = 100.0f;
const float W_kg = 5000.0f * 1000000.0f;   // 5000 kiloton to kg TNT

static const float C[9] = {
    2.611369f,
    -1.690128f,
    0.00805f,
    0.336743f,
    -0.005162f,
    -0.080923f,
    -0.004785f,
    0.007930f,
    0.000768f
};

float compute_overpressure(float R) {
    if (R == 0) return 1e5f;

    float Z = R * powf(W_kg, -1.0f / 3.0f);
    float U = -0.21436f + 1.35034f * log10f(Z);

    float logP = 0.0f;
    float Ui = 1.0f;

    for (int i = 0; i < 9; i++) {
        logP += C[i] * Ui;
        Ui *= U;
    }

    return powf(10.0f, logP);
}
