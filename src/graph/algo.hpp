#pragma once
#include <functional>
#include "graph.hpp"

namespace sanity::graph
{
void depthFirstSearch(
    const Graph& g, uint start,
    const std::function<bool(uint parent, uint curr)>& visitor);
std::vector<std::vector<uint>> connectedComponents(const Graph& g);

}  // namespace sanity::graph
