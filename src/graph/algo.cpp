
#include "algo.hpp"

namespace sanity::graph
{
// continue?
static bool dfsRecursion(
    const Graph& g, uint curr, std::vector<bool>& visited,
    const std::function<bool(uint parent, uint curr)>& visitor)
{
    const auto& node = g.getNode(curr);
    for (const auto& eid : node.edgeIdxList)
    {
        const auto& edge = g.getEdge(eid);
        uint nid = edge.src == curr ? edge.dst : edge.src;
        if (!visited[nid])
        {
            if (!visitor(curr, nid))
            {
                return false;
            }
            visited[nid] = true;
            if (!dfsRecursion(g, nid, visited, visitor))
            {
                return false;
            }
        }
    }
    return true;
}
void depthFirstSearch(
    const Graph& g, uint start,
    const std::function<bool(uint parent, uint curr)>& visitor)
{
    auto visited = std::vector<bool>(g.nodeCount(), false);
    if (!visitor(0, start))
    {
        return;
    }
    visited[start] = true;
    dfsRecursion(g, start, visited, visitor);
}
std::vector<std::vector<uint>> connectedComponents(const Graph& g)
{
    auto visited = std::vector<bool>(g.nodeCount(), false);
    auto components = std::vector<std::vector<uint>>();
    uint comp_i = 0;
    while (true)
    {
        uint start;
        for (start = 0; start < visited.size(); start++)
        {
            if (!visited[start])
            {
                break;
            }
        }
        if (start == visited.size())
        {
            break;
        }
        components.push_back({});
        auto& comp = components[comp_i];
        depthFirstSearch(g, start, [&](uint, uint curr) {
            visited[curr] = true;
            comp.push_back(curr);
            return true;
        });
        comp_i += 1;
    }
    return components;
}

}  // namespace sanity::graph
