#include <iostream>
#include <lemon/lgf_reader.h>
#include <lemon/list_graph.h>
#include "mtx_reader.hpp"
#include "k_min_cut.hpp"

using namespace lemon;


int main()
{
    char mtx_graph[] = "%%MatrixMarket matrix coordinate real general\n"
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

    ListGraph g;
    ListGraph::EdgeMap<int> weights(g);

    readMtxGraph(g, weights, std::istringstream(mtx_graph));

    k_min_cut kmc(g, weights);




    return 0;
}