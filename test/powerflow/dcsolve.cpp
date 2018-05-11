#include <gtest/gtest.h>
#include "powerflow.hpp"
using namespace sanity::powerflow;
using namespace sanity::linear;

static PowerGrid referenceSystem()
{
    PowerGrid grid;
    auto bus1 = grid.addGeneratorBus(0, 1.0, 0, 0);
    auto bus2 = grid.addLoadBus(Complex(40, 20) / 100.0);  // p.u.
    auto bus3 = grid.addLoadBus(Complex(25, 15) / 100.0);
    auto bus4 = grid.addLoadBus(Complex(40, 20) / 100.0);
    auto bus5 = grid.addLoadBus(Complex(50, 20) / 100.0);
    grid.addTransmissionLine(bus1, bus2, Complex(0.05, 0.11), 0.02 / 2);
    grid.addTransmissionLine(bus1, bus3, Complex(0.05, 0.11), 0.02 / 2);
    grid.addTransmissionLine(bus1, bus5, Complex(0.03, 0.08), 0.02 / 2);
    grid.addTransmissionLine(bus2, bus3, Complex(0.04, 0.09), 0.02 / 2);
    grid.addTransmissionLine(bus2, bus5, Complex(0.04, 0.09), 0.02 / 2);
    grid.addTransmissionLine(bus3, bus4, Complex(0.06, 0.13), 0.03 / 2);
    grid.addTransmissionLine(bus4, bus5, Complex(0.04, 0.09), 0.02 / 2);
    return grid;
}

static void printPfSol(const DCPowerFlowModel& model,
                       const DCPowerFlowResult sol)
{
    for (uint is = 0; is < sol.islands.size(); is++)
    {
        std::cout << "island " << is << ":" << std::endl;
        for (uint i = 0; i < sol.islands[is].size(); i++)
        {
            const auto& bus_idx = sol.islands[is][i];
            std::cout << "bus " << bus_idx << ", angle: "
                      << sol.busVoltageAngles[bus_idx] * 180 / 3.1415926
                      << " degrees" << std::endl;
        }
    }
    for (uint i = 0; i < sol.lineRealPowers.size(); i++)
    {
        std::cout << "power on line from " << model.getLine(i).startBus
                  << " to " << model.getLine(i).endBus << ": "
                  << sol.lineRealPowers[i] << std::endl;
    }
}

static DCPowerFlowModel threebus_example_dc()
{
    DCPowerFlowModel grid;
    auto bus1 = grid.addBus(DCBusType::PV, 1.0);
    auto bus2 = grid.addBus(DCBusType::PV, (63.0 - 10.0) / 100.0);
    auto bus3 = grid.addBus(DCBusType::PQ, -90.0 / 100.0);
    grid.addTransmissionLine(bus1, bus2, 0.0576);
    grid.addTransmissionLine(bus2, bus3, 0.092);
    grid.addTransmissionLine(bus1, bus3, 0.17);
    return grid;
}

TEST(powerflow, dc_ref)
{
    auto grid = referenceSystem();
    auto dc = ACModel2DC(grid);
    auto sol = solveDC(dc, solveLU);
    printPfSol(dc, sol);
}

TEST(powerflow, dc_threebus)
{
    auto dc = threebus_example_dc();
    auto sol = solveDC(dc, solveLU);
    printPfSol(dc, sol);
}
