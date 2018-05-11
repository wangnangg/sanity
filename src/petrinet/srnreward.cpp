#include "srnreward.hpp"

namespace sanity::petrinet
{
Real srnProbReward(const StochasticRewardNet& srn,
                   const SrnSteadyStateSolution& sol,
                   const std::vector<Marking>& markings,
                   const MarkingDepReal& reward_func)
{
    Real r = 0;
    for (uint i = sol.nTransient; i < sol.solution.size(); i++)
    {
        const auto& mk = markings[(uint)sol.matrix2node.forward(i)];
        r += reward_func(PetriNetState{&srn, &mk}) * sol.solution(i);
    }
    return r;
}
}  // namespace sanity::petrinet
