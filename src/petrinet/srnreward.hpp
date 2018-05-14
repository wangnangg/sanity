#pragma once
#include "linear.hpp"
#include "srn.hpp"
#include "srnssolve.hpp"

namespace sanity::petrinet
{
Real srnProbReward(const StochasticRewardNet& srn,
                   const GeneralSrnSteadyStateSol& sol,
                   const std::vector<Marking>& markings,
                   const MarkingDepReal& reward_func);

Real srnProbReward(const StochasticRewardNet& srn,
                   const IrreducibleSrnSteadyStateSol& sol,
                   const std::vector<Marking>& markings,
                   const MarkingDepReal& reward_func);

}  // namespace sanity::petrinet
