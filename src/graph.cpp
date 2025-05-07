#include "graph.h"
#include <climits>
#include <algorithm>
#include <queue>

Graph::Graph(int vertices) : V(vertices) {
    capacity.assign(V, vector<int>(V, 0));
    adj.resize(V);
}

void Graph::addEdge(int u, int v, int cap) {
    capacity[u][v] = cap;
    adj[u].push_back(v);
    adj[v].push_back(u); // residual
}

int Graph::bfs(int s, int t, vector<int>& parent) {
    fill(parent.begin(), parent.end(), -1);
    parent[s] = -2;
    queue<pair<int, int>> q;
    q.push({s, INT_MAX});

    while (!q.empty()) {
        int cur = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (int next : adj[cur]) {
            if (parent[next] == -1 && capacity[cur][next] > 0) {
                parent[next] = cur;
                int new_flow = min(flow, capacity[cur][next]);
                if (next == t)
                    return new_flow;
                q.push({next, new_flow});
            }
        }
    }
    return 0;
}

int Graph::edmondsKarp(int s, int t) {
    int flow = 0;
    vector<int> parent(V);

    while (int new_flow = bfs(s, t, parent)) {
        flow += new_flow;
        int cur = t;
        vector<int> path;

        // Trace back the augmenting path
        while (cur != s) {
            int prev = parent[cur];
            capacity[prev][cur] -= new_flow;
            capacity[cur][prev] += new_flow;
            path.push_back(cur);
            cur = prev;
        }
        path.push_back(s);
        reverse(path.begin(), path.end());

        // Save the current step
        steps.push_back({
            capacity,   // snapshot of residual capacities
            path,       // current augmenting path
            new_flow    // flow pushed
        });
    }

    return flow;
}
