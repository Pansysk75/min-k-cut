#pragma once

#include <iostream>
#include <lemon/dfs.h>
#include <lemon/lgf_reader.h>
#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#include <stack>
#include "mtx_reader.hpp"
#include "util.hpp"

class k_min_cut
{
    using ListGraph = lemon::ListGraph;
    static constexpr auto INVALID = lemon::INVALID;

    ListGraph const& _graph;
    ListGraph::EdgeMap<int> const& _weights;

    // The Gomory-Hu tree is encoded in the _p (predecessor) and _fl (min flow) maps as follows:
    // "The edges of T are the final pairs (i,p[i]) for from 2 to n, and edge (i,p[i]) has value fl(i)."

    // The predecessor map
    ListGraph::NodeMap<ListGraph::Node> _p;

public:
    // The min flow map
    ListGraph::NodeMap<int> _fl;
    // The Gomory-Hu Tree
    ListGraph _tree;
    // Tree flows
    ListGraph::EdgeMap<int> _tree_flows;
    // Tree labels
    ListGraph::NodeMap<int> _tree_labels;

    k_min_cut(ListGraph const& graph, ListGraph::EdgeMap<int> const& weights)
      : _graph(graph)
      , _weights(weights)
      , _p(graph)
      , _fl(graph)
      , _tree()
      , _tree_flows(_tree)
      , _tree_labels(_tree)
    {
    }

    void run_gomory_hu()
    {
        /*
    for s:=2 to n do
        begin
            Compute a minimum cut between nodes
            s and t:=p[s] in G; let X be the set of nodes on the s side
            of the cut. Output the maximum s, t flow value f(s, t).
        fl[s]:=f(s, t);
        for i:=1 to n do
            if (i<>s and i is in X and p[i]=t) then p[i]:=s;
        if (p[t] is in X) then
        begin
            p[s] :=p[t];
            p[t]:=s;
            fl[s] :=fl[t];
            fl[t]:=f(s, t);
        end;
    end;
*/
        double time_min_cut = 0;
        double time_relabel = 0;
        double time_total = 0;

        timer t_total;

        // Choose a root node
        ListGraph::NodeIt root(_graph);

        // Initialize the predecessor map
        for (ListGraph::NodeIt n(_graph); n != INVALID; ++n)
        {
            _p[n] = root;
        }
        _p[root] = INVALID;
        _fl[root] = std::numeric_limits<int>::max();
        ;

        for (ListGraph::NodeIt s(_graph); s != INVALID; ++s)
        {
            if (s == root)
                continue;

            timer t_min_cut;

            ListGraph::Node t = _p[s];

            lemon::Preflow<ListGraph, ListGraph::EdgeMap<int>> min_cut(
                _graph, _weights, s, t);
            min_cut.run();

            time_min_cut += t_min_cut.tick();

            _fl[s] = min_cut.flowValue();

            timer t_relabel;

            for (ListGraph::NodeIt i(_graph); i != INVALID; ++i)
            {
                if (i != s && min_cut.minCut(i) && _p[i] == t)
                {
                    _p[i] = s;
                }
            }
            if (_p[t] != INVALID && min_cut.minCut(_p[t]))
            {
                _p[s] = _p[t];
                _p[t] = s;
                _fl[s] = _fl[t];
                _fl[t] = min_cut.flowValue();
            }

            time_relabel += t_relabel.tick();
        }

        // Create the Gomory-Hu tree. The tree is an undirected graph with the same nodes as the original graph
        // and edges (i, p[i]) with weight fl[i].
        // Making it into a graph to make it easier to traverse, however in principle _p is enough to represent the tree.
        // An important property here is that each node has a single parent (except the root node which has none).
        _tree.clear();
        // Turns out you can't just copy a graph in this library while keeping the same node/edge ids
        // We create a map to map the nodes in the original graph to the nodes in the tree
        ListGraph::NodeMap<ListGraph::Node> node_map(_graph);
        for (ListGraph::NodeIt n(_graph); n != INVALID; ++n)
        {
            ListGraph::Node m = _tree.addNode();
            node_map[n] = m;
            _tree_labels[m] = _graph.id(n);
        }

        // Now add the edges
        for (ListGraph::NodeIt n(_graph); n != INVALID; ++n)
        {
            if (_p[n] != INVALID)
            {
                ListGraph::Node m = node_map[n];
                ListGraph::Node p = node_map[_p[n]];
                ListGraph::Edge e = _tree.addEdge(m, p);
                _tree_flows[e] = _fl[n];
            }
        }

        time_total = t_total.tick();

        // Write times to json log
        global_json_logger.add("gh_time_min_cut", time_min_cut);
        global_json_logger.add("gh_time_relabel", time_relabel);
        global_json_logger.add("gh_time_total", time_total);
    }

