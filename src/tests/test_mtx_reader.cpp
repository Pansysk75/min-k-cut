#include <iostream>
#include <lemon/lgf_reader.h>
#include <lemon/list_graph.h>
#include "mtx_reader.hpp"

using namespace lemon;

bool are_graphs_equal(ListGraph& g1, ListGraph& g2)
{
    auto getCount = [](auto first) {
        int count = 0;
        for (auto it = first; it != INVALID; ++it)
            ++count;
        return count;
    };

    if (getCount(ListGraph::NodeIt(g1)) != getCount(ListGraph::NodeIt(g2)) )
    {
        std::cerr << "Number of nodes do not match" << std::endl;
        return false;
    }

    if (getCount(ListGraph::EdgeIt(g1)) != getCount(ListGraph::EdgeIt(g2)))
    {
        std::cerr << "Number of edges do not match" << std::endl;
        return false;
    }

    for (ListGraph::EdgeIt e1(g1), e2(g2); e1 != INVALID && e2 != INVALID;
         ++e1, ++e2)
    {
        if (g1.u(e1) != g2.u(e2) ||
            g1.v(e1) != g2.v(e2))
        {
            std::cerr << "Graph edges do not match" << std::endl;
            return false;
        }
    }

    for (ListGraph::NodeIt n1(g1), n2(g2); n1 != INVALID && n2 != INVALID;
         ++n1, ++n2)
    {
        if (g1.id(n1) != g2.id(n2))
        {
            std::cerr << "Graph node ids do not match" << std::endl;
            return false;
        }
    }
    return true;
}

bool are_maps_equal(ListGraph& g1, ListGraph& g2,
    ListGraph::EdgeMap<int>& m1, ListGraph::EdgeMap<int>& m2)
{
    for (ListGraph::EdgeIt e1(g1), e2(g2); e1 != INVALID && e2 != INVALID;
         ++e1, ++e2)
    {
        if (m1[e1] != m2[e2])
        {
            std::cerr << "Maps do not match" << std::endl;
            return false;
        }
    }
    return true;
}


bool test()
{
    char test_lgf_graph[] = "@nodes\n"
                            "label\n"
                            "0\n"
                            "1\n"
                            "2\n"
                            "3\n"
                            "4\n"
                            "@arcs\n"
                            "   capacity\n"
                            "0 1    1\n"
                            "1 2    1\n"
                            "2 3    1\n"
                            "0 3    1\n"
                            "0 3    1\n"
                            "0 3    1\n"
                            "4 2    1\n";

    char test_mtx_graph[] = "%%MatrixMarket matrix coordinate real general\n"
                            "% (.mtx ids start from 1)\n"
                            "% Sample graph in Matrix Market format\n"
                            "5 5 7\n"
                            "1 2\n"
                            "2 3\n"
                            "3 4\n"
                            "1 4\n"
                            "1 4\n"
                            "1 4\n"
                            "5 3\n";

    ListGraph g_lgf, g_mtx;
    ListGraph::EdgeMap<int> weights_lgf(g_lgf), weights_mtx(g_mtx);

    // Read lgf
    std::istringstream input_lgf(test_lgf_graph);
    GraphReader<ListGraph>(g_lgf, input_lgf)
        .edgeMap("capacity", weights_lgf)
        .run();

    // Read mtx
    std::istringstream input_mtx(test_mtx_graph);
    readMtxGraph(g_mtx, weights_mtx, input_mtx);

    bool success = are_graphs_equal(g_lgf, g_mtx) &&
        are_maps_equal(g_lgf, g_mtx, weights_lgf, weights_mtx);

    if (!success)
        std::cerr << "'test()' failed" << std::endl;

    return success;
}

bool test_with_weights()
{
    char test_lgf_graph[] = "@nodes\n"
                            "label\n"
                            "0\n"
                            "1\n"
                            "2\n"
                            "3\n"
                            "4\n"
                            "@arcs\n"
                            "   capacity\n"
                            "0 1    1\n"
                            "1 2    1\n"
                            "2 3    1\n"
                            "0 3    5\n"
                            "0 3    10\n"
                            "0 3    7\n"
                            "4 2    1\n"
                            "@attributes\n"
                            "source 0\n"
                            "target 3\n";

    char test_mtx_graph[] = "%%MatrixMarket matrix coordinate real general\n"
                            "% (.mtx ids start from 1)\n"
                            "% Sample graph in Matrix Market format\n"
                            "5 5 7\n"
                            "1 2 1\n"
                            "2 3 1\n"
                            "3 4 1\n"
                            "1 4 5\n"
                            "1 4 10\n"
                            "1 4 7\n"
                            "5 3 1\n";

    ListGraph g_lgf, g_mtx;
    ListGraph::EdgeMap<int> weights_lgf(g_lgf), weights_mtx(g_mtx);

    // Read lgf
    std::istringstream input_lgf(test_lgf_graph);
    GraphReader<ListGraph>(g_lgf, input_lgf)
        .edgeMap("capacity", weights_lgf)
        .run();

    // Read mtx
    std::istringstream input_mtx(test_mtx_graph);
    readMtxGraph(g_mtx, weights_mtx, input_mtx);

    bool success = are_graphs_equal(g_lgf, g_mtx) &&
        are_maps_equal(g_lgf, g_mtx, weights_lgf, weights_mtx);

    if (!success)
        std::cerr << "'test_with_weights()' failed" << std::endl;

    return success;
}

int main() {
    
    if (test() && test_with_weights())
        return 0;
    return 1;
}