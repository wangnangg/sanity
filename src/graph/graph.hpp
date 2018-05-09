#pragma once
// undirected graph

#include <vector>
#include "type.hpp"

namespace sanity::graph
{
class Graph
{
public:
    struct Edge
    {
        uint eid;
        uint dst;
    };

    struct Node
    {
        std::vector<Edge> edges;
    };

    Graph() : _nodes(), _edge_count(0) {}
    Graph(uint node_count) : _nodes(node_count), _edge_count(0) {}
    uint addNode()
    {
        uint idx = _nodes.size();
        _nodes.push_back(Node());
        return idx;
    }
    uint addEdge(uint src, uint dst)
    {
        auto eid = edgeCount();
        _edge_count++;
        _nodes[src].edges.push_back({eid, dst});
        _nodes[dst].edges.push_back({eid, src});
        return eid;
    }
    const Node& getNode(uint nid) const { return _nodes[nid]; }
    uint nodeCount() const { return _nodes.size(); }
    uint edgeCount() const { return _edge_count; }

private:
    std::vector<Node> _nodes;
    uint _edge_count;
};

}  // namespace sanity::graph