    // DFS Visitor that invokes some callable on reaching a node
    template <typename F>
    class MyVisitor : public lemon::DfsVisitor<ListGraph>
    {
        F _f;

    public:
        MyVisitor(F f)
          : _f(f)
        {
        }

        void reach(const Node& node)
        {
            _f(node);
        }
    };

    static void print_graph(
        ListGraph const& g, ListGraph::EdgeMap<int> const& weights)
    {
        for (ListGraph::NodeIt n(g); n != INVALID; ++n)
        {
            std::cout << "Node " << g.id(n) << std::endl;
            for (ListGraph::IncEdgeIt e(g, n); e != INVALID; ++e)
            {
                ListGraph::Node target = g.oppositeNode(n, e);
                std::cout << "  -> " << g.id(target) << " (" << weights[e]
                          << ")" << std::endl;
            }
        }
    }

    static void print_supergraph(ListGraph const& g,
        ListGraph::NodeMap<std::vector<ListGraph::Node>> const& supernodes,
        ListGraph::EdgeMap<int> const& weights)
    {
        for (ListGraph::NodeIt n(g); n != INVALID; ++n)
        {
            std::cout << "Node " << g.id(n) << ": ";
            for (ListGraph::Node m : supernodes[n])
            {
                std::cout << g.id(m) << " ";
            }
            std::cout << std::endl;
        }
        for (ListGraph::EdgeIt e(g); e != INVALID; ++e)
        {
            std::cout << "Edge " << g.id(e) << ": " << g.id(g.u(e)) << " -> "
                      << g.id(g.v(e)) << " (" << weights[e] << ")" << std::endl;
        }
    }

