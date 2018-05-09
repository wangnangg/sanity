#pragma once
#include <functional>
#include "graph.hpp"

namespace sanity::graph
{
void depthFirstSearch(
    const Graph& g, uint start,
    const std::function<bool(uint parent, uint curr)>& visitor);
}
