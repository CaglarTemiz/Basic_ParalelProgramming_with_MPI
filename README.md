# 🧠 Parallel Graph Coloring with MPI

This project implements and compares **parallel** and **sequential** graph coloring algorithms using **C++** and **MPI**. The main focus is on the **Block Partition Based Coloring Algorithm**, inspired by the paper by Gebremedhin & Manne (2000), and its performance is evaluated against a baseline **First-Fit** sequential algorithm.

---

## 📂 Project Structure

```
.
├── BlockPartitionBasedColoringAlgorithm.cpp   # Main parallel coloring implementation (MPI)
├── FirstFitAlgorithm.cpp                      # Sequential First-Fit coloring
├── 494_bus.txt                                # Test graph (optional)
├── 1138_bus.txt                               # Test graph (optional)
└── CaglarTemiz_2227940_CENG577_Final.pdf      # Final report (detailed documentation)
```

---

## 🚀 How to Compile and Run

### Prerequisites

- MPI compiler (e.g., `mpic++`)
- C++11 or later
- OpenMPI or compatible MPI runtime

### Build and Run Parallel Version

```bash
mpic++ BlockPartitionBasedColoringAlgorithm.cpp -o parallel_color
mpirun -np 4 ./parallel_color
```

### Build and Run Sequential Version

```bash
g++ FirstFitAlgorithm.cpp -o seq_color -std=c++11
./seq_color
```

---

## 📌 Algorithm Overview

### 🔷 Block Partition Based Coloring (Parallel)
- **Partitioning**: Divides the graph into blocks by vertex index.
- **Parallel Coloring**: Each MPI process colors its block.
- **Conflict Resolution**: Master (Rank 0) detects and resolves conflicts sequentially.

### 🔸 First-Fit (Sequential)
- Iterates over each vertex and assigns the smallest available color.
- Simple but efficient baseline for comparison.

---

## 📈 Performance Notes

- For large graphs (e.g., `494_bus`, `1138_bus`), MPI parallel coloring shows **better scalability** as the graph size increases.
- For small graphs, **sequential First-Fit is faster** due to the overhead of inter-process communication and conflict resolution.

See `CaglarTemiz_2227940_CENG577_Final.pdf` for benchmark graphs and detailed discussions.

---

## 🧪 Sample Graph (Hardcoded Option)

You may test the code using the built-in toy graph:
```cpp
vector<vector<int>> graph = {
    {1, 2, 3},
    {0, 4},
    {0, 4, 5},
    {0, 4},
    {1, 2, 3, 5},
    {2, 4}
};
```

Or use graph files from [SuiteSparse Matrix Collection](https://sparse.tamu.edu/).

---

## 📚 References

- Gebremedhin, A. H., & Manne, F. (2000). *Scalable parallel graph coloring algorithms*. _Concurrency: Practice and Experience_, 12(13), 1131–1146.
- Brelaz, D. (1979). *New methods to color the vertices of a graph*. _Communications of the ACM_, 22(4):251–256.
- Grimmet & McDiarmid (1975). *On coloring random graphs*. _Mathematical Proceedings of the Cambridge Philosophical Society_, 77:313–324.

---

## 🙋 Author

**Çağlar TEMİZ**  
MSc Student in Civil Engineering  
Middle East Technical University  
Project for CENG 577: Parallel Computing
