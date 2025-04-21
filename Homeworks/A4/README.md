
## Project Description

This project simulates two foundational routing algorithms—**Distance Vector Routing (DVR)** and **Link State Routing (LSR)**— using an adjacency‑matrix representation of a network and generate the corresponding routing tables for each node in the network. This simulation will demonstrate how routers compute the shortest or most optimal paths based on shared information.  


## Team Members
- Venkatesh Akula (220109)

- Mohammed Anas (220654)

- Sai Nikhil (221095)


## Files:

* `routing_sim.cpp`: Main source file that simulates both DVR and LSR algorithms.
* `Makefile`: A simple build script to compile and run the simulator.
* `README.md`: This documentation file explaining the design, execution, and expected behavior.

---

## 1 How to Compile:

1. Open a terminal and navigate to the directory containing the source code and Makefile.
2. Run:
   ```bash
   make
   ```
   This will compile the project and create an executable named `routing_sim`.

---


## 2  Running the Simulator

### 2.1 Input‑File Format

```
n                           # number of nodes
row0_0 row0_1 ... row0_n-1  # adjacency‑matrix row 0
row1_0 row1_1 ...           # adjacency‑matrix row 1
...
rown-1_0 ... rown-1_n-1
```

* `0` means **no cost** (no direct link ; ignored except on diagonal).
* `9999` (`INF`) means **unreachable**.
* Any other positive integer is the **link cost** (bandwidth, latency, etc.).

### 2.2 Example Input (`sample.txt`)

```
4
0 10 100 30
10 0 20 40
100 20 0 10
30 40 10 0
```

### 2.3 Run

After compiling the code, execute the program as follows:

```bash
./routing_sim input.txt
```

Where `input.txt` is the path to the file containing the adjacency matrix.


The program outputs:
1. **DVR** tables in each iteration.
2. **LSR** tables (results from Dijkstra per source).



## 3 Algorithms

### Distance Vector Routing (DVR)
- **Protocol**: Bellman-Ford algorithm
- **Initialization**:
  - Cost to self = 0
  - Cost to direct neighbors = edge weight
  - Cost to non-neighbors = ∞ (INF)
- **Update Rule**:  
  `D(i, j) = min∀k (cost(i, k) + D(k, j))`
- **Convergence**: Iterates until no updates occur in a complete pass

### Link State Routing (LSR)
- **Protocol**: Dijkstra's algorithm
- **Requirements**:
  - Full network topology known to each node
- **Process**:
  1. Builds shortest-path tree from each source
  2. Computes paths to all destinations
  3. Derives next-hop routers via path tracing

## 4  Code Structure

## Core Functions

| Function | Purpose |
|----------|---------|
| `readGraphFromFile()` | Reads network topology from text file containing adjacency matrix (INF=9999 for disconnected nodes) |
| `simulateDVR()` | Implements Distance Vector Routing using Bellman-Ford algorithm with iterative neighbor updates until stable routes |
| `simulateLSR()` | Computes optimal routes via Dijkstra's algorithm from each node's perspective (requires full topology) |
| `printDVRTable()` | Displays formatted routing tables showing [Destination → Cost → Next Hop] for DVR |
| `printLSRTable()` | Outputs calculated shortest paths in [Destination → Total Cost → First Hop] format for LSR |





## 5 Implementation Details

### DVR 
- Uses 3-level nested loops:
  1. Outer: Source nodes
  2. Middle: Neighbor nodes
  3. Inner: Destination nodes
- Maintains two matrices:
  - `dist`: Current distance estimates
  - `nextHop`: Best next hop routers

### LSR 
- For each node as source:
  1. Initializes distance and predecessor arrays
  2. Runs Dijkstra's algorithm:
     - Selects closest unvisited node
     - Relaxes all edges
  3. Builds routing table by tracing paths


## 6  Testing

We have tested the simulator code on:

* **Symmetric & asymmetric weights** to make sure that DVR and LSR both give the same metric results.
* **Edge cases**: Checking different graph networks - fully‑connected graph, line topology, star topology.
* **Unreachable links** (`9999`) to confirm correct "INF" behaviour.

All outputs matched hand‑computed routes and Dijkstra checks.

## 7 Team Contribution
- Venkatesh (33.33%): Implementation of LSR (Dijkstra) & I/O parsing
- Sai Nikhil (33.33%): Core implementation of DVR & helper functions
- Anas (33.33%):  Testing & validation test cases, and bug fixes

## 8 Sources Referenced
- Stevens, W. R. (2003). UNIX Network Programming
- C++ Concurrency in Action by Anthony Williams
- Beej's Guide to Network Programming

## 9 Declaration
I declare that this implementation is our original work. All sources have been properly referenced, and no code has been plagiarized.
