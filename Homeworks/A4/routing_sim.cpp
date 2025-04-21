#include <iostream>

#include <vector>
#include <limits>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

const int INF = 9999;

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

void simulateDVR(const vector<vector<int>>& graph) {
    int n = graph.size();
    vector<vector<int>> dist = graph;
    vector<vector<int>> nextHop(n, vector<int>(n));

    //TODO: Complete this
    for (int u = 0; u < n; ++u) {
        for (int v = 0; v < n; ++v) {
            if (u == v) {
                dist[u][v] = 0;
                nextHop[u][v] = -1; // No next hop for self
            } else if (dist[u][v] != INF) {
                nextHop[u][v] = v;
            } else {
                nextHop[u][v] = -1; // No valid next hop
            }
        }
    }

    // Print initial tables
    cout << "--- Initial DVR Tables ---\n";
    for (int i = 0; i < n; ++i) printDVRTable(i, dist, nextHop);

    // Distance Vector algorithm: exchange until convergence
    bool updated;
    int iteration = 0;
    do {
        updated = false;
        vector<vector<int>> newDist = dist;
        vector<vector<int>> newNext = nextHop;

        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                if (dist[u][v] == INF || u == v) continue;
                for (int dest = 0; dest < n; ++dest) {
                    if (dist[v][dest] == INF) continue;
                    int alt = dist[u][v] + dist[v][dest];
                    if (alt < newDist[u][dest]) {
                        newDist[u][dest] = alt;
                        newNext[u][dest] = nextHop[u][v];
                        updated = true;
                    }
                }
            }
        }

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

void printLSRTable(int src, const vector<int>& dist, const vector<int>& prev) {
    cout << "Node " << src << " Routing Table:\n";
    cout << "Dest\tCost\tNext Hop\n";
    for (int i = 0; i < dist.size(); ++i) {
        if (i == src) continue;
        cout << i << "\t" << dist[i] << "\t";
        int hop = i;
        while (prev[hop] != src && prev[hop] != -1)
            hop = prev[hop];
        cout << (prev[hop] == -1 ? -1 : hop) << endl;
    }
    cout << endl;
}

void simulateLSR(const vector<vector<int>>& graph) {
    int n = graph.size();
    for (int src = 0; src < n; ++src) {
        vector<int> dist(n, INF);
        vector<int> prev(n, -1);
        vector<bool> visited(n, false);
        dist[src] = 0;
        
         //TODO: Complete this
         for (int iter = 0; iter < n; ++iter) {
            int u = -1, best = INF + 1;
            for (int i = 0; i < n; ++i) {
                if (!visited[i] && dist[i] < best) {
                    best = dist[i]; u = i;
                }
            }
            if (u == -1) break;
            visited[u] = true;
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

        
        printLSRTable(src, dist, prev);
    }
}

vector<vector<int>> readGraphFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(1);
    }
    
    int n;
    file >> n;
    vector<vector<int>> graph(n, vector<int>(n));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            file >> graph[i][j];

    file.close();
    return graph;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    string filename = argv[1];
    vector<vector<int>> graph = readGraphFromFile(filename);

    cout << "\n--- Distance Vector Routing Simulation ---\n";
    simulateDVR(graph);

    cout << "\n--- Link State Routing Simulation ---\n";
    simulateLSR(graph);

    return 0;
}
