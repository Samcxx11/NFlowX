#pragma once
#include <string>
#include <vector>
using namespace std;

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

struct StepInfo {
    vector<vector<int>> currentCapacity;
    vector<int> path;
    int flow;
};

class Graph {
private:
    int V; // Number of vertices
    vector<vector<int>> capacity;
    vector<vector<int>> adj;
    vector<StepInfo> steps; // Store step-by-step info

public:
    Graph(int vertices);
    void addEdge(int u, int v, int cap);
    int bfs(int s, int t, vector<int>& parent);
    int edmondsKarp(int s, int t);
    void exportToDOT(const string& filename, const vector<vector<int>>& originalCap, int source, int sink);

    const vector<vector<int>>& getCapacityMatrix() const { return capacity; }
    const vector<vector<int>>& getAdjList() const { return adj; }

    const vector<StepInfo>& getSteps() const { return steps; } // New: Step access
};

#endif
