#pragma once

#include <iostream>
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