    void run_gomory_hu_2()
    {
        // This is the implementation of the original Gomory-Hu algorithm
        // It is probably less efficient than Gusfield's algorithm, but it is easier to prove its correctness.

        // The Gomory-Hu Tree begins as a single supernode containing all graph vertices
        // Each iteration of the algorithm is as follows:
        // 1. Select a pair of vertices s-t in a supernode S
        // 2. Find the connected components in T afte removing the supernode, and contract all nodes in each component
        // 3. Compute the minimum cut between s and t in the contracted graph, creating two new supernodes, S1 and S2
        // 4. Add S1 and S2 to the Gomory-Hu Tree, with an edge between them with the value of the minimum cut
        // 5. Connect neighbors of S to S1 and S2, depending on which side of the cut they are in
        // 6. Repeat until all supernodes contain a single vertex

        double time_min_cut = 0;
        double time_contraction = 0;
        double time_total = 0;

        timer t_total;

        // The Gomory-Hu Tree
        ListGraph gh_tree;
        // The min cut values
        ListGraph::EdgeMap<int> gh_tree_flows(gh_tree);
        // The contents of each supernode
        ListGraph::NodeMap<std::vector<ListGraph::Node>> gh_tree_supernodes(
            gh_tree);

        // Supernodes with more than one vertex to process
        std::stack<ListGraph::Node> supernode_stack;

        {
            // Create the initial supernode, containing all nodes
            ListGraph::Node initial_sn = gh_tree.addNode();
            gh_tree_supernodes[initial_sn].clear();
            for (ListGraph::NodeIt n(_graph); n != INVALID; ++n)
            {
                gh_tree_supernodes[initial_sn].push_back(n);
            }

            supernode_stack.push(initial_sn);
        }

        while (!supernode_stack.empty())
        {
            // Grab some supernode from stack
            ListGraph::Node supernode = supernode_stack.top();
            supernode_stack.pop();

            //// Print for debug
            //std::cout << "Gomory-Hu Tree: " << std::endl;
            //print_supergraph(gh_tree, gh_tree_supernodes, gh_tree_flows);

            // Select two random vertices in the supernode
            ListGraph::Node s = gh_tree_supernodes[supernode][0];
            ListGraph::Node t = gh_tree_supernodes[supernode][1];

            timer t_contraction;

            // Contraction step
            // Create a copy of the original graph, which we need to perform node contractions
            ListGraph contracted_graph;
            ListGraph::EdgeMap<int> contracted_weights(contracted_graph);
            ListGraph::NodeMap<ListGraph::Node> original_to_contracted(_graph);
            lemon::GraphCopy<ListGraph, ListGraph> copy(
                _graph, contracted_graph);
            copy.edgeMap(_weights, contracted_weights);
            copy.nodeRef(original_to_contracted);
            copy.run();

            //std::cout << "Contracted Graph (Before): " << std::endl;
            //print_graph(contracted_graph, contracted_weights);

            // A map from supernodes of tree to nodes of the contracted graph.
            // We need this after the split, to connect the neighbors of the old supernode to the new supernodes.
            ListGraph::NodeMap<ListGraph::Node> supernode_to_contracted(
                gh_tree);

            // Now we prepare the DFS algorithm to create the contracted graph
            // We set the current supernode to visited. Then, running a DFS on any unvisited supernode will give us a connected component
            // in the GH tree
            ListGraph::NodeMap<bool> visited(gh_tree, false);
            visited[supernode] = true;

            ListGraph::Node contraction_node = INVALID;

            MyVisitor visitor([&](ListGraph::Node const& visited_supernode) {
                supernode_to_contracted[visited_supernode] =
                    original_to_contracted[contraction_node];
                for (ListGraph::Node n : gh_tree_supernodes[visited_supernode])
                {
                    if (original_to_contracted[n] !=
                        original_to_contracted[contraction_node])
                    {
                        contracted_graph.contract(
                            original_to_contracted[contraction_node],
                            original_to_contracted[n]);
                    }
                }
            });

            // Prepare the DFS algorithm
            lemon::DfsVisit<ListGraph, decltype(visitor)> dfs(gh_tree, visitor);
            dfs.init();
            dfs.reachedMap(visited);

            for (ListGraph::NodeIt sn(gh_tree); sn != INVALID; ++sn)
            {
                if (visited[sn])
                    continue;

                // Aha, this must be some unexplored connected component
                // Arbitrarily contract everything to the first node in this component
                contraction_node = gh_tree_supernodes[sn][0];

                dfs.addSource(sn);
                dfs.start();
            }

            time_contraction += t_contraction.tick();

            //std::cout << "Contracted Graph (After): " << std::endl;
            //print_graph(contracted_graph, contracted_weights);

            timer t_min_cut;

            // Finally, the contracted graph has been created. Run a min-cut algorithm on it
            lemon::Preflow<ListGraph, ListGraph::EdgeMap<int>> min_cut(
                contracted_graph, contracted_weights, original_to_contracted[s],
                original_to_contracted[t]);
            min_cut.run();

            time_min_cut += t_min_cut.tick();

            // Add the two new supernodes
            ListGraph::Node supernode1 = gh_tree.addNode();
            ListGraph::Node supernode2 = gh_tree.addNode();

            // Distribute the nodes of the old supernode to the new supernodes according to the min-cut
            gh_tree_supernodes[supernode1].clear();
            gh_tree_supernodes[supernode2].clear();
            for (ListGraph::Node n : gh_tree_supernodes[supernode])
            {
                if (min_cut.minCut(original_to_contracted[n]))
                {
                    gh_tree_supernodes[supernode1].push_back(n);
                }
                else
                {
                    gh_tree_supernodes[supernode2].push_back(n);
                }
            }

            // Add an edge between the two supernodes with the value of the min-cut
            ListGraph::Edge e = gh_tree.addEdge(supernode1, supernode2);
            gh_tree_flows[e] = min_cut.flowValue();

            // Connect neighbors of the supernode to the new supernodes
            for (ListGraph::IncEdgeIt e(gh_tree, supernode); e != INVALID; ++e)
            {
                ListGraph::Node sn = gh_tree.oppositeNode(supernode, e);

                if (sn == supernode1 || sn == supernode2)
                    continue;

                // See which side of the cut the corresponding node is in
                auto contr = supernode_to_contracted[sn];

                // Not a neighbor
                if (contr == INVALID)
                    continue;

                if (min_cut.minCut(contr))
                {
                    ListGraph::Edge new_e = gh_tree.addEdge(sn, supernode1);
                    gh_tree_flows[new_e] = gh_tree_flows[e];
                }
                else
                {
                    ListGraph::Edge new_e = gh_tree.addEdge(sn, supernode2);
                    gh_tree_flows[new_e] = gh_tree_flows[e];
                }
            }

            // Add the new supernodes to the stack
            if (gh_tree_supernodes[supernode1].size() > 1)
            {
                supernode_stack.push(supernode1);
            }
            if (gh_tree_supernodes[supernode2].size() > 1)
            {
                supernode_stack.push(supernode2);
            }

            // Remove the current supernode from the tree
            gh_tree.erase(supernode);
        }

        // Phew.. that was a lot of code. Finally, populate the class data members
        _tree.clear();

        ListGraph::NodeMap<ListGraph::Node> final_node_map(gh_tree);

        for (ListGraph::NodeIt s(gh_tree); s != INVALID; ++s)
        {
            auto n = _tree.addNode();
            _tree_labels[n] = _graph.id(gh_tree_supernodes[s][0]);
            final_node_map[s] = n;
        }

        for (ListGraph::EdgeIt e(gh_tree); e != INVALID; ++e)
        {
            ListGraph::Node s = final_node_map[gh_tree.u(e)];
            ListGraph::Node t = final_node_map[gh_tree.v(e)];
            ListGraph::Edge f = _tree.addEdge(s, t);
            _tree_flows[f] = gh_tree_flows[e];
        }

        time_total = t_total.tick();

        // Write times to json log
        global_json_logger.add("gh_time_min_cut", time_min_cut);
        global_json_logger.add("gh_time_relabel", time_contraction);
        global_json_logger.add("gh_time_total", time_total);
    }

