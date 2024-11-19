#include <iostream>
#include <lemon/lgf_reader.h>
#include <lemon/list_graph.h>
#include "mtx_reader.hpp"

using namespace lemon;

void test_graph_read()
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

    // compare that the graphs match
    ListGraph::EdgeIt e_lgf(g_lgf), e_mtx(g_mtx);
    while (e_lgf != INVALID && e_mtx != INVALID)
    {
        if (g_lgf.u(e_lgf) != g_mtx.u(e_mtx) ||
            g_lgf.v(e_lgf) != g_mtx.v(e_mtx) ||
            weights_lgf[e_lgf] != weights_mtx[e_mtx])
        {
            std::cerr << "Graphs do not match" << std::endl;
            return;
        }
        ++e_lgf;
        ++e_mtx;
    }
}

int main() {
    test_graph_read();
    return 0;
}