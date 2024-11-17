#include <iostream>
#include <lemon/list_graph.h>
#include <lemon/preflow.h>

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

public:
k_min_cut(ListDigraph const& graph, ListDigraph::NodeMap<int> const& weights)
    : _graph(graph), _weights(weights), _p(graph), _fl(graph)
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
}

int min_k_cut_value(unsigned int k){
    // Sum the k smallest values in _fl
    std::vector<int> k_heap;
    for (ListDigraph::NodeIt n(_graph); n != INVALID; ++n){
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

    //  TODO: implement
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