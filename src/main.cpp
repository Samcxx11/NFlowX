#include "graph.h"
#include "visualizer.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    int V, E;
    cout << "Enter number of nodes: ";
    cin >> V;

    Graph g(V);
    vector<vector<int>> originalCap(V, vector<int>(V, 0));

    cout << "Enter number of edges: ";
    cin >> E;
    cout << "Enter each edge as: from to capacity\n";
    for (int i = 0; i < E; ++i) {
        int u, v, cap;
        cin >> u >> v >> cap;
        g.addEdge(u, v, cap);
        originalCap[u][v] = cap;
    }

    int source, sink;
    cout << "Enter source: ";
    cin >> source;
    cout << "Enter sink: ";
    cin >> sink;

    // Run Edmonds-Karp and store flow + steps
    int maxFlow = g.edmondsKarp(source, sink);
    auto steps = g.getSteps();
    cout << "Maximum Flow: " << maxFlow << endl;
    
    // Print all augmented paths
    cout << "\nAugmented Paths:\n";
    for (const auto& step : steps) {
        cout << "Path: ";
        for (int node : step.path) {
            cout << node;
            if (node != step.path.back()) cout << " -> ";
        }
        cout << " | Flow Added: " << step.flow << endl;
    }

    Visualizer viz;

    // 1. Generate graph PNG via Graphviz (with path info)
    viz.generateGraphvizPNG(g.getCapacityMatrix(), originalCap, g.getAdjList(), maxFlow);

    // 2. Interactive step-by-step visualization showing paths
    if (!steps.empty()) {
        viz.drawInteractiveSteps(steps, originalCap, g.getAdjList(), maxFlow);
    }

    // 3. Show final flow graph with all augmented paths
    viz.drawGraph(g.getCapacityMatrix(), originalCap, g.getAdjList(), maxFlow);

    return 0;
}