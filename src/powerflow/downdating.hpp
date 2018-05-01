#pragma once

#include "model.hpp"
#include "solve.hpp"

namespace sanity::powerflow
{
// estimate the new solution when a line is disconnected, based on the
// original solution
void downdatingOnDisconnectedLineSG(const PowerGrid& grid, int slackBusIdx,
                                    int lineIdx, std::vector<BusState>& sol);

}  // namespace sanity::powerflow
