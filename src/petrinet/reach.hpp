#pragma once

#include "graph.hpp"
#include "linear.hpp"
#include "petrinet.hpp"

namespace sanity::petrinet
{
struct ReachGraph
{
    graph::DiGraph graph;
    std::vector<std::unique_ptr<MarkingIntf>> nodeMarkings;
    std::vector<uint> edgeTrans;
};

ReachGraph genReachGraph(const PetriNet& net, const MarkingIntf& init);
}  // namespace sanity::petrinet
