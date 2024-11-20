#include <iostream>
#include <lemon/lgf_reader.h>
#include <lemon/list_graph.h>
#include "mtx_reader.hpp"
#include "k_min_cut.hpp"
#include "dot_writer.hpp"

using namespace lemon;


int main()
{
    char mtx_graph[] = "%%MatrixMarket matrix coordinate real general\n"
                            "% (.mtx ids start from 1)\n"
                            "% Sample graph in Matrix Market format\n"
                            "5 5 6\n"
                            "1 2 4\n"
                            "2 3 4\n"
                            "3 4 2\n"
                            "1 4 10\n"
                            "1 5 4\n"
                            "5 3 1\n";

    ListGraph g;
    ListGraph::EdgeMap<int> weights(g);

    readMtxGraph(g, weights, std::istringstream(mtx_graph));

    k_min_cut kmc(g, weights);

    kmc.run_gomory_hu();
    kmc.min_k_cut_value(3);

    ListGraph::NodeMap<unsigned int> cut_colors(g);
    kmc.min_k_cut_map(3, cut_colors);
    

    // write original graph to dot file
    std::ofstream dot_file("graph.dot");
    writeDotGraph(g, weights, dot_file);
    writeDotGraph(g, weights);


    // write Gomory-Hu tree to dot file
    std::ofstream dot_file_gh("graph_gh.dot");
    writeDotGraph(kmc._tree, kmc._tree_flows, kmc._tree_labels, dot_file_gh);
    writeDotGraph(kmc._tree, kmc._tree_flows, kmc._tree_labels);
   


    return 0;
}