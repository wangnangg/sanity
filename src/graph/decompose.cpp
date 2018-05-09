
#include "decompose.hpp"
#include "search.hpp"

namespace sanity::graph
{
std::vector<ConnectedComponent> decompCc(const Graph& g)
{
    auto visited = std::vector<bool>(g.nodeCount(), false);
    auto components = std::vector<ConnectedComponent>();
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
            comp.nodes.push_back(curr);
            return true;
        });
        comp_i += 1;
    }
    return components;
}

struct SccNode
{
    size_t depth;
    size_t lowlink;
    bool onstack;
    SccNode() : depth(0), lowlink(0), onstack(false) {}
};

static bool recursiveVisit(const DiGraph& g, std::vector<SccNode>& node_info,
                           uint src_idx, uint& depth,
                           std::vector<uint>& node_stack,
                           std::vector<StronglyConnectedComponent>& scc_list)
{
    auto& src_node = node_info[src_idx];
    src_node.depth = depth;
    src_node.lowlink = depth;
    src_node.onstack = true;
    depth += 1;
    node_stack.push_back(src_idx);
    bool is_bottom = true;
    for (const auto& edge : g.getNode(src_idx).edges)
    {
        uint dst_idx = edge.dst;
        if (dst_idx != src_idx)
        {
            auto& dst_node_info = node_info[dst_idx];
            if (dst_node_info.depth == 0)
            {
                if (!recursiveVisit(g, node_info, dst_idx, depth, node_stack,
                                    scc_list))
                {
                    is_bottom = false;
                }
                if (src_node.lowlink > dst_node_info.lowlink)
                {
                    src_node.lowlink = dst_node_info.lowlink;
                }
            }
            else if (dst_node_info.onstack)
            {
                if (src_node.lowlink > dst_node_info.depth)
                {
                    src_node.lowlink = dst_node_info.depth;
                }
            }
            else
            {
                is_bottom = false;
            }
        }
    }

    if (src_node.lowlink == src_node.depth)
    {
        std::vector<uint> node_list;
        size_t top_node_idx;
        do
        {
            top_node_idx = node_stack.back();
            node_stack.pop_back();
            node_list.push_back(top_node_idx);
            node_info[top_node_idx].onstack = false;
        } while (top_node_idx != src_idx);
        scc_list.push_back({std::move(node_list), is_bottom});
        return false;
    }
    else
    {
        return is_bottom;
    }
}

std::vector<StronglyConnectedComponent> decompScc(const DiGraph& g)
{
    std::vector<StronglyConnectedComponent> scc_list;
    std::vector<SccNode> node_info(g.nodeCount());
    std::vector<uint> scc_stack;
    uint depth = 1;
    for (uint i = 0; i < g.nodeCount(); i++)
    {
        if (node_info[i].depth == 0)
        {
            recursiveVisit(g, node_info, i, depth, scc_stack, scc_list);
        }
    }
    return scc_list;
}

}  // namespace sanity::graph
