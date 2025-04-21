
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

* `0` means **no cost** (self‑loop or unused; ignored except on diagonal).
* `9999` (`INF`) means **no direct link / unreachable**.
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

Where `inputfile.txt` is the path to the file containing the adjacency matrix.


The program outputs:
1. **DVR** tables in each iteration.
2. **LSR** tables (results from Dijkstra per source).


## Algorithm Details:

### Distance Vector Routing (DVR)

- Each node maintains a distance vector: the cost to reach every other node.
- Initially:
  - Cost to self = 0
  - Cost to direct neighbors = edge weight
  - Cost to non-neighbors = ∞
- Nodes update their vectors by applying **Bellman-Ford**:
  ```
  D(i, j) = min over all k ( cost(i, k) + D(k, j) )
  ```
- Repeats until no update occurs in an iteration (convergence).

### Link State Routing (LSR)

- Each node knows the full network topology.
- It runs **Dijkstra's algorithm** to compute the shortest path to every node.
- For each destination, the next hop is traced back using the `prev[]` array from Dijkstra.

## Team Contribution
- Venkatesh (33.33%): Documentation, README, extensive testing & validation
- Sai Nikhil (33.33%): Implemented core architecture and performed stress testing
- Anas (33.33%): Created test cases, synchronization strategy, and bug fixes

## Sources Referenced
- Stevens, W. R. (2003). UNIX Network Programming
- C++ Concurrency in Action by Anthony Williams
- Beej's Guide to Network Programming

## Declaration
I declare that this implementation is our original work. All sources have been properly referenced, and no code has been plagiarized.
