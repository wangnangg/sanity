#include <gtest/gtest.h>
#include "powerflow.hpp"
using namespace sanity::powerflow;

/* The reference system from the book "Power System Load Flow Analysis" by
 * Lynn Powell
 */
PowerGrid referenceSystem()
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

/* example T7.4 from "The Power Flow Problem" by James D. McCalley,Iowa State
 * University
 */
PowerGrid exampleT7_4()
{
    PowerGrid grid;
    auto bus1 = grid.addGeneratorBus(0, 1, 0, 0);
    auto bus2 = grid.addGeneratorBus(0.6661, 1.05, 0, 0);
    auto bus3 = grid.addLoadBus(Complex(2.8653, 1.2244));
    grid.addTransmissionLine(bus1, bus2, Complex(0, 0.1), 0.01);
    grid.addTransmissionLine(bus1, bus3, Complex(0, 0.1), 0.01);
    grid.addTransmissionLine(bus2, bus3, Complex(0, 0.1), 0.01);
    return grid;
}

void printPfSol(const std::vector<BusState>& sol)
{
    for (uint i = 0; i < sol.size(); i++)
    {
        std::cout << "bus " << i << ":"
                  << "P: " << sol[i].power.real()
                  << ", Q: " << sol[i].power.imag()
                  << ", |V|: " << std::abs(sol[i].voltage)
                  << ", arg(V): " << std::arg(sol[i].voltage) << std::endl;
    }
}

TEST(powerflow, newton_ref)
{
    auto grid = referenceSystem();
    auto sol = flatStart(grid, 0);
    auto res = newton(grid, 0, sol, sanity::linear::solve, 100, 1e-6);
    printPfSol(sol);
    std::cout << "error: " << res.error << ", iter:" << res.nIter
              << std::endl;
}

TEST(powerflow, newton_exampleT7_4)
{
    auto grid = exampleT7_4();
    auto sol = flatStart(grid, 0);
    auto res = newton(grid, 0, sol, sanity::linear::solve, 100, 1e-6);
    printPfSol(sol);
    std::cout << "error: " << res.error << ", iter:" << res.nIter
              << std::endl;
}
