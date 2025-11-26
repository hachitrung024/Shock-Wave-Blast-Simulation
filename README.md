# 📘 Radioactive Contamination Simulation using MPI

This project implements a simulation of radioactive contamination spread on a **4000 × 4000** grid using:

* **Sequential computation**
* **Parallel computation with MPI**

The simulation follows the PDE model given in the *Parallel Computing – Lab 2* exercise:
includes **advection**, **diffusion**, **decay**, and **deposition** terms.

Both versions are implemented inside **main.cpp**, selected using command-line parameter.

---

# 🗂 Project Structure

```
.
├── compile.sh               # Script to compile (g++ or mpic++)
├── data/
│   └── radioactive_matrix.csv   # Initial 4000x4000 matrix input
├── main.cpp                 # Contains sequential + MPI implementations
├── main.exe                 # Generated executable
├── output/
│   └── radioactive_matrix.csv   # Simulation output file
├── README.md                # Project documentation
├── run.sh                   # Script to run the program
└── visualize.py             # Python tool to visualize matrix data
```

---

# ⚙️ Compilation

Use the provided script:

```
./compile.sh
```

This script should internally run:

```
mpicxx main.cpp -o main.exe
```


# 🚀 Running the Simulation

The program accepts one argument:

```
./main.exe <mode>
```

Where:

* `0` → run sequential version
* `1` → run MPI version

Examples:

### Sequential:

```
./main.exe 0
```
---

### MPI (4 processes):

```
mpirun -np 4 ./main.exe 1
```

### MPI (any number of processes that divides H):

```
mpirun -np 6 ./main.exe 1
```

> The MPI version in this project supports **any number of processes**, **as long as H % np == 0** (the number of processes must divide the total number of rows).

---

# 🧠 Simulation Overview

The simulation updates each cell using the discretized PDE:

* **Diffusion** using Laplacian
* **Advection** using upwind approximation
* **Radioactive decay** term
* **Deposition** term

Boundary values outside the map are treated as **zero**.

Simulation runs for:

* **100 timesteps**

For each timestep, the program also counts **uncontaminated cells** (`≈ 0`) and prints the count.

---

# 🧩 MPI Parallelization Details

The MPI version performs:

### ✔ Domain decomposition

Grid is split **by rows** across MPI ranks.

### ✔ Ghost row exchange

Each rank exchanges boundary rows with neighbors using:

```
MPI_Send
MPI_Recv
```

### ✔ Local PDE update

Each rank updates only its assigned rows.

### ✔ Global reduction

Count uncontaminated cells using:

```
MPI_Reduce
```

### ✔ Synchronization

Each timestep ends with:

```
MPI_Barrier
```

### ✔ Stop broadcast

After the final iteration, root broadcasts a stop signal:

```
MPI_Bcast
```

### ✔ Gathering results

Final grid is collected using:

```
MPI_Gather  or  MPI_Gatherv
```

Output is written to:

```
output/radioactive_matrix.csv
```

---

# 📊 Visualization

You can visualize the final matrix using Python:

```
python3 visualize.py
```

The script loads:

```
output/radioactive_matrix.csv
```

and generates a heatmap (matplotlib).

---

# 📁 Input & Output

### Input matrix:

```
data/radioactive_matrix.csv
```

Must be a 4000×4000 CSV of floats.

### Output matrix:

```
output/radioactive_matrix.csv
```

Values are written using:

```
fixed  setprecision(2)
```

---

# ⏱ Performance Measurement

`main.cpp` measures wall-clock time using `std::chrono`.

After each run the program prints:

* Number of uncontaminated cells before/after
* Total time taken (seconds)
* Decrease in uncontaminated cells

---

# ✅ Requirements

* C++17
* OpenMPI (or MPICH)
* Python 3 (optional, for visualization)

---

# 📝 Notes

* The MPI version supports **any number of processes** (depending on decomposition logic implemented).
* A 4000×4000 float grid uses ~64 MB RAM; MPI processes scale memory use accordingly.
* Visualization is optional but helpful for checking correctness.