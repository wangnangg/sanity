#pragma once
#include <cassert>
#include <vector>
#include "type.hpp"

namespace sanity::graph
{
class DiGraph
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

    DiGraph() : _nodes(), _edge_count(0) {}
    DiGraph(uint node_count) : _nodes(node_count), _edge_count(0) {}
    uint addNode()
    {
        uint idx = _nodes.size();
        _nodes.push_back(Node());
        return idx;
    }
    uint addEdge(uint src, uint dst)
    {
        assert(src < nodeCount());
        assert(dst < nodeCount());
        auto eid = edgeCount();
        _edge_count++;
        _nodes[src].edges.push_back({eid, dst});
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
