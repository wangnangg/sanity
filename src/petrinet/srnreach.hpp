#pragma once
#include <vector>
#include "reach.hpp"
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
    ReachGraph reachGraph;
    std::vector<MarkingInitProb> initProb;
};
// not dealing with vanishing loops, yet
ReducedReachGenResult srnGenReducedReach(const StochasticRewardNet& srn,
                                         const Marking& init);

linear::SpmatrixTriple srnRateMatrix(const StochasticRewardNet& srn,
                                     const ReachGraph& g,
                                     const linear::Permutation& mat2rg_idx);

}  // namespace sanity::petrinet
