#pragma once
#include <vector>
#include "reach.hpp"
#include "splinear.hpp"
#include "srn.hpp"
#include "type.hpp"

namespace sanity::petrinet
{
struct MarkingInitProb
{
    uint idx;
    Real prob;
};

struct ReducedReachGenResult
{
    graph::DiGraph graph;
    std::vector<Marking> nodeMarkings;
    std::vector<Real> edgeRates;
    std::vector<MarkingInitProb> initProbs;
};
// not dealing with vanishing loops, yet
ReducedReachGenResult genReducedReachGraph(const StochasticRewardNet& srn,
                                           const Marking& mk, Real tol,
                                           uint max_iter);

}  // namespace sanity::petrinet
