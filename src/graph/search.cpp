#include "search.hpp"

namespace sanity::graph
{
// continue?
static bool dfsRecursion(
    const Graph& g, uint curr, std::vector<bool>& visited,
    const std::function<bool(uint parent, uint curr)>& visitor)
{
    const auto& node = g.getNode(curr);
    for (const auto& edge : node.edges)
    {
        uint nid = edge.dst;
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
}  // namespace sanity::graph
