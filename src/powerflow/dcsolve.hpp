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
};

// the solution is the angles
DCPowerFlowResult solveDC(
    const DCPowerFlowModel& model,
    const std::function<void(linear::MatrixMutableView A,
                             linear::VectorMutableView xb)>& linear_solver);

}  // namespace sanity::powerflow
