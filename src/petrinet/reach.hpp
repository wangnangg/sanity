#pragma once

#include "graph.hpp"
#include "linear.hpp"
#include "petrinet.hpp"

namespace sanity::petrinet
{
struct ReachGraph
{
    graph::DiGraph digraph;
    std::vector<Marking> markings;
    std::vector<uint> edge2trans;
};

ReachGraph genReachGraph(const PetriNet& net, const Marking& init);
}  // namespace sanity::petrinet
