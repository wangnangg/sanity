#pragma once
#include <vector>
#include "type.hpp"

namespace sanity::graph
{
class DiGraph
{
public:
    struct Edge
    {
        uint src;
        uint dst;
    };

    struct Node
    {
        std::vector<uint> edgeIdxList;
    };

    DiGraph() {}
    DiGraph(uint node_count) : _nodes(node_count) {}
    uint addNode()
    {
        uint idx = _nodes.size();
        _nodes.push_back(Node());
        return idx;
    }
    uint addEdge(uint src, uint dst)
    {
        auto eid = _edges.size();
        _edges.push_back({src, dst});
        _nodes[src].edgeIdxList.push_back(eid);
        return eid;
    }
    const Node& getNode(uint nid) const { return _nodes[nid]; }
    const Edge& getEdge(uint eid) const { return _edges[eid]; }
    uint nodeCount() const { return _nodes.size(); }
    uint edgeCount() const { return _nodes.size(); }

private:
    std::vector<Node> _nodes;
    std::vector<Edge> _edges;
};

}  // namespace sanity::graph
