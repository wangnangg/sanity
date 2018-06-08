#pragma once
#include "linear.hpp"
#include "srn.hpp"
#include "srnssolve.hpp"

namespace sanity::petrinet
{
Real srnProbReward(const StochasticRewardNet& srn,
                   const GeneralSrnSteadyStateSol& sol,
                   const std::vector<std::unique_ptr<MarkingIntf>>& markings,
                   const MarkingDepReal& reward_func);

Real srnProbReward(const StochasticRewardNet& srn,
                   const IrreducibleSrnSteadyStateSol& sol,
                   const std::vector<std::unique_ptr<MarkingIntf>>& markings,
                   const MarkingDepReal& reward_func);

uint srnPlaceToken(PetriNetState state, uint pid);
Real srnTransRate(PetriNetState state, uint tid);

MarkingDepReal srnPlaceTokenFunc(uint pid);
MarkingDepReal srnTransRateFunc(uint tid);

}  // namespace sanity::petrinet
