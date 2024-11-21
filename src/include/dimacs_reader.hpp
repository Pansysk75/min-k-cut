#pragma once

#include <iostream>
#include <string>
#include <vector>

    template <typename Graph, typename ArcMap>
    std::istream& readDimacsGraph(
        Graph& graph, ArcMap& arc_map, std::istream& is = std::cin)
{
    graph.clear();

    std::string line;

    // Skip comments
    while (getline(is, line))
    {
        if (line[0] != 'c')
        {
            break;
        }
    }

    // Read header
    // Should start with "p ps"
    if (line.substr(0, 4) != "p sp")
    {
        std::cerr << "Invalid or unsupported Dimacs file" << std::endl;
        return is;
    }

    int n, m;
    if (!(std::istringstream(line.substr(4)) >> n >> m))
    {
        std::cerr << "Error reading Dimacs file" << std::endl;
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
        if (!getline(is, line) || line[0] != 'a')
        {
            std::cerr << "Invalid Dimacs data" << std::endl;
            return is;
        }

        std::istringstream line_is(line.substr(1));

        int u, v, w;

        if (!(line_is >> u >> v >> w))
        {
            std::cerr << "Invalid Dimacs data" << std::endl;
            return is;
        }

        auto e =
            graph.addEdge(graph.nodeFromId(u - 1), graph.nodeFromId(v - 1));

        arc_map[e] = w;
    }

    return is;
}
