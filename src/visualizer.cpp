using namespace std;

#include "visualizer.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <algorithm> // for std::find

vector<sf::Vector2f> Visualizer::calculateNodePositions(int V) {
    float angleStep = 2 * 3.14159f / V;
    float radius = 200.0f;
    vector<sf::Vector2f> positions(V);
    
    for (int i = 0; i < V; ++i) {
        float angle = i * angleStep;
        positions[i] = sf::Vector2f(
            400 + radius * cos(angle),
            300 + radius * sin(angle)
        );
    }
    return positions;
}

void Visualizer::drawCommonGraphElements(sf::RenderWindow& window,
                                      const vector<sf::Vector2f>& positions,
                                      const vector<vector<int>>& currentCapacity,
                                      const vector<vector<int>>& originalCap,
                                      const vector<vector<int>>& adj,
                                      const sf::Font& font,
                                      bool highlightPaths,
                                      const vector<int>& path) {
    // Draw edges
    for (int u = 0; u < adj.size(); ++u) {
        for (int v : adj[u]) {
            if (originalCap[u][v] > 0) {
                // Check if this edge is part of the current path
                bool isPathEdge = false;
                if (highlightPaths && !path.empty()) {
                    for (size_t i = 1; i < path.size(); ++i) {
                        if ((path[i-1] == u && path[i] == v) || (path[i-1] == v && path[i] == u)) {
                            isPathEdge = true;
                            break;
                        }
                    }
                }

                // Draw edge with different style if it's part of the path
                sf::Vertex line[] = {
                    sf::Vertex(positions[u], isPathEdge ? sf::Color::Red : sf::Color::Black),
                    sf::Vertex(positions[v], isPathEdge ? sf::Color::Red : sf::Color::Black)
                };
                window.draw(line, 2, sf::Lines);

                // Draw capacity text
                sf::Text capText;
                capText.setFont(font);
                ostringstream ss;
                ss << originalCap[u][v] - currentCapacity[u][v] << "/" << originalCap[u][v];
                capText.setString(ss.str());
                capText.setCharacterSize(14);
                capText.setFillColor(isPathEdge ? sf::Color::Red : sf::Color::Blue);
                sf::Vector2f mid = (positions[u] + positions[v]) / 2.f;
                capText.setPosition(mid.x, mid.y);
                window.draw(capText);
            }
        }
    }

    // Draw nodes with path highlighting
    for (int i = 0; i < positions.size(); ++i) {
        bool isPathNode = highlightPaths && !path.empty() && 
                         (find(path.begin(), path.end(), i) != path.end());
        
        sf::CircleShape node(isPathNode ? 18 : 15); // Larger if part of path
        node.setFillColor(isPathNode ? sf::Color(255, 150, 150) : sf::Color::Green);
        node.setOrigin(node.getRadius(), node.getRadius());
        node.setPosition(positions[i]);
        window.draw(node);

        sf::Text label;
        label.setFont(font);
        label.setString(to_string(i));
        label.setCharacterSize(14);
        label.setFillColor(sf::Color::White);
        label.setPosition(positions[i].x - (isPathNode ? 7 : 6), 
                         positions[i].y - (isPathNode ? 12 : 10));
        window.draw(label);
    }
}

void Visualizer::generateGraphvizPNG(const vector<vector<int>>& capacityMatrix,
                                   const vector<vector<int>>& originalCap,
                                   const vector<vector<int>>& adjList,
                                   int maxFlow) {
    string dotFilename = "graph.dot";
    string pngFilename = "graph.png";

    ofstream dotFile(dotFilename);
    dotFile << "digraph G {\n";

    for (int i = 0; i < capacityMatrix.size(); ++i) {
        dotFile << "  " << i << " [label=\"Node " << i << "\"];\n";
    }

    for (int u = 0; u < adjList.size(); ++u) {
        for (int v : adjList[u]) {
            int cap = originalCap[u][v];
            int flow = cap - capacityMatrix[u][v];
            dotFile << "  " << u << " -> " << v << " [label=\"" << flow << "/" << cap << "\"];\n";
        }
    }

    dotFile << "}\n";
    dotFile.close();

    string command = "dot -Tpng " + dotFilename + " -o " + pngFilename;
    system(command.c_str());
    cout << "Graph generated as " << pngFilename << endl;
}

void Visualizer::drawGraph(const vector<vector<int>>& capacity,
                         const vector<vector<int>>& originalCap,
                         const vector<vector<int>>& adj,
                         int maxFlow) {
    int V = capacity.size();
    vector<sf::Vector2f> positions = calculateNodePositions(V);
    sf::RenderWindow window(sf::VideoMode(800, 600), "NFlowX - Max Flow Visualizer");

    sf::Font font;
    if (!font.loadFromFile("src/ARIAL.TTF")) {
        return;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);
        drawCommonGraphElements(window, positions, capacity, originalCap, adj, font);

        // Show max flow text
        sf::Text flowText;
        flowText.setFont(font);
        flowText.setString("Max Flow: " + to_string(maxFlow) + 
                         "\nClose window to exit");
        flowText.setCharacterSize(18);
        flowText.setFillColor(sf::Color::Red);
        flowText.setPosition(10, 10);
        window.draw(flowText);

        window.display();
    }
}

