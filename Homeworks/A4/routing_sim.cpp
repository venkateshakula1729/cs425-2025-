#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

const int INF = 9999; // A constant to represent infinity

// Print the Distance Vector Routing table for a node
void printDVRTable(int node, const vector<vector<int>>& table, const vector<vector<int>>& nextHop) {
    cout << "Node " << node << " Routing Table:\n";
    cout << "Dest\tCost\tNext Hop\n";
    for (int i = 0; i < table.size(); ++i) {
        cout << i << "\t" << table[node][i] << "\t";
        if (nextHop[node][i] == -1) cout << "-";
        else cout << nextHop[node][i];
        cout << endl;
    }
    cout << endl;
}

// Simulate the Distance Vector Routing (DVR) algorithm
void simulateDVR(const vector<vector<int>>& graph) {
    int n = graph.size();
    vector<vector<int>> dist = graph; // Distance matrix
    vector<vector<int>> nextHop(n, vector<int>(n)); // Next hop matrix

    // Initialize distance and next hop
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (u == v) {
                dist[u][v] = 0;
                nextHop[u][v] = -1; // No next hop for self
            } else if (dist[u][v] != INF) {
                nextHop[u][v] = v; // Directly connected neighbor
            } else {
                nextHop[u][v] = -1; // No valid next hop
            }
        }
    }

    // Print initial tables
    cout << "--- Initial DVR Tables ---\n";
    for (int i = 0; i < n; ++i) printDVRTable(i, dist, nextHop);

    // Distance Vector algorithm loop until no updates (convergence)
    bool updated;
    int iteration = 0;
    do {
        updated = false; // Flag to check if any distance is updated in this iteration

        // Create temporary copies of current distance and nextHop tables
        vector<vector<int>> newDist = dist;
        vector<vector<int>> newNext = nextHop;

        // Iterate over each node u
        for (int u = 0; u < n; ++u) {
            // Check neighbors v of node u
            for (int v = 0; v < n; ++v) {
                if (dist[u][v] == INF || u == v) continue; // Skip if v is not a neighbor or same node

                // Try to reach every destination 'dest' via neighbor v
                for (int dest = 0; dest < n; ++dest) {
                    if (dist[v][dest] == INF) continue; // Skip if v cannot reach dest

                    // Calculate alternative distance to 'dest' via 'v'
                    int alt = dist[u][v] + dist[v][dest];

                    // If the alternative path is shorter, update new distance and next hop
                    if (alt < newDist[u][dest]) {
                        newDist[u][dest] = alt;
                        newNext[u][dest] = nextHop[u][v]; // Set next hop from u to dest via v
                        updated = true; // Mark that an update occurred
                    }
                }
            }
        }

        // If there were updates, apply them and print updated tables
        if (updated) {
            iteration++;
            dist.swap(newDist);
            nextHop.swap(newNext);
            cout << "--- DVR Tables after iteration " << iteration << " ---\n";
            for (int i = 0; i < n; ++i) printDVRTable(i, dist, nextHop);
        }
    } while (updated);

    cout << "--- DVR Final Tables ---\n";
    for (int i = 0; i < n; ++i) printDVRTable(i, dist, nextHop);
}

// Print the Link State Routing (LSR) table for a node
void printLSRTable(int src, const vector<int>& dist, const vector<int>& prev) {
    cout << "Node " << src << " Routing Table:\n";
    cout << "Dest\tCost\tNext Hop\n";
    for (int i = 0; i < dist.size(); ++i) {
        if (i == src) continue;
        cout << i << "\t" << dist[i] << "\t";

        // Reconstruct the path to determine the next hop
        int hop = i;
        while (prev[hop] != src && prev[hop] != -1)
            hop = prev[hop];
        cout << (prev[hop] == -1 ? -1 : hop) << endl;
    }
    cout << endl;
}

// Simulate the Link State Routing (LSR) algorithm using Dijkstra’s algorithm
void simulateLSR(const vector<vector<int>>& graph) {
    int n = graph.size();
    for (int src = 0; src < n; ++src) {
        vector<int> dist(n, INF); // Distance from source to each node
        vector<int> prev(n, -1); // Predecessor node in the path
        vector<bool> visited(n, false); // Visited nodes
        dist[src] = 0;
        
         // Dijkstra’s algorithm
         for (int iter = 0; iter < n; ++iter) {
            int u = -1, best = INF + 1;
            // Find unvisited node with smallest distance
            for (int i = 0; i < n; ++i) {
                if (!visited[i] && dist[i] < best) {
                    best = dist[i]; u = i;
                }
            }
            if (u == -1) break;
            visited[u] = true;

            // Update distances to neighbors
            for (int v = 0; v < n; ++v) {
                if (graph[u][v] != INF && !visited[v]) {
                    int alt = dist[u] + graph[u][v];
                    if (alt < dist[v]) {
                        dist[v] = alt;
                        prev[v] = u;
                    }
                }
            }
        }

        
        printLSRTable(src, dist, prev); // Print LSR table for the source node
    }
}

// Read graph from input file
vector<vector<int>> readGraphFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }
    
    int n;
    file >> n;  // Number of nodes
    vector<vector<int>> graph(n, vector<int>(n));

    // Read adjacency matrix
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            file >> graph[i][j];

    file.close();
    return graph;
}

int main(int argc, char *argv[]) {
    // Check command-line argument for input file
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    string filename = argv[1];
    vector<vector<int>> graph = readGraphFromFile(filename); // Load graph

    cout << "\n--- Distance Vector Routing Simulation ---\n";
    simulateDVR(graph); // Run DVR simulation

    cout << "\n--- Link State Routing Simulation ---\n";
    simulateLSR(graph); // Run LSR simulation

    return 0;
}
