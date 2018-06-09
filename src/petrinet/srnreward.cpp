#include "srnreward.hpp"

namespace sanity::petrinet
{
Real srnProbReward(const StochasticRewardNet& srn,
                   const SrnSteadyStateSol& sol,
                   const std::vector<std::unique_ptr<MarkingIntf>>& markings,
                   const MarkingDepReal& reward_func)
{
    Real r = 0;
    for (uint i = sol.nTransient; i < sol.solution.size(); i++)
    {
        const auto& mk = markings[(uint)sol.matrix2node.forward(i)];
        r += reward_func(PetriNetState{&srn, mk.get()}) * sol.solution(i);
    }
    return r;
}

Real srnCumReward(const StochasticRewardNet& srn,
                  const SrnSteadyStateSol& sol,
                  const std::vector<std::unique_ptr<MarkingIntf>>& markings,
                  const MarkingDepReal& reward_func)
{
    Real r = 0;
    for (uint i = 0; i < sol.nTransient; i++)
    {
        const auto& mk = markings[(uint)sol.matrix2node.forward(i)];
        r += reward_func(PetriNetState{&srn, mk.get()}) * sol.solution(i);
    }
    return r;
}

uint srnPlaceToken(PetriNetState state, uint pid)
{
    return state.marking->nToken(pid);
}
Real srnTransRate(PetriNetState state, uint tid)
{
    auto net = (const StochasticRewardNet*)state.net;
    assert(net->transProps[tid].type == SrnTransType::Exponetial);
    if (net->pnet.isTransitionEnabled(tid, state.marking))
    {
        return net->transProps[tid].val(state);
    }
    else
    {
        return 0.0;
    }
}

MarkingDepReal srnPlaceTokenFunc(uint pid)
{
    return [pid](PetriNetState state) { return srnPlaceToken(state, pid); };
}

MarkingDepReal srnTransRateFunc(uint tid)
{
    return [tid](PetriNetState state) { return srnTransRate(state, tid); };
}

MarkingDepReal srnMttaFunc() { return 1.0; }

Real srnMtta(const StochasticRewardNet& srn, const SrnSteadyStateSol& sol,
             const std::vector<std::unique_ptr<MarkingIntf>>& markings)
{
    return srnCumReward(srn, sol, markings, srnMttaFunc());
}

}  // namespace sanity::petrinet
