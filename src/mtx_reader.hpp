#pragma once

#include <iostream>
#include <string>
#include <vector>

template <typename Graph, typename ArcMap>
std::istream& readMtxGraph(
    Graph& graph, ArcMap& arc_map, std::istream& is = std::cin)
{
    graph.clear();

    std::string line;

    // First line start with "%%MatrixMarket matrix coordinate*"
    if (!getline(is, line) ||
        line.substr(0, 32) != "%%MatrixMarket matrix coordinate")
    {
        std::cerr << "Invalid MatrixMarket header" << std::endl;
        return is;
    }

    // Skip comments
    while (getline(is, line))
    {
        if (line[0] != '%')
        {
            break;
        }
    }

    // Read header
    int n, m;
    if (!(std::istringstream(line) >> n >> n >> m))
    {
        std::cerr << "Invalid MatrixMarket header" << std::endl;
        return is;
    }

    // Add nodes
    for (int i = 0; i < n; ++i)
    {
        graph.addNode();
    }

    // Add edges
    for (int i = 0; i < m; ++i)
    {
        if (!getline(is, line))
        {
            std::cerr << "Invalid MatrixMarket data" << std::endl;
            return is;
        }

        std::istringstream line_is(line);

        int u, v, w;

        if (!(line_is >> u >> v))
        {
            std::cerr << "Invalid MatrixMarket data" << std::endl;
            return is;
        }

        auto e = graph.addEdge(graph.nodeFromId(u - 1), graph.nodeFromId(v - 1));

        // Try parsing weight. If it fails, set it to 1
        if (!(line_is >> w))
        {
            w = 1;
        }

        arc_map[e] = w;
    }

    return is;
}