    int min_k_cut_value(unsigned int k)
    {
        // Sum the k-1 smallest values in _fl

        timer timer;

        unsigned int n_cuts = k - 1;
        std::vector<int> heap;    // A heap of size n_cuts

        for (ListGraph::EdgeIt e(_tree); e != INVALID; ++e)
        {
            heap.push_back(_tree_flows[e]);
            std::push_heap(heap.begin(), heap.end());
            if (heap.size() > n_cuts)
            {
                std::pop_heap(heap.begin(), heap.end());
                heap.pop_back();
            }
        }
        int sum = 0;
        for (int i = 0; i < n_cuts; ++i)
        {
            sum += heap[i];
        }

        // Write time to json log
        global_json_logger.add("min_k_cut_value_time", timer.tick());

        return sum;
    }

    void min_k_cut_map(
        unsigned int k, ListGraph::NodeMap<unsigned int>& cut_map)
    {
        // Creates a cut_map, which stores a unique integer value for each connected component
        // created by the min-k cut

        // Algorithm: Find ids of the k smallest flow values
        // Make a copy of _tree and delete the k corresponding edges
        // Color the connected components of the resulting graph

        timer t_total;

        // Find the k-1 smallest flow values
        timer t_find_min_flows;
        unsigned int n_cuts = k - 1;
        std::vector<ListGraph::Edge> heap;
        auto comp = [&](ListGraph::Edge a, ListGraph::Edge b) {
            return _tree_flows[a] < _tree_flows[b];
        };
        for (ListGraph::EdgeIt e(_tree); e != INVALID; ++e)
        {
            heap.push_back(e);
            std::push_heap(heap.begin(), heap.end(), comp);
            if (heap.size() > n_cuts)
            {
                std::pop_heap(heap.begin(), heap.end(), comp);
                heap.pop_back();
            }
        }
        global_json_logger.add(
            "min_k_cut_map_time_find_min_flows", t_find_min_flows.tick());

        timer t_dfs;

        // Make a copy of _tree
        ListGraph tree_copy;
        lemon::GraphCopy<ListGraph, ListGraph> copy(_tree, tree_copy);
        copy.run();

        // Delete the k corresponding edges
        for (auto& e : heap)
        {
            tree_copy.erase(e);
        }

        // Color the connected components of the resulting graph
        unsigned int color = 0;
        // Do a DFS on the tree_copy
        for (ListGraph::NodeIt n(tree_copy); n != INVALID; ++n)
        {
            if (cut_map[n] == 0)
            {
                color++;
                std::stack<ListGraph::Node> stack;
                stack.push(n);
                while (!stack.empty())
                {
                    ListGraph::Node m = stack.top();
                    stack.pop();
                    cut_map[m] = color;
                    for (ListGraph::OutArcIt e(tree_copy, m); e != INVALID; ++e)
                    {
                        ListGraph::Node v = tree_copy.target(e);
                        if (cut_map[v] == 0)
                        {
                            stack.push(v);
                        }
                    }
                }
            }
        }

        global_json_logger.add("min_k_cut_map_time_dfs", t_dfs.tick());
        global_json_logger.add("min_k_cut_map_time_total", t_total.tick());
    }
};
