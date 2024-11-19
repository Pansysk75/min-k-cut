#pragma once

#include <vector>
#include <iostream>
#include <string>


template <typename Graph, typename ArcMap>
std::istream& readMtxGraph(Graph& graph, ArcMap& arc_map, std::istream& is = std::cin) {
graph.clear();

std::string line;

// First line start with "%%MatrixMarket matrix coordinate*"
if (!getline(is, line) || line.substr(0, 33) != "%%MatrixMarket matrix coordinate") {
    std::cerr << "Invalid MatrixMarket header" << std::endl;
    return is;
}

// Skip comments
while (getline(is, line)) {
    if (line[0] != '%') {
    break;
    }
}

// Read header
int n, m;
if (!(is >> n >> n >> m)) {
    std::cerr << "Invalid MatrixMarket header" << std::endl;
    return is;
}

// Add nodes
std::vector<typename Graph::Node> nodes;
for (int i = 0; i < n; ++i) {
    nodes.push_back(graph.addNode());
}

// Add edges
for (int i = 0; i < m; ++i) {
	int u, v, w;
    if (!(is >> u >> v)) {
        return is;
    }
    auto edge = graph.addEdge(nodes[u - 1], nodes[v - 1]);
    if (is >> w) {
		arc_map[edge] = w;
    }
    else {
        // Default weight to 1
		arc_map[edge] = 1;
    }
}

return is;
}

