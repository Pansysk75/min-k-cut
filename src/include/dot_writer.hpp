
#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace detail {

    std::ostream& writeDotNodes(lemon::ListGraph& graph, std::ostream& os)
    {
        for (lemon::ListGraph::NodeIt n(graph); n != lemon::INVALID; ++n)
        {
            os << "    " << graph.id(n) << ";\n";
        }
        return os;
    }

    template <typename T>
    std::ostream& writeDotNodes(lemon::ListGraph& graph,
        lemon::ListGraph::NodeMap<T>& node_map, std::ostream& os)
    {
        for (lemon::ListGraph::NodeIt n(graph); n != lemon::INVALID; ++n)
        {
            os << "    " << graph.id(n) << " [label=\"" << node_map[n]
               << "\"];\n";
        }
        return os;
    }

    std::ostream& writeDotEdges(lemon::ListGraph& graph, std::ostream& os)
    {
        for (lemon::ListGraph::EdgeIt e(graph); e != lemon::INVALID; ++e)
        {
            os << "    " << graph.id(graph.u(e)) << " -- "
               << graph.id(graph.v(e)) << ";\n";
        }

        return os;
    }

    template <typename T>
    std::ostream& writeDotEdges(lemon::ListGraph& graph,
        lemon::ListGraph::EdgeMap<T>& edge_map, std::ostream& os)
    {
        for (lemon::ListGraph::EdgeIt e(graph); e != lemon::INVALID; ++e)
        {
            os << "    " << graph.id(graph.u(e)) << " -- "
               << graph.id(graph.v(e)) << " [label=\"" << edge_map[e]
               << "\"];\n";
        }
        return os;
    }
};    // namespace detail

template <typename T1, typename T2>
std::ostream& writeDotGraph(lemon::ListGraph& graph,
    lemon::ListGraph::EdgeMap<T1>& edge_map,
    lemon::ListGraph::NodeMap<T2>& node_map, std::ostream& os = std::cout)
{
    os << "graph G {\n";

    detail::writeDotNodes(graph, node_map, os);
    detail::writeDotEdges(graph, edge_map, os);
    
    os << "}\n";

    return os;
}

template <typename T>
std::ostream& writeDotGraph(lemon::ListGraph& graph,
    lemon::ListGraph::EdgeMap<T>& edge_map, std::ostream& os = std::cout)
{
    os << "graph G {\n";
    detail::writeDotNodes(graph, os);
    detail::writeDotEdges(graph, edge_map, os);

    os << "}\n";
    return os;
}

template <typename T>
std::ostream& writeDotGraph(lemon::ListGraph& graph,
    lemon::ListGraph::NodeMap<T>& node_map, std::ostream& os = std::cout)
{
    os << "graph G {\n";
    detail::writeDotNodes(graph, node_map, os);
    detail::writeDotEdges(graph, os);
    os << "}\n";
    return os;
}

std::ostream& writeDotGraph(
    lemon::ListGraph& graph, std::ostream& os = std::cout)
{
    os << "graph G {\n";
    detail::writeDotNodes(graph, os);
    detail::writeDotEdges(graph, os);
    os << "}\n";
    return os;
}