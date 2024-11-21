#include <iostream>
#include <lemon/bfs.h>
#include <lemon/lgf_reader.h>
#include <lemon/list_graph.h>
#include <set>
#include "dimacs_reader.hpp"
#include "dot_writer.hpp"
#include "k_min_cut.hpp"
#include "util.hpp"

using namespace lemon;

void preprocess_graph(ListGraph& g, ListGraph::EdgeMap<int>& weights)
{
    // We need to do some preprocessing:
    // 1. Remove self-loops and parallel edges
    // 2. Make the graph connected
    // This is done by doing successively doing BFS, and connecting random
    // unvisited nodes until all can be reached from the root node
    int n_edges_erased = 0;
    int n_edges_added = 0;

    // Remove loops and parallel edges
    for (ListGraph::NodeIt n(g); n != INVALID; ++n)
    {
        // Avoid removing edges while iterating over them
        std::vector<ListGraph::Edge> edges_to_remove;
        std::set<ListGraph::Node> visited;
        for (ListGraph::IncEdgeIt e(g, n); e != INVALID; ++e)
        {
            ListGraph::Node u = g.u(e);
            ListGraph::Node v = g.v(e);
            // Not sure which is which
            if (v == n)
                std::swap(u, v);
            if (u == v)
            {
                edges_to_remove.push_back(e);
            }
            else if (visited.find(v) != visited.end())
            {
                // Remove edge
                edges_to_remove.push_back(e);
            }
            else
            {
                visited.insert(v);
            }
        }
        for (auto& e : edges_to_remove)
        {
            g.erase(e);
            ++n_edges_erased;
        }
    }

    // Make the graph connected
    ListGraph::Node root = g.nodeFromId(0);
    lemon::Bfs<ListGraph> bfs(g);

    bfs.run(root);

    for (ListGraph::NodeIt n(g); n != INVALID; ++n)
    {
        if (bfs.reached(n) == false)
        {
            // Connect n to root
            // We expect a small number of non connected nodes, so hopefully this should't skew the original graph too much
            ListGraph::Edge e = g.addEdge(root, n);
            weights[e] = 42;
            ++n_edges_added;
            // Run BFS starting from n
            // This should visit any nodes connected to n that were also not connected to the root
            bfs.addSource(n);
            bfs.start();
        }
    }

    std::cout << "Preprocessing: " << n_edges_erased << " edges erased, "
              << n_edges_added << " edges added" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        std::cout << "Benchmark of min-k-cut algorithm using Gomory-Hu Tree"
                  << std::endl;
        std::cout << "Usage: " << argv[0] << " <dimacs_matrix_file>"
                  << std::endl;
        return 1;
    }

    std::string graph_file = argv[1];
    std::ifstream graph_fs(graph_file);

    ListGraph g;
    ListGraph::EdgeMap<int> weights(g);
    readDimacsGraph(g, weights, graph_fs);

    // Remove self-loops, double edges, and connect non-connected components
    preprocess_graph(g, weights);

    // Here begins the actual algorithm
    k_min_cut kmc(g, weights);

    kmc.run_gomory_hu();
    kmc.min_k_cut_value(3);

    ListGraph::NodeMap<unsigned int> cut_colors(g);
    kmc.min_k_cut_map(3, cut_colors);

    // write original graph to dot file
    std::ofstream dot_file("graph.dot");
    writeDotGraph(g, weights, dot_file);
    //writeDotGraph(g, weights);

    // write Gomory-Hu tree to dot file
    std::ofstream dot_file_gh("graph_gh.dot");
    writeDotGraph(kmc._tree, kmc._tree_flows, kmc._tree_labels, dot_file_gh);
    //writeDotGraph(kmc._tree, kmc._tree_flows, kmc._tree_labels);

    return 0;
}