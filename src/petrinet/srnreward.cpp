#include "srnreward.hpp"

namespace sanity::petrinet
{
Real srnProbReward(const StochasticRewardNet& srn,
                   const GeneralSrnSteadyStateSol& sol,
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

Real srnProbReward(const StochasticRewardNet& srn,
                   const IrreducibleSrnSteadyStateSol& sol,
                   const std::vector<Marking>& markings,
                   const MarkingDepReal& reward_func)
{
    Real r = 0;
    for (uint i = 0; i < sol.prob.size(); i++)
    {
        const auto& mk = markings[i];
        r += reward_func(PetriNetState{&srn, &mk}) * sol.prob(i);
    }
    return r;
}
}  // namespace sanity::petrinet
