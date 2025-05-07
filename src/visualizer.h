#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "graph.h" // for StepInfo
using namespace std;

class Visualizer {
public:
    // Basic graph drawing
    void drawGraph(const vector<vector<int>>& capacity,
                   const vector<vector<int>>& originalCap,
                   const vector<vector<int>>& adj,
                   int maxFlow);

    // Step-by-step visualization (multiple windows)
    void drawGraphStepByStep(const vector<StepInfo>& steps,
                            const vector<vector<int>>& originalCap,
                            const vector<vector<int>>& adj,
                            int maxFlow);  // Added maxFlow parameter

    // Interactive step-by-step visualization (single window)
    void drawInteractiveSteps(const vector<StepInfo>& steps,
                            const vector<vector<int>>& originalCap,
                            const vector<vector<int>>& adj,
                            int maxFlow);

    // Graphviz export
    void generateGraphvizPNG(const vector<vector<int>>& capacityMatrix,
                            const vector<vector<int>>& originalCap,
                            const vector<vector<int>>& adjList,
                            int maxFlow);

    // Logging functionality
    void addExplanationLog(const string& log);
    void clearExplanationLogs();
    vector<string> getExplanationLogs() const;

private:
    vector<string> explanationLogs;

    // Helper functions
    void highlightPath(sf::RenderWindow& window,
                     const vector<sf::Vector2f>& positions,
                     const vector<int>& path);
    
    void drawCommonGraphElements(sf::RenderWindow& window,
                               const vector<sf::Vector2f>& positions,
                               const vector<vector<int>>& currentCapacity,
                               const vector<vector<int>>& originalCap,
                               const vector<vector<int>>& adj,
                               const sf::Font& font,
                               bool highlightPaths = false,
                               const vector<int>& path = {});
    
    vector<sf::Vector2f> calculateNodePositions(int V);
    
    // Helper to calculate cumulative flow
    int calculateCumulativeFlow(const vector<StepInfo>& steps, int currentStep);
};

#endif // VISUALIZER_H