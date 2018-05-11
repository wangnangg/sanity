#pragma once
#include <functional>
#include "dcmodel.hpp"
#include "linear.hpp"
#include "type.hpp"

namespace sanity::powerflow
{
struct DCPowerFlowResult
{
    std::vector<Real> busVoltageAngles;
    std::vector<Real> lineRealPowers;
    std::vector<Real> slackBusRealPower;
    std::vector<std::vector<uint>> islands;  // the first being slack
    std::vector<bool>
        islandSolved;  // one island may be impossible to solve
                       // because it doesn't have a generator bus
};

// the solution is the angles
DCPowerFlowResult solveDC(
    const DCPowerFlowModel& model,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView xb)>& linear_solver);

}  // namespace sanity::powerflow
