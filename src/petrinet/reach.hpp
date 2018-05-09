#pragma once

#include "graph.hpp"
#include "linear.hpp"
#include "petrinet.hpp"

namespace sanity::petrinet
{
struct ReachGraph
{
    graph::DiGraph graph;
    std::vector<Marking> nodeMarkings;
    std::vector<uint> edgeTrans;
};

ReachGraph genReachGraph(const PetriNet& net, const Marking& init);
}  // namespace sanity::petrinet
