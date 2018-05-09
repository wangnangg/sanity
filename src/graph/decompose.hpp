#pragma once

#include "digraph.hpp"
#include "graph.hpp"

namespace sanity::graph
{
struct ConnectedComponent
{
    std::vector<uint> nodes;
};

std::vector<ConnectedComponent> decompCc(const Graph& g);

struct StronglyConnectedComponent
{
    std::vector<uint> nodes;
    bool isBottom;
};

std::vector<StronglyConnectedComponent> decompScc(const DiGraph& g);

}  // namespace sanity::graph
