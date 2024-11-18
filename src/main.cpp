#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/preflow.h>
#include <stack>

using namespace lemon;

class k_min_cut
{
    ListDigraph const& _graph;
    ListDigraph::NodeMap<int> const& _weights;

    // The Gomory-Hu tree is encoded in the _p (predecessor) and _fl (min flow) maps as follows:
    // "The edges of T are the final pairs (i,p[i]) for from 2 to n, and edge (i,p[i]) has value fl(i)."

    // The predecessor map
    ListDigraph::NodeMap<ListDigraph::Node> _p;
    // The min flow map
    ListDigraph::NodeMap<int> _fl;
    
    // The Gomory-Hu Tree
    ListDigraph _tree;

public:
k_min_cut(ListDigraph const& graph, ListDigraph::NodeMap<int> const& weights)
    : _graph(graph), _weights(weights), _p(graph), _fl(graph), _tree()
{
}

    
void run_gomory_hu(){
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
    // Choose a root node
    ListDigraph::NodeIt s(_graph);

    for (ListDigraph::NodeIt t(s); t != INVALID; ++t){
        if (s == t) continue;
        Preflow<ListDigraph, ListDigraph::NodeMap<int>> min_cut(_graph, _weights, s, t);
        min_cut.run();
        _fl[t] = min_cut.flowValue();
        for (ListDigraph::NodeIt i(_graph); i != INVALID; ++i){
            if (i != s && min_cut.minCut(i) && _p[i] == t){
                _p[i] = s;
            }
        }
        if (min_cut.minCut(_p[t])){
            _p[s] = _p[t];
            _p[t] = s;
            _fl[s] = _fl[t];
            _fl[t] = min_cut.flowValue();
        }
    }

    // Create the Gomory-Hu tree. The tree is an undirected graph with the same nodes as the original graph
    // and edges (i, p[i]) with weight fl[i].
    // Making it into a graph to make it easier to traverse, however in principle _p is enough to represent the tree.
    // An important property here is that each node has a single parent (except the root node which has none).
    for (ListDigraph::NodeIt n(_graph); n != INVALID; ++n){
        _tree.addNode();
    }

    for (ListDigraph::NodeIt n(_graph); n != INVALID; ++n){
        if (_p[n] != INVALID){
            ListDigraph::Arc a = _tree.addArc(n, _p[n]);
        }
    }
}

int min_k_cut_value(unsigned int k){
    // Sum the k smallest values in _fl
    std::vector<int> k_heap;

    for (ListDigraph::NodeIt n(_graph); n != INVALID; ++n){
        k_heap.push_back(_fl[n]);
        std::push_heap(k_heap.begin(), k_heap.end());
        if (k_heap.size() > k){
            std::pop_heap(k_heap.begin(), k_heap.end());
            k_heap.pop_back();
        }
    }
    int sum = 0;
    for (int i = 0; i < k; ++i){
        sum += k_heap[i];
    }
    return sum;
}

void min_k_cut_map(unsigned int k, ListDigraph::NodeMap<unsigned int>& cut_map){
    // Creates a cut_map, which stores a unique integer value for each connected component
    // created by the min-k cut

    // Algorithm: Find ids of the k smallest values in _fl
    // Make a copy of _tree and delete the k corresponding edges
    // Color the connected components of the resulting graph

    // Find the k smallest values in _fl
    std::vector<ListDigraph::Node> k_heap;
    auto comp = [&](ListDigraph::Node a, ListDigraph::Node b){
        return _fl[a] < _fl[b];
    };
    for (ListDigraph::NodeIt n(_graph); n != INVALID; ++n){
        k_heap.push_back(n);
        std::push_heap(k_heap.begin(), k_heap.end(), comp);
        if (k_heap.size() > k){
            std::pop_heap(k_heap.begin(), k_heap.end(), comp);
            k_heap.pop_back();
        }
    }

    // Make a copy of _tree
    ListDigraph tree_copy;
    DigraphCopy<ListDigraph, ListDigraph> copy(_tree, tree_copy);
    copy.run();

    // Delete the k corresponding edges
    for(auto n : k_heap){
        // By construction, node n only has a single outgoing edge
        ListDigraph::OutArcIt e(_tree, n);
        if (e != INVALID)
            tree_copy.erase(e);
    }

    // Color the connected components of the resulting graph
    unsigned int color = 0;
    // Do a DFS on the tree_copy
    for (ListDigraph::NodeIt n(tree_copy); n != INVALID; ++n){
        if (cut_map[n] == 0){
            color++;
            std::stack<ListDigraph::Node> stack;
            stack.push(n);
            while (!stack.empty()){
                ListDigraph::Node m = stack.top();
                stack.pop();
                cut_map[m] = color;
                for (ListDigraph::OutArcIt e(tree_copy, m); e != INVALID; ++e){
                    ListDigraph::Node v = tree_copy.target(e);
                    if (cut_map[v] == 0){
                        stack.push(v);
                    }
                }
            }
        }
    }
}

};



int main()
{
    using namespace lemon;

    ListDigraph g;
    ListDigraph::Node u = g.addNode();
    ListDigraph::Node v = g.addNode();
    ListDigraph::Arc a = g.addArc(u, v);
    std::cout << "Hello World! This is LEMON library here." << std::endl;
    std::cout << "We have a directed graph with " << countNodes(g) << " nodes "
         << "and " << countArcs(g) << " arc." << std::endl;

    // Create / Import graph
    return 0;
}