void Visualizer::drawGraphStepByStep(const vector<StepInfo>& steps,
                                   const vector<vector<int>>& originalCap,
                                   const vector<vector<int>>& adj,
                                   int maxFlow) {
    int V = originalCap.size();
    vector<sf::Vector2f> positions = calculateNodePositions(V);
    sf::Font font;
    if (!font.loadFromFile("src/ARIAL.TTF")) return;

    for (int stepNum = 0; stepNum < steps.size(); ++stepNum) {
        const auto& step = steps[stepNum];
        sf::RenderWindow window(sf::VideoMode(800, 600), "NFlowX - Step " + to_string(stepNum + 1));

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed || 
                    (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space))
                    window.close();
            }

            window.clear(sf::Color::White);
            drawCommonGraphElements(window, positions, step.currentCapacity, originalCap, adj, font, true, step.path);

            // Calculate cumulative flow
            int cumulativeFlow = 0;
            for (int i = 0; i <= stepNum; i++) {
                cumulativeFlow += steps[i].flow;
            }

            // Build path string
            string pathStr = "Augmenting Path: ";
            for (size_t i = 0; i < step.path.size(); ++i) {
                pathStr += to_string(step.path[i]);
                if (i != step.path.size() - 1) {
                    pathStr += " → ";
                }
            }

            // Show flow info
            sf::Text stepText;
            stepText.setFont(font);
            stepText.setString("Step " + to_string(stepNum + 1) + "/" + to_string(steps.size()) +
                             "\n" + pathStr +
                             "\nFlow Added: " + to_string(step.flow) +
                             "\nTotal Flow: " + to_string(cumulativeFlow) +
                             "\n\nPress SPACE to continue");
            stepText.setCharacterSize(16);
            stepText.setFillColor(sf::Color::Red);
            stepText.setPosition(10, 10);
            window.draw(stepText);

            window.display();
        }
    }
}

void Visualizer::drawInteractiveSteps(const vector<StepInfo>& steps,
                                    const vector<vector<int>>& originalCap,
                                    const vector<vector<int>>& adj,
                                    int maxFlow) {
    int V = originalCap.size();
    vector<sf::Vector2f> positions = calculateNodePositions(V);
    sf::Font font;
    if (!font.loadFromFile("src/ARIAL.TTF")) return;

    int currentStep = 0;
    bool showFinal = false;
    sf::RenderWindow window(sf::VideoMode(800, 600), "NFlowX - Interactive Steps");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Right && currentStep < steps.size() - 1)
                    currentStep++;
                else if (event.key.code == sf::Keyboard::Left && currentStep > 0)
                    currentStep--;
                else if (event.key.code == sf::Keyboard::Space)
                    showFinal = !showFinal;
                else if (event.key.code == sf::Keyboard::Escape)
                    window.close();
            }
        }

        window.clear(sf::Color::White);

        const auto& step = showFinal ? steps.back() : steps[currentStep];
        drawCommonGraphElements(window, positions, step.currentCapacity, originalCap, adj, font, !showFinal, showFinal ? vector<int>() : step.path);

        // Calculate cumulative flow
        int cumulativeFlow = 0;
        for (int i = 0; i <= currentStep; i++) {
            cumulativeFlow += steps[i].flow;
        }

        // Show info
        sf::Text stepText;
        stepText.setFont(font);
        if (showFinal) {
            stepText.setString("Final Flow: " + to_string(maxFlow) + 
                             "\nPress SPACE to return to steps\n" +
                             "ESC to exit");
        } else {
            // Build path string
            string pathStr = "Current Path: ";
            for (size_t i = 0; i < step.path.size(); ++i) {
                pathStr += to_string(step.path[i]);
                if (i != step.path.size() - 1) {
                    pathStr += " → ";
                }
            }

            stepText.setString("Step " + to_string(currentStep + 1) + "/" + to_string(steps.size()) +
                             "\n" + pathStr +
                             "\nFlow Added: " + to_string(step.flow) +
                             "\nTotal Flow: " + to_string(cumulativeFlow) +
                             "\n\nLEFT/RIGHT: Navigate\n" +
                             "SPACE: Show final\n" +
                             "ESC: Exit");
        }
        stepText.setCharacterSize(16);
        stepText.setFillColor(sf::Color::Red);
        stepText.setPosition(10, 10);
        window.draw(stepText);

        window.display();
    }
}

void Visualizer::highlightPath(sf::RenderWindow& window,
                             const vector<sf::Vector2f>& positions,
                             const vector<int>& path) {
    for (size_t i = 1; i < path.size(); ++i) {
        int u = path[i - 1];
        int v = path[i];

        sf::Vertex line[] = {
            sf::Vertex(positions[u], sf::Color::Red),
            sf::Vertex(positions[v], sf::Color::Red)
        };
        window.draw(line, 3, sf::Lines);

        // Add arrowheads to show direction
        sf::Vector2f direction = positions[v] - positions[u];
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        direction = direction / length; // Normalize
        
        sf::ConvexShape arrowhead(3);
        arrowhead.setPoint(0, positions[v]);
        arrowhead.setPoint(1, positions[v] - direction * 15.f + sf::Vector2f(-direction.y, direction.x) * 5.f);
        arrowhead.setPoint(2, positions[v] - direction * 15.f + sf::Vector2f(direction.y, -direction.x) * 5.f);
        arrowhead.setFillColor(sf::Color::Red);
        window.draw(arrowhead);
    }
}

void Visualizer::addExplanationLog(const string& log) {
    explanationLogs.push_back(log);
}

void Visualizer::clearExplanationLogs() {
    explanationLogs.clear();
}

vector<string> Visualizer::getExplanationLogs() const {
    return explanationLogs;
}